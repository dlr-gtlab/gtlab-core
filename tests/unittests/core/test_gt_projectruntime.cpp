/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_coreapplication.h"
#include "gt_coredatamodel.h"
#include "gt_executioncontext.h"
#include "gt_executioneventstream.h"
#include "gt_executableoperation.h"
#include "gt_externalizationmanager.h"
#include "gt_object.h"
#include "gt_project.h"
#include "gt_projectexecutionguard.h"
#include "gt_projectruntime.h"
#include "gt_testhelper.h"

#include <QCoreApplication>
#include <QDir>
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
    bool requiresProject() const override { return s_requiresProject.load(); }
    std::unique_ptr<GtObject> createData(GtExecutionContext const&) const override
    {
        return {};
    }
    std::unique_ptr<GtObject> execute(GtOperationExecutionContext& context) override
    {
        s_started.store(true);
        s_projectContext.store(GtExecutionContext::current() &&
                               GtExecutionContext::current()->project());
        s_receivedData.store(context.data() != nullptr);
        context.events().publish(QStringLiteral("started"));
        context.events().publish(QStringLiteral("progress"));
        while (s_block.load() && !context.cancellation().isCancellationRequested())
        {
            QThread::msleep(1);
        }
        s_cancelObserved.store(context.cancellation().isCancellationRequested());
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
    static std::atomic_bool s_receivedData;
    static std::atomic_bool s_cancelObserved;
};

std::atomic_bool TestOperation::s_requiresProject{false};
std::atomic_bool TestOperation::s_started{false};
std::atomic_bool TestOperation::s_block{false};
std::atomic_bool TestOperation::s_throw{false};
std::atomic_bool TestOperation::s_projectContext{false};
std::atomic_bool TestOperation::s_receivedData{false};
std::atomic_bool TestOperation::s_cancelObserved{false};

class TestGtProjectRuntime : public ::testing::Test
{
protected:
    void SetUp() override
    {
        TestOperation::s_requiresProject.store(false);
        TestOperation::s_started.store(false);
        TestOperation::s_block.store(false);
        TestOperation::s_throw.store(false);
        TestOperation::s_projectContext.store(false);
        TestOperation::s_receivedData.store(false);
        TestOperation::s_cancelObserved.store(false);
        m_application = std::make_unique<GtCoreApplication>(
            QCoreApplication::instance(), GtCoreApplication::AppMode::Batch);
        m_application->init();
        m_runtime = std::make_unique<GtProjectRuntime>();
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
        file.write("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                   "<GTLAB projectname=\"runtime-test\" version=\"1.7.0-rc1\">\n"
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

    GtExecutionEventStream events{GtExecutionId{}};
    std::unique_ptr<GtCoreApplication> m_application;
    std::unique_ptr<GtProjectRuntime> m_runtime;
};
}

TEST_F(TestGtProjectRuntime, InitializesWithoutProjectAndExecutesSynchronously)
{
    auto data = std::make_unique<GtObject>();
    auto result = m_runtime->executeOperation(std::make_unique<TestOperation>(),
                                               std::move(data), events);
    ASSERT_TRUE(result);
    ASSERT_TRUE(result.result);
    EXPECT_EQ(result.result->objectName(), QStringLiteral("detached-result"));
    EXPECT_TRUE(TestOperation::s_started.load());
    EXPECT_TRUE(TestOperation::s_receivedData.load());
    EXPECT_FALSE(TestOperation::s_projectContext.load());
    EXPECT_EQ(m_runtime->state(), GtProjectRuntime::State::Initialized);
}

TEST_F(TestGtProjectRuntime, CloseWithoutProjectIsSafe)
{
    EXPECT_EQ(m_runtime->state(), GtProjectRuntime::State::Initialized);
    EXPECT_EQ(m_runtime->closeProject().code, GtProjectRuntimeResult::Code::InvalidState);
    m_runtime.reset();
}

TEST_F(TestGtProjectRuntime, PublishesOrderedEventsToSuppliedStream)
{
    std::vector<GtExecutionEvent> observed;
    QObject::connect(&events, &GtExecutionEventStream::eventPublished,
                     [&](GtExecutionEvent event) { observed.push_back(std::move(event)); });
    EXPECT_TRUE(m_runtime->executeOperation(std::make_unique<TestOperation>(), {}, events));
    ASSERT_EQ(observed.size(), 2u);
    EXPECT_EQ(observed[0].sequence(), 0u);
    EXPECT_EQ(observed[1].sequence(), 1u);
    EXPECT_EQ(observed[0].executionId(), events.executionId());
}

TEST_F(TestGtProjectRuntime, ReportsStructuredFailure)
{
    TestOperation::s_throw.store(true);
    auto result = m_runtime->executeOperation(std::make_unique<TestOperation>(), {}, events);
    EXPECT_EQ(result.code, GtProjectRuntimeExecutionResult::Code::ExecutionFailed);
    EXPECT_TRUE(result.message.contains(QStringLiteral("expected operation failure")));
    EXPECT_FALSE(result.result);
}

TEST_F(TestGtProjectRuntime, SeesSuppliedCancellationWhileExecuteBlocks)
{
    TestOperation::s_block.store(true);
    GtCancellationToken cancellation;
    std::thread requester([&] {
        while (!TestOperation::s_started.load()) QThread::msleep(1);
        cancellation.requestCancellation();
    });
    auto result = m_runtime->executeOperation(std::make_unique<TestOperation>(), {},
                                               events, cancellation);
    requester.join();
    EXPECT_TRUE(result);
    EXPECT_TRUE(TestOperation::s_cancelObserved.load());
}

TEST_F(TestGtProjectRuntime, RejectsProjectRequiredOperationWithoutProject)
{
    TestOperation::s_requiresProject.store(true);
    auto result = m_runtime->executeOperation(std::make_unique<TestOperation>(), {}, events);
    EXPECT_EQ(result.code, GtProjectRuntimeExecutionResult::Code::ProjectRequired);
    EXPECT_FALSE(TestOperation::s_started.load());
}

TEST_F(TestGtProjectRuntime, PreservesSingleProjectLifecycle)
{
    ASSERT_NE(openProject(), nullptr);
    EXPECT_EQ(m_runtime->state(), GtProjectRuntime::State::ProjectLoaded);
    EXPECT_EQ(m_runtime->openProject(createProject()).code,
              GtProjectRuntimeResult::Code::ProjectAlreadyLoaded);
    EXPECT_TRUE(m_runtime->saveProject());
    EXPECT_TRUE(m_runtime->closeProject());
    EXPECT_EQ(m_runtime->state(), GtProjectRuntime::State::Closed);
    EXPECT_EQ(gtDataModel->currentProject(), nullptr);
}

TEST_F(TestGtProjectRuntime, SaveAndCloseRejectProjectGuard)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    GtProjectExecutionGuard guard;
    ASSERT_EQ(guard.tryAcquire(project), GtProjectExecutionGuard::Result::Acquired);
    EXPECT_EQ(m_runtime->saveProject().code, GtProjectRuntimeResult::Code::ProjectBusy);
    EXPECT_EQ(m_runtime->closeProject().code, GtProjectRuntimeResult::Code::ProjectBusy);
}

