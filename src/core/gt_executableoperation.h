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

    virtual bool requiresProject() const = 0;

    virtual std::unique_ptr<GtObject>
    createData(GtExecutionContext const& context) const = 0;

    virtual std::unique_ptr<GtObject>
    execute(GtOperationExecutionContext& context) = 0;

    virtual GtOperationApplyResult
    applyResult(GtObject const* result, GtExecutionContext& context) const = 0;
};

#endif // GTEXECUTABLEOPERATION_H
