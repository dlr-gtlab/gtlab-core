/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTEXECUTIONEVENT_H
#define GTEXECUTIONEVENT_H

#include "gt_core_exports.h"
#include "gt_operationexecutioncontext.h"

#include <QJsonValue>

/**
 * @brief Event published during one operation execution.
 *
 * Events contain a JSON payload and no process-local pointers. The event stream
 * assigns the execution ID and zero-based sequence number. Operations normally
 * publish events through GtOperationExecutionContext::events().
 */
class GT_CORE_EXPORT GtExecutionEvent
{
public:
    /**
     * @brief Creates an empty event for Qt metatype support.
     */
    GtExecutionEvent();

    /**
     * @brief Creates an execution event.
     *
     * @param executionId ID of the producing execution.
     * @param sequence Zero-based sequence number.
     * @param eventType Non-empty event type.
     * @param payload JSON payload. An undefined value means no payload.
     */
    GtExecutionEvent(GtExecutionId executionId, quint64 sequence,
                     QString eventType, QJsonValue payload = {});

    /**
     * @brief Returns the execution ID.
     */
    GtExecutionId const& executionId() const noexcept;
    /**
     * @brief Returns the zero-based sequence number.
     */
    quint64 sequence() const noexcept;
    /**
     * @brief Returns the event type.
     */
    QString const& eventType() const noexcept;
    /**
     * @brief Returns the JSON payload, or an undefined value.
     */
    QJsonValue const& payload() const noexcept;

private:
    GtExecutionId m_executionId;
    quint64 m_sequence;
    QString m_eventType;
    QJsonValue m_payload;
};

Q_DECLARE_METATYPE(GtExecutionEvent)

#endif // GTEXECUTIONEVENT_H
