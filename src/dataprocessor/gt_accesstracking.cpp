/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_runnable.cpp
 *
 *  Created on: 19.08.2026
 *  Author: Robert Marcenko (AT-TWK)
 */
#include "gt_accesstracking.h"

#include <QHash>


namespace
{

GtAccessTracker*& currentTracker()
{
    thread_local GtAccessTracker* tracker = nullptr;
    return tracker;
}

}

void GtAccessTracker::addAccessedProperty(const QUuid& uuid)
{
    const std::size_t shardIndex = qHash(uuid) % NumShards;
    QMutexLocker locker(&m_shards[shardIndex].mutex);
    m_shards[shardIndex].uuids.insert(uuid);
}

QSet<QUuid> GtAccessTracker::accessedObjects() const
{
    QSet<QUuid> result;
    for (const auto& shard : m_shards)
    {
        QMutexLocker locker(&shard.mutex);
        result.unite(shard.uuids);
    }
    return result;
}

std::size_t GtAccessTracker::count() const
{
    std::size_t total = 0;
    for (const auto& shard : m_shards)
    {
        QMutexLocker locker(&shard.mutex);
        total += shard.uuids.size();
    }
    return total;
}

GtAccessTracker* GtAccessTracker::current()
{
    return currentTracker();
}

GtAccessTrackerScope::GtAccessTrackerScope(std::shared_ptr<GtAccessTracker> tracker)
{
    if (tracker)
    {
        m_active = true;
        m_previous = currentTracker();
        currentTracker() = tracker.get();
    }
}

GtAccessTrackerScope::~GtAccessTrackerScope()
{
    if (m_active)
    {
        currentTracker() = m_previous;
    }
}
