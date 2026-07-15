/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: gt_gridspacing.h
 *
 *  Created on: 15.07.2026
 *  Author: Marius Bröcker (AT-TWK)
 */

#ifndef GT_GRIDSPACING_H
#define GT_GRIDSPACING_H

/// Defiens the actual spacing of a grid
struct GtGridSpacing
{
    double hSpacing = 0.0, vSpacing = 0.0;
};

#endif // GT_GRIDSPACING_H
