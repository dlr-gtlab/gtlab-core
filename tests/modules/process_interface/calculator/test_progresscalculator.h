/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2023 by DLR
 *
 *  Created on: 18.01.2023
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef TESTPROGRESSCALCULATOR_H
#define TESTPROGRESSCALCULATOR_H

#include "gt_calculator.h"

/**
 * @brief The TestProgressCalculator class
 * Calculator to test progress. Starts sleeping for 3 seconds five times
 * to show progress in steps of 20 percent
 */
class TestProgressCalculator : public GtCalculator
{
    Q_OBJECT

public:
    Q_INVOKABLE TestProgressCalculator();

    bool run() override;

};

#endif // TESTPROGRESSCALCULATOR_H
