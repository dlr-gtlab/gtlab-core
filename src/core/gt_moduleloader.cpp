/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_moduleloader.h"
#include "gt_moduleinterface.h"
#include "gt_datamodelinterface.h"
#include "gt_objectfactory.h"
#include "gt_logging.h"
#include "gt_algorithms.h"
#include "gt_versionnumber.h"
#include "gt_coreapplication.h"
#include "internal/gt_moduleupgrader.h"
#include "internal/gt_sharedfunctionhandler.h"
#include "internal/gt_commandlinefunctionhandler.h"

#include <QDir>
#include <QPluginLoader>
#include <QDirIterator>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QDomElement>

#include "gt_algorithms.h"
#include "gt_utilities.h"
#include "gt_qtutilities.h"

namespace
{

/// logs a warning once
const auto logWarnOnce = [](QString const& msg) {
    static QVector<QString> logged;
    if (!logged.contains(msg))
    {
        logged.append(msg);
        gtWarning() << msg;
    }
};

/// logs a debug message once
const auto logDebugOnce = [](QString const& msg) {
    static QVector<QString> logged;
    if (!logged.contains(msg))
    {
        logged.append(msg);
        gtDebug() << msg;
    }
};

class ModuleMetaData
{
public:

    explicit ModuleMetaData(const QString& loc)
       : m_libraryLocation(loc)
    {}

    /**
     * Loads the meta data from the module json file
     */
    void readFromJson(const QJsonObject& json);

    const QString& location() const noexcept
    {
        return m_libraryLocation;
    }

    const QString& moduleId() const noexcept
    {
        return m_id;
    }

    struct Dependency
    {
        // avoids faulty cpp-check warning
        bool optional() const { return isOptional; }

        QString name;
        GtVersionNumber version;
        bool isOptional {false};
    };

    /**
     * @return A list of modules (modulename, version) of which
     * this module depends.
     */
    const std::vector<Dependency>&
    directDependencies() const noexcept
    {
        return m_deps;
    }

    /// Returns a list of modules that allow to suppress this module
    const QStringList& suppressorModules() const noexcept
    {
        return m_suppression;
    }

    const QMap<QString, QString>& environmentVars() const noexcept
    {
        return m_envVars;
    }

private:
    QVariantList
    metaArray(const QJsonObject& metaData, const QString& id)
    {
        return metaData.value(id).toArray().toVariantList();
    }

    QString m_id;
    QString m_libraryLocation;

    std::vector<Dependency> m_deps;
    QMap<QString, QString> m_envVars;
    QStringList m_suppression;
};


// Function to check if a candidate string matches a dependency pattern.
bool
matchesDependency(const QString& dependency, const QString& candidate)
{
    const QString regexPrefix = "regex:";

    // If the dependency starts with the explicit "regex" prefix, use regex matching.
    if (dependency.startsWith(regexPrefix))
    {
        // Remove the prefix to get the actual regex pattern.
        QString pattern = dependency.mid(regexPrefix.length());
        QRegularExpression rx(pattern);

        // Check if the candidate matches the regex pattern.
        return rx.match(candidate).hasMatch();
    }
    else
    {
        // Otherwise, perform an exact, literal match.
        return dependency == candidate;
    }
}

using ModuleMetaMap = std::map<QString, ModuleMetaData>;
ModuleMetaMap loadModuleMeta();

QStringList
getMatchedModuleIds(const QString& dependency,
                    const ModuleMetaMap& allModules)
{
    QStringList result;

    for (auto&& module : allModules)
    {
        auto moduleId = module.second.moduleId();
        if (matchesDependency(dependency, moduleId)) result.push_back(moduleId);
    }

    return result;
}

} // namespace

class GtModuleLoader::Impl
{
public:
    /// Successfully loaded plugins
    QMap<QString, GtModuleInterface*> m_plugins;

    /// Mapping of suppressed plugins to their suppressors
    QMap<QString, QSet<QString>> m_suppressedPlugins;

