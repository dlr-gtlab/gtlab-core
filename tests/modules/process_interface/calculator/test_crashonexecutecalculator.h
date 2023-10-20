/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 9.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef TESTCRASHONEXECUTECALCULATOR_H
#define TESTCRASHONEXECUTECALCULATOR_H

#include "gt_calculator.h"

/**
 * @brief The TestCrashOnExecuteCalculator class
 * Throws an exception in the run method and thus crashes GTlab
 */
class TestCrashOnExecuteCalculator : public GtCalculator
{
    Q_OBJECT

public:

    Q_INVOKABLE TestCrashOnExecuteCalculator();

    bool run() override;
};

#endif // TESTCRASHONEXECUTECALCULATOR_H
