/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTOPERATIONEXECUTIONCONTEXT_H
#define GTOPERATIONEXECUTIONCONTEXT_H

#include "gt_core_exports.h"

#include <atomic>
#include <memory>

#include <QJsonValue>
#include <QString>

class GtObject;

/**
 * @brief Opaque, globally unique identity of one operation execution.
 */
class GT_CORE_EXPORT GtExecutionId
{
public:
    GtExecutionId();

    /// Returns the stable text representation used by logs and protocols.
    QString const& toString() const noexcept;

private:
    QString m_value;
};

/**
 * @brief Cooperative cancellation state shared by one operation invocation.
 */
class GT_CORE_EXPORT GtCancellationToken
{
public:
    GtCancellationToken();

    /// Requests cooperative cancellation; safe to call from another thread.
    void requestCancellation() noexcept;
    /// Returns the shared request state; safe to observe during execution.
    bool isCancellationRequested() const noexcept;

private:
    struct State;
    std::shared_ptr<State> m_state;
};

/**
 * @brief Transport-neutral publication boundary for operation observations.
 *
 * An operation receives this boundary from GtOperationExecutionContext and
 * publishes domain observations only through it. Implementations such as
 * GtExecutionEventStream assign execution order and identity; operations must
 * not write stdout, access a transport, or select a GUI observer themselves.
 */
class GT_CORE_EXPORT GtExecutionEventSink
{
public:
    virtual ~GtExecutionEventSink() = default;

    /**
     * @brief Publishes a JSON-valued domain observation.
     * @param eventType Non-empty, domain-specific event type key.
     * @param payload Optional JSON value tree with no process-local pointers.
     */
    virtual void publish(QString eventType, QJsonValue payload = {}) = 0;

    /**
     * @brief Publishes a detached serializable object observation.
     *
     * Implementations serialize payload to a Memento before exposing the event
     * to observers or a transport adapter.
     */
    virtual void publish(QString eventType, GtObject const& payload) = 0;
};

/**
 * @brief Invocation-local services passed to one GtExecutableOperation::execute call.
 *
 * The context never owns project state or the detached data object. The caller
 * owns the data for the complete invocation; operations must not retain it.
 */
class GT_CORE_EXPORT GtOperationExecutionContext
{
public:
    GtOperationExecutionContext(GtObject* data,
                                GtExecutionEventSink& events,
                                GtCancellationToken cancellation = {},
                                GtExecutionId executionId = {});

    /// Returns borrowed mutable invocation data, or nullptr; do not retain it.
    GtObject* data() noexcept;
    /// Returns borrowed read-only invocation data, or nullptr; do not retain it.
    GtObject const* data() const noexcept;
    /// Returns the immutable identity shared by this complete invocation.
    GtExecutionId const& executionId() const noexcept;
    /// Returns the transport-neutral sink for execution-side observations.
    GtExecutionEventSink& events() noexcept;
    /// Returns shared state that can be observed while another thread requests cancellation.
    GtCancellationToken& cancellation() noexcept;
    /// Returns shared state that can be observed while another thread requests cancellation.
    GtCancellationToken const& cancellation() const noexcept;

private:
    GtObject* m_data;
    GtExecutionEventSink& m_events;
    GtCancellationToken m_cancellation;
    GtExecutionId m_executionId;
};

#endif // GTOPERATIONEXECUTIONCONTEXT_H
