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

/**
 * @brief Transport-neutral observation emitted during one operation execution.
 *
 * An event is a value object. It carries no process-local pointers and may be
 * delivered to local Qt observers or encoded by a transport adapter. An event
 * belongs to exactly one execution and is ordered relative to other events of
 * that execution by its zero-based sequence number.
 *
 * Operations do not construct events directly. They publish a domain event
 * through GtOperationExecutionContext::events(); the associated
 * GtExecutionEventStream supplies the execution identity and sequence number.
 */
class GT_CORE_EXPORT GtExecutionEvent
{
public:
    /**
     * @brief Identifies the representation of the payload.
     *
     * Json stores a JSON value tree directly. MementoXmlBase64 stores detached
     * UTF-8 Memento/XML bytes; the stdio V1 encoder represents these bytes with
     * standard Base64.
     */
    enum class PayloadEncoding { Json, MementoXmlBase64 };

    /// Creates an empty event for Qt metatype construction only.
    GtExecutionEvent();

    /**
     * @brief Creates an event with a JSON-value payload.
     * @param executionId Immutable identity of the execution that emitted it.
     * @param sequence Zero-based order within that execution.
     * @param eventType Non-empty, domain-specific event type key.
     * @param payload JSON value tree; an undefined value represents no payload.
     */
    GtExecutionEvent(GtExecutionId executionId, quint64 sequence,
                     QString eventType, QJsonValue payload = {});

    /**
     * @brief Creates an event with detached Memento/XML payload bytes.
     * @param mementoXml UTF-8 bytes from GtObject::toMemento().
     *
     * The bytes must represent a detached serializable object and must not
     * contain borrowed project state or process-local references.
     */
    GtExecutionEvent(GtExecutionId executionId, quint64 sequence,
                     QString eventType, QByteArray mementoXml);

    /// Returns the immutable identity shared by all events of this invocation.
    GtExecutionId const& executionId() const noexcept;
    /// Returns the strictly increasing, zero-based sequence within the invocation.
    quint64 sequence() const noexcept;
    /// Returns the non-empty domain event type key.
    QString const& eventType() const noexcept;
    /// Returns whether the payload is a JSON tree or detached Memento/XML bytes.
    PayloadEncoding payloadEncoding() const noexcept;
    /// Returns the JSON payload; meaningful only when payloadEncoding() is Json.
    QJsonValue const& jsonPayload() const noexcept;
    /// Returns detached Memento/XML bytes; meaningful only for MementoXmlBase64.
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
