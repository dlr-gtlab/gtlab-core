/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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

#endif // GTMODULELOADER_H