TEST_F(TestGtProjectRuntime, ProjectExecutionScopesContextAndReleasesGuard)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    TestOperation::s_requiresProject.store(true);
    GtProjectRuntimeResult saveDuringExecution;
    GtProjectRuntimeResult closeDuringExecution;
    QObject::connect(&events, &GtExecutionEventStream::eventPublished,
                     [&](GtExecutionEvent) {
        saveDuringExecution = m_runtime->saveProject();
        closeDuringExecution = m_runtime->closeProject();
    });
    auto result = m_runtime->executeOperation(std::make_unique<TestOperation>(), {}, events);
    EXPECT_TRUE(result);
    EXPECT_TRUE(TestOperation::s_projectContext.load());
    EXPECT_EQ(saveDuringExecution.code, GtProjectRuntimeResult::Code::ProjectBusy);
    EXPECT_EQ(closeDuringExecution.code, GtProjectRuntimeResult::Code::ProjectBusy);
    EXPECT_FALSE(GtProjectExecutionGuard::isLocked(project));
    EXPECT_EQ(GtExecutionContext::current(), nullptr);
}

TEST_F(TestGtProjectRuntime, FailureReleasesProjectGuardAndContext)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    TestOperation::s_requiresProject.store(true);
    TestOperation::s_throw.store(true);
    auto result = m_runtime->executeOperation(std::make_unique<TestOperation>(), {}, events);
    EXPECT_EQ(result.code, GtProjectRuntimeExecutionResult::Code::ExecutionFailed);
    EXPECT_FALSE(GtProjectExecutionGuard::isLocked(project));
    EXPECT_EQ(GtExecutionContext::current(), nullptr);
}

TEST_F(TestGtProjectRuntime, RejectsNullOperation)
{
    auto result = m_runtime->executeOperation({}, {}, events);
    EXPECT_EQ(result.code, GtProjectRuntimeExecutionResult::Code::InvalidOperation);
}

TEST_F(TestGtProjectRuntime, RejectsBusyProjectBeforeExecuting)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    TestOperation::s_requiresProject.store(true);
    GtProjectExecutionGuard guard;
    ASSERT_EQ(guard.tryAcquire(project), GtProjectExecutionGuard::Result::Acquired);
    auto result = m_runtime->executeOperation(std::make_unique<TestOperation>(), {}, events);
    EXPECT_EQ(result.code, GtProjectRuntimeExecutionResult::Code::ProjectBusy);
    EXPECT_FALSE(TestOperation::s_started.load());
}

TEST_F(TestGtProjectRuntime, CancellationReleasesProjectGuardAndContext)
{
    auto* project = openProject();
    ASSERT_NE(project, nullptr);
    TestOperation::s_requiresProject.store(true);
    TestOperation::s_block.store(true);
    GtCancellationToken cancellation;
    std::thread requester([&] {
        while (!TestOperation::s_started.load()) QThread::msleep(1);
        cancellation.requestCancellation();
    });
    auto result = m_runtime->executeOperation(std::make_unique<TestOperation>(), {},
                                               events, cancellation);
    requester.join();
    EXPECT_TRUE(result);
    EXPECT_TRUE(TestOperation::s_cancelObserved.load());
    EXPECT_FALSE(GtProjectExecutionGuard::isLocked(project));
    EXPECT_EQ(GtExecutionContext::current(), nullptr);
}

#include "test_gt_projectruntime.moc"
