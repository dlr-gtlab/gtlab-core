/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 9.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef TESTSLEEPCALCULATOR_H
#define TESTSLEEPCALCULATOR_H

#include "gt_calculator.h"
#include "gt_doubleproperty.h"

/**
 * @brief The TestSleepCalculator class
 * Pauses for a specific time interval
 */
class TestSleepCalculator : public GtCalculator
{
    Q_OBJECT

public:

    Q_INVOKABLE TestSleepCalculator();

    bool run() override;

private:

    /// How long to sleep
    GtDoubleProperty m_pTime;
};

#endif // TESTSLEEPCALCULATOR_H
