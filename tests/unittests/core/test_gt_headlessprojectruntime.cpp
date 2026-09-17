/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_coreapplication.h"
#include "gt_coredatamodel.h"
#include "gt_executioncontext.h"
#include "gt_externalizationmanager.h"
#include "gt_executableoperation.h"
#include "gt_headlessprojectruntime.h"
#include "gt_object.h"
#include "gt_project.h"
#include "gt_projectexecutionguard.h"
#include "gt_testhelper.h"

#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QThread>

#include <atomic>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

namespace
{
class TestOperation final : public GtExecutableOperation
{
    Q_OBJECT
public:
    Q_INVOKABLE TestOperation() = default;
    bool requiresProject() const override { return s_requiresProject; }
    std::unique_ptr<GtObject> createData(GtExecutionContext const&) const override
    {
        return {};
    }
    std::unique_ptr<GtObject> execute(GtOperationExecutionContext& context) override
    {
        s_started.store(true);
        s_projectContext.store(GtExecutionContext::current() &&
                               GtExecutionContext::current()->project());
        context.events().publish(QStringLiteral("started"));
        context.events().publish(QStringLiteral("progress"));
        while (s_block.load() && !context.cancellation().isCancellationRequested())
        {
            QThread::msleep(1);
        }
        if (s_throw.load()) throw std::runtime_error("expected operation failure");
        auto result = std::make_unique<GtObject>();
        result->setObjectName(QStringLiteral("detached-result"));
        return result;
    }
    GtOperationApplyStatus applyResult(GtObject const*, GtExecutionContext&) const override
    {
        return GtOperationApplyStatus::success();
    }

