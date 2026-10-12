/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 6.10.2026
 */

#include "gtest/gtest.h"

#include <atomic>
#include <memory>
#include <QSet>
#include <QTemporaryDir>
#include <QUuid>
#include <stdexcept>
#include <thread>
#include <vector>

#include "gt_accesstracking.h"
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
 * @brief Test recorder storing a plain copy of every delivered recording.
 *
 * The executor delivers the calculator- and task-level recordings from the
 * worker thread and the execution-level recording from the main thread,
 * strictly one after the other (the main thread only finalizes the
 * execution-level recording after the worker has delivered its share), so
 * the vector needs no locking.
 */
class TestRecorder : public GtAbstractRecorder
{
public:
    struct Recording
    {
        QUuid id;
        QUuid parentId;
        QUuid componentUuid;
        QString componentClassName;
        QString componentName;
        QDateTime startTime;
        QDateTime finishTime;
        GtProcessComponent::STATE outcome;
        QSet<QUuid> accessedObjects;
        bool finished;
    };

    int recordCalls = 0;
    std::vector<Recording> recordings;

    void
    record(const GtRecording& r) override
    {
        ++recordCalls;

        Recording d;
        d.id = r.id();
        d.parentId = r.parentId();
        d.componentUuid = r.componentUuid();
        d.componentClassName = r.componentClassName();
        d.componentName = r.componentName();
        d.startTime = r.startTime();
        d.finishTime = r.finishTime();
        d.outcome = r.outcome();
        d.accessedObjects = r.accessedObjects();
        d.finished = r.isFinished();

        recordings.push_back(std::move(d));
    }
};

/**
 * @brief Recorder whose backend always fails.
 */
class ThrowingRecorder : public GtAbstractRecorder
{
public:
    std::atomic<int> recordCalls{0};

    void
    record(const GtRecording&) override
    {
        ++recordCalls;
        throw std::runtime_error("backend failure");
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

QUuid
makeUuid(quint64 value)
{
    // Qt 6 has no 5-argument QUuid constructor, so the uuid is built
    // from a canonical string representation with a numeric last block
    return QUuid(QStringLiteral("00000000-0000-0000-0000-%1")
                    .arg(QString::number(value, 16).rightJustified(12, QLatin1Char('0'))));
}

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

/**
 * @brief Finds the recording of the given component (matched by uuid and name).
 */
const TestRecorder::Recording*
findComponentRecording(const TestRecorder& recorder, const QString& componentUuid, const QString& name)
{
    for (const auto& rec : recorder.recordings)
    {
        if (rec.componentUuid == QUuid(componentUuid) && rec.componentName == name)
        {
            return &rec;
        }
    }

    return nullptr;
}

/**
 * @brief Finds a top-level recording of the given component.
 * @param accessedEmpty True to match the passive execution-level recording
 *        (no tracker), false to match the component's own execution recording.
 */
const TestRecorder::Recording*
findTopLevelRecording(const TestRecorder& recorder, const QString& componentUuid, bool accessedEmpty)
{
    for (const auto& rec : recorder.recordings)
    {
        if (rec.parentId.isNull() && rec.componentUuid == QUuid(componentUuid)
            && rec.accessedObjects.isEmpty() == accessedEmpty)
        {
            return &rec;
        }
    }

    return nullptr;
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

    void
    TearDown() override
    {
        // defensive: never leave a test recorder registered globally
        gt::setAccessRecorder(nullptr);
    }
};

TEST_F(TestAccessRecording, executeWithoutRunnableDeliversFailedRecording)
{
    TestRecorder recorder;
    gt::setAccessRecorder(&recorder);

    GtTask task;
    EXPECT_FALSE(task.execute());

    gt::setAccessRecorder(nullptr);

    // a bare task without runnable fails before any access, but still
    // delivers exactly one finished recording
    ASSERT_EQ(recorder.recordCalls, 1);
    const auto& rec = recorder.recordings[0];
    EXPECT_TRUE(rec.finished);
    EXPECT_EQ(rec.componentClassName, "GtTask");
    EXPECT_EQ(rec.componentName, "Task");
    EXPECT_EQ(rec.componentUuid, QUuid(task.uuid()));
    EXPECT_TRUE(rec.parentId.isNull());
    EXPECT_EQ(rec.outcome, GtProcessComponent::FAILED);
    EXPECT_TRUE(rec.startTime.isValid());
    EXPECT_TRUE(rec.finishTime.isValid());
    EXPECT_LE(rec.startTime, rec.finishTime);
}

TEST_F(TestAccessRecording, taskExecutionDeliversThreeRecordings)
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

