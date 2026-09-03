/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_operationexecutioncontext.h"

#include <utility>

#include <QUuid>

struct GtCancellationToken::State
{
    std::atomic_bool requested {false};
};

GtExecutionId::GtExecutionId() :
    m_value(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
}

QString const&
GtExecutionId::toString() const noexcept
{
    return m_value;
}

GtCancellationToken::GtCancellationToken() :
    m_state(std::make_shared<State>())
{
}

void
GtCancellationToken::requestCancellation() noexcept
{
    m_state->requested.store(true, std::memory_order_release);
}

bool
GtCancellationToken::isCancellationRequested() const noexcept
{
    return m_state->requested.load(std::memory_order_acquire);
}

GtOperationExecutionContext::GtOperationExecutionContext(
    GtObject* data, GtExecutionEventSink& events,
    GtCancellationToken cancellation, GtExecutionId executionId) :
    m_data(data),
    m_events(events),
    m_cancellation(std::move(cancellation)),
    m_executionId(std::move(executionId))
{
}

GtObject*
GtOperationExecutionContext::data() noexcept
{
    return m_data;
}

GtObject const*
GtOperationExecutionContext::data() const noexcept
{
    return m_data;
}

GtExecutionId const&
GtOperationExecutionContext::executionId() const noexcept
{
    return m_executionId;
}

GtExecutionEventSink&
GtOperationExecutionContext::events() noexcept
{
    return m_events;
}

GtCancellationToken&
GtOperationExecutionContext::cancellation() noexcept
{
    return m_cancellation;
}

GtCancellationToken const&
GtOperationExecutionContext::cancellation() const noexcept
{
    return m_cancellation;
}
