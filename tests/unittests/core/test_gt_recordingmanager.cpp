/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 12.10.2026
 */

#include "gtest/gtest.h"

#include <QCoreApplication>
#include <atomic>
#include <cstddef>
#include <memory>
#include <thread>

#include "gt_coreapplication.h"
#include "gt_inmemoryrecorder.h"
#include "gt_recording.h"
#include "gt_recordingmanager.h"

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

/// Backend that throws on every delivery
class ThrowingRecorder final : public GtAbstractRecorder
{
public:
    std::atomic<int> recordCalls{0};

    void record(const GtRecording& recording) override
    {
        ++recordCalls;
        throw std::runtime_error("backend failure");
    }
};

/// Backend that reports its own destruction
class FlagRecorder final : public GtAbstractRecorder
{
public:
    std::atomic<int> recordCalls{0};
    std::atomic<bool> destroyed{false};

    ~FlagRecorder() override
    {
        destroyed = true;
    }

    void record(const GtRecording& recording) override
    {
        ++recordCalls;
    }
};

class TestRecordingApplication final : public GtCoreApplication
{
public:
    TestRecordingApplication() :
        GtCoreApplication(QCoreApplication::instance(), AppMode::Batch)
    {
        init();
    }

protected:
    bool initFirstRun() override
    {
        return true;
    }
};

class TestRecordingManagerApplication : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // defensive: no explicit single backend may shadow the manager
        gt::setAccessRecorder(nullptr);

        m_application = std::make_unique<TestRecordingApplication>();
    }

    void TearDown() override
    {
        m_application.reset();
        gt::setAccessRecorder(nullptr);
    }

    std::unique_ptr<TestRecordingApplication> m_application;
};

}

TEST(GtRecordingManager, emptyManagerIsNoOp)
{
    GtRecordingManager manager;

    EXPECT_EQ(manager.recorderCount(), std::size_t(0));
    EXPECT_FALSE(manager.isEnabled());

    GtRecording recording = makeFinishedRecording(1, "component", GtProcessComponent::FINISHED);
    manager.deliver(recording); // no backends: nothing happens, no crash
}

TEST(GtRecordingManager, addRecorderIgnoresNullptr)
{
    GtRecordingManager manager;

    manager.addRecorder(nullptr);

    EXPECT_EQ(manager.recorderCount(), std::size_t(0));
    EXPECT_FALSE(manager.isEnabled());
}

TEST(GtRecordingManager, deliverDispatchesToAllRegisteredBackends)
{
    GtRecordingManager manager;

    auto first = std::make_unique<GtInMemoryRecorder>();
    auto second = std::make_unique<GtInMemoryRecorder>();
    GtInMemoryRecorder* firstRecorder = first.get();
    GtInMemoryRecorder* secondRecorder = second.get();
    manager.addRecorder(std::move(first));
    manager.addRecorder(std::move(second));

    EXPECT_EQ(manager.recorderCount(), std::size_t(2));
    EXPECT_TRUE(manager.isEnabled());

    GtRecording recording = makeFinishedRecording(7, "component", GtProcessComponent::WARN_FINISHED, /*accessedValue=*/8);
    manager.deliver(recording);

    EXPECT_EQ(firstRecorder->count(), 1u);
    EXPECT_EQ(secondRecorder->count(), 1u);

    const QVector<GtRecordingSnapshot> snapshots = firstRecorder->recordings();
    ASSERT_EQ(snapshots.size(), 1u);

    const GtRecordingSnapshot& snapshot = snapshots[0];
    EXPECT_EQ(snapshot.id, recording.id());
    EXPECT_EQ(snapshot.componentUuid, makeUuid(7));
    EXPECT_EQ(snapshot.componentClassName, "Component");
    EXPECT_EQ(snapshot.componentName, "component");
    EXPECT_EQ(snapshot.outcome, GtProcessComponent::WARN_FINISHED);
    EXPECT_TRUE(snapshot.accessedObjects.contains(makeUuid(8)));
}

TEST(GtRecordingManager, backendFailureDoesNotStopRemainingBackends)
{
    GtRecordingManager manager;

    auto failing = std::make_unique<ThrowingRecorder>();
    auto working = std::make_unique<GtInMemoryRecorder>();
    ThrowingRecorder* failingPtr = failing.get();
    GtInMemoryRecorder* workingPtr = working.get();
    manager.addRecorder(std::move(failing));
    manager.addRecorder(std::move(working));

    GtRecording recording = makeFinishedRecording(3, "component", GtProcessComponent::FINISHED);
    manager.deliver(recording);

    EXPECT_EQ(failingPtr->recordCalls.load(), 1);
    EXPECT_EQ(workingPtr->count(), 1u);
}

