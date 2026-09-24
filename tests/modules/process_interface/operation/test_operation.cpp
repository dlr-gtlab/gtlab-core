/* GTlab - Gas Turbine laboratory
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */
#include "test_operation.h"
#include "gt_executioneventstream.h"
#include "gt_operationexecutioncontext.h"
#include "gt_executioncontext.h"
#include <stdexcept>
bool TestOperation::requiresProject() const
{
    return objectName() == QStringLiteral("requires-project");
}

std::unique_ptr<GtObject> TestOperation::execute(GtOperationExecutionContext& context)
{
    if (objectName() == QStringLiteral("fails")) throw std::runtime_error("test operation failure");
    context.events().publish(QStringLiteral("test.started"));
    auto result = std::make_unique<GtObject>();
    result->setObjectName(requiresProject() && GtExecutionContext::current()
                              ? QStringLiteral("project-context-present")
                              : (context.data() ? context.data()->objectName() : QStringLiteral("no-data")));
    context.events().publish(QStringLiteral("test.finished"));
    return result;
}
GtOperationApplyStatus TestOperation::applyResult(GtObject const*, GtExecutionContext&) const
{
    return GtOperationApplyStatus::success();
}
