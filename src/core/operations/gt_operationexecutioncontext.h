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
    /**
     * @brief Creates a new execution identity.
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
     * @brief Returns the stable text representation used by logs and protocols.
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
     * @brief Creates a cancellation token with no cancellation request.
     */
    GtCancellationToken();

    /**
     * @brief Requests cooperative cancellation.
     *
     * This function is safe to call from another thread.
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
 * @brief Temporary transport-neutral boundary for execution-side observations.
 *
 * This foundation intentionally does not define an event envelope, event type,
 * or payload contract. Those contracts are follow-up runtime work.
 */
class GT_CORE_EXPORT GtExecutionEventSink
{
public:
    virtual ~GtExecutionEventSink() = default;

    /**
     * @brief Publishes an observation from the execution location.
     */
    virtual void publish() = 0;
};

/**
 * @brief Context passed to GtExecutableOperation::execute().
 *
 * The caller owns the detached data object for the complete invocation. The
 * context never owns that data or project state, and operations must not retain it.
 */
class GT_CORE_EXPORT GtOperationExecutionContext
{
public:
    /**
     * @brief Creates the context for one operation execution.
     * @param data Optional detached data borrowed from the caller.
     * @param events Observation sink borrowed from the caller.
     * @param cancellation Shared cancellation state for this execution.
     * @param executionId Identity for this execution.
     */
    GtOperationExecutionContext(GtObject* data,
                                GtExecutionEventSink& events,
                                GtCancellationToken cancellation = {},
                                GtExecutionId executionId = {});
    ~GtOperationExecutionContext();

    GtOperationExecutionContext(GtOperationExecutionContext const& other);
    GtOperationExecutionContext& operator=(
        GtOperationExecutionContext const& other);
    GtOperationExecutionContext(GtOperationExecutionContext&& other) noexcept;
    GtOperationExecutionContext& operator=(
        GtOperationExecutionContext&& other) noexcept;

    /**
     * @brief Returns the mutable detached data.
     * @return Borrowed data, or nullptr. Do not retain the pointer.
     */
    GtObject* data() noexcept;

    /**
     * @brief Returns the read-only detached data.
     * @return Borrowed data, or nullptr. Do not retain the pointer.
     */
    GtObject const* data() const noexcept;

    /**
     * @brief Returns the identity of this execution.
     * @return The stable execution identity.
     */
    GtExecutionId const& executionId() const noexcept;

    /**
     * @brief Returns the observation sink for this execution.
     * @return The borrowed observation sink.
     */
    GtExecutionEventSink& events() noexcept;

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
