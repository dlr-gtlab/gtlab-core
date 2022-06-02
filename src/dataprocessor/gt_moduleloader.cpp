/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 11.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_moduleloader.h"
#include "gt_moduleinterface.h"
#include "gt_initmoduleinterface.h"
#include "gt_datamodelinterface.h"
#include "gt_objectfactory.h"
#include "gt_logging.h"
#include "gt_algorithms.h"
#include "gt_versionnumber.h"
#include "gt_moduleupdater.h"

#include <QDebug>
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

void
register_converter(const QString& modId, GtVersionNumber target,
                   ConverterFunction func)
{
    GtModuleUpdater::instance().registerModuleConverter(modId, target, func);
}

GtModuleLoader::GtModuleLoader() :
    m_modulesInitialized(false)
{
}

GtModuleLoader::~GtModuleLoader()
{

}

void
GtModuleLoader::load()
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

    if (modulesDir.exists())
    {
        // initialize module blacklist
        QFile excludeFile(qApp->applicationDirPath() + QDir::separator() +
                          QStringLiteral("_exclude.json"));

        QStringList excludeList;

        if (excludeFile.exists())
        {
            gtDebug() << "module exclude file found!";

            if (excludeFile.open(QIODevice::ReadOnly))
            {
                QByteArray dat = excludeFile.readAll();
                QJsonDocument doc(QJsonDocument::fromJson(dat));

                QJsonObject json = doc.object();

                excludeFile.close();

                QVariantList exModList =
                        json.value(QStringLiteral("modules")
                                   ).toArray().toVariantList();

                foreach (const QVariant& exMod, exModList)
                {
                    QVariantMap modItem = exMod.toMap();
                    const QString name =
                            modItem.value(QStringLiteral("id")).toString();

                    excludeList << name;

                    gtWarning() << "excluding " << name << " module!";
                }
            }
            else
            {
                qWarning() << "could not read module exclude file information!";
            }
        }

        QStringList entryList = modulesDir.entryList(QDir::Files);

        if (!loadHelper(entryList, modulesDir, excludeList))
        {
            gtWarning() << QObject::tr("Could not resolve plugin dependencies");

            foreach (const QString& entry, entryList)
            {
                gtWarning() << entry;
                debugDependencies(modulesDir.absoluteFilePath(entry));
            }
        }


    }
}

QMap<QString, QString>
GtModuleLoader::moduleEnvironmentVars()
{
    QMap<QString, QString> retval;

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

    if (modulesDir.exists())
    {
        QStringList entryList = modulesDir.entryList(QDir::Files);

        foreach (const QString& fileName, entryList)
        {
            // load plugin from entry
            QPluginLoader loader(modulesDir.absoluteFilePath(fileName));

            // get plugin meta data
            QJsonObject metaData = pluginMetaData(loader);

            // get sys_env_vars list
            QVariantList sys_vars = metaArray(metaData,
                                              QStringLiteral("sys_env_vars"));

            foreach (const QVariant& var, sys_vars)
            {
                QVariantMap mitem = var.toMap();

                const QString name =
                        mitem.value(QStringLiteral("name")).toString();
                const QString initVar =
                        mitem.value(QStringLiteral("init")).toString();

                if (!retval.contains(name))
                {
                    retval.insert(name, initVar);
                }
            }
        }
    }

    return retval;
}

QStringList
GtModuleLoader::moduleIds()
{
    return m_plugins.keys();
}

QStringList
GtModuleLoader::moduleDatamodelInterfaceIds()
{
    QStringList retval;
    for_each_key(m_plugins, [&](const QString& e)
    {
      GtDatamodelInterface* dmi =
              dynamic_cast<GtDatamodelInterface*>(m_plugins.value(e));

      if (dmi && dmi->standAlone())
      {
          retval << e;
      }
    });

    return retval;
}

GtVersionNumber
GtModuleLoader::moduleVersion(const QString& id)
{
    if (m_plugins.contains(id))
    {
        return m_plugins.value(id)->version();
    }

    return GtVersionNumber();
}

QString
GtModuleLoader::moduleDescription(const QString& id)
{
    if (m_plugins.contains(id))
    {
        return m_plugins.value(id)->description();
    }

    return QString();
}

