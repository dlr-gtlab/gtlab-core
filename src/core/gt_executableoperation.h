/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTEXECUTABLEOPERATION_H
#define GTEXECUTABLEOPERATION_H

#include "gt_core_exports.h"
#include "gt_executioncontext.h"
#include "gt_operationapplyresult.h"
#include "gt_operationexecutioncontext.h"

#include "gt_object.h"

#include <memory>

/**
 * @brief Transport-neutral executable domain operation.
 */
class GT_CORE_EXPORT GtExecutableOperation : public GtObject
{
    Q_OBJECT

public:
    using GtObject::GtObject;
    ~GtExecutableOperation() override = default;

    /**
     *  Returns whether execution requires a provisioned project.
     *
     * This concerns the execution location only. An operation may still read
     * the originating project while preparing detached invocation data.
     */
    virtual bool requiresProject() const = 0;

    /**
     *  Creates optional detached invocation data on the originating side.
     *
     * Called on the originating project/application thread and must remain
     * lightweight. The returned object is independently owned by the caller
     * and may be nullptr. It must not borrow authoritative-project objects; an
     * execution target receives this data and must not call createData() again.
     */
    virtual std::unique_ptr<GtObject>
    createData(GtExecutionContext const& context) const = 0;

    /**
     *  Performs synchronous execution-side computation.
     *
     * This may be expensive. It may mutate detached data and execution-local
     * project state, but must never mutate the authoritative originating
     * project. The returned detached serializable result is independently owned
     * by the caller and may be nullptr according to the concrete contract.
     */
    virtual std::unique_ptr<GtObject>
    execute(GtOperationExecutionContext& context) = 0;

    /**
     *  Applies a detached result on the originating side.
     *
     * Called on the originating project/application thread and must remain
     * lightweight. result is a nullable, non-owning read-only view that must
     * not be retained. This is the only operation step allowed to make a result
     * authoritative in the originating project; failures are reported through
     * the returned structured result.
     */
    virtual GtOperationApplyResult
    applyResult(GtObject const* result, GtExecutionContext& context) const = 0;
};

#endif // GTEXECUTABLEOPERATION_H
