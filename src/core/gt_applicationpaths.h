/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTAPPLICATIONPATHS_H
#define GTAPPLICATIONPATHS_H

#include "gt_core_exports.h"

#include <QDir>

#include <optional>

/**
 * @brief Resolves standard application directory locations.
 *
 * Installation paths are intended for reading installed resources only.
 * Project files and results remain relative to their project or an explicit
 * user-selected output path.
 */
class GT_CORE_EXPORT GtApplicationPaths
{
public:
    enum class Location
    {
        /**
         * User-specific application settings; intended to persist.
         */
        Config,
        /**
         * Persistent user-owned application data, including downloaded data.
         */
        Data,
        /**
         * Regenerable data that may be removed to reclaim space.
         */
        Cache,
        /**
         * Persistent runtime state, including per-installation crash markers.
         */
        State,
        /**
         * Shared runtime coordination files such as process locks.
         */
        Runtime,
        /**
         * A unique application-wide temporary directory owned by this process.
         */
        Temp
    };

    /**
     * @brief Returns a directory for the requested location.
     *
     * The directory is created if necessary. A returned directory does not
     * guarantee that later file operations will succeed; callers must handle
     * those errors. The Temp location is unique per process, shared by calls
     * in that process, and removed on normal shutdown. It usually remains after
     * abnormal termination and is not cleaned up automatically on later runs.
     * On failure, std::nullopt is returned and error is populated when given.
     */
    static std::optional<QDir> directory(Location type,
                                         QString* error = nullptr);
};

#endif // GTAPPLICATIONPATHS_H
