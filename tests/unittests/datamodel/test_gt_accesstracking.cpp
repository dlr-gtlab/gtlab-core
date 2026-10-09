/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 6.10.2026
 */

#include "gtest/gtest.h"

#include <memory>
#include <thread>
#include <vector>

#include "gt_accesstracking.h"

namespace
{

QUuid
makeUuid(quint64 value)
{
    // Qt 6 has no 5-argument QUuid constructor, so the uuid is built
    // from a canonical string representation with a numeric last block
    return QUuid(QStringLiteral("00000000-0000-0000-0000-%1")
                    .arg(QString::number(value, 16).rightJustified(12, QLatin1Char('0'))));
}

}

TEST(GtAccessTracker, freshTrackerIsEmpty)
{
    GtAccessTracker tracker;

    EXPECT_EQ(tracker.count(), 0u);
    EXPECT_TRUE(tracker.accessedObjects().isEmpty());
}

TEST(GtAccessTracker, addAccessedPropertyDeduplicates)
{
    GtAccessTracker tracker;
    const QUuid uuid = makeUuid(1);

    tracker.addAccessedProperty(uuid);
    tracker.addAccessedProperty(uuid);
    tracker.addAccessedProperty(uuid);

    EXPECT_EQ(tracker.count(), 1u);
    EXPECT_EQ(tracker.accessedObjects(), QSet<QUuid>{uuid});
}

TEST(GtAccessTracker, currentIsNullOnFreshThread)
{
    std::thread thread([]() {
        EXPECT_EQ(GtAccessTracker::current(), nullptr);
    });
    thread.join();
}

TEST(GtAccessTracker, scopeInstallsAndRestoresCurrentTracker)
{
    auto tracker = std::make_shared<GtAccessTracker>();

    EXPECT_EQ(GtAccessTracker::current(), nullptr);
    {
        GtAccessTrackerScope scope(tracker);
        EXPECT_EQ(GtAccessTracker::current(), tracker.get());
    }
    EXPECT_EQ(GtAccessTracker::current(), nullptr);
}

TEST(GtAccessTracker, nestedScopesRestoreOuterTracker)
{
    auto outer = std::make_shared<GtAccessTracker>();
    auto inner = std::make_shared<GtAccessTracker>();

    {
        GtAccessTrackerScope outerScope(outer);
        EXPECT_EQ(GtAccessTracker::current(), outer.get());
        {
            GtAccessTrackerScope innerScope(inner);
            EXPECT_EQ(GtAccessTracker::current(), inner.get());
            inner->addAccessedProperty(makeUuid(42));
        }
        EXPECT_EQ(GtAccessTracker::current(), outer.get());
    }
    EXPECT_EQ(GtAccessTracker::current(), nullptr);

    // Writes in the inner scope must have reached only the inner tracker
    EXPECT_EQ(outer->count(), 0u);
    EXPECT_EQ(inner->count(), 1u);
    EXPECT_TRUE(inner->accessedObjects().contains(makeUuid(42)));
}

TEST(GtAccessTracker, nullScopeIsNoOp)
{
    auto tracker = std::make_shared<GtAccessTracker>();

    {
        GtAccessTrackerScope scope(tracker);
        EXPECT_EQ(GtAccessTracker::current(), tracker.get());
    }
    {
        GtAccessTrackerScope scope(nullptr);
        EXPECT_EQ(GtAccessTracker::current(), nullptr);
    }
    EXPECT_EQ(GtAccessTracker::current(), nullptr);
}

TEST(GtAccessTracker, concurrentWritesAreSafe)
{
    auto tracker = std::make_shared<GtAccessTracker>();
    constexpr std::size_t numThreads = 8;
    constexpr quint64 valuesPerThread = 100000;

    // Thread i adds the uuids [i*50000, (i+2)*50000),
    // the union is [0, 450000)
    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    for (std::size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([tracker, i]() {
            const quint64 begin = quint64(i) * valuesPerThread / 2;
            const quint64 end = quint64(i + 2) * valuesPerThread / 2;
            for (quint64 value = begin; value < end; ++value)
            {
                tracker->addAccessedProperty(makeUuid(value));
            }
        });
    }
    for (auto& thread : threads)
    {
        thread.join();
    }

    EXPECT_EQ(tracker->count(), 450000u);
    EXPECT_EQ(tracker->accessedObjects().size(), 450000u);
}

TEST(GtAccessTracker, parallelExecutionsAreIsolated)
{
    // Simulates two concurrent execution contexts (two trackers),
    // each written by four threads partitioning a 200000 wide uuid window
    constexpr std::size_t numTrackers = 2;
    constexpr std::size_t threadsPerTracker = 4;
    constexpr quint64 valuesPerThread = 50000;
    constexpr quint64 windowSize = 200000;
    constexpr quint64 windowSpacing = 400000;

    std::vector<std::shared_ptr<GtAccessTracker>> trackers(numTrackers);
    std::vector<std::thread> threads;
    for (std::size_t t = 0; t < numTrackers; ++t)
    {
        trackers[t] = std::make_shared<GtAccessTracker>();
        for (std::size_t i = 0; i < threadsPerTracker; ++i)
        {
            threads.emplace_back([tracker = trackers[t], t, i]() {
                const quint64 begin = quint64(t) * windowSpacing
                        + quint64(i) * valuesPerThread;
                for (quint64 value = begin; value < begin + valuesPerThread; ++value)
                {
                    tracker->addAccessedProperty(makeUuid(value));
                }
            });
        }
    }
    for (auto& thread : threads)
    {
        thread.join();
    }

    // 200000 unique uuids per tracker
    for (std::size_t t = 0; t < numTrackers; ++t)
    {
        EXPECT_EQ(trackers[t]->count(), windowSize);
    }

    // No cross context contamination
    QSet<QUuid> intersection = trackers[0]->accessedObjects();
    intersection.intersect(trackers[1]->accessedObjects());
    EXPECT_TRUE(intersection.isEmpty());
}
