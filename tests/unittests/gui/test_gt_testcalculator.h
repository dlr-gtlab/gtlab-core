/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.12.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#ifndef TEST_GT_TESTCALCULATOR_H
#define TEST_GT_TESTCALCULATOR_H

#include "gt_calculator.h"

class TestCalculator : public GtCalculator
{
    Q_OBJECT

public:

    Q_INVOKABLE TestCalculator()
    {

    };

    bool run() override
    {
        return true;
    };
};

#endif // TEST_GT_TESTCALCULATOR_H
