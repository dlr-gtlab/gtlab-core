/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_coreapplication.h"
#include "gt_coredatamodel.h"
#include "gt_executioncontext.h"
#include "gt_executionenvironment.h"
#include "gt_project.h"
#include "operations/gt_executioneventstream.h"
#include "operations/gt_executableoperation.h"

#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>

#include <QCoreApplication>
#include <QMetaObject>
#include <QThread>

namespace
{

class TestProject final : public GtProject
{
public:
    explicit TestProject(QString path = {}) : GtProject(std::move(path))
    {
    }
};

class TestCoreApplication final : public GtCoreApplication
{
public:
    TestCoreApplication() :
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

class ExecutionEnvironmentTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_application = std::make_unique<TestCoreApplication>();
    }

    void TearDown() override
    {
        m_application.reset();
    }

private:
    std::unique_ptr<TestCoreApplication> m_application;
};

class ProbeOperation final : public GtExecutableOperation
{
public:
    explicit ProbeOperation(bool requiresProject = false) :
        GtExecutableOperation(nullptr), m_requiresProject(requiresProject)
    {
    }

    bool requiresProject() const override
    {
        return m_requiresProject;
    }

    std::unique_ptr<GtObject> createData(
        GtExecutionContext const&) const override
    {
        return nullptr;
    }

    GtOperationExecutionResult execute(
        GtOperationExecutionContext& context) override
    {
        ++executeCount;
        executionThread = QThread::currentThread();
        observedData = context.data();
        observedEvents = &context.events();
        auto const* executionContext = GtExecutionContext::current();
        observedExecutionContextWasActive = executionContext != nullptr;
        observedExecutionProject = executionContext
                                       ? executionContext->project()
                                       : nullptr;
        observedApplicationProject = gtApp->currentProject();
        observedDatamodelProject = gtDataModel->currentProject();
        observedCancellationBefore =
            context.cancellation().isCancellationRequested();

        if (executeBehavior)
        {
            return executeBehavior(context);
        }

        return {};
    }

    GtOperationApplyStatus applyResult(
        GtOperationExecutionResult const& executionResult,
        GtExecutionContext&) const override
    {
        appliedStatus = executionResult.status;
        appliedCode = executionResult.code;
        appliedMessage = executionResult.message;
        appliedHasPayload = executionResult.result != nullptr;
        return GtOperationApplyStatus::success();
    }

    std::function<GtOperationExecutionResult(GtOperationExecutionContext&)>
        executeBehavior;
    int executeCount{0};
    QThread* executionThread{nullptr};
    GtObject* observedData{nullptr};
    GtExecutionEventStream* observedEvents{nullptr};
    bool observedExecutionContextWasActive{false};
    GtProject* observedExecutionProject{nullptr};
    GtProject* observedApplicationProject{nullptr};
    GtProject* observedDatamodelProject{nullptr};
    bool observedCancellationBefore{false};

    mutable GtOperationExecutionResult::Status appliedStatus{
        GtOperationExecutionResult::Status::Success};
    mutable QString appliedCode;
    mutable QString appliedMessage;
    mutable bool appliedHasPayload{false};

private:
    bool m_requiresProject;
};

class ThreadAffinityTransfer final
{
public:
    explicit ThreadAffinityTransfer(QObject& object) :
        m_object(&object), m_originalThread(object.thread())
    {
        object.moveToThread(&m_thread);
        m_thread.start();
    }

    ~ThreadAffinityTransfer()
    {
        QMetaObject::invokeMethod(
            m_object,
            [this]() { m_object->moveToThread(m_originalThread); },
            Qt::BlockingQueuedConnection);
        m_thread.quit();
        m_thread.wait();
    }

    ThreadAffinityTransfer(ThreadAffinityTransfer const&) = delete;
    ThreadAffinityTransfer& operator=(ThreadAffinityTransfer const&) = delete;

private:
    QObject* m_object;
    QThread* m_originalThread;
    QThread m_thread;
};

GtOperationExecutionResult outcome(GtOperationExecutionResult::Status status,
                                   bool withPayload)
{
    return {status, QStringLiteral("test-code"),
            QStringLiteral("test-message"),
            withPayload ? std::make_unique<GtObject>() : nullptr};
}

} // namespace

