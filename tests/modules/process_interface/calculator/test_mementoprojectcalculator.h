/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef TEST_MEMENTO_PROJECT_CALCULATOR_H
#define TEST_MEMENTO_PROJECT_CALCULATOR_H

#include "gt_calculator.h"

class TestMementoProjectCalculator final : public GtCalculator
{
    Q_OBJECT

public:
    Q_INVOKABLE TestMementoProjectCalculator();

    bool run() override;
};

#endif // TEST_MEMENTO_PROJECT_CALCULATOR_H
