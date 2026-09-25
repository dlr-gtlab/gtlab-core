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

/**
 * @brief Test operation used by headless operation-execution system tests.
 *
 * Its registered properties configure the returned result, project
 * requirement and optional execution failure. These values are restored from
 * the operation Memento used by the system tests.
 */
class TestOperation final : public GtExecutableOperation
{
    Q_OBJECT
public:
    /**
     * @brief Creates a test operation with default property values.
     */
    Q_INVOKABLE TestOperation();

    /**
     * @brief Checks whether this operation needs a project during execution.
     * @return Value of the requiresProject property.
     */
    bool requiresProject() const override;

    /**
     * @brief Creates no detached input data for this test operation.
     * @param context Execution context of the originating side; unused.
     * @return Null because this test operation has no originating-side input.
     */
    std::unique_ptr<GtObject> createData(
        GtExecutionContext const& context) const override
    {
        Q_UNUSED(context);
        return {};
    }

    /**
     * @brief Publishes test events and returns a configured result object.
     * @param context Execution context containing input data, events and
     *                cancellation state.
     * @return Detached result object, unless the Memento requests a failure.
     */
    std::unique_ptr<GtObject> execute(
        GtOperationExecutionContext& context) override;

    /**
     * @brief Reports successful application of the test result.
     * @param executionResult Optional detached result; unused by this test.
     * @param context Originating-side execution context; unused by this test.
     * @return Successful apply status.
     */
    GtOperationApplyStatus applyResult(
        GtObject const* executionResult,
        GtExecutionContext& context) const override;

private:
    GtStringProperty m_resultName;
    GtBoolProperty m_requiresProject;
    GtBoolProperty m_failOnExecute;
};
#endif
