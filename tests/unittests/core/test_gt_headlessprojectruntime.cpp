/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <atomic>
#include <memory>
#include <thread>

#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QCoreApplication>
#include <QEventLoop>
#include <QThread>

#include "gt_coreapplication.h"
#include "gt_coreprocessexecutor.h"
#include "gt_coredatamodel.h"
#include "gt_externalizationmanager.h"
#include "gt_executioncontext.h"
#include "gt_objectfactory.h"
#include "gt_projectexecutionguard.h"
#include "gt_project.h"
#include "gt_processdata.h"
#include "gt_task.h"
#include "gt_taskgroup.h"
#include "gt_calculator.h"
#include "gt_headlessprojectruntime.h"
#include "gt_processexecutormanager.h"
#include "gt_testhelper.h"

namespace
{
class ContextObservingCalculator : public GtCalculator
{
    Q_OBJECT

public:
    Q_INVOKABLE ContextObservingCalculator() = default;

    static std::atomic<GtProject*> observedProject;

    bool run() override
    {
        const auto* context = GtExecutionContext::current();
        auto* project = context ? context->project() : nullptr;
        observedProject.store(project);
        return project != nullptr;
    }
};

std::atomic<GtProject*> ContextObservingCalculator::observedProject{nullptr};

class LegacyCurrentProjectCalculator : public GtCalculator
{
    Q_OBJECT

public:
    Q_INVOKABLE LegacyCurrentProjectCalculator() = default;

    static std::atomic<GtProject*> observedProject;

    bool run() override
    {
        observedProject.store(gtApp->currentProject());
        return observedProject.load() != nullptr;
    }
};

std::atomic<GtProject*> LegacyCurrentProjectCalculator::observedProject{nullptr};

class InterruptibleCalculator : public GtCalculator
{
    Q_OBJECT

public:
    Q_INVOKABLE InterruptibleCalculator() = default;

    static std::atomic_bool started;
    static std::atomic_bool release;

    bool run() override
    {
        started.store(true);
        auto* task = findParent<GtTask*>();
        while (!release.load() && !(task && task->isInterruptionRequested()))
        {
            QThread::msleep(1);
        }
        return true;
    }
};

std::atomic_bool InterruptibleCalculator::started{false};
std::atomic_bool InterruptibleCalculator::release{false};

class ProgressCalculator : public GtCalculator
{
    Q_OBJECT

public:
    Q_INVOKABLE ProgressCalculator() = default;

    static std::atomic_bool reported;

    bool run() override
    {
        auto* task = findParent<GtTask*>();
        if (task)
        {
            task->setProgress(42);
        }
        reported.store(true);
        while (task && !task->isInterruptionRequested())
        {
            QThread::msleep(1);
        }
        return true;
    }
};

std::atomic_bool ProgressCalculator::reported{false};

class FailingCalculator : public GtCalculator
{
    Q_OBJECT

public:
    Q_INVOKABLE FailingCalculator() = default;

    bool run() override
    {
        return false;
    }
};

class UncooperativeCalculator : public GtCalculator
{
    Q_OBJECT

public:
    Q_INVOKABLE UncooperativeCalculator() = default;

    static std::atomic_bool started;
    static std::atomic_bool release;

    bool run() override
    {
        started.store(true);
        while (!release.load())
        {
            QThread::msleep(1);
        }
        return true;
    }
};

std::atomic_bool UncooperativeCalculator::started{false};
std::atomic_bool UncooperativeCalculator::release{false};

class TestGtHeadlessProjectRuntime : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_application = std::make_unique<GtCoreApplication>(
            QCoreApplication::instance(), GtCoreApplication::AppMode::Batch);
        m_application->init();
        m_runtime = std::make_unique<GtHeadlessProjectRuntime>();
        ASSERT_TRUE(m_runtime->initialize());
    }

    void TearDown() override
    {
        m_runtime.reset();
        m_application.reset();
        gtExternalizationManager->onProjectLoaded(QDir::tempPath());
    }

    QString createProject() const
    {
        const auto directory = gtTestHelper->newTempDir();
        QFile file(directory.filePath(GtProject::mainFilename()));
        EXPECT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
        file.write(QStringLiteral(
                       "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                       "<GTLAB projectname=\"headless-runtime-test\" "
                       "version=\"1.7.0-rc1\">\n"
                       "    <env-footprint><core-ver>2.0.0</core-ver>"
                       "<modules/></env-footprint>\n"
                       "    <comment/>\n"
                       "    <MODULES/>\n"
                       "    <PROCESSES/>\n"
                       "    <LABELS/>\n"
                       "</GTLAB>\n")
                       .toUtf8());
        file.close();
        return directory.absolutePath();
    }

    GtProject* openProject()
    {
        const auto path = createProject();
        EXPECT_TRUE(m_runtime->openProject(path));
        return gtDataModel->currentProject();
    }

    std::unique_ptr<GtCoreApplication> m_application;
    std::unique_ptr<GtHeadlessProjectRuntime> m_runtime;
};
} // namespace

