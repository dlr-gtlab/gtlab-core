/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: gt_abstractprocessexecutor.h
 *
 *  Created on: 17.11.2026
 *  Author: Robert Marcenko (AT-TWK)
 */
#ifndef GT_ABSTRACTPROCESSEXECUTOR_H
#define GT_ABSTRACTPROCESSEXECUTOR_H

#include "gt_core_exports.h"

#include <QObject>

class GtCalculator;
class GtTask;

/**
 * @brief Base class for executors that run process components (calculators
 * and tasks) in a non-local execution mode.
 *
 * Both exec overloads default to local execution, so an executor only needs
 * to override the overloads it actually handles.
 */
class GT_CORE_EXPORT GtAbstractProcessExecutor : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Executes the given calculator.
     * Default implementation runs the calculator locally.
     * @param calculator Calculator to execute
     * @return Whether the execution was successful
     */
    virtual bool exec(GtCalculator* calculator);

    /**
     * @brief Executes one iteration of the given task.
     * Default implementation runs the task iteration locally.
     * @param task Task to execute
     * @return Whether the execution was successful
     */
    virtual bool exec(GtTask* task);

    ~GtAbstractProcessExecutor() override;

protected:
    GtAbstractProcessExecutor();
};

#endif // GT_ABSTRACTPROCESSEXECUTOR_H
