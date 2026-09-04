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

class GtExecutionEventStream;
class GtObject;

/**
 * @brief Stable identity of one operation invocation.
 *
 * A default-constructed id is a new UUID. The execution event stream owns the
 * id used by an operation execution context and its published events.
 */
class GT_CORE_EXPORT GtExecutionId
{
public:
    /// Creates a new UUID-based execution identity.
    GtExecutionId();
    /// Returns the UUID representation without enclosing braces.
    QString const& toString() const noexcept;

private:
    QString m_value;
};

class GT_CORE_EXPORT GtCancellationToken
{
public:
    /// Creates a token whose cancellation has not been requested.
    GtCancellationToken();
    /// Requests cancellation for every copy of this token.
    void requestCancellation() noexcept;
    /// Returns whether cancellation was requested for this shared token state.
    bool isCancellationRequested() const noexcept;

private:
    struct State;
    std::shared_ptr<State> m_state;
};

/**
 * @brief Invocation-local services passed to one GtExecutableOperation::execute call.
 *
 * The context borrows both detached data and its concrete event stream. It does
 * not own project state. The stream is the sole source of this context's
 * execution identity, so context and all published events cannot diverge.
 */
class GT_CORE_EXPORT GtOperationExecutionContext
{
public:
    GtOperationExecutionContext(GtObject* data, GtExecutionEventStream& events,
                                GtCancellationToken cancellation = {});

    /// Returns the detached operation data, which remains owned by the caller.
    GtObject* data() noexcept;
    /// Returns the detached operation data, which remains owned by the caller.
    GtObject const* data() const noexcept;
    /// Returns the id owned by events().
    GtExecutionId const& executionId() const noexcept;
    /// Returns the concrete stream used to publish this execution's events.
    GtExecutionEventStream& events() noexcept;
    /// Returns the shared cancellation token for this invocation.
    GtCancellationToken& cancellation() noexcept;
    /// Returns the shared cancellation token for this invocation.
    GtCancellationToken const& cancellation() const noexcept;

private:
    GtObject* m_data;
    GtExecutionEventStream& m_events;
    GtCancellationToken m_cancellation;
};

#endif // GTOPERATIONEXECUTIONCONTEXT_H