    // the worker thread delivers the calculator- and then the task-level
    // recording; the main thread delivers the passive execution-level
    // recording. Match by properties, never by vector position.
    ASSERT_EQ(recorder.recordCalls, 3);

    const auto* calc = findComponentRecording(recorder, group->calc->uuid(), QStringLiteral("calculator"));
    ASSERT_NE(calc, nullptr);
    EXPECT_EQ(calc->componentClassName, "TestTrackingCalculator");
    EXPECT_TRUE(calc->finished);
    EXPECT_EQ(calc->outcome, GtProcessComponent::FINISHED);
    EXPECT_TRUE(calc->accessedObjects.contains(QUuid(group->data->uuid())));
    EXPECT_TRUE(calc->accessedObjects.contains(QUuid(group->calc->uuid())));
    EXPECT_LE(calc->startTime, calc->finishTime);

    const auto* taskLevel = findTopLevelRecording(recorder, group->task->uuid(), /*accessedEmpty=*/false);
    ASSERT_NE(taskLevel, nullptr);
    EXPECT_EQ(taskLevel->componentClassName, "GtTask");
    EXPECT_EQ(taskLevel->componentName, QStringLiteral("task"));
    EXPECT_TRUE(taskLevel->finished);
    EXPECT_EQ(taskLevel->outcome, GtProcessComponent::FINISHED);
    // the task's own exec() is tracked (its execution mode property is read)
    EXPECT_TRUE(taskLevel->accessedObjects.contains(QUuid(group->task->uuid())));
    // the calculator's data access was recorded in the calculator's own
    // (child) recording, not in the task's
    EXPECT_FALSE(taskLevel->accessedObjects.contains(QUuid(group->data->uuid())));
    EXPECT_LE(taskLevel->startTime, taskLevel->finishTime);

    const auto* execLevel = findTopLevelRecording(recorder, group->task->uuid(), /*accessedEmpty=*/true);
    ASSERT_NE(execLevel, nullptr);
    EXPECT_EQ(execLevel->componentClassName, "GtTask");
    EXPECT_EQ(execLevel->componentName, QStringLiteral("task"));
    EXPECT_TRUE(execLevel->finished);
    EXPECT_EQ(execLevel->outcome, GtProcessComponent::FINISHED);
    EXPECT_TRUE(execLevel->accessedObjects.isEmpty());
    EXPECT_LE(execLevel->startTime, execLevel->finishTime);

    // the calculator executed inside the task: its recording is a child of
    // the task recording, and the passive execution recording spans the whole
    // run (the executor keeps it alive until the worker finished)
    EXPECT_EQ(calc->parentId, taskLevel->id);
    EXPECT_LE(execLevel->startTime, taskLevel->startTime);
    EXPECT_GE(execLevel->finishTime, taskLevel->finishTime);
}

TEST_F(TestAccessRecording, sequentialExecutionsDeliverSeparateRecordings)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    // single project holding two (data, task, calculator) groups;
    // the second execution must deliver a fresh set of recordings
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
    // all three recordings of the first execution are delivered before
    // startTask returns
    ASSERT_EQ(recorder.recordCalls, 3);

    EXPECT_EQ(executor.startTask(group2->task.get()), GtCoreProcessExecutor::TaskExecState::Started);
    ASSERT_EQ(recorder.recordCalls, 6);

