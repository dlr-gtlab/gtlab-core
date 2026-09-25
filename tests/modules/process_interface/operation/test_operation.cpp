/* GTlab - Gas Turbine laboratory
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */
#include "test_operation.h"
#include "gt_executioneventstream.h"
#include "gt_operationexecutioncontext.h"
#include "gt_executioncontext.h"
#include <stdexcept>

TestOperation::TestOperation() :
    m_resultName("resultName", tr("Result Name"), tr("Name of the result")),
    m_requiresProject("requiresProject", tr("Requires Project"),
                      tr("Whether execution requires a project"), false),
    m_failOnExecute("failOnExecute", tr("Fail on Execute"),
                    tr("Whether execution throws an error"), false)
{
    registerProperty(m_resultName);
    registerProperty(m_requiresProject);
    registerProperty(m_failOnExecute);
}

bool
TestOperation::requiresProject() const
{
    return m_requiresProject.getVal();
}

std::unique_ptr<GtObject>
TestOperation::execute(GtOperationExecutionContext& context)
{
    if (m_failOnExecute.getVal())
    {
        throw std::runtime_error("test operation failure");
    }

    context.events().publish(QStringLiteral("test.started"));
    auto result = std::make_unique<GtObject>();
    QString resultName = m_resultName.getVal();
    if (resultName.isEmpty())
    {
        resultName = context.data() ? context.data()->objectName()
                                    : QStringLiteral("no-data");
    }
    if (m_requiresProject.getVal() && GtExecutionContext::current())
    {
        resultName = QStringLiteral("project-context-present");
    }
    result->setObjectName(resultName);
    context.events().publish(QStringLiteral("test.finished"));
    return result;
}

GtOperationApplyStatus
TestOperation::applyResult(GtObject const*, GtExecutionContext&) const
{
    return GtOperationApplyStatus::success();
}
