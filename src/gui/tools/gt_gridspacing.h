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

/// Defines the actual spacing of a grid
struct GtGridSpacing
{
    /// distance or spacing between vertical lines (intersecting the x-axis)
    double hSpacing = 0.0;
    /// distance or spacing between horizontal lines (intersecting the y-axis)
    double vSpacing = 0.0;
};

#endif // GT_GRIDSPACING_H
