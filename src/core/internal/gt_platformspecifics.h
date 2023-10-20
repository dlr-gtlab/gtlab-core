/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */


#ifndef GTPLATFORMSPECIFICS_H
#define GTPLATFORMSPECIFICS_H

#include "gt_core_exports.h"

#include <string>

namespace gt
{
namespace detail
{

GT_CORE_EXPORT std::string systemUsername();

} // namespace detail
} // namespace gt

#endif // GTPLATFORMSPECIFICS_H
