/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_coreapplication.h"
#include "gt_exporttomementocalculator.h"
#include "gt_executionenvironment.h"
#include "gt_executioneventstream.h"
#include "gt_objectfactory.h"
#include "gt_objectmementodiff.h"
#include "gt_objectmemento.h"
#include "gt_package.h"
#include "gt_processfactory.h"
#include "gt_propertystructcontainer.h"
#include "gt_project.h"
#include "gt_structproperty.h"
#include "gt_task.h"
#include "gt_processdata.h"
#include "gt_processtaskoperation.h"
#include "gt_executioncontext.h"
#include "gt_intproperty.h"
#include "gt_property.h"

#include <QCoreApplication>
#include <QDomDocument>
#include <QJsonObject>
#include <QThread>

#include <algorithm>
#include <memory>
#include <thread>
#include <vector>

namespace
{

    class TestProject final : public GtProject
    {
    public:
        TestProject() : GtProject(QString{})
        {
        }
    };
} // namespace

class OperationTaskTestCalculator final : public GtCalculator
{
    Q_OBJECT

public:
    Q_INVOKABLE OperationTaskTestCalculator() :
        m_mode(QStringLiteral("mode"), tr("Mode"), 0),
        m_monitoring(QStringLiteral("monitoring"), tr("Monitoring"), 0),
        m_container(QStringLiteral("monitorValues"),
                    GtPropertyStructContainer::Associative)
    {
        registerProperty(m_mode);
        registerMonitoringProperty(m_monitoring);

        GtPropertyStructDefinition definition(
            QStringLiteral("OperationTaskMonitorEntry"));
        definition.defineMember(QStringLiteral("value"),
                                gt::makeMonitoring(gt::makeIntProperty(0)));
        m_container.registerAllowedType(definition);
        registerMonitoringPropertyStructContainer(m_container);
        m_container.newEntry(QStringLiteral("OperationTaskMonitorEntry"),
                             QStringLiteral("monitor-entry"));
    }

    void setMode(int mode)
    {
        m_mode.setVal(mode);
    }

    bool run() override
    {
        if (m_mode.getVal() == 1 || m_mode.getVal() == 2)
        {
            if (auto* context = GtExecutionContext::current())
            {
                if (context->project())
                {
                    context->project()->setObjectName(
                        QStringLiteral("Execution-side mutation"));
                }
            }
        }

        if (m_mode.getVal() == 2)
        {
            QThread::msleep(400);
            return true;
        }
        if (m_mode.getVal() == 1)
        {
            return false;
        }

        m_monitoring.setVal(17);
        m_container[0].setMemberVal(QStringLiteral("value"), 23);
        setProgress(50);
        return true;
    }

private:
    GtIntProperty m_mode;
    GtIntProperty m_monitoring;
    GtPropertyStructContainer m_container;
};

class OperationTaskTestPackage final : public GtPackage
{
    Q_OBJECT

public:
    Q_INVOKABLE OperationTaskTestPackage() = default;
};

namespace
{
    class TestApplication final : public GtCoreApplication
    {
    public:
        TestApplication() :
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

    class ProcessTaskOperationTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            m_application = std::make_unique<TestApplication>();
            ASSERT_TRUE(gtObjectFactory->registerClass(
                OperationTaskTestCalculator::staticMetaObject));
            ASSERT_TRUE(gtObjectFactory->registerClass(
                OperationTaskTestPackage::staticMetaObject));
            ASSERT_TRUE(gtProcessFactory->calculatorFactory()->registerClass(
                OperationTaskTestCalculator::staticMetaObject,
                QStringLiteral("Unittests")));
        }

        void TearDown() override
        {
            gtProcessFactory->calculatorFactory()->unregisterClass(
                OperationTaskTestCalculator::staticMetaObject);
            gtObjectFactory->unregisterClass(
                OperationTaskTestCalculator::staticMetaObject);
            gtObjectFactory->unregisterClass(
                OperationTaskTestPackage::staticMetaObject);
            m_application.reset();
        }

    private:
        std::unique_ptr<TestApplication> m_application;
    };

} // namespace