    // execution 1: three recordings with a passive execution level
    const auto* task1 = findTopLevelRecording(recorder, group1->task->uuid(), /*accessedEmpty=*/false);
    ASSERT_NE(task1, nullptr);
    EXPECT_EQ(task1->outcome, GtProcessComponent::FINISHED);
    const auto* calc1 = findComponentRecording(recorder, group1->calc->uuid(), QStringLiteral("calculator"));
    ASSERT_NE(calc1, nullptr);
    EXPECT_EQ(calc1->parentId, task1->id);
    // each execution only accessed the objects of its own task group
    EXPECT_TRUE(calc1->accessedObjects.contains(QUuid(group1->data->uuid())));
    EXPECT_FALSE(calc1->accessedObjects.contains(QUuid(group2->data->uuid())));
    const auto* exec1 = findTopLevelRecording(recorder, group1->task->uuid(), /*accessedEmpty=*/true);
    ASSERT_NE(exec1, nullptr);

    // execution 2: three more recordings with a fresh execution level
    const auto* task2 = findTopLevelRecording(recorder, group2->task->uuid(), /*accessedEmpty=*/false);
    ASSERT_NE(task2, nullptr);
    EXPECT_EQ(task2->outcome, GtProcessComponent::FINISHED);
    const auto* calc2 = findComponentRecording(recorder, group2->calc->uuid(), QStringLiteral("calculator"));
    ASSERT_NE(calc2, nullptr);
    EXPECT_EQ(calc2->parentId, task2->id);
    EXPECT_TRUE(calc2->accessedObjects.contains(QUuid(group2->data->uuid())));
    EXPECT_FALSE(calc2->accessedObjects.contains(QUuid(group1->data->uuid())));
    const auto* exec2 = findTopLevelRecording(recorder, group2->task->uuid(), /*accessedEmpty=*/true);
    ASSERT_NE(exec2, nullptr);

    // each execution produced a fresh set of recordings
    EXPECT_NE(task1->id, task2->id);
    EXPECT_NE(calc1->id, calc2->id);
    EXPECT_NE(exec1->id, exec2->id);
}

TEST_F(TestAccessRecording, noRecorderMeansNoRecording)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    auto project = std::make_unique<TestProject>(tempDir.path() + "/project.gtlab");
    project->setObjectName("project");

    auto group = appendTaskGroup(project.get());
    ASSERT_NE(group, nullptr);

    GtCoreProcessExecutor executor;
    ASSERT_TRUE(executor.setSource(project.get()));
    // no access recorder set: execute() takes the fast path, no recording
    // is created, and execution behaves exactly as without recording
    EXPECT_EQ(executor.startTask(group->task.get()), GtCoreProcessExecutor::TaskExecState::Started);
    EXPECT_NE(group->task->currentState(), GtProcessComponent::FAILED);
}

TEST_F(TestAccessRecording, scopeActivatesAndRestoresThreadState)
{
    GtRecording outer;
    outer.setComponentInfo(makeUuid(1), "Outer", "outer");
    outer.setTracker(std::make_unique<GtAccessTracker>());
    outer.start();

    GtRecording inner;
    inner.setComponentInfo(makeUuid(2), "Inner", "inner");
    inner.setTracker(std::make_unique<GtAccessTracker>());
    inner.start();

    {
        GtRecordingScope outerScope(outer);
        EXPECT_EQ(GtRecordingScope::currentRecording(), &outer);
        EXPECT_EQ(GtAccessTracker::current(), outer.tracker().get());

        {
            GtRecordingScope innerScope(inner);
            EXPECT_EQ(GtRecordingScope::currentRecording(), &inner);
            EXPECT_EQ(GtAccessTracker::current(), inner.tracker().get());
            // writes go to the tracker active in the inner scope
            GtAccessTracker::current()->addAccessedProperty(makeUuid(3));
        }

        // the nested scope restored both thread-local slots
        EXPECT_EQ(GtRecordingScope::currentRecording(), &outer);
        EXPECT_EQ(GtAccessTracker::current(), outer.tracker().get());
    }
    // leaving the outermost scope restores the null context
    EXPECT_EQ(GtRecordingScope::currentRecording(), nullptr);
    EXPECT_EQ(GtAccessTracker::current(), nullptr);

    outer.finish(GtProcessComponent::FINISHED);
    inner.finish(GtProcessComponent::FINISHED);

    // the write landed in the inner recording, not the outer one
    EXPECT_TRUE(inner.accessedObjects().contains(makeUuid(3)));
    EXPECT_TRUE(outer.accessedObjects().isEmpty());
}

