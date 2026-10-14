/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "test_gt_recording.h"

#include <stdexcept>
#include <thread>

namespace
{

QList<const GtObject*>
objectsOf(const RecordingTestObserver& observer, const QString& kind)
{
    QList<const GtObject*> result;
    for (auto const& e : observer.entries())
    {
        if (e.kind == kind) result << e.object;
    }
    return result;
}

} // namespace

TEST(GtRecordingThreads, withoutSessionTheCallableRunsUnchanged)
{
    auto wrapped = gt::propagateRecording([]() { return 42; });

    int result = 0;
    std::thread worker([&]() { result = wrapped(); });
    worker.join();

    EXPECT_EQ(result, 42);
}

TEST(GtRecordingThreads, wrappedWorkersReportToTheSameSession)
{
    RecordingTestObject root;
    auto* a = new RecordingTestObject(&root);
    auto* b = new RecordingTestObject(&root);
    RecordingTestObserver observer;
    GtObject component;
    GtRecordingSession session{component, observer, roots(&root)};

    GtRecordingSession* activeInA = nullptr;
    GtRecordingSession* activeInB = nullptr;

    session.run([&]() {
        std::thread workerA(gt::propagateRecording([&]() {
            activeInA = GtRecordingSession::current();
            (void)a->value.getVal();
        }));
        std::thread workerB(gt::propagateRecording([&]() {
            activeInB = GtRecordingSession::current();
            (void)b->value.getVal();
        }));
        workerA.join();
        workerB.join();
        return true;
    });

    EXPECT_EQ(activeInA, &session);
    EXPECT_EQ(activeInB, &session);

    auto const reads = objectsOf(observer, QStringLiteral("read"));
    ASSERT_EQ(reads.size(), 2);
    EXPECT_TRUE(reads.contains(a));
    EXPECT_TRUE(reads.contains(b));
}

TEST(GtRecordingThreads, unwrappedThreadDoesNotInheritSession)
{
    RecordingTestObject root;
    RecordingTestObserver observer;
    GtObject component;
    GtRecordingSession session{component, observer, roots(&root)};

    GtRecordingSession* activeInWorker = &session;

    session.run([&]() {
        std::thread worker([&]() {
            activeInWorker = GtRecordingSession::current();
            (void)root.value.getVal();
        });
        worker.join();
        return true;
    });

    EXPECT_EQ(activeInWorker, nullptr);
    EXPECT_TRUE(objectsOf(observer, QStringLiteral("read")).isEmpty());
}

TEST(GtRecordingThreads, previousStateIsRestoredAlsoOnException)
{
    RecordingTestObject root;
    RecordingTestObserver observer;
    GtObject component;
    GtRecordingSession session{component, observer, roots(&root)};

    session.run([&]() {
        auto throwing = gt::propagateRecording([]() {
            throw std::runtime_error("worker failed");
        });

        GtRecordingSession* afterThrow = &session;
        std::thread worker([&]() {
            try { throwing(); } catch (const std::runtime_error&) {}
            afterThrow = GtRecordingSession::current();
        });
        worker.join();

        EXPECT_EQ(afterThrow, nullptr);
        return true;
    });
}

TEST(GtRecordingThreads, wrapperOutlivingTheSessionRunsUnrecorded)
{
    RecordingTestObject root;
    RecordingTestObserver observer;
    GtObject component;
    GtRecordingSession session{component, observer, roots(&root)};

    std::function<void()> late;
    session.run([&]() {
        late = gt::propagateRecording([&]() { (void)root.value.getVal(); });
        return true;
    });

    GtRecordingSession* activeInWorker = &session;
    std::thread worker([&]() {
        late();
        activeInWorker = GtRecordingSession::current();
    });
    worker.join();

    EXPECT_EQ(activeInWorker, nullptr);
    EXPECT_TRUE(objectsOf(observer, QStringLiteral("read")).isEmpty());
}

TEST(GtRecordingThreads, parallelComponentsReportToTheRightSessions)
{
    // a task session R1 runs two child components in parallel threads;
    // the task's thread waits for both
    RecordingTestObject taskRoot;
    auto* recordedData = new RecordingTestObject(&taskRoot);
    auto* unrecordedData = new RecordingTestObject(&taskRoot);
    RecordingTestObserver taskObserver;
    RecordingTestObserver childObserver;
    GtObject task;
    GtObject recordedChild;
    GtRecordingSession taskSession{task, taskObserver, roots(&taskRoot)};

    QUuid childParentId;

    taskSession.run([&]() {
        // recorded child: its own session (R2) in its thread
        std::thread recorded(gt::propagateRecording([&]() {
            GtRecordingSession childSession{recordedChild, childObserver,
                                            roots(recordedData)};
            childSession.run([&]() {
                (void)recordedData->value.getVal();
                return true;
            });
            childParentId = childSession.parentId();
        }));

        // unrecorded child: runs in the task session (R1)
        std::thread unrecorded(gt::propagateRecording([&]() {
            unrecordedData->value.setVal(1.0);
        }));

        recorded.join();
        unrecorded.join();
        return true;
    });

    EXPECT_EQ(childParentId, taskSession.id());
    EXPECT_EQ(objectsOf(childObserver, QStringLiteral("read")),
              (QList<const GtObject*>{recordedData}));
    EXPECT_TRUE(objectsOf(childObserver, QStringLiteral("written")).isEmpty());

    EXPECT_TRUE(objectsOf(taskObserver, QStringLiteral("read")).isEmpty());
    EXPECT_EQ(objectsOf(taskObserver, QStringLiteral("written")),
              (QList<const GtObject*>{unrecordedData}));

    // nothing leaks after the task finished
    recordedData->value.setVal(2.0);
    EXPECT_EQ(taskObserver.kinds().last(), QStringLiteral("finished"));
    EXPECT_EQ(childObserver.kinds().last(), QStringLiteral("finished"));
}