namespace
{
    std::unique_ptr<GtTask> makeOperationTask(TestProject& project, int mode)
    {
        auto task = std::make_unique<GtTask>();
        task->setParent(&project);
        auto* calculator = new OperationTaskTestCalculator;
        calculator->setMode(mode);
        EXPECT_TRUE(task->appendChild(calculator));
        return task;
    }

    GtOperationExecutionResult executeTaskOperation(
        ProcessTaskOperation& operation, GtTask* task,
        TestProject& executionProject, GtExecutionEventStream& events,
        GtCancellationToken cancellation = {})
    {
        GtOperationExecutionContext operationContext(task, events,
                                                     std::move(cancellation));
        GtExecutionContext executionContext(&executionProject);
        GtExecutionContextScope scope(executionContext);
        return operation.execute(operationContext);
    }
} // namespace

TEST_F(ProcessTaskOperationTest,
       operationRoundtripAndCreateDataReturnDetachedTaskClone)
{
    TestProject project;
    auto* const task = new GtTask;
    task->setParent(&project);
    auto* const calculator = new GtExportToMementoCalculator;
    ASSERT_TRUE(task->appendChild(calculator));

    GtObjectMemento taskMemento = task->toMemento();
    auto reconstructedTask = taskMemento.toObject(*gtObjectFactory);
    auto* const taskFromFactory =
        qobject_cast<GtTask*>(reconstructedTask.get());
    ASSERT_NE(taskFromFactory, nullptr);
    auto* const calculatorFromFactory =
        taskFromFactory->findDirectChild<GtExportToMementoCalculator*>();
    ASSERT_NE(calculatorFromFactory, nullptr);
    EXPECT_EQ(calculatorFromFactory->uuid(), calculator->uuid());

    ProcessTaskOperation original;
    original.setTaskUuid(task->uuid());
    EXPECT_TRUE(original.requiresProject());

    GtObjectMemento operationMemento = original.toMemento();
    auto reconstructed = operationMemento.toObject(*gtObjectFactory);
    auto* const operation =
        qobject_cast<ProcessTaskOperation*>(reconstructed.get());
    ASSERT_NE(operation, nullptr);
    EXPECT_EQ(operation->taskUuid(), task->uuid());

    auto data = operation->createData(GtExecutionContext(&project));
    auto* const detachedTask = qobject_cast<GtTask*>(data.get());
    ASSERT_NE(detachedTask, nullptr);
    EXPECT_NE(detachedTask, task);
    EXPECT_EQ(detachedTask->uuid(), task->uuid());
    EXPECT_EQ(detachedTask->parent(), nullptr);
    auto* const detachedCalculator =
        detachedTask->findDirectChild<GtExportToMementoCalculator*>();
    ASSERT_NE(detachedCalculator, nullptr);
    EXPECT_EQ(detachedCalculator->uuid(), calculator->uuid());
}

TEST_F(ProcessTaskOperationTest,
       coreExportCalculatorUsesNormalObjectFactoryMementoPath)
{
    auto* const calculator = new GtExportToMementoCalculator;
    GtObjectMemento memento = calculator->toMemento();
    delete calculator;

    ASSERT_TRUE(
        gtObjectFactory->knownClass(GT_CLASSNAME(GtExportToMementoCalculator)));
    auto restored = memento.toObject(*gtObjectFactory);
    EXPECT_NE(qobject_cast<GtExportToMementoCalculator*>(restored.get()),
              nullptr);
}

TEST_F(ProcessTaskOperationTest, requiresProjectIsEnforcedByEnvironment)
{
    ProcessTaskOperation operation;
    GtExecutionEventStream events{GtExecutionId{}};
    GtExecutionEnvironment environment;

    const GtExecutionResult result =
        environment.execute(operation, nullptr, events);
    EXPECT_EQ(result.error(), GtExecutionResult::Error::ProjectRequired);
}

