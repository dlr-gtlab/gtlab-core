/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_process_executor.cpp
 *
 *  Created on: 18.11.2026
 *  Author: Robert Marcenko (AT-TWK)
 */
#include "test_process_executor.h"
#include "gt_calculator.h"
#include "gt_task.h"
#include <gt_logging.h>

GtTestExecutor::GtTestExecutor()=default;

bool GtTestExecutor::exec(GtCalculator *calculator)
{
    //Runs before calculator execution
    gtInfo()<<"Process Executor is running for: "<< calculator->objectName();
    bool success = GtAbstractProcessExecutor::exec(calculator);
    //Runs after calculator execution
    gtInfo()<<"Process Executor executed calculator:"<<calculator->objectName()<<" which returned status: "<< success;
    return success;
}

bool GtTestExecutor::exec(GtTask *task)
{
    //Runs before task execution
    gtInfo()<<"Process Executor is running for: "<< task->objectName();
    bool success = GtAbstractProcessExecutor::exec(task);
    //Runs after task execution
    gtInfo()<<"Process Executor executed task:"<<task->objectName()<<" which returned status: "<< success;
    return success;
}

