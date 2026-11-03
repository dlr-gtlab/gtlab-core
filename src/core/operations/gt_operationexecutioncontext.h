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
 * @brief Identifies one operation execution.
 *
 * A default-constructed ID contains a new UUID. The event stream provides the
 * ID used by an operation context and its events.
 */
class GT_CORE_EXPORT GtExecutionId
{
public:
    /**
     * @brief Creates a new UUID-based execution identity.
     */
    GtExecutionId();

    /**
     * @brief Returns whether two execution IDs are equal.
     *
     * @return True if both IDs identify the same execution.
     */
    bool operator==(GtExecutionId const& other) const;
    /**
     * @brief Returns whether two execution IDs differ.
     *
     * @return True if the IDs identify different executions.
     */
    bool operator!=(GtExecutionId const& other) const
    {
        return !(*this == other);
    }
    /**
     * @brief Returns the UUID without braces.
     *
     * @return The execution ID.
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
     * @brief Returns whether cancellation was requested.
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
 * @brief Data and services passed to GtExecutableOperation::execute().
 *
 * The caller owns the detached data and the event stream. The context borrows
 * both and gets its execution ID from the stream. Copies borrow the same data
 * and event stream and share the same cooperative cancellation state.
 */
class GT_CORE_EXPORT GtOperationExecutionContext
{
public:
    /**
     * @brief Creates the context for one operation execution.
     *
     * @param data Optional detached data borrowed from the caller.
     * @param events Event stream borrowed from the caller.
     * @param cancellation Shared cancellation state.
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
     * @brief Returns the mutable detached data.
     *
     * @return Borrowed data, or nullptr. Do not retain the pointer.
     */
    GtObject* data() noexcept;
    /**
     * @brief Returns the read-only detached data.
     *
     * @return Borrowed data, or nullptr. Do not retain the pointer.
     */
    GtObject const* data() const noexcept;
    /**
     * @brief Returns the execution ID from events().
     *
     * @return The execution ID.
     */
    GtExecutionId const& executionId() const noexcept;
    /**
     * @brief Returns the event stream for this execution.
     *
     * @return The borrowed event stream.
     */
    GtExecutionEventStream& events() noexcept;
    /**
     * @brief Returns the shared cancellation state.
     *
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
