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
 * @brief Transport-neutral observation emitted during one operation execution.
 *
 * An event is an immutable value object with a JSON payload. It contains no
 * process-local pointers and can therefore be observed locally through Qt or
 * serialized by a transport adapter. Each event belongs to exactly one
 * execution and its zero-based sequence number defines its order within that
 * execution.
 *
 * Operations normally publish through GtOperationExecutionContext::events()
 * instead of constructing events directly. GtExecutionEventStream assigns the
 * execution identity and sequence number for every publication.
 */
class GT_CORE_EXPORT GtExecutionEvent
{
public:
    /// Creates an empty event for Qt metatype construction only.
    GtExecutionEvent();

    /**
     * @brief Creates one JSON-valued execution event.
     * @param executionId Immutable identity of the producing execution.
     * @param sequence Zero-based order within that execution.
     * @param eventType Non-empty, domain-specific event type key.
     * @param payload JSON value; an undefined value represents no payload.
     */
    GtExecutionEvent(GtExecutionId executionId, quint64 sequence,
                     QString eventType, QJsonValue payload = {});

    /// Returns the identity shared by all events of this execution.
    GtExecutionId const& executionId() const noexcept;
    /// Returns the strictly increasing, zero-based per-execution sequence.
    quint64 sequence() const noexcept;
    /// Returns the domain-specific type key describing this event.
    QString const& eventType() const noexcept;
    /// Returns the JSON payload, or an undefined value when no payload was set.
    QJsonValue const& payload() const noexcept;

private:
    GtExecutionId m_executionId;
    quint64 m_sequence;
    QString m_eventType;
    QJsonValue m_payload;
};

Q_DECLARE_METATYPE(GtExecutionEvent)

#endif // GTEXECUTIONEVENT_H
