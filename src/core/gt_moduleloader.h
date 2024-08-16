/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMODULELOADER_H
#define GTMODULELOADER_H

#include "gt_core_exports.h"

#include <QMap>
#include <QVariantList>

#include <memory>

#include <gt_logging.h>
#include <gt_globals.h>

class QStringList;
class QJsonObject;
class QPluginLoader;
class QDir;
class GtModuleInterface;
class GtVersionNumber;

/**
 * @brief The GtModuleLoader class
 */
class GT_CORE_EXPORT GtModuleLoader
{

public:
    /**
     * @brief GtModuleLoader
     */
    GtModuleLoader();

    /**
     * @brief ~GtModuleLoader
     */
    virtual ~GtModuleLoader();

    /**
     * @brief loads all modules
     */
    void load();

    /**
     * @brief Try to load a single module.
     *
     * The module might be located outside of the modules directory.
     * The given module overrides any module in the module directory
     * with the same moduleId.
     *
     * @param moduleLocation The filename of the module
     * @return True, if loading was successful
     */
    bool loadSingleModule(const QString& moduleLocation);

    /**
     * @brief Returns list of all module environment variables.
     * @return List of all module environment variables.
     */
    static QMap<QString, QString> moduleEnvironmentVars();

    /**
     * @brief Returns identification strings of all loaded modules.
     * @return module identification strings
     */
    QStringList moduleIds() const;

    /**
     * @brief Returns identification strings of all loaded modules which
     * inherit the datamodel interface.
     * @return module identification strings
     */
    QStringList moduleDatamodelInterfaceIds() const;

    /**
     * @brief Returns version number of module for given id. Returns -1
     * for non existing modules.
     * @param module identification string
     * @return module version number
     */
    GtVersionNumber moduleVersion(const QString& id) const;

    /**
     * @brief Returns description of module for given id. Returns empty
     * string for non existing modules.
     * @param module identification string
     * @return module description
     */
    QString moduleDescription(const QString& id) const;

    /**
     * @brief Returns classname of package for a given module id. Returns empty
     * string for non existing modules.
     * @param module identification string
     * @return package classname
     */
    QString modulePackageId(const QString& id) const;

    /**
     * @brief Initializes all loaded modules.
     */
    void initModules();

    /**
     * @brief The function allows to suppress the loading of a module from
     *        another module (the suppressor).
     *
     * The typical use case is a conditional loading of module, e.g. only if
     * certain requirements are met before another module can be loaded.
     *
     * It adds the suppressorModuleId to the set of suppressors of the module
     * with the id suppressedModuleId.
     *
     * @param suppressorModuleId Module id of the module that suppresses
     * the module with the id suppressedModuleId.
     * @param suppressedModuleId Module id of the module that is suppressed
     * by the module with the id suppressorModuleId.
     */
    void addSuppression(const QString& suppressorModuleId,
        const QString& suppressedModuleId);

    /**
     * @brief Returns authors name of module for given id. Returns empty
     * string for non existing modules.
     * @param module identification string
     * @return module authors name
     */
    QString moduleAuthor(const QString& id) const;

    /**
     * @brief Returns authors contact of module for given id. Returns empty
     * string for non existing modules.
     * @param module identification string
     * @return module description
     */
    QString moduleContact(const QString& id) const;

    /**
     * @brief Returns licence information of module for given id. Returns empty
     * string for non existing modules.
     * @param module identification string
     * @return module licence information
     */
    QString moduleLicence(const QString& id) const;

    /**
     * @brief Used to find out for which interface version a module was built against
     *
     * Should be called on one type of interface at a time,
     * as it only returns one interface.
     *
     * @param pluginObj           The plugin object
     * @param listOfInterfaces    A list of know interface ids
     * @return
     */
    static QString getSupportedInterfaceByModule(
        QObject* pluginObj, const QStringList& listOfInterfaces);
protected:
    /**
     * @brief check
     * @param plugin
     * @return
     */
    virtual bool check(GtModuleInterface* plugin) const;

    /**
     * @brief insert
     * @param plugin
     */
    virtual void insert(GtModuleInterface* plugin);

private:
    /// Private implementation
    class Impl;
    std::unique_ptr<Impl> m_pimpl;
};


/**
 * @brief Helper function to check, whether a module support a specific interface
 * @param soName The name / filename of a module
 * @param plugin The pointer to a module
 * @return
 */
template<typename InterfaceType, typename Plugin>
inline InterfaceType* checkInterface(const QString& soName, Plugin* plugin)
{
    auto obj = dynamic_cast<QObject*>(plugin);
    if (!obj) return nullptr;

    InterfaceType* itf = qobject_cast<InterfaceType*>(obj);

    if (!itf && dynamic_cast<InterfaceType*>(obj))
    {
        // The interface ID does not match, but it is the same class.
        // Thus, we have a version mismatch

        // find out for which old version the module was compiled
        auto oldItf = GtModuleLoader::getSupportedInterfaceByModule(
            obj, gtGetOutdatedItfVersions<InterfaceType>());

        gtError() << QObject::tr("Version mismatch of module '%1' "
                                 "with GTlab interface '%2'. "
                                 "It was compiled against '%3'")
                         .arg(soName,
                              qobject_interface_iid<InterfaceType*>(),
                              oldItf);

    }

    return itf;
}

#endif // GTMODULELOADER_H
