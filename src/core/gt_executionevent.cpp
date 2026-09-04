/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_executionevent.h"

#include <utility>

GtExecutionEvent::GtExecutionEvent() : m_sequence(0) {}

GtExecutionEvent::GtExecutionEvent(GtExecutionId executionId, quint64 sequence,
                                   QString eventType, QJsonValue payload) :
    m_executionId(std::move(executionId)),
    m_sequence(sequence),
    m_eventType(std::move(eventType)),
    m_payload(std::move(payload))
{
}

GtExecutionId const& GtExecutionEvent::executionId() const noexcept
{
    return m_executionId;
}

quint64 GtExecutionEvent::sequence() const noexcept
{
    return m_sequence;
}

QString const& GtExecutionEvent::eventType() const noexcept
{
    return m_eventType;
}

QJsonValue const& GtExecutionEvent::payload() const noexcept
{
    return m_payload;
}
