/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 20.05.2022
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMODULEUPDATER_H
#define GTMODULEUPDATER_H

#include <memory>

#include "gt_moduleinterface.h"

class GtModuleUpdaterImpl;

/**
 * @brief This class is used to centralize the logic for upgrading project data
 * provided by specific module upgrade routines.
 */
class GtModuleUpdater
{
public:
    /**
     * @brief Singleton.
     * @return Instance of module updater.
     */
    static GtModuleUpdater& instance();

    /**
     * @brief Rigisters an upgrade routine for a specific module.
     * @param modId Module identification string.
     * @param target Target version number of module.
     * @param func Upgrad function.
     */
    void registerModuleConverter(const QString& modId, GtVersionNumber target,
                                 ConverterFunction func);

    /**
     * @brief Debugs registered upgrade routines.
     */
    void debugModuleConverter();

    /**
     * @brief Performs an upgrade of the data.
     * @param moduleFootprint Footprint of all modules of the current dataset
     * that need to be upgraded.
     * @param files All files that contain datasets to be upgraded.
     */
    void update(const QMap<QString,
                GtVersionNumber>& moduleFootprint,
                const QStringList& files) const;

    /**
     * @brief Checks if upgrades are available for a given module footprint.
     * @param moduleFootprint Footprint of all modules of the current dataset
     * to be checked for upgrades.
     * @return True if upgrades are available.
     */
    bool upgradesAvailable(const QMap<QString,
                           GtVersionNumber>& moduleFootprint) const;

    /**
     * @brief Returns the identification strings of all modules for which at
     * least one upgrade is available.
     * @param moduleFootprint Footprint of all modules of the current dataset
     * to be checked for upgrades.
     * @return List of module identification strings
     */
    QStringList availableModuleUpgrades(const QMap<QString,
                                       GtVersionNumber>& moduleFootprint) const;

    /**
     * @brief Gives a list of version numbers of all available upgrades of a
     * specific module.
     * @param moduleId identifications string of the module for which upgrades
     * are to be checked.
     * @param savedVer Version of the module used to create the currently
     * saved dataset.
     * @return List of version numbers of upgrad routines.
     */
    QList<GtVersionNumber> availableUpgrades(const QString& moduleId,
                                         const GtVersionNumber& savedVer) const;

private:
    /// Private implementation
    std::unique_ptr<GtModuleUpdaterImpl> m_pimpl;

    /**
     * @brief Private constructor.
     */
    GtModuleUpdater();

};

#endif // GTMODULEUPDATER_H