    const std::map<QString, ModuleMetaData> m_metaData{loadModuleMeta()};

    /// Modules initialized indicator.
    bool m_modulesInitialized{false};

    /**
     * @brief performLoading
     *
     *        Implementeds the actual loading logic
     *
     * @param modulesToLoad Modules that need to be loaded
     * @param metaMap Map to the module meta data
     * @param failedModules List of modules failed to load
     * @return
     */
    bool performLoading(GtModuleLoader& moduleLoader,
                        const QStringList& modulesToLoad,
                        const ModuleMetaMap &metaMap,
                        QStringList& failedModules);
    /**
     * @brief checkDependency
     * The dependencies of the module are given by a list and are compared
     * to the modules which are already loaded by the program
     * If a needed dependecy is not loaded in as a plugin in the
     * program the function will return false, else true.
     * The function will also return false if the needed module is available
     * but the version is older than defined in the dependency
     *
     * @param meta The module meta data
     * @return true if all dependencies are ok
     */
    bool dependenciesOkay(const ModuleMetaData& meta);

    /**
     * @brief printDependencies
     */
    static void printDependencies(const ModuleMetaData& meta);
    static void printDependencies(const QStringList& modules,
                                  const std::map<QString, ModuleMetaData>& map);

    /**
     * @brief Checks whether the module with the given moduleId is suppressed
     * by another module. If the module is suppressed by another module,
     * it checks if this suppression is allowed. Returns true if there is a
     * valid suppression, otherwise returns false.
     * @param m The meta data of the module
     * @return True if there is a valid suppression, otherwise returns false.
     */
    bool isSuppressed(const ModuleMetaData& m) const;

    /**
     * @brief Returns the list of all modules that can be loaded
     * @return
     */
    QStringList getAllLoadableModuleIds() const
    {
        QStringList moduleIds;
        std::transform(m_metaData.begin(), m_metaData.end(),
           std::back_inserter(moduleIds),
           [](const std::pair<QString, ModuleMetaData>& metaEntry)
           {
               return metaEntry.second.moduleId();
           });
        return moduleIds;

    }
};

GtModuleLoader::GtModuleLoader() :
    m_pimpl{std::make_unique<GtModuleLoader::Impl>()}
{ }

GtModuleLoader::~GtModuleLoader() = default;

namespace
{

QDir getModuleDirectory()
{
#ifndef Q_OS_ANDROID
    QString path = QCoreApplication::applicationDirPath() +
                   QDir::separator() + QStringLiteral("modules");
#else
    QString path = QCoreApplication::applicationDirPath();
#endif

    QDir modulesDir(path);
#ifdef Q_OS_WIN
    modulesDir.setNameFilters(QStringList() << QStringLiteral("*.dll"));
#endif

    return modulesDir;
}

QStringList getModuleFilenames()
{
    auto modulesDir = getModuleDirectory();

    if (!modulesDir.exists())
    {
        return {};
    }

    // file names in dir
    auto files = modulesDir.entryList(QDir::Files);

    // convert to absolute file names
    std::transform(files.begin(), files.end(), files.begin(),
                   [&modulesDir](const QString& localFileName) {
        return modulesDir.absoluteFilePath(localFileName);
    });

    return files;
}

QStringList getModulesToExclude()
{

    // initialize module blacklist
    QFile excludeFile(qApp->applicationDirPath() + QDir::separator() +
                      QStringLiteral("_exclude.json"));

    if (!excludeFile.exists())
    {
        return {};
    }

    logDebugOnce("Module exclude file found!");

    if (!excludeFile.open(QIODevice::ReadOnly))
    {
        logWarnOnce(
            QObject::tr("Failed to open the module exclude file!")
        );
        return {};
    }

    QJsonDocument doc(QJsonDocument::fromJson(excludeFile.readAll()));

    excludeFile.close();

    if (doc.isNull())
    {
        logWarnOnce(
            QObject::tr("Failed to parse the module exclude file!")
        );
        return {};
    }

    QJsonObject json = doc.object();

    QVariantList exModList =
            json.value(QStringLiteral("modules")).toArray().toVariantList();

    QStringList excludeList;

    for (const QVariant& exMod : qAsConst(exModList))
    {
        QVariantMap modItem = exMod.toMap();
        const QString name =
            modItem.value(QStringLiteral("id")).toString();

        excludeList << name;
    }

    return excludeList;
}

/**
 * @brief Take care to log crashed modules
 */
class CrashedModulesLog
{
public:
    CrashedModulesLog() :
        settings(GtCoreApplication::localApplicationIniFilePath(),
                 QSettings::IniFormat)
    {
        crashed_mods = settings.value(QStringLiteral("loading_crashed"))
                           .toStringList();
    }