TEST_F(ExecutionEnvironmentTest, ExecutesWithoutAProject)
{
    GtExecutionEnvironment environment;
    ProbeOperation operation;
    GtExecutionEventStream events(GtExecutionId{});

    auto result = environment.execute(operation, nullptr, events);

    ASSERT_EQ(result.error(), GtExecutionResult::Error::None);
    EXPECT_TRUE(result.message().isEmpty());
    ASSERT_NE(result.operationResult(), nullptr);
    EXPECT_EQ(result.operationResult()->status,
              GtOperationExecutionResult::Status::Success);
    EXPECT_EQ(operation.executeCount, 1);
    EXPECT_EQ(environment.project(), nullptr);
}

TEST_F(ExecutionEnvironmentTest, BorrowsProjectWithoutTakingOwnership)
{
    auto project = std::make_unique<TestProject>();
    TestProject* const borrowedProject = project.get();
    {
        GtExecutionEnvironment environment(borrowedProject);
        EXPECT_EQ(environment.project(), borrowedProject);
        EXPECT_EQ(static_cast<GtExecutionEnvironment const&>(environment)
                      .project(),
                  borrowedProject);
    }
    EXPECT_EQ(project.get(), borrowedProject);
}

TEST_F(ExecutionEnvironmentTest,
       ProjectIndependentOperationGetsAnEmptyContextWithoutInspectingProject)
{
    TestProject environmentProject;
    TestProject surroundingProject;
    GtExecutionContext surroundingContext(&surroundingProject);
    GtExecutionContextScope surroundingScope(surroundingContext);
    ThreadAffinityTransfer projectOnOtherThread(environmentProject);

    GtExecutionEnvironment environment(&environmentProject);
    ProbeOperation operation;
    GtExecutionEventStream events(GtExecutionId{});

    auto result = environment.execute(operation, nullptr, events);

    ASSERT_EQ(result.error(), GtExecutionResult::Error::None);
    ASSERT_NE(result.operationResult(), nullptr);
    EXPECT_EQ(operation.executeCount, 1);
    EXPECT_TRUE(operation.observedExecutionContextWasActive);
    EXPECT_EQ(operation.observedExecutionProject, nullptr);
    EXPECT_EQ(operation.observedApplicationProject, nullptr);
    EXPECT_EQ(operation.observedDatamodelProject, nullptr);
    EXPECT_EQ(GtExecutionContext::current(), &surroundingContext);
}

TEST_F(ExecutionEnvironmentTest,
       RequiredProjectWithoutEnvironmentProjectIsError)
{
    GtExecutionEnvironment environment;
    ProbeOperation operation(true);
    GtExecutionEventStream events(GtExecutionId{});

    auto result = environment.execute(operation, nullptr, events);

    EXPECT_EQ(result.error(), GtExecutionResult::Error::ProjectRequired);
    EXPECT_NE(result.message().size(), 0);
    EXPECT_EQ(result.operationResult(), nullptr);
    EXPECT_EQ(operation.executeCount, 0);
}

TEST_F(ExecutionEnvironmentTest, RequiredOperationSeesExecutionLocalProject)
{
    TestProject project;
    GtExecutionEnvironment environment(&project);
    ProbeOperation operation(true);
    GtExecutionEventStream events(GtExecutionId{});

    auto result = environment.execute(operation, nullptr, events);

    ASSERT_EQ(result.error(), GtExecutionResult::Error::None);
    EXPECT_EQ(operation.observedExecutionProject, &project);
    EXPECT_EQ(operation.observedApplicationProject, &project);
    EXPECT_EQ(operation.observedDatamodelProject, &project);
    EXPECT_EQ(GtExecutionContext::current(), nullptr);
}

TEST_F(ExecutionEnvironmentTest, ReusesEnvironmentSequentiallyForSameProject)
{
    TestProject project;
    GtExecutionEnvironment environment(&project);
    ProbeOperation operation(true);
    GtExecutionEventStream firstEvents(GtExecutionId{});
    GtExecutionEventStream secondEvents(GtExecutionId{});

    auto first = environment.execute(operation, nullptr, firstEvents);
    auto second = environment.execute(operation, nullptr, secondEvents);

    EXPECT_EQ(first.error(), GtExecutionResult::Error::None);
    EXPECT_EQ(second.error(), GtExecutionResult::Error::None);
    EXPECT_EQ(operation.executeCount, 2);
    EXPECT_EQ(operation.observedExecutionProject, &project);
    EXPECT_EQ(GtExecutionContext::current(), nullptr);
}

