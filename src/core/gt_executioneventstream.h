/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTEXECUTIONEVENTSTREAM_H
#define GTEXECUTIONEVENTSTREAM_H

#include "gt_core_exports.h"
#include "gt_executionevent.h"

#include <mutex>

#include <QObject>

class GtObject;

/**
 * @brief Ordered local event stream and event-sink implementation for one execution.
 *
 * Give this stream to GtOperationExecutionContext when invoking an operation.
 * Each publish call receives the stream execution identity and the next sequence
 * number atomically, then emits eventPublished(). Therefore direct Qt observers
 * see the same order as transport adapters connected to this signal. Connect
 * observers before calling GtExecutableOperation::execute(). Qt uses queued
 * delivery automatically when sender and receiver have different thread affinity.
 *
 * The stream is transport-neutral: it neither knows nor writes stdout. Attach a
 * GtStdioExecutionEventEncoder to eventPublished() only at a process boundary.
 */
class GT_CORE_EXPORT GtExecutionEventStream : public QObject,
                                              public GtExecutionEventSink
{
    Q_OBJECT

public:
    /**
     * @brief Creates a stream for one operation invocation.
     * @param executionId Identity that is assigned to every emitted event.
     */
    explicit GtExecutionEventStream(GtExecutionId executionId,
                                    QObject* parent = nullptr);

    /// Returns the invocation identity used for every published event.
    GtExecutionId const& executionId() const noexcept;

    /**
     * @brief Publishes a JSON-valued domain observation.
     *
     * eventType must not be empty. Sequence assignment and signal emission are
     * serialized, including when multiple execution-side threads publish.
     */
    void publish(QString eventType, QJsonValue payload = {}) override;

    /**
     * @brief Publishes a detached GtObject observation as Memento/XML.
     *
     * The object is serialized before observers receive the event, so the event
     * does not expose the object's process-local address.
     */
    void publish(QString eventType, GtObject const& payload) override;

signals:
    /// Emitted once per accepted publication, in per-execution sequence order.
    void eventPublished(GtExecutionEvent event);

private:
    void publish(GtExecutionEvent event);

    GtExecutionId m_executionId;
    quint64 m_nextSequence {0};
    std::recursive_mutex m_mutex;
};

#endif // GTEXECUTIONEVENTSTREAM_H
