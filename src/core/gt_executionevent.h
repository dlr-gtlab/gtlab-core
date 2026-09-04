/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTEXECUTIONEVENT_H
#define GTEXECUTIONEVENT_H

#include "gt_core_exports.h"
#include "gt_operationexecutioncontext.h"

#include <QByteArray>
#include <QJsonValue>

class GT_CORE_EXPORT GtExecutionEvent
{
public:
    enum class PayloadEncoding { Json, MementoXmlBase64 };

    GtExecutionEvent();
    GtExecutionEvent(GtExecutionId executionId, quint64 sequence,
                     QString eventType, QJsonValue payload = {});
    GtExecutionEvent(GtExecutionId executionId, quint64 sequence,
                     QString eventType, QByteArray mementoXml);

    GtExecutionId const& executionId() const noexcept;
    quint64 sequence() const noexcept;
    QString const& eventType() const noexcept;
    PayloadEncoding payloadEncoding() const noexcept;
    QJsonValue const& jsonPayload() const noexcept;
    QByteArray const& mementoXml() const noexcept;

private:
    GtExecutionId m_executionId;
    quint64 m_sequence;
    QString m_eventType;
    PayloadEncoding m_payloadEncoding;
    QJsonValue m_jsonPayload;
    QByteArray m_mementoXml;
};

Q_DECLARE_METATYPE(GtExecutionEvent)

#endif // GTEXECUTIONEVENT_H