TEST_F(ExecutionEnvironmentTest, RunsSynchronouslyInCallingThreadAndBorrowsData)
{
    GtExecutionEnvironment environment;
    ProbeOperation operation;
    GtObject data;
    GtExecutionEventStream events(GtExecutionId{});

    auto result = environment.execute(operation, &data, events);

    ASSERT_EQ(result.error(), GtExecutionResult::Error::None);
    EXPECT_EQ(operation.executionThread, QThread::currentThread());
    EXPECT_EQ(operation.observedData, &data);
    EXPECT_EQ(&events, operation.observedEvents);
    EXPECT_FALSE(operation.observedCancellationBefore);
}

TEST_F(ExecutionEnvironmentTest, OptionalInvocationDataMayBeNull)
{
    GtExecutionEnvironment environment;
    ProbeOperation operation;
    GtExecutionEventStream events(GtExecutionId{});

    auto result = environment.execute(operation, nullptr, events);

    ASSERT_EQ(result.error(), GtExecutionResult::Error::None);
    EXPECT_EQ(operation.observedData, nullptr);
}

TEST_F(ExecutionEnvironmentTest, OperationPublishesToSuppliedEventStream)
{
    GtExecutionEnvironment environment;
    ProbeOperation operation;
    GtExecutionEventStream events(GtExecutionId{});
    QList<GtExecutionEvent> published;
    QObject::connect(&events, &GtExecutionEventStream::eventPublished,
                     [&published](GtExecutionEvent event) {
                         published.push_back(std::move(event));
                     });
    operation.executeBehavior = [&events](
                                    GtOperationExecutionContext& context) {
        EXPECT_EQ(&context.events(), &events);
        context.events().publish(QStringLiteral("progress"));
        return GtOperationExecutionResult{};
    };

    auto result = environment.execute(operation, nullptr, events);

    ASSERT_EQ(result.error(), GtExecutionResult::Error::None);
    ASSERT_EQ(published.size(), 1);
    EXPECT_EQ(published.front().eventType(), QStringLiteral("progress"));
    EXPECT_EQ(published.front().sequence(), 0);
    EXPECT_EQ(published.front().executionId().toString(),
              events.executionId().toString());
}

TEST_F(ExecutionEnvironmentTest,
       PreservesEveryOperationStatusWithAndWithoutPayload)
{
    const GtOperationExecutionResult::Status statuses[] = {
        GtOperationExecutionResult::Status::Success,
        GtOperationExecutionResult::Status::Failed,
        GtOperationExecutionResult::Status::Cancelled};

    for (auto status : statuses)
    {
        for (bool withPayload : {false, true})
        {
            GtExecutionEnvironment environment;
            ProbeOperation operation;
            GtExecutionEventStream events(GtExecutionId{});
            operation.executeBehavior = [status, withPayload](
                                            GtOperationExecutionContext&) {
                return outcome(status, withPayload);
            };

            auto result = environment.execute(operation, nullptr, events);

            ASSERT_EQ(result.error(), GtExecutionResult::Error::None);
            ASSERT_NE(result.operationResult(), nullptr);
            EXPECT_EQ(result.operationResult()->status, status);
            EXPECT_EQ(result.operationResult()->code,
                      QStringLiteral("test-code"));
            EXPECT_EQ(result.operationResult()->message,
                      QStringLiteral("test-message"));
            EXPECT_EQ(result.operationResult()->result != nullptr,
                      withPayload);
        }
    }
}

TEST_F(ExecutionEnvironmentTest,
       PreCancelledInvocationDoesNotCallOperationAndReturnsCancelled)
{
    GtExecutionEnvironment environment;
    ProbeOperation operation;
    GtExecutionEventStream events(GtExecutionId{});
    GtCancellationToken cancellation;
    cancellation.requestCancellation();

    auto result = environment.execute(operation, nullptr, events, cancellation);

    ASSERT_EQ(result.error(), GtExecutionResult::Error::None);
    ASSERT_NE(result.operationResult(), nullptr);
    EXPECT_EQ(result.operationResult()->status,
              GtOperationExecutionResult::Status::Cancelled);
    EXPECT_EQ(result.operationResult()->result, nullptr);
    EXPECT_EQ(operation.executeCount, 0);
}

