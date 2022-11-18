/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 9.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "test_sleepcalculator.h"

#include <QThread>

TestSleepCalculator::TestSleepCalculator() :
    m_pTime{"time", "Time to sleep", "Time to sleep",
            GtUnit::Category::Time, 5}
{
    setObjectName("Sleep");

    registerProperty(m_pTime);
}

bool
TestSleepCalculator::run()
{
    gtInfo().nospace() << "Sleeping for " << m_pTime() << "s...";

    QThread::currentThread()->msleep(m_pTime * 1000);

    return true;
}
