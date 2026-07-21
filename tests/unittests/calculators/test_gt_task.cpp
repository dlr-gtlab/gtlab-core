/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_gt_task.cpp
 */

#include "gtest/gtest.h"

#include <QDir>
#include <QMetaObject>
#include <memory>

#include "core/test_gt_processtestclasses.h"

#include "gt_abstractrunnable.h"
#include "gt_boolproperty.h"
#include "gt_object.h"
#include "gt_objectfactory.h"
#include "gt_objectlinkproperty.h"
#include "gt_objectpathproperty.h"
#include "gt_propertyconnection.h"
#include "gt_task.h"

class TestTaskProcessComponent : public GtProcessComponent
{
public:
    bool result = true;
    STATE resultingState = FINISHED;
    int execCalls = 0;

    bool exec() override
    {
        ++execCalls;
        setState(resultingState);
        return result;
    }
};

class WarningCalculator : public GtCalculator
{
public:
    bool run() override
    {
        setState(WARN_FINISHED);
        return true;
    }
};

class TestTaskRunnable : public GtAbstractRunnable
{
public:
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

    void addLinkedObject(GtObject* object)
    {
        m_linkedObjects.append(object);
    }

    void setSuccessful(bool value)
    {
        m_successfulRun = value;
    }

    void addOutput(const GtObjectMemento& memento)
    {
        m_outputData.append(memento);
    }
};

class TestableGtTask : public GtTask
{
public:
    using GtTask::collectMonitoringData;
    using GtTask::collectPropertyConnections;
    using GtTask::linkedObjects;
    using GtTask::runChildElements;
    using GtTask::setRunnable;

    bool setUpResult = true;
    EVALUATION evaluationResult = EVAL_OK;

    bool setUp() override
    {
        return setUpResult;
    }

    EVALUATION evaluate() override
    {
        return evaluationResult;
    }

    void setMaxIterationSteps(int value)
    {
        m_maxIter.setVal(value);
    }
};

class TestableLinkedTask : public TestableGtTask
{
public:
    TestableLinkedTask() :
        linkProp("link", "Link", "Link", this, {GT_CLASSNAME(GtObject)}),
        pathProp("path", "Path", "Path", "", this, {GT_CLASSNAME(GtObject)})
    {
        registerProperty(linkProp);
        registerProperty(pathProp);
    }

    void setLinkedUuid(const QString& uuid)
    {
        linkProp.setVal(uuid);
    }

    void setLinkedPath(const QString& path)
    {
        pathProp.setVal(path);
    }

private:
    GtObjectLinkProperty linkProp;
    GtObjectPathProperty pathProp;
};

class TestGtTask : public ::testing::Test
{
protected:
    void SetUp() override
    {
        if (!gtObjectFactory->knownClass(GT_CLASSNAME(TestGtCalculator)))
        {
            gtObjectFactory->registerClass(TestGtCalculator::staticMetaObject);
        }

        if (!gtObjectFactory->knownClass(GT_CLASSNAME(TestGtProcessComponent)))
        {
            gtObjectFactory->registerClass(
                TestGtProcessComponent::staticMetaObject);
        }

        if (!gtObjectFactory->knownClass(GT_CLASSNAME(WarningCalculator)))
        {
            gtObjectFactory->registerClass(WarningCalculator::staticMetaObject);
        }

        if (!gtObjectFactory->knownClass(GT_CLASSNAME(GtPropertyConnection)))
        {
            gtObjectFactory->registerClass(
                GtPropertyConnection::staticMetaObject);
        }
    }
};

TEST_F(TestGtTask, calculatorsAndProcessComponentsReturnDirectChildren)
{
    TestableGtTask task;
    TestGtCalculator calculator;
    GtTask childTask;

    ASSERT_TRUE(task.appendChild(&calculator));
    ASSERT_TRUE(task.appendChild(&childTask));

    EXPECT_EQ(task.calculators().size(), 1);
    EXPECT_EQ(task.calculators().front(), &calculator);
    EXPECT_EQ(task.processComponents().size(), 2);
}