TEST_F(ExecutionEnvironmentTest,
       CancellationDuringExecutionIsVisibleAndDoesNotRewriteOutcome)
{
    for (auto status : {GtOperationExecutionResult::Status::Success,
                        GtOperationExecutionResult::Status::Failed})
    {
        GtExecutionEnvironment environment;
        ProbeOperation operation;
        GtExecutionEventStream events(GtExecutionId{});
        GtCancellationToken cancellation;
        operation.executeBehavior = [status](
                                        GtOperationExecutionContext& context) {
            context.cancellation().requestCancellation();
            EXPECT_TRUE(context.cancellation().isCancellationRequested());
            return outcome(status, false);
        };

        auto result = environment.execute(operation, nullptr, events,
                                          cancellation);

        ASSERT_EQ(result.error(), GtExecutionResult::Error::None);
        ASSERT_NE(result.operationResult(), nullptr);
        EXPECT_EQ(result.operationResult()->status, status);
        EXPECT_TRUE(cancellation.isCancellationRequested());
    }
}

TEST_F(ExecutionEnvironmentTest, ConvertsEscapingExceptionToBoundaryFailure)
{
    GtExecutionEnvironment environment;
    ProbeOperation operation;
    operation.executeBehavior = [](GtOperationExecutionContext&)
        -> GtOperationExecutionResult {
        throw std::runtime_error("operation failed");
    };
    GtExecutionEventStream events(GtExecutionId{});

    auto result = environment.execute(operation, nullptr, events);

    EXPECT_EQ(result.error(), GtExecutionResult::Error::UnhandledException);
    EXPECT_EQ(result.message(), QStringLiteral("operation failed"));
    EXPECT_EQ(result.operationResult(), nullptr);
}

TEST_F(ExecutionEnvironmentTest, ConvertsUnknownExceptionToBoundaryFailure)
{
    GtExecutionEnvironment environment;
    ProbeOperation operation;
    operation.executeBehavior = [](GtOperationExecutionContext&)
        -> GtOperationExecutionResult {
        throw 42;
    };
    GtExecutionEventStream events(GtExecutionId{});

    auto result = environment.execute(operation, nullptr, events);

    EXPECT_EQ(result.error(), GtExecutionResult::Error::UnhandledException);
    EXPECT_FALSE(result.message().isEmpty());
    EXPECT_EQ(result.operationResult(), nullptr);
}

TEST_F(ExecutionEnvironmentTest,
       RejectsOperationOnAnotherThreadWithoutMarshalling)
{
    GtExecutionEnvironment environment;
    ProbeOperation operation;
    ThreadAffinityTransfer operationOnOtherThread(operation);
    GtExecutionEventStream events(GtExecutionId{});

    auto result = environment.execute(operation, nullptr, events);

    EXPECT_EQ(result.error(), GtExecutionResult::Error::WrongThread);
    EXPECT_EQ(result.operationResult(), nullptr);
    EXPECT_EQ(operation.executeCount, 0);
}

TEST_F(ExecutionEnvironmentTest, RejectsDetachedDataOnAnotherThread)
{
    GtExecutionEnvironment environment;
    ProbeOperation operation;
    GtObject data;
    ThreadAffinityTransfer dataOnOtherThread(data);
    GtExecutionEventStream events(GtExecutionId{});

    auto result = environment.execute(operation, &data, events);

    EXPECT_EQ(result.error(), GtExecutionResult::Error::WrongThread);
    EXPECT_EQ(result.operationResult(), nullptr);
    EXPECT_EQ(operation.executeCount, 0);
}

TEST_F(ExecutionEnvironmentTest, RejectsRequiredProjectOnAnotherThread)
{
    TestProject project;
    GtExecutionEnvironment environment(&project);
    ProbeOperation operation(true);
    ThreadAffinityTransfer projectOnOtherThread(project);
    GtExecutionEventStream events(GtExecutionId{});

    auto result = environment.execute(operation, nullptr, events);

    EXPECT_EQ(result.error(), GtExecutionResult::Error::WrongThread);
    EXPECT_EQ(result.operationResult(), nullptr);
    EXPECT_EQ(operation.executeCount, 0);
}

TEST_F(ExecutionEnvironmentTest, ApplyResultReceivesCompleteOutcome)
{
    ProbeOperation operation;
    auto executionResult = outcome(GtOperationExecutionResult::Status::Failed,
                                   true);
    GtExecutionContext context;

    EXPECT_TRUE(operation.applyResult(executionResult, context).succeeded());
    EXPECT_EQ(operation.appliedStatus,
              GtOperationExecutionResult::Status::Failed);
    EXPECT_EQ(operation.appliedCode, QStringLiteral("test-code"));
    EXPECT_EQ(operation.appliedMessage, QStringLiteral("test-message"));
    EXPECT_TRUE(operation.appliedHasPayload);
}
