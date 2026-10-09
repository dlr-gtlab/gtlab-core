/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTPROCESSTASKOPERATION_H
#define GTPROCESSTASKOPERATION_H

#include "gt_core_exports.h"
#include "gt_executableoperation.h"
#include "gt_stringproperty.h"

/**
 * @brief Serializable project-diff result returned by ProcessTaskOperation.
 */
class GT_CORE_EXPORT GtProcessTaskOperationResult : public GtObject
{
    Q_OBJECT

public:
    Q_INVOKABLE GtProcessTaskOperationResult();

    QString diffXml() const;
    void setDiffXml(QString xml);

private:
    GtStringProperty m_diffXml;
};

/**
 * @brief Runs an existing GtTask against the execution-local project.
 */
class GT_CORE_EXPORT ProcessTaskOperation : public GtExecutableOperation
{
    Q_OBJECT

public:
    Q_INVOKABLE ProcessTaskOperation();

    bool requiresProject() const override;

    QString taskUuid() const;
    void setTaskUuid(QString uuid);

    std::unique_ptr<GtObject> createData(
        GtExecutionContext const& context) const override;

    GtOperationExecutionResult execute(
        GtOperationExecutionContext& context) override;

    GtOperationApplyStatus applyResult(
        GtOperationExecutionResult const& executionResult,
        GtExecutionContext& context) const override;

private:
    GtStringProperty m_taskUuid;
};

#endif // GTPROCESSTASKOPERATION_H
