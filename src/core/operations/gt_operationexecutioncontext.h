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
    /**
     * @brief Creates a new UUID-based execution identity.
     */
    GtExecutionId();

    /**
     * @brief Returns whether two execution identities are equal.
     * @return True if both identities represent the same execution.
     */
    bool operator==(GtExecutionId const& other) const;
    /**
     * @brief Returns whether two execution identities differ.
     * @return True if the identities represent different executions.
     */
    bool operator!=(GtExecutionId const& other) const
    {
        return !(*this == other);
    }
    /**
     * @brief Returns the UUID representation without enclosing braces.
     * @return The execution identity.
     */
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
    /**
     * @brief Creates a token whose cancellation has not been requested.
     */
    GtCancellationToken();
    /**
     * @brief Requests cancellation for every copy of this token.
     */
    void requestCancellation() noexcept;
    /**
     * @brief Returns whether cancellation was requested for this shared token state.
     *
     * This function is safe to call while another thread requests cancellation.
     *
     * @return True if cancellation was requested.
     */
    bool isCancellationRequested() const noexcept;

private:
    struct State;
    std::shared_ptr<State> m_state;
};

/**
 * @brief Context passed to GtExecutableOperation::execute().
 *
 * The caller owns the detached data object for the complete invocation. The
 * context borrows the event stream and never owns project state. The stream is
 * the only source of the execution identity for this context and its events.
 */
class GT_CORE_EXPORT GtOperationExecutionContext
{
public:
    /**
     * @brief Creates the context for one operation execution.
     * @param data Optional detached data borrowed from the caller.
     * @param events Event stream borrowed from the caller.
     * @param cancellation Shared cancellation state for this execution.
     */
    GtOperationExecutionContext(GtObject* data, GtExecutionEventStream& events,
                                GtCancellationToken cancellation = {});
    ~GtOperationExecutionContext();

    GtOperationExecutionContext(GtOperationExecutionContext const& other);
    GtOperationExecutionContext& operator=(
        GtOperationExecutionContext const& other);
    GtOperationExecutionContext(GtOperationExecutionContext&& other) noexcept;
    GtOperationExecutionContext& operator=(
        GtOperationExecutionContext&& other) noexcept;

    /**
     * @brief Returns the mutable detached operation data.
     * @return Borrowed data, or nullptr. Do not retain the pointer.
     */
    GtObject* data() noexcept;
    /**
     * @brief Returns the read-only detached operation data.
     * @return Borrowed data, or nullptr. Do not retain the pointer.
     */
    GtObject const* data() const noexcept;
    /**
     * @brief Returns the identity owned by events().
     * @return The stable execution identity.
     */
    GtExecutionId const& executionId() const noexcept;
    /**
     * @brief Returns the event stream for this execution.
     * @return The borrowed event stream.
     */
    GtExecutionEventStream& events() noexcept;
    /**
     * @brief Returns the shared cancellation state.
     * @return The cancellation token for this execution.
     */
    GtCancellationToken& cancellation() noexcept;
    /**
     * @brief Returns the shared cancellation state.
     * @return The cancellation token for this execution.
     */
    GtCancellationToken const& cancellation() const noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

#endif // GTOPERATIONEXECUTIONCONTEXT_H
