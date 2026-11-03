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
 * @brief Publishes events for one operation execution.
 *
 * Give one stream to GtOperationExecutionContext. The stream assigns the shared
 * execution ID and sequence numbers. Connect local observers to eventPublished().
 *
 * Concurrent and reentrant calls are queued. All observers receive events in
 * sequence order, and signal handlers run without the stream lock.
 */
class GT_CORE_EXPORT GtExecutionEventStream : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Creates an event stream for one operation execution.
     *
     * @param executionId ID assigned to published events.
     * @param parent Optional Qt owner.
     */
    explicit GtExecutionEventStream(GtExecutionId executionId,
                                    QObject* parent = nullptr);

    /**
     * @brief Returns the execution ID assigned to this stream.
     */
    GtExecutionId const& executionId() const noexcept;

    /**
     * @brief Publishes an event.
     *
     * @param eventType Non-empty event type.
     * @param payload JSON payload. An undefined value means no payload.
     * @throws std::invalid_argument if @p eventType is empty.
     *
     * This function is safe to call from multiple threads. It assigns the next
     * sequence number before notifying observers.
     */
    void publish(QString eventType, QJsonValue payload = {});

signals:
    /**
     * @brief Emitted for each event in sequence order.
     */
    void eventPublished(GtExecutionEvent event);

private:
    GtExecutionId m_executionId;
    quint64 m_nextSequence{0};
    std::deque<GtExecutionEvent> m_pending;
    std::mutex m_mutex;
    bool m_dispatching{false};
};

#endif // GTEXECUTIONEVENTSTREAM_H
