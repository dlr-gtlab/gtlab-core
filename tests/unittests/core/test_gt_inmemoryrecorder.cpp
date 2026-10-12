/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 12.10.2026
 */

#include "gtest/gtest.h"

#include <QSet>
#include <QUuid>
#include <cstddef>
#include <memory>
#include <thread>
#include <vector>

#include "gt_inmemoryrecorder.h"
#include "gt_recording.h"

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

GtRecording
makeFinishedRecording(quint64 componentValue, const QString& name, GtProcessComponent::STATE outcome, quint64 accessedValue = 0)
{
    GtRecording recording;
    recording.setComponentInfo(makeUuid(componentValue), "Component", name);
    if (accessedValue != 0)
    {
        recording.setTracker(std::make_unique<GtAccessTracker>());
        recording.tracker()->addAccessedProperty(makeUuid(accessedValue));
    }
    recording.start();
    recording.finish(outcome);
    return recording;
}

}

class TestInMemoryRecorder : public ::testing::Test
{
protected:
    void
    TearDown() override
    {
        // defensive: never leave a test recorder registered globally
        gt::setAccessRecorder(nullptr);
    }
};

TEST_F(TestInMemoryRecorder, collectsDeliveredRecordings)
{
    GtInMemoryRecorder recorder;
    gt::setAccessRecorder(&recorder);

    GtRecording first = makeFinishedRecording(1, "outer", GtProcessComponent::FINISHED, /*accessedValue=*/2);
    gt::deliverRecording(first);

    GtRecording second = makeFinishedRecording(3, "inner", GtProcessComponent::WARN_FINISHED);
    second.setParentId(first.id());
    gt::deliverRecording(second);

    gt::setAccessRecorder(nullptr);

    ASSERT_EQ(recorder.count(), 2u);

    const QVector<GtRecordingSnapshot> snapshots = recorder.recordings();
    ASSERT_EQ(snapshots.size(), 2);

    const auto& a = snapshots[0];
    EXPECT_EQ(a.id, first.id());
    EXPECT_TRUE(a.parentId.isNull());
    EXPECT_EQ(a.componentUuid, makeUuid(1));
    EXPECT_EQ(a.componentClassName, "Component");
    EXPECT_EQ(a.componentName, "outer");
    EXPECT_EQ(a.outcome, GtProcessComponent::FINISHED);
    EXPECT_TRUE(a.startTime.isValid());
    EXPECT_TRUE(a.finishTime.isValid());
    EXPECT_LE(a.startTime, a.finishTime);
    // Qt 6's QSet has no initializer-list constructor
    QSet<QUuid> expectedAccessed;
    expectedAccessed.insert(makeUuid(2));
    EXPECT_EQ(a.accessedObjects, expectedAccessed);

    const auto& b = snapshots[1];
    EXPECT_EQ(b.id, second.id());
    EXPECT_EQ(b.parentId, first.id());
    EXPECT_EQ(b.componentName, "inner");
    EXPECT_EQ(b.outcome, GtProcessComponent::WARN_FINISHED);
    EXPECT_TRUE(b.accessedObjects.isEmpty());
}

TEST_F(TestInMemoryRecorder, clearRemovesRecordings)
{
    GtInMemoryRecorder recorder;

    for (quint64 i = 0; i < 3; ++i)
    {
        recorder.record(makeFinishedRecording(100 + i, "c", GtProcessComponent::FINISHED));
    }

    ASSERT_EQ(recorder.count(), 3u);

    recorder.clear();

    EXPECT_EQ(recorder.count(), 0u);
    EXPECT_TRUE(recorder.recordings().isEmpty());
}

TEST_F(TestInMemoryRecorder, repeatedExecutionsOfSameComponentAreSeparateEntries)
{
    GtInMemoryRecorder recorder;

    // two executions of the same component: identical component info,
    // two distinct recordings
    for (int run = 0; run < 2; ++run)
    {
        recorder.record(makeFinishedRecording(7, "calc", GtProcessComponent::FINISHED));
    }

    const QVector<GtRecordingSnapshot> snapshots = recorder.recordings();
    ASSERT_EQ(snapshots.size(), 2);

    EXPECT_EQ(snapshots[0].componentUuid, snapshots[1].componentUuid);
    EXPECT_NE(snapshots[0].id, snapshots[1].id);
}

TEST_F(TestInMemoryRecorder, collectsConcurrentDeliveries)
{
    GtInMemoryRecorder recorder;

    constexpr int threadCount = 2;
    constexpr int perThread = 5000;

    std::vector<std::thread> workers;
    workers.reserve(threadCount);
    for (int t = 0; t < threadCount; ++t)
    {
        workers.emplace_back([&recorder, t]()
        {
            for (int i = 0; i < perThread; ++i)
            {
                const quint64 componentValue = quint64(t) * perThread + i + 1;
                recorder.record(makeFinishedRecording(componentValue, "worker", GtProcessComponent::FINISHED));
            }
        });
    }
    for (auto& worker : workers)
    {
        worker.join();
    }

    EXPECT_EQ(recorder.count(), static_cast<std::size_t>(threadCount * perThread));

    // every delivery landed exactly once
    QSet<QUuid> seen;
    int duplicates = 0;
    for (const auto& snapshot : recorder.recordings())
    {
        // Qt 6's QSet::insert returns an iterator, so a size check detects
        // a duplicate
        const std::size_t before = seen.size();
        seen.insert(snapshot.componentUuid);
        if (seen.size() == before)
        {
            ++duplicates;
        }
    }
    EXPECT_EQ(duplicates, 0);
    EXPECT_EQ(seen.size(), static_cast<std::size_t>(threadCount * perThread));
}
