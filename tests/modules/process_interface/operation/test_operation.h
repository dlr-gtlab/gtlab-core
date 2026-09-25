/* GTlab - Gas Turbine laboratory
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */
#ifndef TEST_OPERATION_H
#define TEST_OPERATION_H
#include "gt_executableoperation.h"
#include "gt_boolproperty.h"
#include "gt_object.h"
#include "gt_stringproperty.h"
class TestOperation final : public GtExecutableOperation
{
    Q_OBJECT
public:
    Q_INVOKABLE TestOperation();
    bool requiresProject() const override;
    std::unique_ptr<GtObject> createData(
        GtExecutionContext const&) const override
    {
        return {};
    }
    std::unique_ptr<GtObject> execute(
        GtOperationExecutionContext& context) override;
    GtOperationApplyStatus applyResult(GtObject const*,
                                       GtExecutionContext&) const override;

private:
    GtStringProperty m_resultName;
    GtBoolProperty m_requiresProject;
    GtBoolProperty m_failOnExecute;
};
#endif
