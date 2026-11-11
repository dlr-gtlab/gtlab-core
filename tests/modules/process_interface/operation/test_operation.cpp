/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "test_operation.h"

#include "gt_executioncontext.h"
#include "gt_objectgroup.h"
#include "operations/gt_executioneventstream.h"

#include <QDir>
#include <QJsonObject>

#include <stdexcept>
#include <thread>

namespace
{
    std::unique_ptr<GtObject> makeOutcomePayload(QString name)
    {
        auto result = std::make_unique<GtObjectGroup>();
        result->setObjectName(std::move(name));
        return result;
    }
} // namespace

TestOperation::TestOperation(GtObject* parent) :
    GtExecutableOperation(parent),
    m_requiresProject(QStringLiteral("requiresProject"), tr("Requires Project"),
                      tr("Whether execution requires a project"), false),
    m_returnsFailure(QStringLiteral("returnsFailure"), tr("Returns Failure"),
                     tr("Whether execution returns a failure"), false),
    m_returnsCancellation(QStringLiteral("returnsCancellation"),
                          tr("Returns Cancellation"),
                          tr("Whether execution returns cancellation"), false),
    m_throwsException(QStringLiteral("throwsException"), tr("Throws Exception"),
                      tr("Whether execution throws an exception"), false)
{
    setObjectName(QStringLiteral("Test Executable Operation"));
    registerProperty(m_requiresProject);
    registerProperty(m_returnsFailure);
    registerProperty(m_returnsCancellation);
    registerProperty(m_throwsException);
}

bool
TestOperation::requiresProject() const
{
    return m_requiresProject.getVal();
}

std::unique_ptr<GtObject>
TestOperation::createData(GtExecutionContext const&) const
{
    return std::make_unique<GtObjectGroup>();
}

GtOperationExecutionResult
TestOperation::execute(GtOperationExecutionContext& context)
{
    const auto* executionContext = GtExecutionContext::current();
    const bool hasProject = executionContext && executionContext->project();
    const QJsonObject payload{
        {QStringLiteral("executionContextActive"), executionContext != nullptr},
        {QStringLiteral("dataProvided"), context.data() != nullptr},
        {QStringLiteral("workingDirectory"), QDir::currentPath()},
        {QStringLiteral("projectVisible"), hasProject}};

    context.events().publish(QStringLiteral("test.operation.started"), payload);
    gtInfo() << "Test executable operation emitted a normal GTlab log";

    if (m_throwsException.getVal())
    {
        throw std::runtime_error("Test operation exception.");
    }

    if (m_returnsFailure.getVal())
    {
        return {GtOperationExecutionResult::Status::Failed,
                QStringLiteral("test_operation_failed"),
                QStringLiteral("Test operation returned a failure."),
                makeOutcomePayload(QStringLiteral("Failed Outcome Payload"))};
    }

    if (m_returnsCancellation.getVal())
    {
        return {
            GtOperationExecutionResult::Status::Cancelled,
            QStringLiteral("test_operation_cancelled"),
            QStringLiteral("Test operation was cancelled."),
            makeOutcomePayload(QStringLiteral("Cancelled Outcome Payload"))};
    }

    // Regression coverage for events published from a worker thread:
    // queued delivery can leave this event unprocessed in the synchronous
    // console command. The system test requires both events in the output.
    auto* const eventStream = &context.events();
    std::thread eventPublisher([eventStream, payload] {
        eventStream->publish(QStringLiteral("test.operation.completed"),
                             payload);
    });
    eventPublisher.join();

    auto result = std::make_unique<GtObjectGroup>();
    result->setObjectName(QStringLiteral("Test Operation Result"));
    return {
        GtOperationExecutionResult::Status::Success, {}, {}, std::move(result)};
}

GtOperationApplyStatus
TestOperation::applyResult(GtOperationExecutionResult const& executionResult,
                           GtExecutionContext&) const
{
    if (executionResult.status == GtOperationExecutionResult::Status::Success)
    {
        return GtOperationApplyStatus::success();
    }

    return GtOperationApplyStatus::failure(
        QStringLiteral("The test operation did not succeed."));
}
