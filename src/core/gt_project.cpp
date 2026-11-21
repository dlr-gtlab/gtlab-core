/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 29.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_project.h"
#include "gt_coredatamodel.h"
#include "gt_abstractloadinghelper.h"
#include "gt_processdata.h"
#include "gt_projectprovider.h"
#include "gt_task.h"
#include "gt_footprint.h"
#include "gt_projectanalyzer.h"
#include "gt_projectdependencyanalyzer.h"
#include "gt_projectexecutionguard.h"
#include "internal/gt_projectio.h"
#include "internal/gt_moduleupgrader.h"

#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_coreapplication.h"
#include "gt_package.h"
#include "gt_labeldata.h"
#include "gt_label.h"
#include "gt_xmlutilities.h"
#include "gt_versionnumber.h"
#include "gt_externalizedobject.h"
#include "gt_externalizationmanager.h"
#include "gt_qtutilities.h"
#include "gt_filesystem.h"

#include "gt_logging.h"

#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QSet>
#include <QTemporaryDir>
#include <QTextStream>
#include <QUuid>
#include <QXmlStreamReader>

#include <cassert>

namespace
{

QString
packageModuleId(const GtPackage& package)
{
    const QString className =
        QString::fromLatin1(package.metaObject()->className());
    const QString id = gtObjectFactory->moduleId(className);
    return gtApp && gtApp->modulePackageId(id) == className ? id : QString{};
}

std::unique_ptr<GtPackage>
newPackage(const QString& moduleId)
{
    if (!gtApp) return {};
    return gt::unique_qobject_cast<GtPackage>(std::unique_ptr<GtObject>(
        gtObjectFactory->newObject(gtApp->modulePackageId(moduleId))));
}

std::unique_ptr<GtPackage>
loadPackage(const QString& moduleId, const QDomElement& root)
{
    if (root.tagName() != QLatin1String("GTLABMODULE") ||
        root.attribute(QStringLiteral("uuid")).isEmpty())
    {
        return {};
    }

    auto package = newPackage(moduleId);
    if (!package) return {};

    const QString recordedModule = root.attribute(QStringLiteral("module"));
    const QString recordedClass = root.attribute(QStringLiteral("class"));
    if ((!recordedModule.isEmpty() && recordedModule != moduleId) ||
        (!recordedClass.isEmpty() &&
         recordedClass != QLatin1String(package->metaObject()->className())))
    {
        return {};
    }

    package->setUuid(root.attribute(QStringLiteral("uuid")));
    if (!package->readData(root)) return {};
    return package;
}

bool
hasExternalizedData(const GtPackage& package)
{
    return !package.findChildren<GtExternalizedObject*>().isEmpty();
}

QString
storedPackageUuid(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return {};

    QXmlStreamReader xml(&file);
    if (!xml.readNextStartElement() ||
        xml.name() != QLatin1String("GTLABMODULE"))
        return {};
    return xml.attributes().value(QLatin1String("uuid")).toString();
}

bool
copyPackageDirectory(const QDir& source, const QDir& target,
                     bool projectRoot = true)
{
    if (!source.isReadable() || !target.mkpath(QStringLiteral(".")))
        return false;

    const auto entries = source.entryInfoList(
        QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
    for (const auto& entry : entries)
    {
        if (projectRoot && entry.isDir() &&
            entry.fileName().compare(QStringLiteral("backup"),
                                     Qt::CaseInsensitive) == 0)
        {
            continue;
        }
        // A module-owned link could lead outside the disposable copy.
        if (entry.isSymLink()) return false;

        const auto destination = target.filePath(entry.fileName());
        if (entry.isDir())
        {
            if (!copyPackageDirectory(QDir(entry.absoluteFilePath()),
                                      QDir(destination), false))
                return false;
        }
        else if (!entry.isFile() ||
                 !QFile::copy(entry.absoluteFilePath(), destination))
        {
            return false;
        }
    }
    return true;
}

enum class PackageStorage
{
    Standalone,
    Auxiliary,
    SnapshotFailed
};

PackageStorage
packageStorage(GtPackage& package, const QDir& source)
{
    if (hasExternalizedData(package) || !package.hasStandaloneData())
        return PackageStorage::Auxiliary;

    // A hook may delegate to its base and only read if a companion file
    // exists. Give it an isolated copy of the directory for this check.
    QTemporaryDir scratch;
    const QDir copy(scratch.path());
    const auto relativePath =
        QDir(source.canonicalPath()).relativeFilePath(copy.path());
    if (!scratch.isValid() ||
        (!QDir::isAbsolutePath(relativePath) &&
         !relativePath.startsWith("../")) ||
        !copyPackageDirectory(source, copy))
    {
        return PackageStorage::SnapshotFailed;
    }
    return package.hasStandaloneData(&copy) ? PackageStorage::Standalone
                                            : PackageStorage::Auxiliary;
}

gt::xml::ClassModuleMap
taskClassModuleIds(const QString& projectPath)
{
    gt::xml::ClassModuleMap result;
    QDirIterator files(projectPath + QDir::separator() + QStringLiteral("tasks"),
                       {QStringLiteral("*.gttask")}, QDir::Files,
                       QDirIterator::Subdirectories);
    while (files.hasNext())
    {
        QFile file(files.next());
        QDomDocument document;
        if (!gt::xml::readDomDocumentFromFile(file, document, true)) continue;

        const auto mappings =
            gt::xml::readClassModuleMap(document.documentElement());
        gt::insertMissingInto(result, mappings);
    }
    return result;
}

} // namespace

struct GtProject::Impl
{
    explicit Impl(const QString& projectPath) :
        path(projectPath),
        pathProp(QStringLiteral("path"), GtProject::tr("Path"),
                 GtProject::tr("Project path"), projectPath)
    {
    }

    /// Path to project directory on hard drive
    QString path;

    /// True if project data was successfully loaded
    bool valid{false};

    /// True if project data is currently loaded into the data model
    bool isOpen{false};

    /// True if project data upgrades are available
    bool upgradesAvailable{false};

    /// Whether to internalize all external data when saving
    bool internalizeOnSave{false};

    GtProjectSettings projectSettings;

    /// Last serialized selection (also used while the project is closed).
    QStringList declaredModuleIds;
    QMap<QString, QDomElement> moduleDeclarations;

    /// Declarations for which the loader did not produce a package.
    QStringList unresolvedModuleIds;

    /// Backing storage for the reference returned by moduleIds() while open.
    mutable QStringList moduleIdsSnapshot;

    /// Module ids associated with classes used by this project
    QMap<QString, QString> classModuleIds;

    /// Project path property. Only used for visualization in the GUI.
    GtStringProperty pathProp;