TEST(GtHeadlessTaskStatus, InvalidStatusIsNotDone)
{
    GtHeadlessTaskStatus status;

    EXPECT_FALSE(status.isDone());
    EXPECT_EQ(status.state, GtHeadlessTaskStatus::State::Invalid);
}

TEST(GtHeadlessTaskStatus, TerminalStatesAreDone)
{
    for (const auto state : {GtHeadlessTaskStatus::State::Finished,
                             GtHeadlessTaskStatus::State::Failed,
                             GtHeadlessTaskStatus::State::Cancelled,
                             GtHeadlessTaskStatus::State::Shutdown})
    {
        GtHeadlessTaskStatus status;
        status.state = state;
        EXPECT_TRUE(status.isDone());
    }
}

TEST(GtHeadlessTaskHandle, DefaultHandleIsInvalid)
{
    GtHeadlessTaskHandle handle;

    EXPECT_FALSE(handle.isValid());
    EXPECT_TRUE(handle.id().isEmpty());
    EXPECT_FALSE(handle.status().isDone());
    EXPECT_EQ(handle.cancel().code,
              GtHeadlessTaskCancellationResult::Code::TaskUnavailable);
}

TEST(GtHeadlessProjectRuntime, StartsInCreatedState)
{
    GtHeadlessProjectRuntime runtime;

    EXPECT_EQ(runtime.state(), GtHeadlessProjectRuntime::State::Created);
    EXPECT_TRUE(runtime.projectPath().isEmpty());
}

TEST(GtHeadlessProjectRuntime, RejectsProjectBeforeInitialization)
{
    GtHeadlessProjectRuntime runtime;
    GtHeadlessRuntimeResult result = runtime.openProject(QStringLiteral("missing"));

    EXPECT_EQ(result.code, GtHeadlessRuntimeResult::Code::InvalidState);
    EXPECT_FALSE(result.succeeded());
}

TEST(GtHeadlessProjectRuntime, ReportsMissingCoreServices)
{
    GtHeadlessProjectRuntime runtime;
    const auto result = runtime.initialize();

    EXPECT_EQ(result.code, GtHeadlessRuntimeResult::Code::CoreUnavailable);
    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(runtime.state(), GtHeadlessProjectRuntime::State::Created);
}

TEST_F(TestGtHeadlessProjectRuntime, ListsTasksAndRejectsUnknownTask)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);

    auto* task = new GtTask;
    task->setObjectName(QStringLiteral("task"));
    ASSERT_NE(project->processData()->taskGroup(), nullptr);
    ASSERT_TRUE(project->processData()->taskGroup()->appendChild(task));

    const auto descriptors = m_runtime->listTasks();
    ASSERT_EQ(descriptors.size(), 1);
    EXPECT_EQ(descriptors.front().taskId, QStringLiteral("task"));
    EXPECT_EQ(descriptors.front().group,
              GtTaskGroup::defaultUserGroupId());

    GtHeadlessRuntimeResult result;
    const auto handle = m_runtime->submitTask(QStringLiteral("missing"), &result);
    EXPECT_FALSE(handle.isValid());
    EXPECT_EQ(result.code, GtHeadlessRuntimeResult::Code::TaskNotFound);
}

