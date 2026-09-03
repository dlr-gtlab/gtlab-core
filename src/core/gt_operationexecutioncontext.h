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

#include <QString>

class GtObject;

/**
 * @brief Opaque, globally unique identity of one operation execution.
 */
class GT_CORE_EXPORT GtExecutionId
{
public:
    GtExecutionId();

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

    void requestCancellation() noexcept;
    bool isCancellationRequested() const noexcept;

private:
    struct State;
    std::shared_ptr<State> m_state;
};

/**
 * @brief Transport-neutral boundary for execution-side operation observations.
 */
class GT_CORE_EXPORT GtExecutionEventSink
{
public:
    virtual ~GtExecutionEventSink() = default;

    virtual void publish(QString const& eventType) = 0;
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

    GtObject* data() noexcept;
    GtObject const* data() const noexcept;
    GtExecutionId const& executionId() const noexcept;
    GtExecutionEventSink& events() noexcept;
    GtCancellationToken& cancellation() noexcept;
    GtCancellationToken const& cancellation() const noexcept;

private:
    GtObject* m_data;
    GtExecutionEventSink& m_events;
    GtCancellationToken m_cancellation;
    GtExecutionId m_executionId;
};

#endif // GTOPERATIONEXECUTIONCONTEXT_H