    /// User definable comment
    QString comment;
};

GtProject::GtProject(const QString& path) : pimpl(std::make_unique<Impl>(path))
{
    pimpl->valid = loadMetaData();
    pimpl->upgradesAvailable = checkForUpgrades();

    pimpl->pathProp.setReadOnly(true);

    registerProperty(pimpl->pathProp);

    if (gtApp)
    {
        connect(&pimpl->projectSettings, &GtProjectSettings::changed, this,
                []() {
                    if (gtApp->session()) gtApp->session()->save();
                });
    }
}

void
GtProject::setModuleIds(const QStringList& list)
{
    if (!isOpen())
    {
        pimpl->declaredModuleIds = list;
    }
}

const QString
GtProject::mainFilename()
{
    return QStringLiteral("project.") + mainFileExtension();
}

const QString
GtProject::mainFileExtension()
{
    return QStringLiteral("gtlab");
}

const QString
GtProject::moduleExtension()
{
    return QStringLiteral("gtmod");
}

const GtProjectSettings &
GtProject::getProjectSettings() const
{
    return pimpl->projectSettings;
}

GtProjectSettings &
GtProject::projectSettings()
{
    return pimpl->projectSettings;
}

QString
GtProject::comment() const
{
    return pimpl->comment;
}

void
GtProject::setComment(const QString& comment)
{
    pimpl->comment = comment;
    changed();
}

void
GtProject::setInternalizeOnSave(bool value)
{
    pimpl->internalizeOnSave = value;
}

QStringList
GtProject::availableModuleUpgrades() const
{
    GtProjectAnalyzer analyzer(this);
    GtFootprint footprint = analyzer.footPrint();

    // collect footprints
    QMap<QString, GtVersionNumber> footprints = footprint.fullVersionInfo();

    return gt::detail::GtModuleUpgrader::instance()
        .availableModuleUpgrades(footprints);
}

QList<GtVersionNumber>
GtProject::availableUpgrades(const QString& moduleId) const
{
    GtProjectAnalyzer analyzer(this);
    GtFootprint footprint = analyzer.footPrint();

    QMap<QString, GtVersionNumber> verInfo = footprint.fullVersionInfo();

    if (verInfo.contains(moduleId))
    {
        GtVersionNumber savedVer = verInfo.value(moduleId);

        return gt::detail::GtModuleUpgrader::instance()
            .availableUpgrades(moduleId, savedVer);
    }

    return {};
}

void
GtProject::upgradeProjectData()
{
    if (!isValid())
    {
        return;
    }

    const QStringList availUpgrades = availableModuleUpgrades();

    if (availUpgrades.isEmpty())
    {
        return;
    }

    GtProjectAnalyzer analyzer(this);
    GtFootprint footprint = analyzer.footPrint();

    QDir pdir(pimpl->path);
    QStringList entryList;

    for (const auto& mid : pimpl->declaredModuleIds)
    {
        renameOldModuleFile(path(), mid);
        const auto filename = moduleDataPath(mid);
        if (QFile::exists(filename)) entryList.append(filename);
    }

    entryList << pdir.absoluteFilePath(mainFilename());

    //exted list with task files
    QString tasksPath = pimpl->path + "/tasks";
    QDirIterator it(tasksPath,
                    QStringList() << "*.gttask",
                    QDir::Files,
                    QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        entryList << it.next();
    }

    gtDebug() << "upgrading files: " << entryList;

    // collect all version information
    QMap<QString, GtVersionNumber> versInfo = footprint.fullVersionInfo();

    bool saveWithLinkFiles = getProjectSettings()
                                 .ownObjectFileSerializationEnabled();

    gt::detail::GtModuleUpgrader::instance()
        .upgrade(objectName(), saveWithLinkFiles, versInfo, entryList);

    // update project footprint for updated module
    updateModuleFootprint(availUpgrades);

    pimpl->valid = loadMetaData();
    pimpl->upgradesAvailable = checkForUpgrades();
}


gt::filesystem::CopyStatus
gt::project::copyProjectData(const QDir& projectDir,
                             const QDir& targetDir,
                             int copyProjectFlags)
{


    int copyDirFlags = gt::filesystem::Recursive;
    if (copyProjectFlags & ForceOverwrite)
    {
        copyDirFlags |= gt::filesystem::OverwriteFiles;
    }

    QString reString;
    if (copyProjectFlags & IgnoreBackupMd)
    {
        reString += R"(^(?!GTlabBackUpMessage.md$).*)";
    }
    if (!(copyProjectFlags & IncludeBackups))
    {
        reString += R"(^(?!backup\/.*$).*)";
    }

    QRegularExpression regex(reString);
    return filesystem::copyDir(projectDir, targetDir, copyDirFlags, regex);
}



bool
GtProject::updateProjectMetaData(const QDir& projectPath,
                          const GtProject::ProjectMetaData& data)
{
    auto projectDoc = GtProject::readProjectData(projectPath);

    if (projectDoc.isNull())
    {
        return false;
    }

    auto projectRoot = projectDoc.documentElement();

    // we assume to have a valid gtlab project

    // update project name
    projectRoot.setAttribute("projectname", data.projectName);

    // store the project
    QString filename = projectPath.path() + QDir::separator() + mainFilename();
    QFile pfile(filename);

    // we have read from it before so it still should be there
    assert(pfile.exists());

    return gt::xml::writeDomDocumentToFile(filename, projectDoc, true);
}

void
GtProject::createBackup(QString const& message) const
{
    if (!isValid())
    {
        return;
    }

    QDir pdir(pimpl->path);

    assert(pdir.exists());

    QString timeStamp = QDateTime::currentDateTime().toString(
                "yyyyMMddhhmmss");

    QDir bdir(gt::project::backupDirPath(*this) + QDir::separator() + timeStamp);

    if (bdir.exists())
    {
        gtError() << "backup folder already exists!";
        return;
    }

    if (!bdir.mkpath("."))
    {
        gtError() << "could not create backup path!";
        return;
    }

    // backup all project files,
    // do not force overwrite and don't include backup directories
    if (gt::project::copyProjectData(pdir, bdir, 0) !=
        gt::filesystem::CopyStatus::Success)
    {
        gtError() << "Error while copying project files";
    }

    // backup project file
    QFile file(pdir.absoluteFilePath(mainFilename()));
    file.copy(bdir.absoluteFilePath(mainFilename()));

    if (!message.isEmpty())
    {
        QFile messageFile(bdir.absoluteFilePath(
                              gt::project::backUpMessageFileName()));

        if (!messageFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            gtWarning() << tr("Could not add message file to backup folder");
            return;
        }

        QTextStream stream(&messageFile);
        stream << message;

        messageFile.close();
    }

    return;
}

GtProject::RestoreStatus
GtProject::restoreBackupFiles(const QString& timeStamp)
{
    using S = GtProject::RestoreStatus;

    assert(!isOpen());

    QDir projectDir(path());
    assert(projectDir.exists());

    QDir backupDir(path() + QDir::separator() + "backup" + QDir::separator() +
                   timeStamp);

    if (!backupDir.exists())
    {
        gtError().nospace().noquote()
            << "Backup folder '" << timeStamp << "' not found";
        return S::ErrorNoBackupFound;
    }

    static QRegularExpression projectModuleTasksRex(
        R"(^[\w\s\-{}]+\.(gtlab|gtmod)$|^tasks\/[\w\s\-{}\/]+\.gttask$)");

    const auto projectFiles = gt::filesystem::directoryEntries(projectDir,
                                                         true,
                                                         projectModuleTasksRex);

    auto moveRelativePath = [](const QDir& srcDir,
                               const QDir& targetDir,
                               const QString& rel) -> bool
    {
        const QString dst = targetDir.filePath(rel);
        const QString src = srcDir.filePath(rel);

        QFileInfo dstInfo(dst);
        if (!QDir().mkpath(dstInfo.dir().absolutePath())) return false;
        if (QFile::exists(dst) && !QFile::remove(dst)) return false;

        return QFile::rename(src, dst);
    };

    // move outdated files to a temp dir
    QTemporaryDir temp;

    if (!temp.isValid())
    {
        gtError() << "Failed to create temporary directory";
        return S::ErrorCopyFailed;
    }

    QDir tempDir(temp.path());

    auto stash = [&](const QString& rel) {
        return moveRelativePath(projectDir, tempDir, rel);
    };

    auto restore = [&](const QString& rel) {
        return moveRelativePath(tempDir, projectDir, rel);
    };

    // stash files, since the backup might fail
    for (const auto& file : projectFiles)
    {
        if (!stash(file))
        {
            gtError().nospace().noquote() << "Failed to stash '" << file << "'";
            // try to undo partial stash best-effort
            for (const auto& f : projectFiles) restore(f);
            return S::ErrorCopyFailed;
        }
    }

    if (gt::project::copyProjectData(backupDir, projectDir,
            gt::project::ForceOverwrite | gt::project::IgnoreBackupMd) !=
        gt::filesystem::CopyStatus::Success)
    {
        // restore project files since backup restored has failed
        for (const auto& f : projectFiles) restore(f);
        return S::ErrorCopyFailed;
    }

    // The closed project instance must reflect the restored manifest before
    // it is reopened, irrespective of how the restore was requested.
    pimpl->valid = loadMetaData();
    pimpl->upgradesAvailable = checkForUpgrades();
    return pimpl->valid ? S::Success : S::ErrorCopyFailed;
}

bool
GtProject::loadMetaData()
{
    QDomElement root = readProjectData(path()).documentElement();

    if (root.isNull())
    {
        gtWarning() << tr("Root is null!");
        return false;
    }

    QString projectname = root.attribute(QStringLiteral("projectname"));
    if (projectname.isEmpty())
    {
        gtError() << tr("Invalid GTlab project file!");
        return false;
    }

    setObjectName(projectname);

    // read comment
    QDomElement cdata = root.firstChildElement(QStringLiteral("comment"));

    if (!cdata.isNull())
    {
        //gtDebug() << "(" << projectname << ") comment found!";
        pimpl->comment = cdata.text();
    }

    // module meta data
    readModuleMetaData(root);

    pimpl->classModuleIds = gt::xml::readClassModuleMap(root);
    gt::insertMissingInto(pimpl->classModuleIds, taskClassModuleIds(path()));

    return true;
}

bool
GtProject::checkForUpgrades() const
{
    if (!pimpl->valid)
    {
        return false;
    }

    GtProjectAnalyzer analyzer(this);
    GtFootprint footprint = analyzer.footPrint();

    // collect all version information
    QMap<QString, GtVersionNumber> versInfo = footprint.fullVersionInfo();

    return gt::detail::GtModuleUpgrader::instance()
        .upgradesAvailable(versInfo);
}

void
GtProject::readModuleMetaData(const QDomElement& root)
{
    pimpl->declaredModuleIds.clear();
    pimpl->moduleDeclarations.clear();

    /* module informations */
    QDomElement mdata = root.firstChildElement(QStringLiteral("MODULES"));

    if (mdata.isNull())
    {
        return;
    }

    QDomElement module = mdata.firstChildElement(QStringLiteral("MODULE"));
    while (!module.isNull())
    {
        QString mid = module.attribute(QStringLiteral("name"));

        if (mid.isEmpty())
        {
            gtWarning() << objectName() << ": " <<  tr("Invalid module found!");
        }
        else if (pimpl->declaredModuleIds.contains(mid))
        {
            gtWarning() << objectName() << ": "
                        <<  tr("Multiple module definition!")
                        << " (" << mid << ")";
        }
        else
        {
            if (!gtApp || !gtApp->moduleIds().contains(mid))
            {
                gtWarning().nospace() << objectName() << ": "
                                      << tr("Unknown module: ") << mid;
            }
            pimpl->declaredModuleIds << mid;
            pimpl->moduleDeclarations.insert(mid, module);
        }

        module = module.nextSiblingElement(QStringLiteral("MODULE"));
    }
}

void
GtProject::updateClassModuleIds()
{
    gt::xml::ClassModuleMap updated;
    for (const GtObject* object : findChildren<GtObject*>())
    {
        const QString className = object->isDummy()
                                      ? object->toMemento().className()
                                      : object->metaObject()->className();
        // skip visited classes
        if (className.isEmpty() || updated.contains(className)) continue;
        QString moduleId;
        if (const auto* factory = object->factory())
        {
            moduleId = factory->moduleId(className);
        }
        if (moduleId.isEmpty())
        {
            moduleId = gtObjectFactory->moduleId(className);
        }
        if (moduleId.isEmpty())
        {
            moduleId = pimpl->classModuleIds.value(className);
        }
        if (!moduleId.isEmpty())
        {
            updated.insert(className, moduleId);
        }
    }

    // merge remaining classes not handled by loaded modules
    for (auto kv = pimpl->classModuleIds.cbegin();
         kv != pimpl->classModuleIds.cend(); ++kv)
    {
        // skip visited classes
        if (kv.value().isEmpty() || updated.contains(kv.key())) continue;
        updated.insert(kv.key(), kv.value());
    }

    pimpl->classModuleIds = std::move(updated);
}

GtObject*
GtProject::readProcessData()
{
    GtProcessData* data = new GtProcessData;
    data->setFactory(gtObjectFactory);
    data->setDefault(true);
    data->read(path());

    return data;
}

GtObject*
GtProject::readLabelData(const GtObjectList& moduleData)
{
    GtLabelData* data = new GtLabelData;
    data->setDefault(true);
    data->setFactory(gtObjectFactory);

    // read saved label data here
    QDomElement root = readProjectData(path()).documentElement();

    if (!root.isNull())
    {
        /* label informations */
        QDomElement ldata = root.firstChildElement(QStringLiteral("LABELS"));

        if (!ldata.isNull())
        {
            QDomElement le = ldata.firstChildElement(QStringLiteral("object"));
            while (!le.isNull())
            {
                QString fieldClass = le.attribute(QStringLiteral("class"));

                if (fieldClass ==
                        QLatin1String(GtLabel::staticMetaObject.className()))
                {
                    GtObjectMemento memento(le);
                    if (!memento.isNull())
                    {
                        auto label =
                            memento.restore_unique<GtLabel>(gtObjectFactory);

                        if (label)
                        {
                            if (data->addLabel(label.get()))
                            {
                                // data takes ownership of the label
                                auto* addedLabel = label.release();
                                Q_ASSERT(addedLabel);
                            }
                            else
                            {
                                gtWarning() << tr("could not add label") << " ("
                                            << tr("already exists") << ")";
                            }
                        }
                    }
                }

                le = le.nextSiblingElement(QStringLiteral("object"));
            }
        }
    }

    // read datamodel label entries
    foreach (GtObject* obj, moduleData)
    {
        QList<GtLabel*> labels = obj->findChildren<GtLabel*>();
        foreach (GtLabel* label, labels)
        {
            data->addLabel(label->objectName());
        }
    }

    // add default label
    data->addDefaultLabel();

    return data;
}

GtObjectList
GtProject::readModuleData()
{
    GtObjectList packages;
    for (const QString& mid : pimpl->declaredModuleIds)
    {
        if (!gtApp || gtApp->modulePackageId(mid).isEmpty()) continue;

        renameOldModuleFile(path(), mid);
        QStringList warnings;
        QDomDocument document = gt::xml::loadProjectXmlWithLinkedObjects(
            moduleDataPath(mid), &warnings);
        for (const auto& warning : warnings)
            gtWarning() << warning;

        QDomElement root = document.documentElement();
        gt::insertMissingInto(pimpl->classModuleIds,
                              gt::xml::takeClassModuleMap(root));

        auto package = warnings.isEmpty() ? loadPackage(mid, root) : nullptr;
        if (!package || !package->readMiscData(QDir(path())))
        {
            gtWarning() << tr("Could not load project package for '%1'. Its "
                              "data will be preserved.")
                               .arg(mid);
            continue;
        }

        gtExternalizationManager->initExternalizedObjects(*package);
        packages.append(package.release());
    }
    return packages;
}

QString
GtProject::packageSlotError(const QString& moduleId) const
{
    if (!isOpen() || GtProjectExecutionGuard::isLocked(this))
    {
        return tr("Project packages can only be edited in an open project "
                  "while no task is executing.");
    }

    if (!gtApp || gtApp->modulePackageId(moduleId).isEmpty())
    {
        return tr("No loaded data-model module provides a package for '%1'.")
            .arg(moduleId);
    }

    if (moduleIds().contains(moduleId, Qt::CaseInsensitive))
    {
        return tr("The project already contains a package or an unresolved "
                  "package declaration for '%1'.")
            .arg(moduleId);
    }
    return {};
}

std::unique_ptr<GtPackage>
GtProject::createPackage(const QString& moduleId, QString& error) const
{
    error = packageSlotError(moduleId);
    if (!error.isEmpty()) return {};

    auto package = newPackage(moduleId);
    if (!package)
    {
        error = tr("Could not instantiate the registered package for '%1'.")
                    .arg(moduleId);
    }
    return package;
}

std::unique_ptr<GtPackage>
GtProject::importPackage(const QString& filePath, QString& error) const
{
    const QFileInfo file(filePath);
    if (file.suffix().compare(moduleExtension(), Qt::CaseInsensitive) != 0)
    {
        error = tr("Select a .gtmod project package file.");
        return {};
    }

    if (!isOpen() || GtProjectExecutionGuard::isLocked(this))
    {
        error = tr("Project packages can only be imported in an open project "
                   "while no task is executing.");
        return {};
    }

    QStringList warnings;
    auto document =
        gt::xml::loadProjectXmlWithLinkedObjects(filePath, &warnings);
    auto root = document.documentElement();
    if (!warnings.isEmpty())
    {
        error = tr("Could not read the package or its linked objects. %1")
                    .arg(warnings.join(QLatin1Char('\n')));
        return {};
    }
    if (root.tagName() != QLatin1String("GTLABMODULE"))
    {
        error = tr("The file does not contain a valid GTlab project package.");
        return {};
    }

    // Legacy package files identify their module by the standard filename.
    QString moduleId = root.attribute(QStringLiteral("module"));
    if (moduleId.isEmpty() && gtApp)
    {
        for (const auto& id : gtApp->moduleDatamodelInterfaceIds())
        {
            if (file.completeBaseName().compare(id, Qt::CaseInsensitive) != 0)
                continue;
            if (!moduleId.isEmpty())
            {
                error = tr("The filename matches more than one loaded module.");
                return {};
            }
            moduleId = id;
        }
    }

    if (moduleId.isEmpty()) moduleId = file.completeBaseName();
    error = packageSlotError(moduleId);
    if (!error.isEmpty()) return {};

    gt::xml::takeClassModuleMap(root);
    for (auto element = root.firstChildElement(); !element.isNull();
         element = element.nextSiblingElement())
    {
        if (element.tagName() != QLatin1String("object"))
        {
            error = tr("The package contains unsupported module-specific XML.");
            return {};
        }
    }

    auto package = loadPackage(moduleId, root);
    if (!package)
    {
        error = tr("The file contains invalid data for the registered package "
                   "of '%1'.")
                    .arg(moduleId);
        return {};
    }

    const QUuid packageUuid(package->uuid());
    if (!packageUuid.isNull() &&
        QUuid(storedPackageUuid(moduleDataPath(moduleId))) == packageUuid)
    {
        error = tr("The package UUID matches a retained package file in the "
                   "target project.");
        return {};
    }

    // Preserving UUIDs keeps references intact, including references in
    // module-defined properties.
    QSet<QUuid> targetIds{QUuid(uuid())};
    for (const auto* object : findChildren<GtObject*>())
    {
        targetIds.insert(QUuid(object->uuid()));
    }

    QMap<QString, QString> objectClasses;
    const auto elements = root.elementsByTagName(QStringLiteral("object"));
    objectClasses.insert(
        package->uuid(),
        QString::fromLatin1(package->metaObject()->className()));
    for (int i = 0; i < elements.count(); ++i)
    {
        const auto element = elements.at(i).toElement();
        const auto id = element.attribute(QStringLiteral("uuid"));
        if (QUuid(id).isNull() || objectClasses.contains(id))
        {
            error =
                tr("The package contains a missing or duplicate object UUID.");
            return {};
        }
        objectClasses.insert(id, element.attribute(QStringLiteral("class")));
    }

    QList<GtObject*> importedObjects = package->findChildren<GtObject*>();
    importedObjects.prepend(package.get());
    QSet<QUuid> importedIds;
    for (const auto* object : importedObjects)
    {
        const auto& id = object->uuid();
        const QUuid uuid(id);
        if (uuid.isNull() || importedIds.contains(uuid) ||
            targetIds.contains(uuid))
        {
            error = tr("The package contains a missing, duplicate or already "
                       "used object UUID (%1).")
                        .arg(id);
            return {};
        }
        importedIds.insert(uuid);
        if (object->isDummy())
        {
            error =
                tr("The package contains an unavailable or unreadable object.");
            return {};
        }
        const auto it = objectClasses.find(id);
        if (it != objectClasses.end())
        {
            if (it.value() != QLatin1String(object->metaObject()->className()))
            {
                error =
                    tr("A package object does not match its registered class.");
                return {};
            }
            objectClasses.erase(it);
        }
    }
    if (!objectClasses.isEmpty() ||
        !root.elementsByTagName(QStringLiteral("objectref")).isEmpty())
    {
        error = tr("The complete package object tree could not be loaded.");
        return {};
    }

    if (hasExternalizedData(*package))
    {
        error = tr("Importing packages with externalized data is unsupported.");
        return {};
    }
    switch (packageStorage(*package, QDir(file.absolutePath())))
    {
    case PackageStorage::Auxiliary:
        error = tr("This package uses module-specific auxiliary data or "
                   "cannot be reconstructed from its object tree.");
        return {};
    case PackageStorage::SnapshotFailed:
        error = tr("Could not verify the package's companion files in an "
                   "isolated directory.");
        return {};
    case PackageStorage::Standalone:
        break;
    }

    gtExternalizationManager->initExternalizedObjects(*package);
    return package;
}

QString
GtProject::packageDeletionError(GtPackage& package) const
{
    if (package.parent() != this || !isOpen())
        return tr("The package is not part of an open project.");

    if (GtProjectExecutionGuard::isLocked(this))
        return tr("Cannot delete a project package while a task is executing.");

    switch (packageStorage(package, QDir(path())))
    {
    case PackageStorage::Auxiliary:
        return tr("This package has externalized or module-specific data that "
                  "cannot safely be restored by Undo.");
    case PackageStorage::SnapshotFailed:
        return tr("Could not verify that the package can be restored by Undo.");
    case PackageStorage::Standalone:
        return {};
    }
    return {};
}

bool
GtProject::saveModuleData()
{
    if (!isOpen()) return false;

    bool needsRecoveryCopy = false;
    QMap<QString, GtPackage*> packages;
    QSet<QString> savedSlots;
    for (auto* package : findDirectChildren<GtPackage*>())
    {
        const auto mid = packageModuleId(*package);
        if (mid.isEmpty() || savedSlots.contains(mid.toLower()) ||
            pimpl->unresolvedModuleIds.contains(mid, Qt::CaseInsensitive))
        {
            gtError() << tr("Cannot save a project package with an unknown "
                            "or duplicate module slot.");
            return false;
        }
        savedSlots.insert(mid.toLower());
        packages.insert(mid, package);

        const auto filename = moduleDataPath(mid);
        if (!QFileInfo::exists(filename)) continue;

        if (pimpl->declaredModuleIds.contains(mid) &&
            storedPackageUuid(filename) == package->uuid())
            continue;

        if (packageStorage(*package, QDir(path())) !=
            PackageStorage::Standalone)
        {
            gtError() << tr("Cannot replace the old package files for '%1' "
                            "without risking auxiliary data.")
                             .arg(mid);
            return false;
        }
        needsRecoveryCopy = true;
    }

    // A stale master may still refer to XML files shared by the project's
    // link directory. Preserve the complete previous disk state for recovery
    // before any linked or auxiliary files are written.
    if (needsRecoveryCopy)
    {
        QDir backups(gt::project::backupDirPath(*this));
        const auto recoveryName =
            QStringLiteral("packages-") +
            QUuid::createUuid().toString(QUuid::WithoutBraces);
        if (!backups.mkpath(QStringLiteral("."))) return false;
        QTemporaryDir staging(
            backups.filePath(QStringLiteral(".packages-XXXXXX")));
        if (!staging.isValid() ||
            !copyPackageDirectory(QDir(path()), QDir(staging.path())) ||
            !backups.rename(QFileInfo(staging.path()).fileName(), recoveryName))
        {
            gtError() << tr("Could not back up the old project package files.");
            return false;
        }
        gtInfo() << tr("Old project package files backed up to %1.")
                        .arg(backups.absoluteFilePath(recoveryName));
    }

    if (!saveExternalizedObjectData())
    {
        gtError() << tr("Could not save externalized project data.");
        return false;
    }

    updateClassModuleIds();
    for (auto it = packages.cbegin(); it != packages.cend(); ++it)
    {
        const auto& mid = it.key();
        auto* package = it.value();
        QDomDocument document;
        document.appendChild(document.createProcessingInstruction(
            QStringLiteral("xml"),
            QStringLiteral("version=\"1.0\" encoding=\"utf-8\"")));
        auto root = document.createElement(QStringLiteral("GTLABMODULE"));
        root.setAttribute(QStringLiteral("uuid"), package->uuid());
        root.setAttribute(QStringLiteral("module"), mid);
        root.setAttribute(
            QStringLiteral("class"),
            QString::fromLatin1(package->metaObject()->className()));
        document.appendChild(root);

        if (!package->saveData(root, document) ||
            !package->saveMiscData(QDir(path())))
        {
            gtError()
                << tr("Could not save project package for '%1'.").arg(mid);
            return false;
        }

        const auto moduleMappings =
            gt::xml::classModuleMapForObjects(pimpl->classModuleIds, root);
        gt::xml::writeClassModuleMap(root, document, moduleMappings);
        if (!saveProjectFiles(moduleDataPath(mid), document)) return false;
    }

    return true;
}

bool
GtProject::saveProjectOverallData()
{
    if (!isOpen())
    {
        return false;
    }

    gtDebug().noquote() << tr("Saving project '%1'...").arg(objectName());

    updateClassModuleIds();

    QDomDocument document;
    QDomProcessingInstruction header = document.createProcessingInstruction(
            QStringLiteral("xml"),
            QStringLiteral("version=\"1.0\" encoding=\"utf-8\""));
    document.appendChild(header);

    QDomElement rootElement =
            document.createElement(QStringLiteral("GTLAB"));

    rootElement.setAttribute(QStringLiteral("projectname"), objectName());
    rootElement.setAttribute(QStringLiteral("version"),
                             gtApp->version().toString());

    // footprint is derived from the modules that the project requires,
    // not from all modules loaded in the current GTlab environment
    QDomDocument footPrintDoc;
    GtFootprint footPrint = createProjectFootprint();
    footPrintDoc.setContent(footPrint.exportToXML(), true);

    rootElement.appendChild(footPrintDoc.documentElement());

    QDomElement commentElement =
            document.createElement(QStringLiteral("comment"));
    QDomText cTxt = document.createTextNode(pimpl->comment);
    commentElement.appendChild(cTxt);
    rootElement.appendChild(commentElement);

    saveModuleMetaData(rootElement, document);

    gt::xml::writeClassModuleMap(rootElement, document, pimpl->classModuleIds);

    if (!saveProcessData(rootElement, document))
    {
        return false;
    }

    saveLabelData(rootElement, document);

    document.appendChild(rootElement);

    QString filename = pimpl->path + QDir::separator() + mainFilename();

    if (!saveProjectFiles(filename, document)) return false;

    // Keep the closed-project selection at the last successful save, even
    // when subsequent edits are discarded on close.
    readModuleMetaData(rootElement);
    return true;
}

bool
GtProject::saveExternalizedObjectData()
{
    bool success{true};
    auto objects = findChildren<GtExternalizedObject*>();

    // force internalization of all objects
    if (pimpl->internalizeOnSave)
    {
        gtDebug() << "Internalizing object data...";

        int counter = 0;
        for (auto* obj : qAsConst(objects))
        {
            success &= obj->internalize();
            counter += obj->isFetched();
        }

        gtInfo() << tr("Successfully internalized") << counter
                 << tr("out of") << objects.count() << tr("objects!");

        return success;
    }

    if (!gtExternalizationManager->isExternalizationEnabled())
    {
        return true;
    }

    gtDebug() << "Saving externalized object data...";

    for (auto* obj : qAsConst(objects))
    {
        // only externalize the object if its not referenced as the data wont be
        // cleared otherwise and will be saved as memento
        if (obj->refCount() == 0)
        {
            success &= obj->externalize();
        }
    }

    return success;
}

void
GtProject::saveModuleMetaData(QDomElement& root, QDomDocument& doc)
{
    writePackageSelection(root, doc);
}

void
GtProject::writePackageSelection(QDomElement& root, QDomDocument& doc) const
{
    QDomElement modulesElement = doc.createElement(QStringLiteral("MODULES"));

    for (const QString& mid : moduleIds())
    {
        const auto original = pimpl->moduleDeclarations.value(mid);
        const bool hasPackage = findPackage(mid);
        if (!hasPackage && !original.isNull())
        {
            modulesElement.appendChild(doc.importNode(original, true));
            continue;
        }

        QDomElement moduleElement = doc.createElement(QStringLiteral("MODULE"));
        moduleElement.setAttribute(QStringLiteral("name"), mid);
        if (hasPackage)
        {
            const auto version = gtApp->moduleVersion(mid);
            if (!version.isNull())
                moduleElement.setAttribute(QStringLiteral("version"),
                                           version.toString());
        }
        modulesElement.appendChild(moduleElement);
    }

    root.appendChild(modulesElement);
}

bool
GtProject::saveProcessData(QDomElement& /*root*/, QDomDocument& /*doc*/)
{
    const GtProcessData* pd = processData();

    if (pd)
    {
        return pd->save(path());
    }

    return true;
}

void
GtProject::saveLabelData(QDomElement& root, QDomDocument& doc)
{
    QDomElement ldElement = doc.createElement(QStringLiteral("LABELS"));

    GtLabelData* ld = labelData();

    if (ld)
    {
        foreach (const GtLabel* label, ld->findDirectChildren<GtLabel*>())
        {
            GtObjectMemento memento = label->toMemento();
            ldElement.appendChild(memento.documentElement());
        }
    }

    root.appendChild(ldElement);
}

QDomDocument
GtProject::readProjectData(const QDir& projectPath)
{
    return GtProjectIO::readProjectData(projectPath);
}

QList<GtLabel*>
GtProject::findLabelUsagesHelper(GtObject* obj, GtLabel* label)
{
    QList<GtLabel*> retval;

    QList<GtObject*> childs = obj->findDirectChildren<GtObject*>();

    foreach (GtObject* c, childs)
    {
        GtLabel* l = qobject_cast<GtLabel*>(c);

        if (l)
        {
            if (l->objectName() == label->objectName())
            {
                retval.append(l);
            }
        }
        else
        {
            retval.append(findLabelUsagesHelper(c, label));
        }
    }

    return retval;
}

void
GtProject::renameOldModuleFile(const QString& path, const QString& modId)
{
    QString filename = path + QDir::separator() + modId.toLower() +
                       + "." + mainFileExtension();

    QFile file(filename);

    if (!file.exists())
    {
        // no old module files found - everything ok
        return;
    }

    file.rename(path + QDir::separator() + modId.toLower() + "." +
                moduleExtension());
}


bool
GtProject::saveProjectFiles(const QString& filePath, const QDomDocument& doc)
{
    return GtProjectIO::saveProjectFiles(
        filePath,
        doc,
        projectSettings().ownObjectFileSerializationEnabled());
}

GtFootprint
GtProject::createProjectFootprint() const
{
    // determine the modules that the current project uses directly
    GtProjectDependencyAnalyzer analyzer(this);

    // resolve the complete set of modules that the project requires,
    // including transitive module dependencies
    const gt::Modules modules = gtApp ? gtApp->modules() : gt::Modules{};
    const auto requirements = modules.requirementsFor(
        QStringList(analyzer.directlyRequiredModuleIds().cbegin(),
                    analyzer.directlyRequiredModuleIds().cend()));

    QSet<QString> requiredModules{requirements.moduleIds.cbegin(),
                                  requirements.moduleIds.cend()};

    // entries that cannot be resolved must not lead to a pruning of the
    // previously stored footprint
    QSet<QString> unresolved{requirements.unresolved.cbegin(),
                             requirements.unresolved.cend()};
    unresolved.unite(analyzer.unknownUsedClassNames());

    // A declaration without an instantiated package may still have recoverable
    // data and dependencies, even if its package file is currently missing.
    const auto packageSelection = moduleIds();
    for (const QString& mid : packageSelection)
    {
        requiredModules.insert(mid);
        if (!findPackage(mid))
        {
            unresolved.insert(mid);
        }
    }

    // read the previously stored footprint to preserve version information
    QMap<QString, GtVersionNumber> oldModules;
    const QString oldFootprintData = readFootprint();
    if (!oldFootprintData.isEmpty())
    {
        oldModules = GtFootprint(oldFootprintData).modules();
    }

    QMap<QString, GtVersionNumber> moduleVersions;

    for (const QString& mid : requiredModules)
    {
        const bool unresolvedPackage =
            packageSelection.contains(mid) && !findPackage(mid);
        auto version =
            unresolvedPackage ? GtVersionNumber{} : modules.version(mid);
        if (version.isNull())
        {
            version = oldModules.value(mid, GtVersionNumber());
        }
        if (version.isNull() && unresolvedPackage)
        {
            version =
                GtVersionNumber(pimpl->moduleDeclarations.value(mid).attribute(
                    QStringLiteral("version")));
        }
        moduleVersions.insert(mid, version);
    }

    // If the dependency metadata of a required module is unavailable, its
    // transitive dependency closure cannot be recomputed safely. In this
    // case, keep previously stored footprint entries that cannot be proven
    // unused because of the unresolved dependency. Once all required module
    // metadata is available again, a later save can fully recompute the
    // closure and remove stale entries.
    if (!unresolved.isEmpty())
    {
        gtDebug() << tr("Project footprint cannot be recalculated completely"
                        " (unresolved dependencies: %1).")
                         .arg(
                             QStringList(unresolved.cbegin(), unresolved.cend())
                                 .join(", "));

        for (auto it = oldModules.cbegin(); it != oldModules.cend(); ++it)
        {
            if (!moduleVersions.contains(it.key()))
            {
                moduleVersions.insert(it.key(), it.value());
            }
        }
    }

    return GtFootprint::fromProjectModules(moduleVersions);
}

void
GtProject::updateModuleFootprint(const QStringList& modIds)
{
    gtDebug() << "GtProject::updateModuleFootprint...";

    auto document = readProjectData(pimpl->path);
    if (document.isNull())
    {
        return;
    }

    QString filename = path() + QDir::separator() + mainFilename();

    QDomElement pdata = document.documentElement();
    QDomElement footprint = pdata.firstChildElement(
                QStringLiteral("env-footprint"));


    if (footprint.isNull())
    {
        return;
    }

    if (modIds.contains(GtFootprint::frameworkIdentificationString()))
    {
        QDomElement core_ver =
                footprint.firstChildElement(QStringLiteral("core-ver"));

        if (core_ver.isNull())
        {
            return;
        }

        QDomElement core_ver_new = document.createElement("core-ver");
        footprint.replaceChild(core_ver_new, core_ver);

        QDomText t = document.createTextNode(gtApp->version().toString());
        core_ver_new.appendChild(t);
    }

    QDomElement mods = footprint.firstChildElement(QStringLiteral("modules"));

    if (mods.isNull())
    {
        return;
    }

    QDomElement mod = mods.firstChildElement(QStringLiteral("module"));
    while (!mod.isNull())
    {
        QDomElement modIdEl = mod.firstChildElement(QStringLiteral("id"));
        QDomElement modVerEl = mod.firstChildElement(QStringLiteral("ver"));

        if (!modIdEl.isNull() && !modVerEl.isNull())
        {
            if (modIds.contains(modIdEl.text()))
            {
                gtDebug() << "updating footprint " << modIdEl.text() << "...";
                gtDebug() << "file " << filename << "...";
                gtDebug() << "  |-> old = " << modVerEl.text();
                gtDebug() << "  |-> new = "
                          << gtApp->moduleVersion(modIdEl.text()).toString();

                modVerEl.firstChild().setNodeValue(
                            gtApp->moduleVersion(modIdEl.text()).toString());
            }
        }

        mod = mod.nextSiblingElement(QStringLiteral("module"));
    }

    if (!gt::xml::writeDomDocumentToFile(filename, document, true))
    {
        gtError() << filename << QStringLiteral(": ")
                  << "Failed to save project data!";
    }
}

GtProject::~GtProject() = default;

void
GtProject::markOpen()
{
    pimpl->unresolvedModuleIds.clear();
    for (const auto& mid : pimpl->declaredModuleIds)
    {
        if (!findPackage(mid)) pimpl->unresolvedModuleIds.append(mid);
    }
    pimpl->isOpen = true;
}

void
GtProject::markClosed()
{
    pimpl->isOpen = false;
    pimpl->unresolvedModuleIds.clear();
}

QMap<QString, QString>&
GtProject::classModuleIds()
{
    return pimpl->classModuleIds;
}

const QString&
GtProject::path() const
{
    return pimpl->path;
}

QString
GtProject::moduleDataPath(const QString &moduleId) const
{
    return path() + QDir::separator() + moduleId.toLower() + "." +
            GtProject::moduleExtension();
}

bool
GtProject::isValid() const
{
    return pimpl->valid;
}

bool
GtProject::isOpen() const
{
    return pimpl->isOpen;
}

bool
GtProject::upgradesAvailable() const
{
    return pimpl->upgradesAvailable;
}

bool
GtProject::upgradeProject(const QString& newProjectFilePath)
{
    if (newProjectFilePath.isEmpty() || path() == newProjectFilePath)
    {
        gtDebug() << "backup and overwriting project data...";

            // upgrade project data in separate thread if possible
            gtApp->loadingProcedure(gt::makeLoadingHelper([this]() {
                                        createBackup();
                                        upgradeProjectData();
                                    }).get());

        return true;
    }
    else
    {
        gtDebug() << "upgrading data as new project...";
        gtDebug() << "  |-> " << QFileInfo(newProjectFilePath).fileName();
        gtDebug() << "  |-> " << newProjectFilePath;

        auto newProject = GtProjectProvider::duplicateExistingProject(
            QDir(path()),
            QDir(newProjectFilePath),
            QFileInfo(newProjectFilePath).fileName()
            );

        if (!newProject)
        {
            gtError() << "Could not save project to new directory";
            return false;
        }

        gtApp->loadingProcedure(gt::makeLoadingHelper([&newProject]() {
                                    newProject->upgradeProjectData();
                                }).get());

        gtDataModel->newProject(newProject.release(), false);

        return true;
    }
}

GtProcessData*
GtProject::processData()
{
    return findDirectChild<GtProcessData*>(QStringLiteral("Process Data"));
}

GtProcessData const*
GtProject::processData() const
{
    return const_cast<GtProject*>(this)->processData();
}

GtLabelData*
GtProject::labelData()
{
    return findDirectChild<GtLabelData*>(QStringLiteral("Label Data"));
}

const GtLabelData*
GtProject::labelData() const
{
    return const_cast<GtProject*>(this)->labelData();
}

GtTask*
GtProject::findProcess(const QString& name)
{
    if (auto* pdata = processData())
    {
        return pdata->findProcess(name);
    }

    return nullptr;
}

const GtTask*
GtProject::findProcess(const QString& name) const
{
    return const_cast<GtProject*>(this)->findProcess(name);
}

QStringList
GtProject::taskIds() const
{
    auto const* pdata = processData();
    if (!pdata)
    {
        return {};
    }

    return gt::objectNames(pdata->currentProcessList());
}

QMap<QString, QStringList>
GtProject::fullTaskIds() const
{
    auto const* pdata = processData();
    if (!pdata)
    {
        return {};
    }

    QMap<QString, QStringList> retval;

    QStringList const groupIds = pdata->userGroupIds() +
                                 pdata->customGroupIds();

    for (QString const& group : groupIds)
    {
        retval.insert(group, gt::objectNames(pdata->processList(group)));
    }

    return retval;
}

GtPackage*
GtProject::findPackage(const QString& mid)
{
    // class name of package
    const QString pkgId = gtApp ? gtApp->modulePackageId(mid) : QString{};
    if (pkgId.isEmpty()) return nullptr;

    auto childs = findDirectChildren<GtPackage*>();
    auto iter = std::find_if(std::begin(childs), std::end(childs),
                 [&pkgId](const GtPackage* pkg) {
        return pkg->metaObject()->className() == pkgId;
    });

    return iter != std::end(childs) ? *iter : nullptr;
}

const GtPackage*
GtProject::findPackage(const QString& mid) const
{
    return const_cast<GtProject*>(this)->findPackage(mid);
}

const QStringList&
GtProject::moduleIds() const
{
    if (!isOpen()) return pimpl->declaredModuleIds;

    QStringList ids = pimpl->unresolvedModuleIds;
    for (const auto* package : findDirectChildren<GtPackage*>())
    {
        const auto mid = packageModuleId(*package);
        if (!mid.isEmpty() && !ids.contains(mid)) ids.append(mid);
    }
    if (pimpl->moduleIdsSnapshot != ids)
        pimpl->moduleIdsSnapshot = std::move(ids);
    return pimpl->moduleIdsSnapshot;
}

QString
GtProject::classModuleId(const QString& className) const
{
    return pimpl->classModuleIds.value(className);
}

int
GtProject::numberOfLabelUsages(GtLabel* label) const
{
    if (!label)
    {
        return -1;
    }

    return const_cast<GtProject*>(this)->findLabelUsages(label).size();
}

QList<GtLabel*>
GtProject::findLabelUsages(GtLabel* label)
{
    QList<GtLabel*> usages;

    if (!label)
    {
        return usages;
    }

    QList<GtPackage*> packages = findDirectChildren<GtPackage*>();

    foreach (GtPackage* package, packages)
    {
        usages.append(findLabelUsagesHelper(package, label));
    }

    return usages;
}

GtObjectMemento
GtProject::toProjectDataMemento()
{
    GtObjectGroup* group = new GtObjectGroup;
    group->setUuid(QStringLiteral("-"));

    QList<GtPackage*> packages = findDirectChildren<GtPackage*>();

    foreach (GtPackage* package, packages)
    {
        GtObject* obj = package->clone();

        if (obj)
        {
            group->appendChild(obj);
        }
    }

    GtObjectMemento memento = group->toMemento();

    delete group;

    return memento;
}

bool
GtProject::fromProjectDataMemento(GtObjectMemento& memento)
{
    if (memento.isNull())
    {
        return false;
    }

    auto group = memento.restore_unique<GtObjectGroup>(gtObjectFactory);

    if (!group)
    {
        return false;
    }

    QList<GtPackage*> packages = group->findDirectChildren<GtPackage*>();

    foreach (GtPackage* package, packages)
    {
        GtPackage* oldPkg = findDirectChild<GtPackage*>(package->objectName());

        if (!oldPkg)
        {
            return false;
        }

        oldPkg->fromMemento(package->toMemento());
    }

    return true;
}

bool
GtProject::renameProject(const QString& str)
{
    if (isOpen())
    {
        return false;
    }

    if (str.isEmpty())
    {
        return false;
    }

    if (str == objectName())
    {
        return false;
    }

    if (!gtApp->session())
    {
        return false;
    }

    if (gtApp->session()->projectIds().contains(str))
    {
        return false;
    }

    // rename project within project file
    QDomElement projectData = readProjectData(path()).documentElement();

    if (projectData.isNull())
    {
        return false;
    }

    projectData.setAttribute(QStringLiteral("projectname"), str);

    QDomDocument document;
    QDomProcessingInstruction header = document.createProcessingInstruction(
            QStringLiteral("xml"),
            QStringLiteral("version=\"1.0\" encoding=\"utf-8\""));
    document.appendChild(header);

    document.appendChild(projectData);

    QString filename = pimpl->path + QDir::separator() + mainFilename();

    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gtWarning() << objectName() << QStringLiteral(": ")
                    << tr("Failed to save project data!");
        return false;
    }

