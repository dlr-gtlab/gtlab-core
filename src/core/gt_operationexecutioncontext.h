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
 * @brief Temporary transport-neutral boundary for execution-side observations.
 *
 * This foundation intentionally does not define an event envelope, event type,
 * or payload contract. Those contracts are follow-up runtime work.
 */
class GT_CORE_EXPORT GtExecutionEventSink
{
public:
    virtual ~GtExecutionEventSink() = default;

    /// Publishes an execution-side observation without defining its event data.
    virtual void publish(QString eventType, QJsonValue payload = {}) = 0;
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