TEST_F(ProcessTaskOperationTest,
       cancellationSubscriptionHandlesRequestsBeforeAndAfterSubscribe)
{
    int callbacks = 0;
    GtCancellationToken cancellation;
    auto subscription = cancellation.subscribe([&callbacks] { ++callbacks; });
    cancellation.requestCancellation();
    cancellation.requestCancellation();
    EXPECT_EQ(callbacks, 1);

    GtCancellationToken alreadyCancelled;
    alreadyCancelled.requestCancellation();
    auto lateSubscription =
        alreadyCancelled.subscribe([&callbacks] { ++callbacks; });
    EXPECT_EQ(callbacks, 2);

    int disconnectedCallbacks = 0;
    GtCancellationToken cancellable;
    {
        auto disconnected = cancellable.subscribe(
            [&disconnectedCallbacks] { ++disconnectedCallbacks; });
    }
    cancellable.requestCancellation();
    EXPECT_EQ(disconnectedCallbacks, 0);
}

TEST_F(ProcessTaskOperationTest,
       failedOutcomeNeverAppliesEvenWhenItCarriesAProjectDiff)
{
    TestProject project;
    const GtObjectMemento before = project.toMemento();
    project.setObjectName(QStringLiteral("Changed project"));
    const GtObjectMemento after = project.toMemento();
    GtObjectMementoDiff diff(before, after);
    project.setObjectName(QStringLiteral("Original project"));

    auto payload = std::make_unique<GtProcessTaskOperationResult>();
    payload->setDiffXml(QString::fromUtf8(diff.toByteArray()));
    GtOperationExecutionResult outcome{
        GtOperationExecutionResult::Status::Failed, {}, {}, std::move(payload)};
    GtExecutionContext context(&project);
    ProcessTaskOperation operation;

    EXPECT_TRUE(operation.applyResult(outcome, context).succeeded());
    EXPECT_EQ(project.objectName(), QStringLiteral("Original project"));
}

TEST_F(ProcessTaskOperationTest, cancelledOutcomeCanApplyANonEmptyProjectDiff)
{
    TestProject project;
    GtObjectMemento before = project.toProjectDataMemento();
    before.setUuid(project.uuid());
    before.setIdent(QStringLiteral("Original project"));
    project.setObjectName(QStringLiteral("Intermediate project"));
    GtObjectMemento after = project.toProjectDataMemento();
    after.setUuid(project.uuid());
    after.setIdent(project.objectName());
    GtObjectMementoDiff diff(before, after);
    project.setObjectName(QStringLiteral("Original project"));

    auto payload = std::make_unique<GtProcessTaskOperationResult>();
    payload->setDiffXml(QString::fromUtf8(diff.toByteArray()));
    GtOperationExecutionResult outcome{
        GtOperationExecutionResult::Status::Cancelled,
        {},
        {},
        std::move(payload)};
    GtExecutionContext context(&project);
    ProcessTaskOperation operation;

    EXPECT_TRUE(operation.applyResult(outcome, context).succeeded());
    EXPECT_EQ(project.objectName(), QStringLiteral("Intermediate project"));
}

TEST_F(ProcessTaskOperationTest,
       applyResultRejectsChangesToPropertiesTouchedByTheTask)
{
    TestProject project;
    project.setObjectName(QStringLiteral("Before task"));
    const GtObjectMemento before = project.toMemento();
    project.setObjectName(QStringLiteral("Task result"));
    const GtObjectMemento after = project.toMemento();
    GtObjectMementoDiff diff(before, after);
    project.setObjectName(QStringLiteral("Newer user edit"));

    auto payload = std::make_unique<GtProcessTaskOperationResult>();
    payload->setDiffXml(QString::fromUtf8(diff.toByteArray()));
    GtOperationExecutionResult outcome{
        GtOperationExecutionResult::Status::Success,
        {},
        {},
        std::move(payload)};
    GtExecutionContext context(&project);
    ProcessTaskOperation operation;

    EXPECT_FALSE(operation.applyResult(outcome, context).succeeded());
    EXPECT_EQ(project.objectName(), QStringLiteral("Newer user edit"));
}