TEST_F(TestGtHeadlessProjectRuntime, ExecutesTaskWithProjectExecutionContext)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    auto* taskGroup = project->processData()->taskGroup();
    ASSERT_NE(taskGroup, nullptr);

    gtObjectFactory->registerClass(GtTask::staticMetaObject);
    gtObjectFactory->registerClass(ContextObservingCalculator::staticMetaObject);
    ContextObservingCalculator::observedProject.store(nullptr);

    auto task = std::make_unique<GtTask>();
    task->setObjectName(QStringLiteral("context-task"));
    auto calculator = std::make_unique<ContextObservingCalculator>();
    ASSERT_TRUE(task->appendChild(calculator.release()));
    ASSERT_TRUE(taskGroup->appendChild(task.release()));

    GtHeadlessRuntimeResult result;
    const auto handle = m_runtime->submitTask(QStringLiteral("context-task"),
                                               &result);
    ASSERT_TRUE(handle.isValid());
    ASSERT_TRUE(result.succeeded());

    GtHeadlessTaskStatus foreignThreadStatus;
    std::thread foreignThread([&]() { foreignThreadStatus = handle.status(); });
    foreignThread.join();
    EXPECT_EQ(foreignThreadStatus.state, GtHeadlessTaskStatus::State::Invalid);

    const auto status = handle.wait(5000);
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    EXPECT_TRUE(status.isDone());
    EXPECT_EQ(status.state, GtHeadlessTaskStatus::State::Finished);
    EXPECT_EQ(ContextObservingCalculator::observedProject.load(), project);
}

TEST_F(TestGtHeadlessProjectRuntime, PreservesLegacyCurrentProjectCompatibility)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    auto* taskGroup = project->processData()->taskGroup();
    ASSERT_NE(taskGroup, nullptr);

    gtObjectFactory->registerClass(GtTask::staticMetaObject);
    gtObjectFactory->registerClass(LegacyCurrentProjectCalculator::staticMetaObject);
    LegacyCurrentProjectCalculator::observedProject.store(nullptr);

    auto task = std::make_unique<GtTask>();
    task->setObjectName(QStringLiteral("legacy-current-project-task"));
    auto calculator = std::make_unique<LegacyCurrentProjectCalculator>();
    ASSERT_TRUE(task->appendChild(calculator.release()));
    ASSERT_TRUE(taskGroup->appendChild(task.release()));

    GtHeadlessRuntimeResult result;
    const auto handle = m_runtime->submitTask(QStringLiteral("legacy-current-project-task"),
                                               &result);
    ASSERT_TRUE(handle.isValid());
    ASSERT_TRUE(result.succeeded());

    const auto status = handle.wait(5000);
    EXPECT_EQ(status.state, GtHeadlessTaskStatus::State::Finished);
    EXPECT_EQ(status.result, GtHeadlessTaskStatus::Result::Succeeded);
    EXPECT_EQ(LegacyCurrentProjectCalculator::observedProject.load(), project);
}

TEST_F(TestGtHeadlessProjectRuntime, ReportsTaskProgress)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    auto* taskGroup = project->processData()->taskGroup();
    ASSERT_NE(taskGroup, nullptr);

    gtObjectFactory->registerClass(GtTask::staticMetaObject);
    gtObjectFactory->registerClass(ProgressCalculator::staticMetaObject);
    ProgressCalculator::reported.store(false);

    auto task = std::make_unique<GtTask>();
    task->setObjectName(QStringLiteral("progress-task"));
    auto calculator = std::make_unique<ProgressCalculator>();
    ASSERT_TRUE(task->appendChild(calculator.release()));
    ASSERT_TRUE(taskGroup->appendChild(task.release()));

    GtHeadlessRuntimeResult result;
    const auto handle = m_runtime->submitTask(QStringLiteral("progress-task"), &result);
    ASSERT_TRUE(result.succeeded());

    QElapsedTimer timer;
    timer.start();
    while (!ProgressCalculator::reported.load() && timer.elapsed() < 5000)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        QThread::msleep(1);
    }
    ASSERT_TRUE(ProgressCalculator::reported.load());
    GtHeadlessTaskStatus progressStatus;
    timer.restart();
    while (progressStatus.progress != 42 && timer.elapsed() < 5000)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        progressStatus = handle.status();
        QThread::msleep(1);
    }
    ASSERT_EQ(progressStatus.progress, 42);
    ASSERT_TRUE(handle.cancel().succeeded());
    EXPECT_EQ(handle.wait(5000).result, GtHeadlessTaskStatus::Result::Cancelled);
}

