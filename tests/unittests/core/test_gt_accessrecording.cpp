/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 6.10.2026
 */

#include "gtest/gtest.h"

#include <memory>
#include <QTemporaryDir>

#include "gt_calculator.h"
#include "gt_calculatorfactory.h"
#include "gt_coreprocessexecutor.h"
#include "gt_doubleproperty.h"
#include "gt_objectfactory.h"
#include "gt_objectlinkproperty.h"
#include "gt_project.h"
#include "gt_recording.h"
#include "gt_task.h"

/**
 * @brief Simple data object with a single double property.
 */
class TestDataObject : public GtObject
{
    Q_OBJECT

public:
    GtDoubleProperty m_value{"value", "Value"};

    /// Invokable no-arg ctor: the task clone re-instantiates linked objects
    /// via QMetaObject::newInstance(), which needs a no-argument constructor.
    Q_INVOKABLE
    TestDataObject() :
        GtObject()
    {
        setObjectName(QStringLiteral("TestDataObject"));
        registerProperty(m_value);
    }
};

/**
 * @brief Calculator with an object link property.
 *
 * During execution it resolves the linked data object through the
 * runnable's linked objects, reads its value property, and additionally
 * reads its own link property so that the calculator itself is recorded
 * as accessed object.
 */
class TestTrackingCalculator : public GtCalculator
{
    Q_OBJECT

public:
    GtObjectLinkProperty m_input;

    /// Invokable default constructor: the object factory re-instantiates the
    /// calculator via QMetaObject::newInstance() when the task is cloned, which
    /// requires a no-argument constructor. The input link is created empty and
    /// assigned afterwards through setInputUuid().
    Q_INVOKABLE
    TestTrackingCalculator() :
        GtCalculator(),
        m_input{"input", "Input", "Linked input data object", this,
                 {GT_CLASSNAME(TestDataObject)}}
    {
        setObjectName(QStringLiteral("TestTrackingCalculator"));
        registerProperty(m_input);
    }

    /// Assigns the linked input object after construction (see above).
    void
    setInputUuid(const QString& uuid)
    {
        m_input.setVal(uuid);
    }

    bool
    run() override
    {
        bool dataFound = false;

        for (const QPointer<GtObject>& obj : linkedObjects())
        {
            if (auto* data = qobject_cast<TestDataObject*>(obj.data()))
            {
                dataFound = true;
                (void)data->m_value.getVal();
            }
        }

        // also read the own link property (qualified call: the derived class'
        // valueToVariant(QString, bool*) hides the base class' tracked no-arg overload)
        (void)m_input.GtAbstractProperty::valueToVariant();

        return dataFound;
    }
};

/**
 * @brief Test recorder counting all calls and storing the recordings.
 */
class TestRecorder : public GtAbstractRecorder
{
public:
    int initLinkedObjectsCalls = 0;
    int recordChangesCalls = 0;
    int recordAccessObjectsCalls = 0;
    int createActivityCalls = 0;

    QSet<QUuid> lastAccessedObjects;
    QList<QPointer<GtObject>> lastLinkedObjects;
    QList<GtRecording> activities;

    bool
    initLinkedObjects(const QList<QPointer<GtObject>> linkedObjects) override
    {
        ++initLinkedObjectsCalls;
        lastLinkedObjects = linkedObjects;
        return true;
    }

    bool
    recordChanges(const QList<QPointer<GtObject>> linkedObjects) override
    {
        ++recordChangesCalls;
        lastLinkedObjects = linkedObjects;
        return true;
    }

    bool
    recordAccessObjects(const QSet<QUuid> accessedObjects, QList<QPointer<GtObject>> /*linkedObjects*/) override
    {
        ++recordAccessObjectsCalls;
        lastAccessedObjects = accessedObjects;
        return true;
    }

    bool
    createActivity(const GtRecording& recording) override
    {
        ++createActivityCalls;
        activities.append(recording);
        return true;
    }
};

/**
 * @brief GtProject subclass with a public constructor
 * (the GtProject constructor is protected).
 */
class TestProject : public GtProject
{
public:
    explicit TestProject(const QString& path) :
        GtProject(std::move(path))
    {
    }
};

namespace
{

/**
 * @brief A (data, task, calculator) group with the calculator linked to the data.
 */
struct TestTaskGroup
{
    std::unique_ptr<TestDataObject> data;
    std::unique_ptr<GtTask> task;
    std::unique_ptr<TestTrackingCalculator> calc;
};

/**
 * @brief Appends one (data, task, calculator) group to the given project.
 */
std::unique_ptr<TestTaskGroup>
appendTaskGroup(GtProject* project)
{
    auto group = std::make_unique<TestTaskGroup>();

    group->data = std::make_unique<TestDataObject>();
    group->data->setFactory(gtObjectFactory);
    if (!project->appendChild(group->data.get()))
    {
        return nullptr;
    }

    group->task = std::make_unique<GtTask>();
    group->task->setObjectName(QStringLiteral("task"));
    group->task->setFactory(gtObjectFactory);
    if (!project->appendChild(group->task.get()))
    {
        return nullptr;
    }

    group->calc = std::make_unique<TestTrackingCalculator>();
    group->calc->setObjectName(QStringLiteral("calculator"));
    group->calc->setInputUuid(group->data->uuid());
    // run the calculator locally: a core-only unit-test build loads no process
    // modules, so no "parent"/plugin executor is available for the default mode
    group->calc->setExecModeLocal();
    group->calc->setFactory(gtObjectFactory);
    if (!group->task->appendChild(group->calc.get()))
    {
        return nullptr;
    }

    return group;
}

}

