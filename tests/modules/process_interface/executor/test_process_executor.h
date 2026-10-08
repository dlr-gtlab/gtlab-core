/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_process_executor.h
 *
 *  Created on: 18.11.2026
 *  Author: Robert Marcenko (AT-TWK)
 */
#ifndef TEST_PROCESS_EXECUTOR_H
#define TEST_PROCESS_EXECUTOR_H

#include "gt_abstractprocessexecutor.h"

class GtTestExecutor : public GtAbstractProcessExecutor
{
    Q_OBJECT

public:
    Q_INVOKABLE GtTestExecutor();

    bool exec(GtCalculator* calculator) Q_DECL_OVERRIDE;

    bool exec(GtTask* task) Q_DECL_OVERRIDE;

};

#endif // TEST_PROCESS_EXECUTOR_H