TEST_F(TestGtHeadlessProjectRuntime, CancelsRunningTask)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    auto* taskGroup = project->processData()->taskGroup();
    ASSERT_NE(taskGroup, nullptr);

    gtObjectFactory->registerClass(GtTask::staticMetaObject);
    gtObjectFactory->registerClass(InterruptibleCalculator::staticMetaObject);
    InterruptibleCalculator::started.store(false);
    InterruptibleCalculator::release.store(false);

    auto task = std::make_unique<GtTask>();
    task->setObjectName(QStringLiteral("cancellable-task"));
    auto calculator = std::make_unique<InterruptibleCalculator>();
    ASSERT_TRUE(task->appendChild(calculator.release()));
    ASSERT_TRUE(taskGroup->appendChild(task.release()));

    GtHeadlessRuntimeResult result;
    const auto handle = m_runtime->submitTask(QStringLiteral("cancellable-task"),
                                               &result);
    ASSERT_TRUE(handle.isValid());
    ASSERT_TRUE(result.succeeded());

    QElapsedTimer timer;
    timer.start();
    while (!InterruptibleCalculator::started.load() && timer.elapsed() < 5000)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        QThread::msleep(1);
    }
    ASSERT_TRUE(InterruptibleCalculator::started.load());
    GtHeadlessTaskCancellationResult foreignThreadCancellation;
    std::thread foreignThread([&]() {
        foreignThreadCancellation = handle.cancel();
    });
    foreignThread.join();
    EXPECT_EQ(foreignThreadCancellation.code,
              GtHeadlessTaskCancellationResult::Code::WrongThread);

    const auto cancellation = handle.cancel();
    ASSERT_TRUE(cancellation.succeeded());
    EXPECT_EQ(cancellation.code,
              GtHeadlessTaskCancellationResult::Code::Accepted);
    InterruptibleCalculator::release.store(true);

    const auto status = handle.wait(5000);
    EXPECT_EQ(status.state, GtHeadlessTaskStatus::State::Cancelled);
    EXPECT_EQ(status.result, GtHeadlessTaskStatus::Result::Cancelled);
    EXPECT_FALSE(GtProjectExecutionGuard::isBusy(project));
    EXPECT_EQ(handle.cancel().code,
              GtHeadlessTaskCancellationResult::Code::AlreadyCompleted);
}

TEST_F(TestGtHeadlessProjectRuntime, LostTaskObjectBecomesTerminal)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    auto* taskGroup = project->processData()->taskGroup();
    ASSERT_NE(taskGroup, nullptr);

    auto* task = new GtTask;
    task->setObjectName(QStringLiteral("lost-task"));
    ASSERT_TRUE(taskGroup->appendChild(task));

    GtHeadlessRuntimeResult result;
    const auto handle = m_runtime->submitTask(QStringLiteral("lost-task"),
                                               &result);
    ASSERT_TRUE(handle.isValid());
    ASSERT_TRUE(result.succeeded());

    delete task;

    const auto status = handle.wait(100);
    EXPECT_EQ(status.state, GtHeadlessTaskStatus::State::Failed);
    EXPECT_EQ(status.result, GtHeadlessTaskStatus::Result::TaskUnavailable);
    EXPECT_TRUE(status.isDone());
}

TEST_F(TestGtHeadlessProjectRuntime, FailedTaskReleasesProjectGuard)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    auto* taskGroup = project->processData()->taskGroup();
    ASSERT_NE(taskGroup, nullptr);

    gtObjectFactory->registerClass(GtTask::staticMetaObject);
    gtObjectFactory->registerClass(FailingCalculator::staticMetaObject);
    auto task = std::make_unique<GtTask>();
    task->setObjectName(QStringLiteral("failing-task"));
    auto calculator = std::make_unique<FailingCalculator>();
    ASSERT_TRUE(task->appendChild(calculator.release()));
    ASSERT_TRUE(taskGroup->appendChild(task.release()));

    GtHeadlessRuntimeResult result;
    const auto handle = m_runtime->submitTask(QStringLiteral("failing-task"), &result);
    ASSERT_TRUE(result.succeeded());
    const auto status = handle.wait(5000);
    EXPECT_EQ(status.state, GtHeadlessTaskStatus::State::Failed);
    EXPECT_EQ(status.result, GtHeadlessTaskStatus::Result::ExecutionFailed);
    EXPECT_FALSE(GtProjectExecutionGuard::isBusy(project));
}

