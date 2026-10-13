/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef TEST_OPERATION_H
#define TEST_OPERATION_H

#include "gt_boolproperty.h"
#include "gt_executableoperation.h"

#include <memory>

class TestOperation final : public GtExecutableOperation
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit TestOperation(GtObject* parent = nullptr);

    bool requiresProject() const override;

    std::unique_ptr<GtObject> createData(
        GtExecutionContext const& context) const override;

    GtOperationExecutionResult execute(
        GtOperationExecutionContext& context) override;

    GtOperationApplyStatus applyResult(
        GtOperationExecutionResult const& executionResult,
        GtExecutionContext& context) const override;

private:
    GtBoolProperty m_requiresProject;
    GtBoolProperty m_returnsFailure;
    GtBoolProperty m_returnsCancellation;
    GtBoolProperty m_throwsException;
};

#endif // TEST_OPERATION_H
