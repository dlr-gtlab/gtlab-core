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

#endif // GT_GLOBALS_H
