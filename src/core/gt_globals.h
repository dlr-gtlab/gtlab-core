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

#define GT_VERSION_MAJOR 2
#define GT_VERSION_MINOR 0
#define GT_VERSION_PATCH 0
#define GT_VERSION_PRE_RELEASE "alpha.1"
#define GT_VERSION_BUILD "1"
#define GT_MAINTENANCETOOL "MaintenanceTool"

/*
   GT_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define GT_VERSION      GT_VERSION_CHECK(GT_VERSION_MAJOR, GT_VERSION_MINOR, GT_VERSION_PATCH)

/*
   can be used like #if (GT_VERSION >= GT_VERSION_CHECK(4, 4, 0))
*/
#define GT_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

#ifndef GT_MODULE_ID
#error GT_MODULE_ID must be defined globally when using GTlab Core Libraries. \
Please define the macro using -DGT_MODULE_ID =<module_id>
#endif

#define GT_MODULENAME() GT_MODULE_ID

#endif // GT_GLOBALS_H