    void push(const QString& moduleFile)
    {
        crashed_mods << moduleFile;
        sync();
    }

    /**
     * @brief Returns a list of crashed modules on last start
     */
    QStringList crashedModules() const
    {
        return crashed_mods;
    }

    void pop()
    {
        crashed_mods.removeLast();
        sync();
    }

    /**
     * Temporarily store the current module. If the app crashes,
     * this will be persitently stored as crashed
     */
    auto makeSnapshot(const QString& currentModuleLocation)
    {
        push(currentModuleLocation);

        return gt::finally([this](){
            pop();
        });
    }

private:
    void sync()
    {
        settings.setValue(QStringLiteral("loading_crashed"), crashed_mods);
        settings.sync();
    }

    QSettings settings;
    QStringList crashed_mods;

};

/**
 * @brief Filters a module meta map according the the function keepModule
 * @param modules Input map of modules
 * @param keepModule Function of signature (bool*)(const ModuleMetaData&)
 * @return Output map of filtered modules
 */
template <typename FilterFun>
ModuleMetaMap filterModules(const ModuleMetaMap& modules, FilterFun keepModule)
{
    ModuleMetaMap result;
    for (const auto& entry : modules)
    {
        const auto& moduleMeta = entry.second;
        if (keepModule(moduleMeta)) {
            result.insert(entry);
        }
    }

    return result;
}

/**
 * @brief Loads the meta data of a single module
 * @param moduleFileName The path to the plugin
 * @return
 */
ModuleMetaData loadModuleMeta(const QString& moduleFileName)
{
    assert(QFile(moduleFileName).exists());

    // load plugin from entry
    QPluginLoader loader(moduleFileName);

    ModuleMetaData meta(moduleFileName);
    meta.readFromJson(loader.metaData());

    return meta;
}

ModuleMetaMap loadModuleMeta()
{
    std::map<QString, ModuleMetaData> metaData;

    const auto moduleFiles = getModuleFilenames();
    for (const QString& moduleFile : moduleFiles)
    {
        const auto meta = loadModuleMeta(moduleFile);
        metaData.insert(std::make_pair(meta.moduleId(), meta));
    }

    const auto crashed_mods = CrashedModulesLog().crashedModules();

    // Remove all modules, that have been crashed earlies
    metaData = filterModules(metaData, [&](const ModuleMetaData& m){
        if (crashed_mods.contains(m.location()))
        {
            logWarnOnce(
                QObject::tr("Module '%1' caused a crash in a previous run. "
                            "Skipping module!").arg(m.moduleId())
            );
            return false;
        }
        return true;
    });

    // Remove all modules, that should be exluded
    const auto excludeList = getModulesToExclude();
    metaData = filterModules(metaData, [&](const ModuleMetaData& m){
        if (excludeList.contains(m.moduleId()))
        {
            logWarnOnce(
                QObject::tr("Module '%1' found in exclude list. "
                            "Skipping module!").arg(m.moduleId())
            );
            return false;
        }
        return true;
    });

    return metaData;
}

} // namespace

