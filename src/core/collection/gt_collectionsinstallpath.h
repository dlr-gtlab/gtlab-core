/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GT_COLLECTIONSINSTALLPATH_H
#define GT_COLLECTIONSINSTALLPATH_H

#include "gt_core_exports.h"

#include <QString>
#include <optional>

namespace gt
{
namespace detail
{

/**
 * @brief Retrieve the current collectionsInstallPath
 * @param mkpath If true, create the required directory path
 * @param ok Is true when path is valid, otherwise false
 * @return
 */
const QString GT_CORE_EXPORT collectionsInstallPath(bool mkpath = false,
                                                    bool* ok = nullptr);


/**
 * @brief Retrieve the default collectionsInstallPath
 * @return
 */
const QString GT_CORE_EXPORT collectionsInstallDefaultPath();
} // namespace detail

} // namespace gt


#endif // GT_COLLECTIONSINSTALLPATH_H