QString
GtModuleLoader::modulePackageId(const QString& id)
{
    if (m_plugins.contains(id))
    {
        GtDatamodelInterface* dmi =
                dynamic_cast<GtDatamodelInterface*>(m_plugins.value(id));

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
    if (m_modulesInitialized)
    {
        gtWarning() << "Modules already initialized!";
        return;
    }

    for (auto const& value : qAsConst(m_plugins))
    {
        value->init();
    }

    m_modulesInitialized = true;
}

void
GtModuleLoader::debugModuleUpdater()
{
    GtModuleUpdater::instance().debugModuleConverter();
}

void
GtModuleLoader::updateModuleData(const QMap<QString,
                                 GtVersionNumber>& moduleFootprint,
                                 const QStringList& moduleData)
{
    GtModuleUpdater::instance().update(moduleFootprint, moduleData);
}

bool
GtModuleLoader::check(GtModuleInterface* plugin)
{
    if (m_plugins.contains(plugin->ident()))
    {
        return false;
    }

    GtDatamodelInterface* dmp = dynamic_cast<GtDatamodelInterface*>(plugin);

    // contains dynamic linked datamodel classes
    if (dmp)
    {
        if (!gtObjectFactory->invokable(dmp->package()))
        {
            return false;
        }
        if (gtObjectFactory->containsDuplicates(dmp->data()))
        {
            return false;
        }

        if (!gtObjectFactory->allInvokable(dmp->data()))
        {
            return false;
        }
    }

    return true;
}

void
GtModuleLoader::insert(GtModuleInterface* plugin)
{
    m_plugins.insert(plugin->ident(), plugin);

    GtDatamodelInterface* dmp = dynamic_cast<GtDatamodelInterface*>(plugin);

    // contains dynamic linked datamodel classes
    if (dmp)
    {
        gtObjectFactory->registerClasses(dmp->data());
        gtObjectFactory->registerClass(dmp->package());
    }
}

QJsonObject
GtModuleLoader::pluginMetaData(const QPluginLoader& loader)
{
    return loader.metaData().value(QStringLiteral("MetaData")).toObject();
}

QVariantList
GtModuleLoader::metaArray(const QJsonObject& metaData, const QString& id)
{
    return metaData.value(id).toArray().toVariantList();
}

bool
GtModuleLoader::loadHelper(QStringList& entries, const QDir& modulesDir,
                           const QStringList& excludeList)
{
    // check whether module entry list is empty
    if (entries.isEmpty())
    {
        // no more entries -> loading successful
        return true;
    }

    // get module entry list size
    const int noe = entries.size();

    // initialize loading fail log
    QString iniFileName = roamingPath() + QDir::separator() +
                          QStringLiteral("last_run.ini");
    QSettings settings(iniFileName, QSettings::IniFormat);
    QStringList crashed_mods =
            settings.value(QStringLiteral("loading_crashed")).toStringList();

    // loading procedure
    foreach (const QString& fileName, entries)
    {
        if (crashed_mods.contains(modulesDir.absoluteFilePath(fileName)))
        {
            gtWarning() << fileName <<
                           QObject::tr(" loading skipped (last run crash)");

            entries.removeOne(fileName);

            continue;
        }

        // load plugin from entry
        QPluginLoader loader(modulesDir.absoluteFilePath(fileName));

        // get plugin meta data
        QJsonObject metaData = pluginMetaData(loader);

        // get dependency list
        QVariantList deps = metaArray(metaData,
                                      QStringLiteral("dependencies"));

//        debugDependencies(modulesDir.absoluteFilePath(fileName));

        // check outstanding dependencies
        if (checkDependency(deps, fileName))
        {
            // store temporary module information in loading fail log
            crashed_mods << modulesDir.absoluteFilePath(fileName);
            settings.setValue(QStringLiteral("loading_crashed"), crashed_mods);
            settings.sync();

            // no unresolved dependencies found -> load plugin
            QObject* instance = loader.instance();

            // check plugin object
            if (instance)
            {
                gtDebug() << QObject::tr("loading ") << fileName << "...";

                GtModuleInterface* plugin =
                        qobject_cast<GtModuleInterface*>(instance);

                if (plugin && !excludeList.contains(plugin->ident()) &&
                        check(plugin))
                {
                    insert(plugin);
                }
                else
                {
                    delete plugin;
                }
            }
            else
            {
                // could not recreate plugin
                gtWarning() << loader.errorString();
            }

            // no application crash... clear loading fail log
            crashed_mods.removeLast();
            settings.setValue(QStringLiteral("loading_crashed"), crashed_mods);
            settings.sync();

            entries.removeOne(fileName);
        }

    }

    if (noe == entries.size())
    {
        return false;
    }

    if (loadHelper(entries, modulesDir, excludeList))
    {
        return true;
    }

    return loadHelper(entries, modulesDir, excludeList);
}

bool
GtModuleLoader::checkDependency(const QVariantList& deps,
                                const QString& moduleFileName)
{
    if (deps.isEmpty())
    {
        return true;
    }

    foreach (const QVariant& var, deps)
    {
        QVariantMap mitem = var.toMap();

        const QString name = mitem.value(QStringLiteral("name")).toString();
        const GtVersionNumber version(
                mitem.value(QStringLiteral("version")).toString());

        //        gtDebug() << "dep = " << name;

        // check dependency
        if (!m_plugins.contains(name))
        {
            return false;
        }

        // check version
        const GtVersionNumber depVersion = m_plugins.value(name)->version();

        if (depVersion < version)
        {
            gtError().noquote() << QObject::tr("Loading")
                    << moduleFileName + QStringLiteral(":");

            gtError().noquote()
                    << QObject::tr("Dependency -")
                    << name << QObject::tr("- is outdated! (needed: >=")
                    << version.toString() + QObject::tr("; current:")
                    << depVersion.toString() + QStringLiteral(")");
            return false;
        }
        else if (depVersion > version)
        {
            gtInfo().noquote()
                    << QObject::tr("Dependency -") << name
                    << QObject::tr("- has a newer version than the module")
                    << moduleFileName
                    << QObject::tr("requires. (needed: >=")
                    << version.toString() + QObject::tr("; current:")
                    << depVersion.toString() + QStringLiteral(")");
        }
    }

    return true;
}

void
GtModuleLoader::debugDependencies(const QString& path)
{
    QPluginLoader loader(path);

    QJsonObject metaData = pluginMetaData(loader);
    QVariantList deps = metaArray(metaData,
                                  QStringLiteral("dependencies"));

    gtWarning() << "#### " << path;

    foreach (const QVariant& var, deps)
    {
        QVariantMap mitem = var.toMap();
        gtWarning() << "   |-> " << mitem.value(QStringLiteral("name"));
        gtWarning() << "   |-> " << mitem.value(QStringLiteral("version"));
    }
}

QString
GtModuleLoader::roamingPath()
{
#ifdef _WIN32
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#endif
    return QStandardPaths::writableLocation(
               QStandardPaths::GenericConfigLocation);
}
