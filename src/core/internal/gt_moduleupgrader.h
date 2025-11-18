/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 20.05.2022
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMODULEUPGRADER_H
#define GTMODULEUPGRADER_H

#include <memory>

#include "gt_moduleinterface.h"

namespace gt
{
namespace detail
{

/**
 * @brief This class is used to centralize the logic for upgrading project data
 * provided by specific module upgrade routines.
 */
class GtModuleUpgrader
{
public:
    /**
     * @brief Singleton.
     * @return Instance of module updater.
     */
    static GtModuleUpgrader& instance();

    /**
     * @brief Rigisters an upgrade routine for a specific module.
     * @param modId Module identification string.
     * @param target Target version of the module data, after using the upgrade function. 
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
     * @param projectName The name of the project to upgrade
     * @param moduleFootprint Footprint of all modules of the current dataset
     * that need to be upgraded.
     * @param files All files that contain datasets to be upgraded.
     */
    void upgrade(const QString& projectName,
                 const QMap<QString,
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
     * @param moduleIdent identifications string of the module for which
     * upgrades are to be checked.
     * @param savedVer Version of the module used to create the currently
     * saved dataset.
     * @return List of version numbers of upgrade routines.
     */
    QList<GtVersionNumber> availableUpgrades(const QString& moduleIdent,
                                         const GtVersionNumber& savedVer) const;

private:
    /// Private implementation
    class Impl;
    std::unique_ptr<Impl> m_pimpl;

    /**
     * @brief Private constructor.
     */
    GtModuleUpgrader();

};

} // namespace detail
} // namespace gt

#endif // GTMODULEUPGRADER_H
