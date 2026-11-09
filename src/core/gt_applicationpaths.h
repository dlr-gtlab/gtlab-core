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
 * @brief Provides application-owned writable directories.
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
        Config,
        Data,
        Cache,
        State,
        Runtime,
        Temp
    };

    /**
     * @brief Returns a usable directory for the requested location.
     *
     * The directory is created if necessary. The Temp location is a unique
     * application-wide directory shared by calls in this process and removed
     * on normal process shutdown. On failure, std::nullopt is returned and
     * error is populated when provided.
     */
    static std::optional<QDir> directory(Location type,
                                         QString* error = nullptr);
};

#endif // GTAPPLICATIONPATHS_H
