/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GT_MIGRATECOLLECTIONSDATA_H
#define GT_MIGRATECOLLECTIONSDATA_H

#include "gt_core_exports.h"

#include <QFileInfo>
#include <QString>

#include <optional>

class GtSettings;

namespace gt
{
namespace detail
{
namespace collectionmigration
{

enum class State
{
    Pending,
    Completed,
    Suppressed
};

/**
 * @brief Convert a migration state to its stored string.
 * @param state Migration state to convert.
 * @return The string used in application settings.
 */
GT_CORE_EXPORT QString stateToString(State state);

/**
 * @brief Read a migration state from its stored string.
 * @param state String stored in application settings.
 * @return The matching state, or State::Pending for an unrecognized value.
 */
GT_CORE_EXPORT State stringToState(const QString& state);

/**
 * @brief Read the collection migration state from application settings.
 * @param appSettings Settings to read.
 * @return The migration state, or State::Pending for an unrecognized value.
 */
GT_CORE_EXPORT State settingToState(const GtSettings& appSettings);

/**
 * @brief Resolve the location of the legacy Collections folder.
 * @return The absolute path, or std::nullopt if the installation directory
 * cannot be resolved. The folder may be missing.
 */
GT_CORE_EXPORT std::optional<QString> legacyPath();

/**
 * @brief Resolve the migration lock file in the application configuration directory.
 * @return The lock file path as QFileInfo, or std::nullopt if the configuration
 * directory is unavailable.
 */
GT_CORE_EXPORT const std::optional<QFileInfo> lockFile();

/**
 * @brief Resolve a migration source directory without following a source link.
 * @param sourcePath Path to the source directory.
 * @return The canonical path, or an empty string if the source is missing,
 * is not a directory, or resolves outside its expected location.
 */
GT_CORE_EXPORT QString canonicalMigrationSource(const QString& sourcePath);

/**
 * @brief Check whether a migration destination is outside the source directory.
 * @param sourcePath Legacy Collections directory.
 * @param destinationPath Destination directory, which may be missing.
 * @return True if the source is a valid migration directory and the destination
 * resolves outside it.
 */
GT_CORE_EXPORT bool isMigrationDestinationSafe(const QString& sourcePath,
                                               const QString& destinationPath);

/**
 * @brief Copy legacy collections under an exclusive lock.
 * @param migratedDestination If provided, receives the destination on success.
 * @return True if the copy succeeds.
 */
GT_CORE_EXPORT bool runMigration(QString* migratedDestination = nullptr);

} // namespace collectionmigration
} // namespace detail
} // namespace gt

#endif // GT_MIGRATECOLLECTIONSDATA_H