TEST_F(TestGtTask, requestInterruptionMarksTask)
{
    TestableGtTask task;

    EXPECT_FALSE(task.isInterruptionRequested());

    task.requestInterruption();

    EXPECT_TRUE(task.isInterruptionRequested());
    EXPECT_EQ(task.currentState(), GtProcessComponent::TERMINATION_REQUESTED);
}

TEST_F(TestGtTask, execFailsWithoutRunnableParent)
{
    TestableGtTask task;

    EXPECT_FALSE(task.exec());
    EXPECT_EQ(task.currentState(), GtProcessComponent::FAILED);
}

TEST_F(TestGtTask, execFailsWhenSetUpFails)
{
    TestTaskRunnable runnable;
    TestableLinkedTask task;
    task.setParent(&runnable);
    task.setUpResult = false;

    EXPECT_FALSE(task.exec());
    EXPECT_EQ(task.currentState(), GtProcessComponent::FAILED);
}

TEST_F(TestGtTask, execIgnoresMissingLinkedObjects)
{
    TestTaskRunnable runnable;
    TestableLinkedTask task;
    task.setParent(&runnable);
    task.setLinkedUuid("missing-uuid");
    task.setLinkedPath("missing/path");
    task.evaluationResult = GtTask::EVAL_FINISHED;

    ASSERT_TRUE(task.exec());
    EXPECT_TRUE(task.linkedObjects().isEmpty());
    EXPECT_EQ(task.currentState(), GtProcessComponent::FINISHED);
}

TEST_F(TestGtTask, runChildElementsFailsWhenChildExecutionFails)
{
    TestableGtTask task;
    TestTaskProcessComponent child;
    child.result = false;
    child.resultingState = GtProcessComponent::FAILED;

    ASSERT_TRUE(task.appendChild(&child));

    EXPECT_FALSE(task.runChildElements());
    EXPECT_EQ(child.execCalls, 1);
    EXPECT_EQ(task.currentState(), GtProcessComponent::FAILED);
    EXPECT_EQ(task.currentIterationStep(), 1);
}

TEST_F(TestGtTask, runChildElementsTerminatesWhenInterruptionWasRequested)
{
    TestableGtTask task;
    TestTaskProcessComponent child;

    ASSERT_TRUE(task.appendChild(&child));
    task.requestInterruption();

    EXPECT_FALSE(task.runChildElements());
    EXPECT_EQ(child.execCalls, 1);
    EXPECT_EQ(task.currentState(), GtProcessComponent::TERMINATED);
}

TEST_F(TestGtTask, runChildElementsFailsOnWarningWhenCalculatorRequestsIt)
{
    TestableGtTask task;
    WarningCalculator calculator;
    auto* failOnWarn =
        qobject_cast<GtBoolProperty*>(calculator.findProperty("failOnWarn"));
    ASSERT_NE(failOnWarn, nullptr);
    failOnWarn->setVal(true);

    ASSERT_TRUE(task.appendChild(&calculator));

    EXPECT_FALSE(task.runChildElements());
    EXPECT_EQ(calculator.currentState(), GtProcessComponent::FAILED);
    EXPECT_EQ(task.currentState(), GtProcessComponent::FAILED);
}

TEST_F(TestGtTask, runIterationReflectsEvaluationState)
{
    TestableGtTask task;
    TestTaskProcessComponent child;

    ASSERT_TRUE(task.appendChild(&child));

    task.evaluationResult = GtTask::EVAL_FINISHED;
    EXPECT_TRUE(task.runIteration());
    EXPECT_EQ(task.currentState(), GtProcessComponent::FINISHED);

    task.setState(GtProcessComponent::NONE);
    task.evaluationResult = GtTask::EVAL_FAILED;
    EXPECT_FALSE(task.runIteration());
    EXPECT_EQ(task.currentState(), GtProcessComponent::FAILED);
}