TEST_F(ProcessTaskOperationTest,
       invalidLaterDiffStepDoesNotApplyEarlierProjectChanges)
{
    TestProject project;
    const GtObjectMemento before = project.toMemento();
    project.setObjectName(QStringLiteral("Task result"));
    const GtObjectMemento after = project.toMemento();
    GtObjectMementoDiff diff(before, after);
    project.setObjectName(QStringLiteral("Original project"));

    QDomDocument diffDocument;
    ASSERT_TRUE(diffDocument.setContent(diff.toByteArray()));
    QDomElement invalidStep =
        diffDocument.documentElement().cloneNode(true).toElement();
    invalidStep.setAttribute(QStringLiteral("uuid"),
                             QStringLiteral("missing-project-object"));
    ASSERT_TRUE(diffDocument.appendChild(invalidStep).isNull() == false);

    auto payload = std::make_unique<GtProcessTaskOperationResult>();
    payload->setDiffXml(QString::fromUtf8(diffDocument.toByteArray()));
    GtOperationExecutionResult outcome{
        GtOperationExecutionResult::Status::Success,
        {},
        {},
        std::move(payload)};
    GtExecutionContext context(&project);
    ProcessTaskOperation operation;

    EXPECT_FALSE(operation.applyResult(outcome, context).succeeded());
    EXPECT_EQ(project.objectName(), QStringLiteral("Original project"));
}

TEST_F(ProcessTaskOperationTest,
       directProjectPackageChangesUseTheRealProjectDiffRoot)
{
    TestProject project;
    GtObjectMemento before = project.toProjectDataMemento();
    before.setUuid(project.uuid());
    auto* package = new OperationTaskTestPackage;
    package->setObjectName(QStringLiteral("Added package"));
    ASSERT_TRUE(project.appendChild(package));
    GtObjectMemento after = project.toProjectDataMemento();
    after.setUuid(project.uuid());
    GtObjectMementoDiff diff(before, after);
    ASSERT_FALSE(diff.isNull());
    package->disconnectFromParent();
    delete package;

    auto payload = std::make_unique<GtProcessTaskOperationResult>();
    payload->setDiffXml(QString::fromUtf8(diff.toByteArray()));
    GtOperationExecutionResult outcome{
        GtOperationExecutionResult::Status::Success,
        {},
        {},
        std::move(payload)};
    GtExecutionContext context(&project);
    ProcessTaskOperation operation;

    EXPECT_TRUE(operation.applyResult(outcome, context).succeeded());
    auto packages = project.findDirectChildren<OperationTaskTestPackage*>();
    ASSERT_EQ(packages.size(), 1);
    EXPECT_EQ(packages.front()->objectName(), QStringLiteral("Added package"));
}

TEST_F(ProcessTaskOperationTest,
       executionPublishesContainerMonitoringChangesWithProcessEvents)
{
    TestProject originatingProject;
    auto task = makeOperationTask(originatingProject, 0);
    ProcessTaskOperation operation;
    operation.setTaskUuid(task->uuid());
    auto data = operation.createData(GtExecutionContext(&originatingProject));
    auto* const detachedTask = qobject_cast<GtTask*>(data.get());
    ASSERT_NE(detachedTask, nullptr);

    TestProject executionProject;
    GtExecutionEventStream events{GtExecutionId{}};
    std::vector<GtExecutionEvent> published;
    QObject::connect(&events, &GtExecutionEventStream::eventPublished,
                     [&published](GtExecutionEvent const& event) {
                         published.push_back(event);
                     });

    const auto outcome =
        executeTaskOperation(operation, detachedTask, executionProject, events);
    ASSERT_EQ(outcome.status, GtOperationExecutionResult::Status::Success);

    bool foundContainerMonitoringEvent = false;
    bool foundMonitoringEvent = false;
    bool foundProgressEvent = false;
    for (const auto& event : published)
    {
        if (event.eventType() ==
            QStringLiteral("process.monitoring_property_changed"))
        {
            const auto payload = event.payload().toObject();
            const QString property = payload.value("property").toString();
            foundContainerMonitoringEvent |=
                property ==
                    QStringLiteral("monitorValues[monitor-entry].value") &&
                payload.value("value").toInt() == 23;
            foundMonitoringEvent |= property == QStringLiteral("monitoring") &&
                                    payload.value("value").toInt() == 17;
        }
        foundProgressEvent |=
            event.eventType() == QStringLiteral("process.progress_changed");
    }
    EXPECT_TRUE(foundContainerMonitoringEvent);
    EXPECT_TRUE(foundMonitoringEvent);
    EXPECT_TRUE(foundProgressEvent);
}

