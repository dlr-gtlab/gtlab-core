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

/// Source and destination paths for an eligible collection migration.
struct CollectionMigrationPaths
{
    QString oldPath;
    QString newPath;
};

/**
 * @brief Check whether legacy collections are eligible for migration.
 *
 * Migration is offered when the installation's legacy Collections directory
 * contains collection subdirectories and the configured destination is
 * missing or empty. This check leaves the destination uncreated.
 *
 * @return The source and destination paths, or std::nullopt if migration
 *         is inapplicable.
 */
GT_CORE_EXPORT std::optional<CollectionMigrationPaths> checkCollectionMigration();

/**
 * @brief Copy the legacy collection tree to a missing or empty destination.
 *
 * The copy is staged beside the destination before publication. The source
 * data is preserved, and existing destination contents are protected.
 *
 * @param oldPath Path to the legacy Collections directory.
 * @param newPath Path to the configured Collections directory.
 * @return true if the copy was placed at the destination, false on failure
 *         or if the destination already contains data.
 */
GT_CORE_EXPORT bool migrateCollectionData(const QString& oldPath, const QString& newPath);

} // namespace detail
} // namespace gt

#endif // GT_MIGRATECOLLECTIONSDATA_H