TEST_F(TestGtTask, collectMonitoringDataIncludesChildMonitoringProperties)
{
    TestableGtTask task;
    TestGtProcessComponent child;

    ASSERT_TRUE(task.appendChild(&child));
    child.addMonitoringVar("name", 42);

    auto data = task.collectMonitoringData();

    ASSERT_TRUE(data.contains(child.uuid()));
    auto monData = data.getData(child.uuid());
    const auto keys = monData.data().keys();
    ASSERT_EQ(keys.size(), 1);
    EXPECT_TRUE(keys.front().startsWith("monitoringVars[{"));
    EXPECT_TRUE(keys.front().endsWith("}].value"));
    EXPECT_EQ(monData.getData(keys.front()), QVariant(42));
}

TEST_F(TestGtTask, collectPropertyConnectionsIncludesDirectAndNestedConnections)
{
    TestableGtTask task;
    GtTask childTask;
    GtPropertyConnection direct;
    GtPropertyConnection nested;

    ASSERT_TRUE(task.appendChild(&childTask));
    ASSERT_TRUE(task.appendChild(&direct));
    ASSERT_TRUE(childTask.appendChild(&nested));

    auto connections = task.collectPropertyConnections();

    EXPECT_EQ(connections.size(), 2);
    EXPECT_TRUE(connections.contains(&direct));
    EXPECT_TRUE(connections.contains(&nested));
}

TEST_F(TestGtTask, handleRunnableFinishedStoresOutputAndFinishes)
{
    TestableGtTask task;
    GtObject object;
    object.setObjectName("source");
    auto runnable = std::make_unique<TestTaskRunnable>();
    auto* rawRunnable = runnable.get();
    task.setRunnable(rawRunnable);
    runnable.release();

    int finishedCount = 0;
    QObject::connect(
        &task, &GtTask::finished, [&finishedCount]() { ++finishedCount; });

    rawRunnable->setSuccessful(true);
    rawRunnable->addOutput(object.toMemento());

    ASSERT_TRUE(QMetaObject::invokeMethod(&task, "handleRunnableFinished"));

    EXPECT_EQ(task.currentState(), GtProcessComponent::FINISHED);
    EXPECT_EQ(task.dataToMerge().size(), 1);
    EXPECT_EQ(finishedCount, 1);
}

TEST_F(TestGtTask, handleRunnableFinishedMarksFailureOnUnsuccessfulRunnable)
{
    TestableGtTask task;
    auto runnable = std::make_unique<TestTaskRunnable>();
    auto* rawRunnable = runnable.get();
    task.setRunnable(rawRunnable);
    runnable.release();

    rawRunnable->setSuccessful(false);

    ASSERT_TRUE(QMetaObject::invokeMethod(&task, "handleRunnableFinished"));

    EXPECT_EQ(task.currentState(), GtProcessComponent::FAILED);
    EXPECT_TRUE(task.dataToMerge().isEmpty());
}

TEST_F(TestGtTask, handleRunnableFinishedMarksWarningFinishedWhenChildWarns)
{
    TestableGtTask task;
    TestTaskProcessComponent child;
    auto runnable = std::make_unique<TestTaskRunnable>();
    auto* rawRunnable = runnable.get();
    task.setRunnable(rawRunnable);
    runnable.release();

    ASSERT_TRUE(task.appendChild(&child));
    child.setState(GtProcessComponent::WARN_FINISHED);
    rawRunnable->setSuccessful(true);

    ASSERT_TRUE(QMetaObject::invokeMethod(&task, "handleRunnableFinished"));

    EXPECT_EQ(task.currentState(), GtProcessComponent::WARN_FINISHED);
}

TEST_F(TestGtTask, monitoringDataCanBeStoredAndCleared)
{
    TestableGtTask task;
    GtMonitoringDataSet data;
    GtMonitoringData row;
    row.addData("value", 1);
    data.insert("uuid", row);

    task.onMonitoringDataAvailable(1, data);

    EXPECT_EQ(task.monitoringDataSize(), 1);

    task.clearMonitoringData();

    EXPECT_EQ(task.monitoringDataSize(), 0);
}
