/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_executioneventstream.h"

#include "gt_object.h"
#include "gt_objectmemento.h"

#include <stdexcept>
#include <utility>

GtExecutionEventStream::GtExecutionEventStream(GtExecutionId executionId,
                                                 QObject* parent) :
    QObject(parent),
    m_executionId(std::move(executionId))
{
    qRegisterMetaType<GtExecutionEvent>();
}

GtExecutionId const&
GtExecutionEventStream::executionId() const noexcept
{
    return m_executionId;
}

void GtExecutionEventStream::publish(QString eventType, QJsonValue payload)
{
    publish(GtExecutionEvent(m_executionId, 0, std::move(eventType),
                             std::move(payload)));
}

void GtExecutionEventStream::publish(QString eventType, GtObject const& payload)
{
    publish(GtExecutionEvent(m_executionId, 0, std::move(eventType),
                             payload.toMemento().toByteArray()));
}

void GtExecutionEventStream::publish(GtExecutionEvent event)
{
    if (event.eventType().isEmpty()) {
        throw std::invalid_argument("Execution event type must not be empty");
    }

    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    event = event.payloadEncoding() == GtExecutionEvent::PayloadEncoding::Json ?
                GtExecutionEvent(m_executionId, m_nextSequence++, event.eventType(),
                                 event.jsonPayload()) :
                GtExecutionEvent(m_executionId, m_nextSequence++, event.eventType(),
                                 event.mementoXml());
    emit eventPublished(event);
}
