/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_operationexecutioncontext.h"

#include "gt_executioneventstream.h"

#include <utility>

#include <QUuid>

struct GtCancellationToken::State
{
    std::atomic_bool requested{false};
};

struct GtOperationExecutionContext::Impl
{
    Impl(GtObject* data, GtExecutionEventStream& events,
         GtCancellationToken cancellation) :
        data(data),
        events(&events), cancellation(std::move(cancellation))
    {
    }

    GtObject* data;
    GtExecutionEventStream* events;
    GtCancellationToken cancellation;
};

GtExecutionId::GtExecutionId() :
    m_value(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
}

bool GtExecutionId::operator==(GtExecutionId const& other) const
{
    return m_value == other.m_value;
}

QString const&
GtExecutionId::toString() const noexcept
{
    return m_value;
}

GtCancellationToken::GtCancellationToken() : m_state(std::make_shared<State>())
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
    GtObject* data, GtExecutionEventStream& events,
    GtCancellationToken cancellation) :
    m_impl(std::make_unique<Impl>(data, events, std::move(cancellation)))
{
}

GtOperationExecutionContext::~GtOperationExecutionContext() = default;

GtOperationExecutionContext::GtOperationExecutionContext(
    GtOperationExecutionContext const& other) :
    m_impl(std::make_unique<Impl>(*other.m_impl))
{
}

GtOperationExecutionContext&
GtOperationExecutionContext::operator=(GtOperationExecutionContext const& other)
{
    if (this != &other)
    {
        m_impl = std::make_unique<Impl>(*other.m_impl);
    }

    return *this;
}

GtOperationExecutionContext::GtOperationExecutionContext(
    GtOperationExecutionContext&& other) noexcept = default;

GtOperationExecutionContext& GtOperationExecutionContext::operator=(
    GtOperationExecutionContext&& other) noexcept = default;

GtObject*
GtOperationExecutionContext::data() noexcept
{
    return m_impl->data;
}

GtObject const*
GtOperationExecutionContext::data() const noexcept
{
    return m_impl->data;
}

GtExecutionId const&
GtOperationExecutionContext::executionId() const noexcept
{
    return m_impl->events->executionId();
}

GtExecutionEventStream&
GtOperationExecutionContext::events() noexcept
{
    return *m_impl->events;
}

GtCancellationToken&
GtOperationExecutionContext::cancellation() noexcept
{
    return m_impl->cancellation;
}

GtCancellationToken const&
GtOperationExecutionContext::cancellation() const noexcept
{
    return m_impl->cancellation;
}
