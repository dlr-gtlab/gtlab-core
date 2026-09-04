/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTEXECUTIONEVENTSTREAM_H
#define GTEXECUTIONEVENTSTREAM_H

#include "gt_core_exports.h"
#include "gt_executionevent.h"

#include <deque>
#include <mutex>

#include <QObject>

/**
 * @brief Concrete operation-facing publisher and Qt-observable stream.
 *
 * Give one stream to GtOperationExecutionContext for each operation invocation.
 * The stream assigns a common execution identity and strictly increasing
 * sequence numbers to its events. Local observers connect to eventPublished();
 * Qt selects queued delivery automatically when sender and receiver have
 * different thread affinity.
 *
 * Publications from multiple threads and publications made reentrantly by an
 * observer are queued and drained in sequence order. Signal handlers are never
 * called while the stream mutex is held.
 */
class GT_CORE_EXPORT GtExecutionEventStream : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Creates a stream for one operation invocation.
     * @param executionId Identity assigned to every event published by the stream.
     * @param parent Optional Qt owner of the stream.
     */
    explicit GtExecutionEventStream(GtExecutionId executionId,
                                    QObject* parent = nullptr);

    /// Returns the immutable identity assigned to this stream's events.
    GtExecutionId const& executionId() const noexcept;

    /**
     * @brief Publishes a JSON-valued domain observation.
     *
     * @param eventType Non-empty, domain-specific event type key.
     * @param payload JSON value; an undefined value represents no payload.
     *
     * This function is safe to call concurrently. It assigns the next sequence
     * number before notifying observers.
     */
    void publish(QString eventType, QJsonValue payload = {});

signals:
    /// Emitted once per accepted publication in per-execution sequence order.
    void eventPublished(GtExecutionEvent event);

private:
    GtExecutionId m_executionId;
    quint64 m_nextSequence {0};
    std::deque<GtExecutionEvent> m_pending;
    std::mutex m_mutex;
    bool m_dispatching {false};
};

#endif // GTEXECUTIONEVENTSTREAM_H
