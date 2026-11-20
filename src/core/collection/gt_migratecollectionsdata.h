/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GT_MIGRATECOLLECTIONSDATA_H
#define GT_MIGRATECOLLECTIONSDATA_H

#include "gt_core_exports.h"

#include <QString>

#include <optional>

namespace gt
{
namespace detail
{

struct CollectionMigrationPaths
{
    QString oldPath;
    QString newPath;
};

GT_CORE_EXPORT std::optional<CollectionMigrationPaths> checkCollectionMigration();
GT_CORE_EXPORT bool migrateCollectionData(const QString& oldPath, const QString& newPath);

} // namespace detail
} // namespace gt

#endif // GT_MIGRATECOLLECTIONSDATA_H