bool
GtModuleLoader::loadSingleModule(const QString& moduleLocation)
{
    if (!QFile(moduleLocation).exists())
    {
        gtError() << QObject::tr("Cannot load module. "
                                 "Module File '%1' does not exists.")
                         .arg(moduleLocation);
        return false;
    }

    const auto moduleMeta = loadModuleMeta(moduleLocation);
    if (moduleMeta.moduleId().isEmpty())
    {
        gtError() << QObject::tr("Cannot load module. "
                                 "File '%1' is not a module.")
                         .arg(moduleLocation);
        return false;
    }

    const QStringList modulesToLoad{moduleMeta.moduleId()};

    // the meta data from the module directory
    auto moduleMetaMap = m_pimpl->m_metaData;

    // replace possibly existing module by the current module
    const auto it = moduleMetaMap.find(moduleMeta.moduleId());
    if (it != moduleMetaMap.end())
    {
        // module already exist in metadata, replace
        it->second = moduleMeta;
    }
    else
    {
        moduleMetaMap.insert(std::make_pair(moduleMeta.moduleId(), moduleMeta));
    }

    QStringList failedModules;
    if (!m_pimpl->performLoading(*this, modulesToLoad,
                                 moduleMetaMap, failedModules))
    {
        gtError().verbose() << QObject::tr("Some modules failed to load!");
        Impl::printDependencies(failedModules, moduleMetaMap);
        return false;
    }

    return true;
}

void
GtModuleLoader::load()
{
    auto allModulesIds = m_pimpl->getAllLoadableModuleIds();
    auto moduleMetaMap = m_pimpl->m_metaData;

    QStringList failedModules;
    if (!m_pimpl->performLoading(*this, allModulesIds,
                                 moduleMetaMap, failedModules))
    {
        gtError().verbose() << QObject::tr("Some modules failed to load!");
        Impl::printDependencies(failedModules, moduleMetaMap);
    }
}

QMap<QString, QString>
GtModuleLoader::moduleEnvironmentVars()
{
    auto moduleMeta = loadModuleMeta();

    QMap<QString, QString> retval;
    for (const auto& mod : moduleMeta)
    {
        retval.insert(mod.second.environmentVars());
    }

    return retval;
}

QStringList
GtModuleLoader::moduleIds() const
{
    return m_pimpl->m_plugins.keys();
}

QStringList
GtModuleLoader::moduleDatamodelInterfaceIds() const
{
    QStringList retval;
    gt::for_each_key(m_pimpl->m_plugins, [&](const QString& e){
        GtDatamodelInterface* dmi =
               dynamic_cast<GtDatamodelInterface*>(m_pimpl->m_plugins.value(e));

        if (dmi && dmi->standAlone())
        {
            retval << e;
        }
    });

    return retval;
}

GtVersionNumber
GtModuleLoader::moduleVersion(const QString& id) const
{
    if (m_pimpl->m_plugins.contains(id))
    {
        return m_pimpl->m_plugins.value(id)->version();
    }

    return GtVersionNumber();
}

QString
GtModuleLoader::moduleDescription(const QString& id) const
{
    if (m_pimpl->m_plugins.contains(id))
    {
        return m_pimpl->m_plugins.value(id)->description();
    }

    return QString();
}

QString
GtModuleLoader::moduleAuthor(const QString& id) const
{
    if (m_pimpl->m_plugins.contains(id))
    {
        return m_pimpl->m_plugins.value(id)->metaInformation().author;
    }

    return QString();
}

QString
GtModuleLoader::moduleContact(const QString& id) const
{
    if (m_pimpl->m_plugins.contains(id))
    {
        return m_pimpl->m_plugins.value(id)->metaInformation().authorContact;
    }

    return QString();
}

QString
GtModuleLoader::moduleLicence(const QString& id) const
{
    if (m_pimpl->m_plugins.contains(id))
    {
        return m_pimpl->m_plugins.value(id)->metaInformation().licenseShort;
    }

    return QString();
}

