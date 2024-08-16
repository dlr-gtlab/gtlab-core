/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_globals.h
 *
 *  Created on: 03.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_GLOBALS_H
#define GT_GLOBALS_H

#include "gt_version.h"

#include <QStringList>

#define GT_MAINTENANCETOOL "MaintenanceTool"

#ifndef GT_MODULE_ID
#error GT_MODULE_ID must be defined globally when using GTlab Core Libraries. \
Please define the macro using -DGT_MODULE_ID =<module_id>
#endif

#define GT_MODULENAME() GT_MODULE_ID

// Macro to mark a function as deprecated for removal in a specific version
#define deprecated_from(major, minor, message)                             \
[[deprecated("Will be removed in GTlab " \
    #major "." #minor ". " message)]] \

template <typename InterfaceType>
inline QStringList gtGetOutdatedItfVersions()
{
    return {};
}

/**
 * Defines previous interface versions for error tracking
 *
 * Usage:
 *
 * GT_OLD_INTERFACES(GtModuleInterface,
 *                  "de.dlr.gtlab.GtModuleInterface/1.7",
 *                  "de.dlr.gtlab.GtModuleInterface/2.0")
 */
#define GT_OLD_INTERFACES(TYPE, ...) template <> \
inline QStringList gtGetOutdatedItfVersions<TYPE>() {return {__VA_ARGS__};}

#endif // GT_GLOBALS_H
