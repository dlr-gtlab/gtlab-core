/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 14.3.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GT_VERSION_H
#define GT_VERSION_H

#define GT_VERSION_MAJOR 2
#define GT_VERSION_MINOR 0
#define GT_VERSION_PATCH 8
#define GT_VERSION_PRE_RELEASE ""
#define GT_VERSION_BUILD ""

/*
   GT_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define GT_VERSION      GT_VERSION_CHECK(GT_VERSION_MAJOR, GT_VERSION_MINOR, GT_VERSION_PATCH)

/*
   can be used like #if (GT_VERSION >= GT_VERSION_CHECK(4, 4, 0))
*/
#define GT_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

#endif // GT_VERSION_H