QString
GtModuleLoader::getSupportedInterfaceByModule(QObject *pluginObj,
                                              const QStringList &lOfItfs)
{
    auto found = std::find_if(std::begin(lOfItfs), std::end(lOfItfs),
                 [&pluginObj](const QString& itfName) {
        return pluginObj->qt_metacast(itfName.toUtf8().constData()) != nullptr;
    });

    return found != lOfItfs.end() ? *found : "unknown";
}

QString
GtModuleLoader::modulePackageId(const QString& id) const
{
    if (m_pimpl->m_plugins.contains(id))
    {
        GtDatamodelInterface* dmi =
                dynamic_cast<GtDatamodelInterface*>(m_pimpl->m_plugins.value(id));

        if (dmi)
        {
            return dmi->package().className();
        }
    }

    return {};
}

void
GtModuleLoader::initModules()
{
    if (m_pimpl->m_modulesInitialized)
    {
        gtWarning() << QObject::tr("Modules already initialized!");
        return;
    }

    for (auto const& value : qAsConst(m_pimpl->m_plugins))
    {
        value->init();
    }

    m_pimpl->m_modulesInitialized = true;
}

void
GtModuleLoader::addSuppression(const QString& suppressorModuleId,
        const QString& suppressedModuleId)
{
    m_pimpl->m_suppressedPlugins[suppressedModuleId] << suppressorModuleId;
}


bool
GtModuleLoader::check(GtModuleInterface* plugin) const
{
    const auto errorString = [=](){
        return QObject::tr("Loading module '%1' failed:").arg(plugin->ident());
    };

    if (m_pimpl->m_plugins.contains(plugin->ident()))
    {
        gtWarning() << errorString()
                    << QObject::tr("Module was already loaded!");
        return false;
    }

    GtDatamodelInterface* dmp = checkInterface<GtDatamodelInterface>(
        plugin->ident(), plugin);

    // contains dynamic linked datamodel classes
    if (dmp)
    {
        if (!gtObjectFactory->invokable(dmp->package()))
        {
            gtWarning() << errorString()
                        << QObject::tr("Package is not invokable!");
            return false;
        }
        if (gtObjectFactory->containsDuplicates(dmp->data()))
        {
            gtWarning() << errorString()
                        << QObject::tr("A data class is already defined!");
            return false;
        }

        if (!gtObjectFactory->allInvokable(dmp->data()))
        {
            gtWarning() << errorString()
                        << QObject::tr("Not all data classes are invokable!");
            return false;
        }
    }

    return true;
}

void
GtModuleLoader::insert(GtModuleInterface* plugin)
{
    m_pimpl->m_plugins.insert(plugin->ident(), plugin);

    // register converter funcs
    foreach (const auto& r, plugin->upgradeRoutines())
    {
      gt::detail::GtModuleUpgrader::instance()
            .registerModuleConverter(plugin->ident(), r.target, r.f);
    }

    // register all interface functions of the module
    foreach(const auto& sharedFunction, plugin->sharedFunctions())
    {
        gt::interface::detail::registerFunction(plugin->ident(),
                                                sharedFunction);
    }

    // register all commandline functions of the module
    foreach(const auto& commandLineFunction, plugin->commandLineFunctions())
    {
        gt::commandline::registerFunction(commandLineFunction);
    }

    GtDatamodelInterface* dmp = checkInterface<GtDatamodelInterface>(plugin->ident(), plugin);

    // contains dynamic linked datamodel classes
    if (dmp)
    {
        gtObjectFactory->registerClasses(dmp->data());
        gtObjectFactory->registerClass(dmp->package());
    }
}