TEST_F(TestGtHeadlessProjectRuntime, ShutdownTimeoutStoresTerminalShutdownStatus)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    auto* taskGroup = project->processData()->taskGroup();
    ASSERT_NE(taskGroup, nullptr);

    gtObjectFactory->registerClass(GtTask::staticMetaObject);
    gtObjectFactory->registerClass(UncooperativeCalculator::staticMetaObject);
    UncooperativeCalculator::started.store(false);
    UncooperativeCalculator::release.store(false);

    auto task = std::make_unique<GtTask>();
    task->setObjectName(QStringLiteral("uncooperative-task"));
    auto calculator = std::make_unique<UncooperativeCalculator>();
    ASSERT_TRUE(task->appendChild(calculator.release()));
    ASSERT_TRUE(taskGroup->appendChild(task.release()));

    GtHeadlessRuntimeResult result;
    const auto handle = m_runtime->submitTask(QStringLiteral("uncooperative-task"), &result);
    ASSERT_TRUE(result.succeeded());

    QElapsedTimer timer;
    timer.start();
    while (!UncooperativeCalculator::started.load() && timer.elapsed() < 5000)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        QThread::msleep(1);
    }
    ASSERT_TRUE(UncooperativeCalculator::started.load());

    std::thread releaser([] {
        QThread::msleep(5500);
        UncooperativeCalculator::release.store(true);
    });
    m_runtime.reset();
    releaser.join();

    EXPECT_EQ(handle.status().state, GtHeadlessTaskStatus::State::Shutdown);
    EXPECT_EQ(handle.status().result,
              GtHeadlessTaskStatus::Result::RuntimeShutdown);
}

TEST_F(TestGtHeadlessProjectRuntime, SubmitsTaskByUuid)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    auto* processData = project->processData();
    ASSERT_NE(processData, nullptr);
    auto* customGroup = processData->createNewTaskGroup(
        QStringLiteral("uuid-custom"), GtTaskGroup::CUSTOM);
    ASSERT_NE(customGroup, nullptr);

    auto task = std::make_unique<GtTask>();
    task->setObjectName(QStringLiteral("uuid-task"));
    const auto taskUuid = task->uuid();
    ASSERT_TRUE(customGroup->appendChild(task.release()));
    ASSERT_TRUE(processData->switchCurrentTaskGroup(
        GtTaskGroup::defaultUserGroupId(), GtTaskGroup::USER, project->path()));

    GtHeadlessRuntimeResult result;
    const auto handle = m_runtime->submitTask(taskUuid, &result);
    ASSERT_TRUE(handle.isValid());
    ASSERT_TRUE(result.succeeded());
    EXPECT_EQ(handle.wait(5000).state, GtHeadlessTaskStatus::State::Finished);
}

TEST_F(TestGtHeadlessProjectRuntime, ListTasksRestoresCustomTaskGroup)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    auto* processData = project->processData();
    ASSERT_NE(processData, nullptr);
    auto* customGroup = processData->createNewTaskGroup(
        QStringLiteral("custom"), GtTaskGroup::CUSTOM);
    ASSERT_NE(customGroup, nullptr);
    ASSERT_TRUE(processData->switchCurrentTaskGroup(
        customGroup->objectName(), GtTaskGroup::CUSTOM, project->path()));

    auto* task = new GtTask;
    task->setObjectName(QStringLiteral("custom-task"));
    ASSERT_TRUE(customGroup->appendChild(task));

    const auto descriptors = m_runtime->listTasks();

    EXPECT_FALSE(descriptors.isEmpty());
    ASSERT_NE(processData->taskGroup(), nullptr);
    EXPECT_EQ(processData->taskGroup()->objectName(), customGroup->objectName());
}

TEST_F(TestGtHeadlessProjectRuntime, SubmitTaskRestoresCurrentTaskGroup)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    auto* processData = project->processData();
    ASSERT_NE(processData, nullptr);
    auto* customGroup = processData->createNewTaskGroup(
        QStringLiteral("custom"), GtTaskGroup::CUSTOM);
    ASSERT_NE(customGroup, nullptr);

    auto* task = new GtTask;
    task->setObjectName(QStringLiteral("custom-task"));
    ASSERT_TRUE(customGroup->appendChild(task));
    ASSERT_TRUE(processData->switchCurrentTaskGroup(
        GtTaskGroup::defaultUserGroupId(), GtTaskGroup::USER, project->path()));

    GtHeadlessRuntimeResult result;
    const auto handle = m_runtime->submitTask(QStringLiteral("custom/custom-task"),
                                               &result);
    ASSERT_TRUE(handle.isValid());
    ASSERT_TRUE(result.succeeded());
    EXPECT_EQ(handle.wait(5000).state, GtHeadlessTaskStatus::State::Finished);
    ASSERT_NE(processData->taskGroup(), nullptr);
    EXPECT_EQ(processData->taskGroup()->objectName(),
              GtTaskGroup::defaultUserGroupId());
}