    static std::atomic_bool s_requiresProject;
    static std::atomic_bool s_started;
    static std::atomic_bool s_block;
    static std::atomic_bool s_throw;
    static std::atomic_bool s_projectContext;
};

std::atomic_bool TestOperation::s_requiresProject{false};
std::atomic_bool TestOperation::s_started{false};
std::atomic_bool TestOperation::s_block{false};
std::atomic_bool TestOperation::s_throw{false};
std::atomic_bool TestOperation::s_projectContext{false};

class TestGtHeadlessProjectRuntime : public ::testing::Test
{
protected:
    void SetUp() override
    {
        TestOperation::s_requiresProject.store(false);
        TestOperation::s_started.store(false);
        TestOperation::s_block.store(false);
        TestOperation::s_throw.store(false);
        TestOperation::s_projectContext.store(false);
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

    void drainUntilDone(GtHeadlessOperationHandle const& handle)
    {
        QElapsedTimer timer;
        timer.start();
        while (!handle.status().isDone() && timer.elapsed() < 5000)
        {
            QCoreApplication::processEvents();
            QThread::msleep(1);
        }
    }

    QString createProject() const
    {
        const auto directory = gtTestHelper->newTempDir();
        QFile file(directory.filePath(GtProject::mainFilename()));
        EXPECT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
        file.write("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                   "<GTLAB projectname=\"headless-test\" version=\"1.7.0-rc1\">\n"
                   "<env-footprint><core-ver>2.0.0</core-ver><modules/></env-footprint>\n"
                   "<comment/><MODULES/><PROCESSES/><LABELS/></GTLAB>\n");
        file.close();
        return directory.absolutePath();
    }

    GtProject* openProject()
    {
        EXPECT_TRUE(m_runtime->openProject(createProject()));
        return gtDataModel->currentProject();
    }

    std::unique_ptr<GtCoreApplication> m_application;
    std::unique_ptr<GtHeadlessProjectRuntime> m_runtime;
};
}

TEST(GtHeadlessOperationHandle, DefaultHandleIsInvalid)
{
    GtHeadlessOperationHandle handle;
    EXPECT_FALSE(handle.isValid());
    EXPECT_TRUE(handle.id().isEmpty());
    EXPECT_FALSE(handle.status().isDone());
    EXPECT_FALSE(handle.cancel().succeeded());
}

TEST_F(TestGtHeadlessProjectRuntime, SubmitsAndRunsDetachedOperation)
{
    GtHeadlessRuntimeResult result;
    auto handle = m_runtime->submitOperation(std::make_unique<TestOperation>(), {}, &result);
    ASSERT_TRUE(result.succeeded());
    ASSERT_TRUE(handle.isValid());
    EXPECT_EQ(handle.status().state, GtHeadlessOperationStatus::State::Queued);
    const auto rejectedWait = handle.wait(0);
    EXPECT_TRUE(rejectedWait.waitRejected);
    drainUntilDone(handle);
    ASSERT_EQ(handle.status().state, GtHeadlessOperationStatus::State::Finished);
    EXPECT_EQ(handle.status().result, GtHeadlessOperationStatus::Result::Succeeded);
    ASSERT_TRUE(handle.result());
    EXPECT_EQ(handle.result()->objectName(), QStringLiteral("detached-result"));
    EXPECT_EQ(handle.events()->executionId().toString(), handle.id());
}

TEST_F(TestGtHeadlessProjectRuntime, PublishesOrderedOperationEvents)
{
    auto handle = m_runtime->submitOperation(std::make_unique<TestOperation>());
    std::vector<GtExecutionEvent> events;
    QObject::connect(handle.events().data(), &GtExecutionEventStream::eventPublished,
                     [&](GtExecutionEvent event) { events.push_back(std::move(event)); });
    drainUntilDone(handle);
    ASSERT_EQ(events.size(), 2u);
    EXPECT_EQ(events[0].sequence(), 0u);
    EXPECT_EQ(events[1].sequence(), 1u);
    EXPECT_EQ(events[0].executionId(), handle.events()->executionId());
}

TEST_F(TestGtHeadlessProjectRuntime, HandleCopiesShareTerminalSnapshotAndResult)
{
    auto handle = m_runtime->submitOperation(std::make_unique<TestOperation>());
    auto copy = handle;
    drainUntilDone(handle);
    EXPECT_EQ(copy.status().state, GtHeadlessOperationStatus::State::Finished);
    ASSERT_TRUE(copy.result());
    EXPECT_EQ(copy.result()->objectName(), QStringLiteral("detached-result"));
}

TEST_F(TestGtHeadlessProjectRuntime, ReportsStructuredOperationFailure)
{
    TestOperation::s_throw.store(true);
    auto handle = m_runtime->submitOperation(std::make_unique<TestOperation>());
    drainUntilDone(handle);
    const auto status = handle.status();
    EXPECT_EQ(status.state, GtHeadlessOperationStatus::State::Failed);
    EXPECT_EQ(status.result, GtHeadlessOperationStatus::Result::ExecutionFailed);
    EXPECT_NE(status.error.indexOf(QStringLiteral("expected operation failure")), -1);
}

TEST_F(TestGtHeadlessProjectRuntime, CancellationCanBeRequestedFromAnotherThread)
{
    TestOperation::s_block.store(true);
    auto handle = m_runtime->submitOperation(std::make_unique<TestOperation>());
    std::thread cancelWhenStarted([handle] {
        while (!TestOperation::s_started.load()) QThread::msleep(1);
        handle.cancel();
    });
    drainUntilDone(handle);
    cancelWhenStarted.join();
    EXPECT_EQ(handle.status().state, GtHeadlessOperationStatus::State::Cancelled);
}

TEST_F(TestGtHeadlessProjectRuntime, CancellationBeforeStartSkipsExecution)
{
    auto handle = m_runtime->submitOperation(std::make_unique<TestOperation>());
    ASSERT_TRUE(handle.cancel());
    QCoreApplication::processEvents();
    EXPECT_EQ(handle.status().state, GtHeadlessOperationStatus::State::Cancelled);
    EXPECT_FALSE(TestOperation::s_started.load());
}

TEST_F(TestGtHeadlessProjectRuntime, ShutdownMakesQueuedHandleTerminal)
{
    auto handle = m_runtime->submitOperation(std::make_unique<TestOperation>());
    m_runtime.reset();
    EXPECT_EQ(handle.status().state, GtHeadlessOperationStatus::State::Shutdown);
    EXPECT_EQ(handle.status().result,
              GtHeadlessOperationStatus::Result::RuntimeShutdown);
}

TEST_F(TestGtHeadlessProjectRuntime, ForeignThreadCanWaitForCompletion)
{
    auto handle = m_runtime->submitOperation(std::make_unique<TestOperation>());
    GtHeadlessOperationStatus waited;
    std::thread waiter([&] { waited = handle.wait(5000); });
    drainUntilDone(handle);
    waiter.join();
    EXPECT_EQ(waited.state, GtHeadlessOperationStatus::State::Finished);
}

TEST_F(TestGtHeadlessProjectRuntime, RejectsProjectRequiredOperationWithoutProject)
{
    TestOperation::s_requiresProject.store(true);
    GtHeadlessRuntimeResult result;
    const auto handle = m_runtime->submitOperation(std::make_unique<TestOperation>(), {}, &result);
    EXPECT_FALSE(handle.isValid());
    EXPECT_EQ(result.code, GtHeadlessRuntimeResult::Code::OperationRejected);
}

TEST_F(TestGtHeadlessProjectRuntime, PreservesSingleProjectLifecycle)
{
    ASSERT_NE(openProject(), nullptr);
    EXPECT_EQ(m_runtime->state(), GtHeadlessProjectRuntime::State::ProjectLoaded);
    EXPECT_EQ(m_runtime->openProject(createProject()).code,
              GtHeadlessRuntimeResult::Code::ProjectAlreadyLoaded);
    EXPECT_TRUE(m_runtime->saveProject());
    EXPECT_TRUE(m_runtime->closeProject());
    EXPECT_EQ(m_runtime->state(), GtHeadlessProjectRuntime::State::Closed);
    EXPECT_EQ(gtDataModel->currentProject(), nullptr);
}

TEST_F(TestGtHeadlessProjectRuntime, SaveAndCloseRejectProjectGuard)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    GtProjectExecutionGuard guard;
    ASSERT_EQ(guard.tryAcquire(project), GtProjectExecutionGuard::Result::Acquired);
    EXPECT_EQ(m_runtime->saveProject().code, GtHeadlessRuntimeResult::Code::ProjectBusy);
    EXPECT_EQ(m_runtime->closeProject().code, GtHeadlessRuntimeResult::Code::ProjectBusy);
}

TEST_F(TestGtHeadlessProjectRuntime, ProjectOperationReceivesScopedExecutionContext)
{
    ASSERT_NE(openProject(), nullptr);
    TestOperation::s_requiresProject.store(true);
    auto handle = m_runtime->submitOperation(std::make_unique<TestOperation>());
    ASSERT_TRUE(handle.isValid());
    drainUntilDone(handle);
    EXPECT_EQ(handle.status().state, GtHeadlessOperationStatus::State::Finished);
    EXPECT_TRUE(TestOperation::s_projectContext.load());
}

#include "test_gt_headlessprojectruntime.moc"
