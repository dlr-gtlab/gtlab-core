/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_gt_coreprocessexecutor.cpp
 */

#include "gtest/gtest.h"

#include <memory>

#include "gt_coreprocessexecutor.h"
#include "gt_task.h"

class TestExecutor : public GtCoreProcessExecutor
{
public:
    explicit TestExecutor(QObject* parent = nullptr) :
        GtCoreProcessExecutor(parent)
    {
    }

    using GtCoreProcessExecutor::clearCurrentTask;

    int executeCalls = 0;
    int terminateCalls = 0;
    bool terminateResult = true;

    void setCurrentTask(GtTask* task)
    {
        m_current = task;
    }

protected:
    void execute() override
    {
        ++executeCalls;
    }

    bool terminateCurrentTask() override
    {
        ++terminateCalls;
        return terminateResult;
    }
};

class TestGtCoreProcessExecutor : public ::testing::Test
{
protected:
    static std::unique_ptr<GtTask> makeTask(const QString& name = {})
    {
        auto task = std::make_unique<GtTask>();
        task->setObjectName(name);
        return task;
    }

    static GtTask* appendChildTask(GtTask& parent, const QString& name = {})
    {
        auto child = std::make_unique<GtTask>();
        child->setObjectName(name);
        auto* childPtr = child.get();

        EXPECT_TRUE(parent.appendChild(childPtr));
        child.release();

        return childPtr;
    }

    TestExecutor executor;
    GtObject source;
};

TEST_F(TestGtCoreProcessExecutor, queueTaskRejectsNullAndDuplicateTasks)
{
    auto task = makeTask("task");

    int queueChanges = 0;
    QObject::connect(&executor, &GtCoreProcessExecutor::queueChanged,
                     [&queueChanges]() { ++queueChanges; });

    EXPECT_FALSE(executor.queueTask(nullptr));
    EXPECT_TRUE(executor.queueTask(task.get()));
    EXPECT_EQ(task->currentState(), GtProcessComponent::QUEUED);
    EXPECT_TRUE(executor.taskQueued(task.get()));
    EXPECT_FALSE(executor.queueTask(task.get()));
    EXPECT_EQ(queueChanges, 1);
}

TEST_F(TestGtCoreProcessExecutor,
       executeNextTaskEmitsCompletionWhenQueueIsEmpty)
{
    int completed = 0;
    QObject::connect(&executor, &GtCoreProcessExecutor::allTasksCompleted,
                     [&completed]() { ++completed; });

    EXPECT_FALSE(executor.executeNextTask());
    EXPECT_EQ(completed, 1);
    EXPECT_EQ(executor.executeCalls, 0);
}

TEST_F(TestGtCoreProcessExecutor, runTaskQueuesTaskAndTriggersExecute)
{
    auto task = makeTask("task");
    source.setObjectName("source");

    ASSERT_TRUE(executor.setSource(&source));

    int queueChanges = 0;
    QObject::connect(&executor, &GtCoreProcessExecutor::queueChanged,
                     [&queueChanges]() { ++queueChanges; });

    EXPECT_TRUE(executor.runTask(task.get()));
    EXPECT_EQ(executor.executeCalls, 1);
    EXPECT_EQ(executor.currentRunningTask(), task.get());
    EXPECT_TRUE(executor.taskQueued(task.get()));
    EXPECT_EQ(queueChanges, 2);
}

TEST_F(TestGtCoreProcessExecutor, removeAndMoveOperationsUpdateQueue)
{
    auto task1 = makeTask("task-1");
    auto task2 = makeTask("task-2");
    auto task3 = makeTask("task-3");

    ASSERT_TRUE(executor.queueTask(task1.get()));
    ASSERT_TRUE(executor.queueTask(task2.get()));
    ASSERT_TRUE(executor.queueTask(task3.get()));

    executor.moveTaskUp(task3.get());
    EXPECT_EQ(executor.queue().at(0), task1.get());
    EXPECT_EQ(executor.queue().at(1), task3.get());
    EXPECT_EQ(executor.queue().at(2), task2.get());

    executor.moveTaskDown(task1.get());
    EXPECT_EQ(executor.queue().at(0), task3.get());
    EXPECT_EQ(executor.queue().at(1), task1.get());
    EXPECT_EQ(executor.queue().at(2), task2.get());

    executor.removeFromQueue(task1.get());
    EXPECT_FALSE(executor.taskQueued(task1.get()));
    EXPECT_EQ(task1->currentState(), GtProcessComponent::NONE);
    EXPECT_EQ(executor.queue().size(), 2);
}

TEST_F(TestGtCoreProcessExecutor,
       setSourceAndCustomProjectPathRequireEmptyQueue)
{
    auto task = makeTask("task");

    EXPECT_TRUE(executor.setSource(&source));
    EXPECT_TRUE(executor.setCustomProjectPath("/tmp/project"));
    ASSERT_TRUE(executor.queueTask(task.get()));
    EXPECT_FALSE(executor.setSource(&source));
    EXPECT_FALSE(executor.setCustomProjectPath("/tmp/other-project"));
}

TEST_F(TestGtCoreProcessExecutor, terminateUsesCurrentTaskAndClearsQueuedTasks)
{
    auto task1 = makeTask("task-1");
    auto task2 = makeTask("task-2");

    ASSERT_TRUE(executor.setSource(&source));
    ASSERT_TRUE(executor.runTask(task1.get()));
    ASSERT_TRUE(executor.queueTask(task2.get()));

    EXPECT_FALSE(executor.terminateTask(task2.get()));
    EXPECT_EQ(executor.terminateCalls, 0);

    EXPECT_TRUE(executor.terminateTask(task1.get()));
    EXPECT_EQ(executor.terminateCalls, 1);

    EXPECT_TRUE(executor.terminateAllTasks());
    EXPECT_EQ(executor.terminateCalls, 2);
    EXPECT_TRUE(executor.queue().isEmpty());
}

TEST_F(TestGtCoreProcessExecutor,
       clearCurrentTaskMarksTerminationAndSkipsQueuedChildren)
{
    auto task = makeTask("task");
    GtTask* child = appendChildTask(*task, "child");
    ASSERT_NE(child, nullptr);

    ASSERT_TRUE(executor.queueTask(task.get()));
    task->setState(GtProcessComponent::TERMINATION_REQUESTED);
    executor.setCurrentTask(task.get());

    executor.clearCurrentTask();

    EXPECT_EQ(task->currentState(), GtProcessComponent::TERMINATED);
    EXPECT_EQ(child->currentState(), GtProcessComponent::SKIPPED);
    EXPECT_EQ(executor.currentRunningTask(), nullptr);
    EXPECT_TRUE(executor.queue().isEmpty());
}

TEST_F(TestGtCoreProcessExecutor, clearCurrentTaskMarksNotReadyTaskAsFailed)
{
    auto task = makeTask("task");

    ASSERT_TRUE(executor.queueTask(task.get()));
    executor.setCurrentTask(task.get());

    executor.clearCurrentTask();

    EXPECT_EQ(task->currentState(), GtProcessComponent::FAILED);
    EXPECT_EQ(executor.currentRunningTask(), nullptr);
    EXPECT_TRUE(executor.queue().isEmpty());
}