TEST_F(TestAccessRecording, scopeSuspendAndResume)
{
    // suspend without nesting: both thread-local slots are cleared,
    // resume restores them
    GtRecording rec;
    rec.setTracker(std::make_unique<GtAccessTracker>());
    rec.start();
    {
        GtRecordingScope scope(rec);
        ASSERT_EQ(GtRecordingScope::currentRecording(), &rec);
        ASSERT_EQ(GtAccessTracker::current(), rec.tracker().get());

        scope.suspend();
        EXPECT_EQ(GtRecordingScope::currentRecording(), nullptr);
        EXPECT_EQ(GtAccessTracker::current(), nullptr);

        scope.resume();
        EXPECT_EQ(GtRecordingScope::currentRecording(), &rec);
        EXPECT_EQ(GtAccessTracker::current(), rec.tracker().get());
    }
    EXPECT_EQ(GtRecordingScope::currentRecording(), nullptr);
    EXPECT_EQ(GtAccessTracker::current(), nullptr);

    // suspend while nested: the outer context is restored, and a scope
    // destroyed while suspended still leaves the outer context active
    GtRecording outer;
    outer.setTracker(std::make_unique<GtAccessTracker>());
    outer.start();
    {
        GtRecordingScope outerScope(outer);

        GtRecording inner;
        inner.setTracker(std::make_unique<GtAccessTracker>());
        inner.start();
        {
            GtRecordingScope innerScope(inner);
            ASSERT_EQ(GtRecordingScope::currentRecording(), &inner);
            innerScope.suspend();
            EXPECT_EQ(GtRecordingScope::currentRecording(), &outer);
            EXPECT_EQ(GtAccessTracker::current(), outer.tracker().get());
        }
        EXPECT_EQ(GtRecordingScope::currentRecording(), &outer);
        EXPECT_EQ(GtAccessTracker::current(), outer.tracker().get());
    }
    EXPECT_EQ(GtRecordingScope::currentRecording(), nullptr);
    EXPECT_EQ(GtAccessTracker::current(), nullptr);
}

TEST_F(TestAccessRecording, passiveScopeDisablesTracking)
{
    // a recording without a tracker must explicitly disable tracking while
    // it is active - it must not leave a previously-active tracker running -
    // and restore that tracker on exit
    auto tracker = std::make_unique<GtAccessTracker>();
    {
        GtAccessTrackerScope trackerScope(tracker.get());
        ASSERT_EQ(GtAccessTracker::current(), tracker.get());

        GtRecording passive;
        passive.start();
        {
            GtRecordingScope scope(passive);
            EXPECT_EQ(GtRecordingScope::currentRecording(), &passive);
            EXPECT_EQ(GtAccessTracker::current(), nullptr);
        }
        EXPECT_EQ(GtAccessTracker::current(), tracker.get());
    }
    EXPECT_EQ(GtAccessTracker::current(), nullptr);
}

TEST_F(TestAccessRecording, finishIsIdempotent)
{
    GtRecording rec;
    rec.setComponentInfo(makeUuid(1), "C", "c");
    rec.setTracker(std::make_unique<GtAccessTracker>());
    rec.tracker()->addAccessedProperty(makeUuid(2));
    rec.start();

    rec.finish(GtProcessComponent::FINISHED);
    const QDateTime finishTime = rec.finishTime();
    const QSet<QUuid> accessed = rec.accessedObjects();

    // a second finish leaves all captured data unchanged
    rec.finish(GtProcessComponent::FAILED);
    EXPECT_EQ(rec.finishTime(), finishTime);
    EXPECT_EQ(rec.outcome(), GtProcessComponent::FINISHED);
    EXPECT_EQ(rec.accessedObjects(), accessed);
    EXPECT_TRUE(rec.isFinished());

    // the tracker was released on the first finish
    EXPECT_EQ(rec.tracker().get(), nullptr);
}