TEST_F(ProcessTaskOperationTest,
       cancellationInterruptsTaskAndReturnsAnEmptyProjectDiff)
{
    TestProject originatingProject;
    originatingProject.setObjectName(QStringLiteral("Origin project"));
    auto task = makeOperationTask(originatingProject, 2);
    ProcessTaskOperation operation;
    operation.setTaskUuid(task->uuid());
    auto data = operation.createData(GtExecutionContext(&originatingProject));
    auto* const detachedTask = qobject_cast<GtTask*>(data.get());
    ASSERT_NE(detachedTask, nullptr);

    TestProject executionProject;
    GtExecutionEventStream events{GtExecutionId{}};
    GtCancellationToken cancellation;
    std::vector<GtExecutionEvent> published;
    QObject::connect(&events, &GtExecutionEventStream::eventPublished,
                     [&published](GtExecutionEvent const& event) {
                         published.push_back(event);
                     });
    std::thread cancelAfterStart([cancellation]() mutable {
        QThread::msleep(80);
        cancellation.requestCancellation();
    });

    const auto outcome = executeTaskOperation(
        operation, detachedTask, executionProject, events, cancellation);
    cancelAfterStart.join();

    ASSERT_EQ(outcome.status, GtOperationExecutionResult::Status::Cancelled);
    const auto* const result =
        qobject_cast<GtProcessTaskOperationResult const*>(outcome.result.get());
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->diffXml().trimmed().isEmpty());
    EXPECT_EQ(originatingProject.objectName(),
              QStringLiteral("Origin project"));
    GtExecutionContext originContext(&originatingProject);
    EXPECT_TRUE(operation.applyResult(outcome, originContext).succeeded());
    EXPECT_EQ(originatingProject.objectName(),
              QStringLiteral("Origin project"));

    EXPECT_TRUE(std::any_of(
        published.begin(), published.end(), [](GtExecutionEvent const& event) {
            return event.eventType() ==
                       QStringLiteral("process.state_changed") &&
                   event.payload().toObject().value("state").toString() ==
                       QStringLiteral("TERMINATED");
        }));
}

TEST_F(ProcessTaskOperationTest, failedTaskMapsToFailedAndDoesNotApplyChanges)
{
    TestProject originatingProject;
    originatingProject.setObjectName(QStringLiteral("Origin project"));
    auto task = makeOperationTask(originatingProject, 1);
    ProcessTaskOperation operation;
    operation.setTaskUuid(task->uuid());
    auto data = operation.createData(GtExecutionContext(&originatingProject));
    auto* const detachedTask = qobject_cast<GtTask*>(data.get());
    ASSERT_NE(detachedTask, nullptr);

    TestProject executionProject;
    GtExecutionEventStream events{GtExecutionId{}};
    const auto outcome =
        executeTaskOperation(operation, detachedTask, executionProject, events);
    ASSERT_EQ(outcome.status, GtOperationExecutionResult::Status::Failed);
    GtExecutionContext originContext(&originatingProject);
    EXPECT_TRUE(operation.applyResult(outcome, originContext).succeeded());
    EXPECT_EQ(originatingProject.objectName(),
              QStringLiteral("Origin project"));
}

TEST_F(ProcessTaskOperationTest, successfulOutcomeWithoutResultIsRejected)
{
    GtOperationExecutionResult outcome;
    GtExecutionContext context;
    ProcessTaskOperation operation;
    EXPECT_FALSE(operation.applyResult(outcome, context).succeeded());
}

#include "test_gt_processtaskoperation.moc"