void
createAdjacencyMatrixImpl(const QStringList& modulesToLoad,
                          const ModuleMetaMap& allModules,
                          std::map<QString, QStringList>& matrix)
{
    for (const auto& moduleId : modulesToLoad)
    {
        // If the module is already in the matrix,
        // it will not overwrite the current module
        auto insertResult = matrix.insert(std::make_pair(moduleId, QStringList{}));

        if (!insertResult.second)
        {
            // continue, module is already in matrix, stop recursion
            continue;
        }

        auto moduleIt = allModules.find(moduleId);
        if (moduleIt == allModules.end())
        {
            // dependency not found, skip it
            continue;
        }

        // Add dependencies to matrix
        auto& moduleDeps = insertResult.first->second;
        for (const auto& dep : moduleIt->second.directDependencies())
        {
            auto matchedModulIds = getMatchedModuleIds(dep.name, allModules);
            moduleDeps.append(matchedModulIds);
        }

        // recurse into dependencies
        createAdjacencyMatrixImpl(moduleDeps, allModules, matrix);
    }
}

/**
 * @brief Creates a map of all modules that need to be loaded,
 *        with key=moduleId and value=ModuleDependencies
 * @param modulesToLoad The list of modules to be included
 * @param allModules    The map of metadata of all modules (to query dependencies)
 * @return
 */
std::map<QString, QStringList>
createAdjacencyMatrix(const QStringList& modulesToLoad,
                      const ModuleMetaMap& allModules)
{
    std::map<QString, QStringList> adjMatrix;
    createAdjacencyMatrixImpl(modulesToLoad, allModules, adjMatrix);
    return adjMatrix;
}

/**
 * @brief Solves, which modules need to be loaded and returns the correct
 *        order of loading
 *
 * @param modulesIdsToLoad The ids of all modules that should be loaded
 * @param metaMap The map of module metadata whoch provides dependency
 *                information.
 *
 * @returns All resolved modules in the correct order, that need to be loaded
 */
QStringList
getSortedModulesToLoad(const QStringList& modulesIdsToLoad,
                       const ModuleMetaMap& metaMap)
{
    // create adjacency matrix
    const auto moduleMatrix = createAdjacencyMatrix(modulesIdsToLoad, metaMap);

    // sort modules in the correct order of dependencies
    auto sortedModuleIds = gt::topo_sort(moduleMatrix);

    if (sortedModuleIds.size() != moduleMatrix.size())
    {
        // there is a cyclic module dependency
        gtFatal() << QObject::tr("Cannot load modules, there is a dependency cycle.");
        for (auto&& m : moduleMatrix)
        {
            if (!m.second.empty()) gtInfo() <<
                    QString("'%1' needs").arg(m.first) << m.second;
        }

        sortedModuleIds.clear();
    }

    std::reverse(std::begin(sortedModuleIds), std::end(sortedModuleIds));

    // Only include modules that are actually found in metadata
    auto sortedModuleIdFiltered = QStringList{};
    std::copy_if(sortedModuleIds.begin(), sortedModuleIds.end(),
                 std::back_inserter(sortedModuleIdFiltered),
                 [&metaData = metaMap](const QString& moduleId){
        return metaData.find(moduleId) != metaData.end();
    });

    return sortedModuleIdFiltered;
}

bool
GtModuleLoader::Impl::performLoading(GtModuleLoader& moduleLoader,
                                 const QStringList& moduleIds,
                                 const ModuleMetaMap& metaMap,
                                 QStringList& failedModules)
{
    // initialize loading fail log
    CrashedModulesLog crashLog;

    auto sortedModuleIds = getSortedModulesToLoad(moduleIds, metaMap);

    QStringList successfullyLoaded{};

    // loading procedure
    for (const auto& currentModuleId : qAsConst(sortedModuleIds))
    {
        auto moduleIt = metaMap.find(currentModuleId);
        assert(moduleIt != metaMap.end());

        const ModuleMetaData& moduleMeta = moduleIt->second;

        if (isSuppressed(moduleMeta) || !dependenciesOkay(moduleMeta))
        {
            continue;
        }

        // store temporary module information in loading fail log
        auto _ = crashLog.makeSnapshot(moduleMeta.location());

        // load plugin from entry
        QPluginLoader loader(moduleMeta.location());
        std::unique_ptr<QObject> plugin(loader.instance());

        // check plugin object
        if (!plugin)
        {
            // could not recreate plugin
            gtError() << loader.errorString();
            continue;
        }

        gtDebug().medium().nospace()
                << QObject::tr("loading ") << moduleMeta.location() << "...";

        // check that plugin is a GTlab module
        auto module = gt::transfer_unique(std::move(plugin),[&](QObject* o) {
            return checkInterface<GtModuleInterface>(moduleMeta.location(), o);
        });

        if (module && moduleLoader.check(module.get()))
        {
            moduleLoader.insert(module.get());
            module.release()->onLoad();
            successfullyLoaded.push_back(currentModuleId);
        }
    }

    failedModules = std::move(sortedModuleIds);

    // remove successfully loaded from sortedModuleIds
    for (const auto& modId : qAsConst(successfullyLoaded))
    {
        failedModules.removeAll(modId);
    }

    return failedModules.empty();
}

