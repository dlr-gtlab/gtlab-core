/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_gt_taskrunner.cpp
 */

#include "gtest/gtest.h"

#include <QDir>

#include "gt_abstractrunnable.h"
#include "gt_objectfactory.h"
#include "gt_task.h"
#include "gt_taskrunner.h"

class TestTaskRunnerRunnable : public GtAbstractRunnable
{
public:
    int sourceDataCount() const
    {
        return m_inputData.size();
    }

    int queueSize() const
    {
        return m_queue.size();
    }

    GtProcessComponent* queuedComponent(int idx = 0) const
    {
        return m_queue.value(idx, nullptr);
    }

    void appendOutputObject(const GtObject& obj)
    {
        m_outputData << GtObjectMemento(&obj);
    }

    void finish()
    {
        emit runnableFinished();
    }

    void run() override
    {
    }

    QDir tempDir() override
    {
        return {};
    }

    bool clearTempDir(const QString&) override
    {
        return true;
    }

    QString projectPath() override
    {
        return {};
    }
};

class TestGtTaskRunner : public ::testing::Test
{
protected:
    void SetUp() override
    {
        gtObjectFactory->registerClass(GtTask::staticMetaObject);

        task.setFactory(gtObjectFactory);
        childTask.setFactory(gtObjectFactory);

        task.appendChild(&childTask);
        source.appendChild(&sourceChild);
    }

    GtTask task;
    GtTask childTask;
    GtObject source;
    GtObject sourceChild;
};

TEST_F(TestGtTaskRunner, setUpRejectsInvalidInput)
{
    auto runnable = std::make_unique<TestTaskRunnerRunnable>();
    GtTaskRunner runner(&task);
    GtTaskRunner nullTaskRunner(nullptr);

    EXPECT_FALSE(runner.setUp(runnable.get(), nullptr));
    EXPECT_FALSE(nullTaskRunner.setUp(runnable.get(), &source));
    EXPECT_TRUE(runner.setUp(runnable.get(), &source));
    EXPECT_FALSE(runner.setUp(runnable.get(), &source));
}

TEST_F(TestGtTaskRunner, setUpCopiesSourceAndQueuesClonedTask)
{
    auto runnable = std::make_unique<TestTaskRunnerRunnable>();
    GtTaskRunner runner(&task);

    source.setObjectName("source");
    sourceChild.setObjectName("sourceChild");
    task.setObjectName("task");
    childTask.setObjectName("childTask");

    ASSERT_TRUE(runner.setUp(runnable.get(), &source));

    EXPECT_EQ(runnable->sourceDataCount(), 1);
    EXPECT_EQ(runnable->queueSize(), 1);

    auto* queuedTask = qobject_cast<GtTask*>(runnable->queuedComponent());
    ASSERT_NE(queuedTask, nullptr);
    EXPECT_NE(queuedTask, &task);
    EXPECT_EQ(queuedTask->parent(), runnable.get());

    auto clonedChildren = queuedTask->findDirectChildren<GtTask*>();
    ASSERT_EQ(clonedChildren.size(), 1);
    EXPECT_NE(clonedChildren.front(), &childTask);
}

TEST_F(TestGtTaskRunner, handleRunnableFinishedCollectsOutputAndEmitsFinished)
{
    auto* runnable = new TestTaskRunnerRunnable;
    GtTaskRunner runner(&task);
    GtObject resultObject;
    resultObject.setObjectName("result");

    ASSERT_TRUE(runner.setUp(runnable, &source));

    bool finished = false;
    QObject::connect(&runner, &GtTaskRunner::finished,
                     [&finished]() { finished = true; });

    runnable->appendOutputObject(resultObject);
    runnable->finish();

    EXPECT_TRUE(finished);
    ASSERT_EQ(runner.dataToMerge().size(), 1);
    EXPECT_EQ(runner.dataToMerge().front().uuid(), resultObject.uuid());
}