TEST(GtRecordingManager, backendFailuresNeverPropagate)
{
    GtRecordingManager manager;
    manager.addRecorder(std::make_unique<ThrowingRecorder>());
    manager.addRecorder(std::make_unique<ThrowingRecorder>());

    GtRecording recording = makeFinishedRecording(4, "component", GtProcessComponent::FAILED);

    EXPECT_NO_THROW(manager.deliver(recording));
}

TEST(GtRecordingManager, ownsBackendsAndDestroysThemWithoutCallback)
{
    auto recorder = std::make_unique<FlagRecorder>();
    FlagRecorder* raw = recorder.get();

    GtRecording recording = makeFinishedRecording(5, "component", GtProcessComponent::FINISHED);

    {
        GtRecordingManager manager;
        manager.addRecorder(std::move(recorder));
        manager.deliver(recording);

        EXPECT_EQ(raw->recordCalls.load(), 1);
        EXPECT_FALSE(raw->destroyed.load());
    } // manager destroyed here

    EXPECT_TRUE(raw->destroyed.load());
    EXPECT_EQ(raw->recordCalls.load(), 1); // no extra callback from the destructor
}

TEST(GtRecordingManager, concurrentDeliveriesReachAllBackends)
{
    GtRecordingManager manager;

    auto first = std::make_unique<GtInMemoryRecorder>();
    auto second = std::make_unique<GtInMemoryRecorder>();
    GtInMemoryRecorder* firstRecorder = first.get();
    GtInMemoryRecorder* secondRecorder = second.get();
    manager.addRecorder(std::move(first));
    manager.addRecorder(std::move(second));

    const int kDeliveriesPerThread = 250;

    auto worker = [&manager]()
    {
        for (int i = 0; i < kDeliveriesPerThread; ++i)
        {
            GtRecording recording = makeFinishedRecording(quint64(i), "worker", GtProcessComponent::FINISHED);
            manager.deliver(recording);
        }
    };

    std::thread firstThread(worker);
    std::thread secondThread(worker);
    firstThread.join();
    secondThread.join();

    EXPECT_EQ(firstRecorder->count(), std::size_t(2 * kDeliveriesPerThread));
    EXPECT_EQ(secondRecorder->count(), std::size_t(2 * kDeliveriesPerThread));
}

TEST_F(TestRecordingManagerApplication, managerBackendsMakeRecordingEnabled)
{
    // no backend registered yet
    EXPECT_FALSE(gt::isAccessRecordingEnabled());

    m_application->recordingManager()->addRecorder(std::make_unique<GtInMemoryRecorder>());

    // the manager's backends are visible through the gt:: seam
    EXPECT_TRUE(gt::isAccessRecordingEnabled());
    EXPECT_EQ(m_application->recordingManager()->recorderCount(), 1u);
}

TEST_F(TestRecordingManagerApplication, deliverRecordingRoutesToApplicationManager)
{
    auto backend = std::make_unique<GtInMemoryRecorder>();
    GtInMemoryRecorder* recorder = backend.get();
    m_application->recordingManager()->addRecorder(std::move(backend));

    GtRecording recording = makeFinishedRecording(11, "component", GtProcessComponent::FINISHED, /*accessedValue=*/12);
    gt::deliverRecording(recording);

    EXPECT_EQ(recorder->count(), 1u);

    const QVector<GtRecordingSnapshot> snapshots = recorder->recordings();
    ASSERT_EQ(snapshots.size(), 1u);

    const GtRecordingSnapshot& snapshot = snapshots[0];
    EXPECT_EQ(snapshot.id, recording.id());
    EXPECT_EQ(snapshot.componentName, "component");
    EXPECT_EQ(snapshot.outcome, GtProcessComponent::FINISHED);
    EXPECT_TRUE(snapshot.accessedObjects.contains(makeUuid(12)));
}

TEST_F(TestRecordingManagerApplication, explicitSingleBackendWinsOverApplicationManager)
{
    auto backend = std::make_unique<GtInMemoryRecorder>();
    GtInMemoryRecorder* managerRecorder = backend.get();
    m_application->recordingManager()->addRecorder(std::move(backend));

    GtInMemoryRecorder explicitBackend;
    gt::setAccessRecorder(&explicitBackend);

    GtRecording first = makeFinishedRecording(21, "component", GtProcessComponent::FINISHED);
    gt::deliverRecording(first);
    EXPECT_EQ(explicitBackend.count(), 1u);
    EXPECT_EQ(managerRecorder->count(), 0u);

    // fall back to the application's manager again
    gt::setAccessRecorder(nullptr);

    GtRecording second = makeFinishedRecording(22, "component", GtProcessComponent::WARN_FINISHED);
    gt::deliverRecording(second);
    EXPECT_EQ(explicitBackend.count(), 1u);
    EXPECT_EQ(managerRecorder->count(), 1u);
}
