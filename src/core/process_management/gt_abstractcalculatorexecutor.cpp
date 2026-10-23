/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractcalculatorexecutor.cpp
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_abstractcalculatorexecutor.h"

#include "gt_logging.h"

bool
GtAbstractCalculatorExecutor::exec(GtTask* Task)
{
    if (!Task) return false;

    // executor does not provide a task specific implementation,
    // defaulting to local execution of the task
    gtWarning() << tr("The executor does not have a task specific "
                      "implementation and defaults to local execution "
                      "of the task");
    return Task->runIteration();
}

GtAbstractCalculatorExecutor::GtAbstractCalculatorExecutor()
{
    // nothing to do here
}

GtAbstractCalculatorExecutor::~GtAbstractCalculatorExecutor() = default;
