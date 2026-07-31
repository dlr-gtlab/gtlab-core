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
        while (!release.load())
        {
            QThread::msleep(1);
        }
        return true;
    }
};

std::atomic_bool InterruptibleCalculator::started{false};
std::atomic_bool InterruptibleCalculator::release{false};

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
    EXPECT_FALSE(handle.cancel());
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
    ASSERT_TRUE(handle.cancel());
    InterruptibleCalculator::release.store(true);

    EXPECT_EQ(handle.wait(5000).state, GtHeadlessTaskStatus::State::Cancelled);
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
    EXPECT_TRUE(status.isDone());
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
}

#include "test_gt_headlessprojectruntime.moc"
