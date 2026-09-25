/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTEXECUTIONENVIRONMENT_H
#define GTEXECUTIONENVIRONMENT_H

#include "gt_core_exports.h"
#include "gt_executionresult.h"
#include "operations/gt_operationexecutioncontext.h"

class GtExecutionEventStream;
class GtExecutableOperation;
class GtObject;
class GtProject;

/**
 * @brief Executes one operation synchronously with an optional local project.
 *
 * The environment borrows the project and does not manage its lifetime. Each
 * call runs in the calling thread. The caller must provision the operation,
 * optional data, and required project for that thread. The environment does
 * not initialize Core, manage project I/O, schedule work, or transport data.
 */
class GT_CORE_EXPORT GtExecutionEnvironment
{
public:
    /**
     * @brief Creates an environment around an optional borrowed project.
     * @param project Pre-provisioned project, not owned by the environment.
     */
    explicit GtExecutionEnvironment(GtProject* project = nullptr) noexcept;

    /**
     * @brief Returns the borrowed project.
     * @return Project pointer, or nullptr when none was supplied.
     */
    GtProject* project() noexcept;

    /**
     * @brief Returns the borrowed project.
     * @return Read-only project pointer, or nullptr when none was supplied.
     */
    GtProject const* project() const noexcept;

    /**
     * @brief Executes an operation in the calling thread.
     *
     * A project-required operation receives this environment's project
     * through GtExecutionContext. A project-independent operation receives an
     * explicitly empty context, even when this environment has a project.
     * Data and the event stream are borrowed for this synchronous call. The
     * operation, data, and any required project must have affinity to the
     * calling thread. The event stream is safe to publish from any thread.
     *
     * Cancellation is cooperative. A token cancelled before execution skips
     * the operation and returns a Cancelled operation outcome. Once execution
     * starts, the environment preserves the status returned by the operation.
     * An exception escaping the operation becomes an environment error.
     *
     * @param operation Operation to execute, borrowed for this call.
     * @param data Optional detached data, borrowed for this call.
     * @param events Event stream used by the operation, borrowed for this call.
     * @param cancellation Shared cooperative cancellation state.
     * @return Operation outcome or a structured environment failure.
     */
    GtExecutionResult execute(GtExecutableOperation& operation,
                              GtObject* data,
                              GtExecutionEventStream& events,
                              GtCancellationToken cancellation = {});

private:
    GtProject* m_project;
};

#endif // GTEXECUTIONENVIRONMENT_H
