/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractprocessexecutor.cpp
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#include "gt_abstractprocessexecutor.h"

#include "gt_calculator.h"
#include "gt_task.h"

bool
GtAbstractProcessExecutor::exec(GtCalculator* calculator)
{
    return calculator ? calculator->run() : false;
}

bool
GtAbstractProcessExecutor::exec(GtTask* task)
{
    if (!task) return false;

    // Initialize individual evaluator setting
    if (!task->setUp())
    {
        task->setState(GtProcessComponent::FAILED);
        return false;
    }

    return task->runIteration();
}

GtAbstractProcessExecutor::GtAbstractProcessExecutor() = default;

GtAbstractProcessExecutor::~GtAbstractProcessExecutor() = default;
