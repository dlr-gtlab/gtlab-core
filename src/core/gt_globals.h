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
#define GT_VERSION_ADDITIONAL "dp6"
#define GT_MAINTENANCETOOL "MaintenanceTool"

/*
   GT_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define GT_VERSION      GT_VERSION_CHECK(GT_VERSION_MAJOR, GT_VERSION_MINOR, GT_VERSION_PATCH)

/*
   can be used like #if (GT_VERSION >= GT_VERSION_CHECK(4, 4, 0))
*/
#define GT_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))


#endif // GT_GLOBALS_H
