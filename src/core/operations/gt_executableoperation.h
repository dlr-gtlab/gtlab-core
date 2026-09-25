/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTEXECUTABLEOPERATION_H
#define GTEXECUTABLEOPERATION_H

#include "gt_core_exports.h"
#include "gt_executioncontext.h"
#include "gt_operationapplystatus.h"
#include "gt_operationexecutioncontext.h"
#include "gt_operationexecutionresult.h"

#include "gt_object.h"

#include <memory>

/**
 * @brief Executable operation containing domain behavior.
 */
class GT_CORE_EXPORT GtExecutableOperation : public GtObject
{
    Q_OBJECT

public:
    using GtObject::GtObject;
    ~GtExecutableOperation() override = default;

    /**
     * @brief Returns whether execute() needs a GTlab project in addition to
     *        its detached input data.
     *
     * This applies at the location where execute() runs. The operation may
     * still read the originating project while createData() prepares input.
     *
     * @return True if execute() requires a project in addition to its input.
     */
    virtual bool requiresProject() const = 0;

    /**
     * @brief Creates optional detached input data on the originating side.
     *
     * Called on the originating project/application thread and must remain
     * lightweight. The result must not borrow objects from that project. The
     * execution location receives it and must not call createData() again.
     *
     * @param context Execution information from the originating side.
     * @return An optional data object owned by the caller, or nullptr.
     */
    virtual std::unique_ptr<GtObject> createData(
        GtExecutionContext const& context) const = 0;

    /**
     * @brief Performs synchronous computation at the execution location.
     *
     * This call may be expensive. It may change detached data and a project at
     * the execution location, but never the originating project.
     *
     * @param context Input data and services for this execution.
     * @return The operation execution outcome, including its status and
     *         optional result payload.
     */
    virtual GtOperationExecutionResult execute(
        GtOperationExecutionContext& context) = 0;

    /**
     * @brief Applies an operation execution outcome on the originating side.
     *
     * Called on the originating project/application thread and must remain
     * lightweight. executionResult is the complete, non-owning operation
     * outcome from execute() and must not be retained. The operation decides
     * how each status and optional result payload affects the originating
     * project. This is the only operation lifecycle step that may apply
     * execution output to the originating project.
     *
     * @param executionResult Complete operation execution outcome from
     *        execute().
     * @param context Context of the originating-side execution.
     * @return The status of applying the operation execution outcome.
     */
    virtual GtOperationApplyStatus applyResult(
        GtOperationExecutionResult const& executionResult,
        GtExecutionContext& context) const = 0;
};

#endif // GTEXECUTABLEOPERATION_H