class TestAccessRecording : public ::testing::Test
{
protected:
    void
    SetUp() override
    {
        gtObjectFactory->registerClass(GtTask::staticMetaObject);
        gtObjectFactory->registerClass(TestDataObject::staticMetaObject);
        gtObjectFactory->registerClass(TestTrackingCalculator::staticMetaObject);
        // The task clone re-instantiates task children (the calculators and
        // their linked data objects) through the calculator factory
        gtCalculatorFactory->registerClass(TestDataObject::staticMetaObject);
        gtCalculatorFactory->registerClass(TestTrackingCalculator::staticMetaObject);
    }
};

TEST_F(TestAccessRecording, executionRecordsAccessedObjects)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    auto project = std::make_unique<TestProject>(tempDir.path() + "/project.gtlab");
    project->setObjectName("project");

    auto group = appendTaskGroup(project.get());
    ASSERT_NE(group, nullptr);

    TestRecorder recorder;
    GtCoreProcessExecutor executor;
    ASSERT_TRUE(executor.setSource(project.get()));
    executor.setAccessRecorder(&recorder);

    EXPECT_EQ(executor.startTask(group->task.get()), GtCoreProcessExecutor::TaskExecState::Started);

    // recorder called exactly once per execution
    EXPECT_EQ(recorder.initLinkedObjectsCalls, 1);
    EXPECT_EQ(recorder.recordChangesCalls, 1);
    EXPECT_EQ(recorder.recordAccessObjectsCalls, 1);
    EXPECT_EQ(recorder.createActivityCalls, 1);
    ASSERT_EQ(recorder.activities.size(), 1);

    const GtRecording& recording = recorder.activities.first();
    EXPECT_FALSE(recording.contextUuid().isNull());
    EXPECT_EQ(recording.activityObject(), group->task.get());

    // the linked data object was accessed (its value property was read during execution)
    EXPECT_TRUE(recorder.lastAccessedObjects.contains(QUuid(group->data->uuid())));
    // the calculator itself was accessed (its link property was read during execution)
    EXPECT_TRUE(recorder.lastAccessedObjects.contains(QUuid(group->calc->uuid())));
}

TEST_F(TestAccessRecording, sequentialExecutionsCreateDistinctContexts)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    // single project holding two (data, task, calculator) groups;
    // the second execution must re-derive its source from the task's parent project
    auto project = std::make_unique<TestProject>(tempDir.path() + "/project.gtlab");
    project->setObjectName("project");

    auto group1 = appendTaskGroup(project.get());
    ASSERT_NE(group1, nullptr);
    auto group2 = appendTaskGroup(project.get());
    ASSERT_NE(group2, nullptr);

    TestRecorder recorder;
    GtCoreProcessExecutor executor;
    ASSERT_TRUE(executor.setSource(project.get()));
    executor.setAccessRecorder(&recorder);

    EXPECT_EQ(executor.startTask(group1->task.get()), GtCoreProcessExecutor::TaskExecState::Started);
    EXPECT_EQ(executor.startTask(group2->task.get()), GtCoreProcessExecutor::TaskExecState::Started);

    ASSERT_EQ(recorder.activities.size(), 2);

    const GtRecording& recording1 = recorder.activities.at(0);
    const GtRecording& recording2 = recorder.activities.at(1);

    EXPECT_FALSE(recording1.contextUuid().isNull());
    EXPECT_FALSE(recording2.contextUuid().isNull());
    EXPECT_NE(recording1.contextUuid(), recording2.contextUuid());
    EXPECT_EQ(recording1.activityObject(), group1->task.get());
    EXPECT_EQ(recording2.activityObject(), group2->task.get());

    // each execution only accessed the objects of its own task group
    EXPECT_TRUE(recorder.lastAccessedObjects.contains(QUuid(group2->data->uuid())));
}

TEST_F(TestAccessRecording, executionWithoutRecorderDoesNotFail)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    auto project = std::make_unique<TestProject>(tempDir.path() + "/project.gtlab");
    project->setObjectName("project");

    auto group = appendTaskGroup(project.get());
    ASSERT_NE(group, nullptr);

    GtCoreProcessExecutor executor;
    ASSERT_TRUE(executor.setSource(project.get()));
    // no access recorder set

    EXPECT_EQ(executor.startTask(group->task.get()), GtCoreProcessExecutor::TaskExecState::Started);
}

#include "test_gt_accessrecording.moc"
