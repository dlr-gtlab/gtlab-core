/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_executionresult.h"

#include <utility>

GtExecutionResult::GtExecutionResult(GtOperationExecutionResult result) :
    m_value(std::move(result))
{
}

GtExecutionResult::GtExecutionResult(Failure failure) :
    m_value(std::move(failure))
{
}

GtExecutionResult
GtExecutionResult::fromError(Error error, QString message)
{
    return GtExecutionResult(Failure{error, std::move(message)});
}

GtExecutionResult::Error
GtExecutionResult::error() const noexcept
{
    if (auto const* failure = std::get_if<Failure>(&m_value))
    {
        return failure->error;
    }

    return Error::None;
}

QString const&
GtExecutionResult::message() const noexcept
{
    if (auto const* failure = std::get_if<Failure>(&m_value))
    {
        return failure->message;
    }

    static const QString noMessage;
    return noMessage;
}

GtOperationExecutionResult const*
GtExecutionResult::operationResult() const noexcept
{
    return std::get_if<GtOperationExecutionResult>(&m_value);
}