    QTextStream stream(&file);
    stream << document.toString(5);

    file.close();

    setObjectName(str);

    acceptChanges();

    return true;
}

QString
GtProject::readFootprint() const
{
    QString filename = pimpl->path + QDir::separator() + mainFilename();

    QFile file(filename);

    if (!file.exists())
    {
        qWarning() << "WARNING: file does not exists!";
        qWarning() << " |-> " << filename;

        return QString();
    }

    QDomDocument document;

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!gt::xml::readDomDocumentFromFile(file, document, true, &errorStr,
                                          &errorLine, &errorColumn))
    {
        gtDebug() << tr("XML ERROR!") << " " << tr("line") << ": "
                  << errorLine << " " << tr("column") << ": "
                  << errorColumn << " -> " << errorStr;

        return QString();
    }

    QDomElement root = document.documentElement();

    if (root.isNull() || (root.tagName() != QLatin1String("GTLAB")))
    {
        gtDebug() << "ERROR: Invalid GTlab project file!";
        return QString();
    }

    QString retval;

    QDomElement ftprnt =
            root.firstChildElement(QStringLiteral("env-footprint"));

    if (ftprnt.isNull())
    {
        gtDebug() << "Footprint not found in project file!";
        return QString();
    }

    QTextStream stream(&retval);
    ftprnt.save(stream, 4);

    return retval;
}

QString
gt::project::backUpMessageFileName()
{
    return QStringLiteral("GTlabBackUpMessage.md");
}

QString
gt::project::backupDirPath(const GtProject& proj)
{
    return {proj.path() + QDir::separator() + "backup"};
}
