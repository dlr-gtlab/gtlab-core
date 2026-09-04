/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_executionevent.h"

#include <utility>

GtExecutionEvent::GtExecutionEvent() :
    m_sequence(0),
    m_payloadEncoding(PayloadEncoding::Json)
{
}

GtExecutionEvent::GtExecutionEvent(GtExecutionId executionId, quint64 sequence,
                                   QString eventType, QJsonValue payload) :
    m_executionId(std::move(executionId)),
    m_sequence(sequence),
    m_eventType(std::move(eventType)),
    m_payloadEncoding(PayloadEncoding::Json),
    m_jsonPayload(std::move(payload))
{
}

GtExecutionEvent::GtExecutionEvent(GtExecutionId executionId, quint64 sequence,
                                   QString eventType, QByteArray mementoXml) :
    m_executionId(std::move(executionId)),
    m_sequence(sequence),
    m_eventType(std::move(eventType)),
    m_payloadEncoding(PayloadEncoding::MementoXmlBase64),
    m_mementoXml(std::move(mementoXml))
{
}

GtExecutionId const&
GtExecutionEvent::executionId() const noexcept
{
    return m_executionId;
}

quint64
GtExecutionEvent::sequence() const noexcept
{
    return m_sequence;
}

QString const&
GtExecutionEvent::eventType() const noexcept
{
    return m_eventType;
}

GtExecutionEvent::PayloadEncoding
GtExecutionEvent::payloadEncoding() const noexcept
{
    return m_payloadEncoding;
}

QJsonValue const&
GtExecutionEvent::jsonPayload() const noexcept
{
    return m_jsonPayload;
}

QByteArray const&
GtExecutionEvent::mementoXml() const noexcept
{
    return m_mementoXml;
}