GtModuleInterface*
getMatchingDependency(const QString& dependencyName,
                      const QMap<QString, GtModuleInterface*>& allModules)
{
    auto iter = std::find_if(allModules.begin(), allModules.end(),
                             [&](GtModuleInterface* module) {
        return matchesDependency(dependencyName, module->ident());
    });

    if (iter == allModules.end()) return nullptr;

    return iter.value();
}


/**
 * @brief Checks a optional dependency of a module and warns, if required
 */
void
checkOptionalDependency(const QString& moduleId,
                       const ModuleMetaData::Dependency& dependency,
                       const QMap<QString, GtModuleInterface*>& allModules)
{
    assert(dependency.optional());

    GtModuleInterface* dep = getMatchingDependency(dependency.name, allModules);

    // check dependency
    if (!dep)
    {
        gtWarning() << QObject::tr("Module '%1' has optional dependency '%2'"
                                   ", which could not be met! "
                                   "Not all functions will be available.")
                           .arg(moduleId, dependency.name);
        return;
    }

    // dependency exists, check version
    const GtVersionNumber currentVersion =
        dep->version();

    if (currentVersion < dependency.version)
    {
        gtWarning() << QObject::tr("Module '%1' optionally requires module "
                                   "'%2', which is outdated "
                                   "(needed >= %3, current: %4). "
                                   "This may lead to unexpected behaviour!")
                           .arg(moduleId, dep->ident(),
                                dependency.version.toString(),
                                currentVersion.toString());
    }
}

/**
 * @brief Checks a required dependency
 *
 * returns false, if the dependency could not be met, i.e.
 *  - dependency does not exists or
 *  - dependency is outdated
 */
bool
checkRequiredDependency(const QString& moduleId,
                        const ModuleMetaData::Dependency& dependency,
                        const QMap<QString, GtModuleInterface*>& allModules)
{

    assert(!dependency.optional());

    GtModuleInterface* dep = getMatchingDependency(dependency.name, allModules);

    // check dependency
    if (!dep)
    {
        gtError() << QObject::tr("Cannot load module '%1' due to missing "
                                 "dependency '%2'")
                         .arg(moduleId, dependency.name);
        return false;
    }

    // check version
    const auto currentVersion = dep->version();

    if (currentVersion < dependency.version)
    {
        gtError().nospace() << QObject::tr("Loading")
                            << moduleId + QStringLiteral(":");
        gtError()
            << QObject::tr("Dependency '%1' is outdated!").arg(dependency.name)
            << QObject::tr("(needed: >= %1, current: %2)")
                   .arg(dependency.version.toString(),
                        currentVersion.toString());
        return false;
    }
    else if (currentVersion > dependency.version)
    {
        gtInfo().medium()
            << QObject::tr("Dependency '%1' has a newer version than "
                           "the module '%2' requires")
                   .arg(dep->ident(), moduleId)
            << QObject::tr("(needed: >= %1, current: %2)")
                   .arg(dependency.version.toString(),
                        currentVersion.toString());
    }

    return true;
}