TEST_F(TestAccessRecording, exceptionUnwindRestoresThreadState)
{
    GtRecording outer;
    outer.setTracker(std::make_unique<GtAccessTracker>());
    outer.start();
    GtRecordingScope outerScope(outer);

    GtRecording inner;
    inner.setTracker(std::make_unique<GtAccessTracker>());
    inner.start();

    bool caught = false;
    try
    {
        GtRecordingScope innerScope(inner);
        throw std::runtime_error("unwind");
    }
    catch (const std::runtime_error&)
    {
        caught = true;
    }
    EXPECT_TRUE(caught);

    // the scope destructor ran during unwinding and restored the outer
    // context (both thread-local slots)
    EXPECT_EQ(GtRecordingScope::currentRecording(), &outer);
    EXPECT_EQ(GtAccessTracker::current(), outer.tracker().get());
}

TEST_F(TestAccessRecording, scopeOnWorkerThreadActivatesAndRestores)
{
    // a scope activated on a worker thread sees a fresh (null) thread-local
    // state, records there, and restores the null context on exit; the main
    // thread's context must stay untouched
    GtRecording rec;
    rec.setComponentInfo(makeUuid(1), "Worker", "worker");
    rec.setTracker(std::make_unique<GtAccessTracker>());
    rec.start();

    struct WorkerResult
    {
        bool sawActive = false;
        bool restoredNull = false;
    };
    WorkerResult result;

    const QUuid accessed = makeUuid(2);
    std::thread worker{[&rec, &result, accessed]()
    {
        {
            GtRecordingScope scope(rec);
            result.sawActive = (GtRecordingScope::currentRecording() == &rec)
                && (GtAccessTracker::current() == rec.tracker().get());
            if (result.sawActive)
            {
                GtAccessTracker::current()->addAccessedProperty(accessed);
            }
        }
        result.restoredNull = (GtRecordingScope::currentRecording() == nullptr)
            && (GtAccessTracker::current() == nullptr);
    }};
    worker.join();

    EXPECT_TRUE(result.sawActive);
    EXPECT_TRUE(result.restoredNull);

    // the access was recorded in the worker's tracker and is visible
    // after the recording finished
    rec.finish(GtProcessComponent::FINISHED);
    EXPECT_TRUE(rec.accessedObjects().contains(accessed));

    // the main thread's thread-local slots were never touched
    EXPECT_EQ(GtRecordingScope::currentRecording(), nullptr);
    EXPECT_EQ(GtAccessTracker::current(), nullptr);
}

TEST_F(TestAccessRecording, backendFailureDoesNotChangeExecutionResult)
{
    ThrowingRecorder recorder;

    // a failed execution: the backend failure must not turn a clean
    // failure into an uncaught exception
    gt::setAccessRecorder(&recorder);
    {
        GtTask task;
        EXPECT_FALSE(task.execute());
    }
    gt::setAccessRecorder(nullptr);
    EXPECT_EQ(recorder.recordCalls.load(), 1);

    // a successful execution: the same backend failure must not change
    // the task's outcome
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    auto project = std::make_unique<TestProject>(tempDir.path() + "/project.gtlab");
    project->setObjectName("project");

    auto group = appendTaskGroup(project.get());
    ASSERT_NE(group, nullptr);

    GtCoreProcessExecutor executor;
    ASSERT_TRUE(executor.setSource(project.get()));
    executor.setAccessRecorder(&recorder);
    EXPECT_EQ(executor.startTask(group->task.get()), GtCoreProcessExecutor::TaskExecState::Started);

    // all three recordings of the execution were delivered (the backend
    // failures were swallowed) and the task still finished normally
    EXPECT_EQ(recorder.recordCalls.load(), 4);
    EXPECT_NE(group->task->currentState(), GtProcessComponent::FAILED);
}

#include "test_gt_accessrecording.moc"
