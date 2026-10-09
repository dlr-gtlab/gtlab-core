/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_runnable.cpp
 *
 *  Created on: 19.08.2026
 *  Author: Robert Marcenko (AT-TWK)
 */
#pragma once

#include "gt_datamodel_exports.h"

#include <QMutex>
#include <QSet>
#include <quuid.h>

#include <array>
#include <cstddef>
#include <memory>


class GT_DATAMODEL_EXPORT GtAccessTracker
{
public:
    /// Number of lock-protected shards
    static constexpr std::size_t NumShards=256;

    GtAccessTracker(const GtAccessTracker &) = delete;
    GtAccessTracker & operator = (const GtAccessTracker &) = delete;

    /// Adds a uuid to the shard derived from it
    void
    addAccessedProperty(const QUuid& uuid);
    /// Merges all shards and returns the union of the recorded uuids
    QSet<QUuid>
    accessedObjects() const;
    /// Total number of recorded uuids
    std::size_t
    count() const;
    /// The tracker active on the current thread, or nullptr if none is active
    static GtAccessTracker*
    current();

    GtAccessTracker() = default;
    ~GtAccessTracker() = default;

private:
    struct Shard
    {
        mutable QMutex mutex;
        QSet<QUuid> uuids;
    };

    std::array<Shard, NumShards> m_shards;
};


class GT_DATAMODEL_EXPORT GtAccessTrackerScope
{
public:
    explicit GtAccessTrackerScope(std::shared_ptr<GtAccessTracker> tracker);
    ~GtAccessTrackerScope();
    GtAccessTrackerScope(const GtAccessTrackerScope &) = delete;
    GtAccessTrackerScope & operator = (const GtAccessTrackerScope &) = delete;
    GtAccessTrackerScope(GtAccessTrackerScope &&) = delete;
    GtAccessTrackerScope & operator =(GtAccessTrackerScope &&) = delete;

private:
    GtAccessTracker* m_previous{nullptr};
    bool m_active{false};
};