bool
GtModuleLoader::Impl::dependenciesOkay(const ModuleMetaData& meta)
{
    for (const ModuleMetaData::Dependency& dep : meta.directDependencies())
    {
        if (dep.optional())
        {
            checkOptionalDependency(meta.moduleId(), dep, m_plugins);
            continue;
        }

        // not optional
        if (!checkRequiredDependency(meta.moduleId(), dep, m_plugins))
        {
            // module cannot be loaded
            return false;
        }
    }

    return true;
}

void
GtModuleLoader::Impl::printDependencies(
    const QStringList& modules,
    const std::map<QString, ModuleMetaData>& map)
{
    if (gt::log::Logger::instance().verbosity() < gt::log::Everything)
    {
        return;
    }

    gtWarning() << QObject::tr("Module dependencies:");

    for (const auto& module : modules)
    {
        auto iter = map.find(module);
        assert(iter != map.end());

        Impl::printDependencies(iter->second);
    }
}

void
GtModuleLoader::Impl::printDependencies(const ModuleMetaData& meta)
{
    gtWarning() << QString("### %1 (%2)")
                   .arg(meta.moduleId(), meta.location());

    for (const ModuleMetaData::Dependency& dep : meta.directDependencies())
    {
        gtWarning() << QObject::tr("###   - %1 (%2)%3")
            .arg(dep.name, dep.version.toString(),
                 dep.optional() ? " (optional)" : "");
    }
}

bool
GtModuleLoader::Impl::isSuppressed(const ModuleMetaData& meta) const
{
    const auto& moduleId = meta.moduleId();
    const auto& allowedSupprs = meta.suppressorModules();

    // check if there is a set of suppressors for the given moduleId
    const auto it = m_suppressedPlugins.find(moduleId);
    if (it ==  m_suppressedPlugins.end())
        return false;

    // search for intersection between set of suppressors and allowedSupprs
    const auto& supressorList = *it;
    auto res = std::find_if(allowedSupprs.begin(), allowedSupprs.end(),
                [supressorList](const QString& allowedSuppr){
                    return supressorList.contains(allowedSuppr);});

    if (res != allowedSupprs.end())
    {
        gtWarning() << QObject::tr("'%1' is suppressed by '%2'")
                       .arg(moduleId, *res);
        return true;
    }

    return false;
}

/**
 * Loads the meta data from the module json file
 */
void
ModuleMetaData::readFromJson(const QJsonObject &pluginMetaData)
{
    auto json = pluginMetaData.value(QStringLiteral("MetaData")).toObject();

    // read plugin/module id
    m_id = pluginMetaData.value("IID").toString();

    // read dependencies
    QVariantList deps = metaArray(json, QStringLiteral("dependencies"));

    m_deps.clear();
    for (const auto& d : qAsConst(deps))
    {
        QVariantMap mitem = d.toMap();

        auto name = mitem.value(QStringLiteral("name")).toString();
        GtVersionNumber version(mitem.value(QStringLiteral("version"))
                                    .toString());

        auto isOptionalVar = mitem.value(QStringLiteral("optional"));
        bool isOptional = isOptionalVar.isValid() ?
                              isOptionalVar.toBool() : false;

        m_deps.push_back({name, version, isOptional});
    }

    // get sys_env_vars list
    QVariantList sys_vars = metaArray(json,
                                      QStringLiteral("sys_env_vars"));

    m_envVars.clear();
    for (const QVariant& var : qAsConst(sys_vars))
    {
        QVariantMap mitem = var.toMap();

        const QString name =
            mitem.value(QStringLiteral("name")).toString();
        const QString initVar =
            mitem.value(QStringLiteral("init")).toString();

        if (!m_envVars.contains(name))
        {
            m_envVars.insert(name, initVar);
        }
    }

    // get suppressor list
    QVariantList supprs = metaArray(json,
                                    QStringLiteral("allowSuppressionBy"));

    m_suppression.clear();
    for (const auto & s : qAsConst(supprs))
    {
        m_suppression.push_back(s.toString());
    }
}
