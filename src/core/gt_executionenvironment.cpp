/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_executionenvironment.h"

#include "gt_executioncontext.h"
#include "operations/gt_executioneventstream.h"
#include "operations/gt_executableoperation.h"
#include "gt_object.h"
#include "gt_project.h"

#include <exception>
#include <utility>

#include <QThread>

GtExecutionEnvironment::GtExecutionEnvironment(GtProject* project) noexcept :
    m_project(project)
{
}

GtProject*
GtExecutionEnvironment::project() noexcept
{
    return m_project;
}

GtProject const*
GtExecutionEnvironment::project() const noexcept
{
    return m_project;
}

GtExecutionResult
GtExecutionEnvironment::execute(GtExecutableOperation& operation,
                                GtObject* data,
                                GtExecutionEventStream& events,
                                GtCancellationToken cancellation)
{
    try
    {
        auto* const callingThread = QThread::currentThread();
        if (operation.thread() != callingThread)
        {
            return GtExecutionResult::fromError(
                GtExecutionResult::Error::WrongThread,
                QStringLiteral(
                    "The operation is not affiliated "
                    "with the calling thread."));
        }

        const bool requiresProject = operation.requiresProject();
        if (requiresProject && !m_project)
        {
            return GtExecutionResult::fromError(
                GtExecutionResult::Error::ProjectRequired,
                QStringLiteral(
                    "This operation requires an execution project."));
        }

        if (data && data->thread() != callingThread)
        {
            return GtExecutionResult::fromError(
                GtExecutionResult::Error::WrongThread,
                QStringLiteral(
                    "The operation data is not affiliated "
                    "with the calling thread."));
        }

        GtProject* executionProject = nullptr;
        if (requiresProject)
        {
            if (m_project->thread() != callingThread)
            {
                return GtExecutionResult::fromError(
                    GtExecutionResult::Error::WrongThread,
                    QStringLiteral(
                        "The execution project is not affiliated "
                        "with the calling thread."));
            }

            executionProject = m_project;
        }

        GtExecutionContext executionContext(executionProject);
        GtExecutionContextScope contextScope(executionContext);

        if (cancellation.isCancellationRequested())
        {
            GtOperationExecutionResult result;
            result.status = GtOperationExecutionResult::Status::Cancelled;
            return GtExecutionResult(std::move(result));
        }

        GtOperationExecutionContext operationContext(data, events,
                                                     std::move(cancellation));
        return GtExecutionResult(operation.execute(operationContext));
    }
    catch (std::exception const& exception)
    {
        return GtExecutionResult::fromError(
            GtExecutionResult::Error::UnhandledException,
            QString::fromUtf8(exception.what()));
    }
    catch (...)
    {
        return GtExecutionResult::fromError(
            GtExecutionResult::Error::UnhandledException,
            QStringLiteral(
                "An unhandled exception escaped operation execution."));
    }
}
