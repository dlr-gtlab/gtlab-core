/* GTlab - Gas Turbine laboratory
 * Source File: gt_globals.h
 * copyright 2009-2020 by DLR
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

#endif // GT_GLOBALS_H