TEST_F(TestGtHeadlessProjectRuntime, SaveAndCloseRejectBusyProject)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);

    GtProjectExecutionGuard guard;
    ASSERT_EQ(guard.tryAcquire(project),
              GtProjectExecutionGuard::Result::Acquired);

    EXPECT_EQ(m_runtime->saveProject().code,
              GtHeadlessRuntimeResult::Code::ProjectBusy);
    EXPECT_EQ(m_runtime->closeProject().code,
              GtHeadlessRuntimeResult::Code::ProjectBusy);
}

TEST_F(TestGtHeadlessProjectRuntime, SavesProjectSuccessfully)
{
    ASSERT_NE(openProject(), nullptr);
    EXPECT_TRUE(m_runtime->saveProject());
}

TEST_F(TestGtHeadlessProjectRuntime, CloseFailedCanBeRetried)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);

    ASSERT_TRUE(gtDataModel->closeProject(project));
    EXPECT_EQ(m_runtime->closeProject().code,
              GtHeadlessRuntimeResult::Code::CloseFailed);
    EXPECT_EQ(m_runtime->state(), GtHeadlessProjectRuntime::State::CloseFailed);

    EXPECT_TRUE(m_runtime->closeProject());
    EXPECT_EQ(m_runtime->state(), GtHeadlessProjectRuntime::State::Closed);
}

TEST_F(TestGtHeadlessProjectRuntime, RestoresExecutorFlagsAfterTask)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    auto* taskGroup = project->processData()->taskGroup();
    ASSERT_NE(taskGroup, nullptr);
    auto* executor = gt::processExecutorManager().currentExecutor();
    ASSERT_NE(executor, nullptr);
    const auto original = GtCoreProcessExecutor::Flags{gt::DryExecution};
    executor->setCoreExecutorFlags(original);

    auto* task = new GtTask;
    task->setObjectName(QStringLiteral("flags-task"));
    ASSERT_TRUE(taskGroup->appendChild(task));
    GtHeadlessRuntimeResult result;
    const auto handle = m_runtime->submitTask(QStringLiteral("flags-task"), &result);
    ASSERT_TRUE(result.succeeded());
    ASSERT_TRUE(handle.wait(5000).isDone());
    EXPECT_EQ(executor->coreExecutorFlags(), original);
}

TEST_F(TestGtHeadlessProjectRuntime, RejectsSecondProject)
{
    ASSERT_NE(openProject(), nullptr);

    const auto result = m_runtime->openProject(createProject());
    EXPECT_EQ(result.code,
              GtHeadlessRuntimeResult::Code::ProjectAlreadyLoaded);
    EXPECT_EQ(m_runtime->state(), GtHeadlessProjectRuntime::State::ProjectLoaded);
}

TEST_F(TestGtHeadlessProjectRuntime, InitializesAndShutsDownWithoutProject)
{
    m_runtime.reset();
    m_runtime = std::make_unique<GtHeadlessProjectRuntime>();
    ASSERT_TRUE(m_runtime->initialize());
    EXPECT_EQ(m_runtime->state(), GtHeadlessProjectRuntime::State::Initialized);
}

TEST_F(TestGtHeadlessProjectRuntime, ClosesProjectSuccessfully)
{
    ASSERT_NE(openProject(), nullptr);

    EXPECT_TRUE(m_runtime->closeProject());
    EXPECT_EQ(m_runtime->state(), GtHeadlessProjectRuntime::State::Closed);
    EXPECT_TRUE(m_runtime->projectPath().isEmpty());
    EXPECT_EQ(gtDataModel->currentProject(), nullptr);
}

TEST_F(TestGtHeadlessProjectRuntime, CompletedHandleRemainsUsableAfterRuntimeCleanup)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    auto* taskGroup = project->processData()->taskGroup();
    ASSERT_NE(taskGroup, nullptr);

    auto* task = new GtTask;
    task->setObjectName(QStringLiteral("retained-handle-task"));
    ASSERT_TRUE(taskGroup->appendChild(task));

    GtHeadlessRuntimeResult result;
    const auto handle = m_runtime->submitTask(QStringLiteral("retained-handle-task"),
                                               &result);
    ASSERT_TRUE(handle.isValid());
    ASSERT_TRUE(result.succeeded());
    ASSERT_EQ(handle.wait(5000).state, GtHeadlessTaskStatus::State::Finished);

    m_runtime.reset();

    EXPECT_EQ(handle.status().state, GtHeadlessTaskStatus::State::Finished);
    EXPECT_EQ(handle.status().result, GtHeadlessTaskStatus::Result::Succeeded);
}

#include "test_gt_headlessprojectruntime.moc"
