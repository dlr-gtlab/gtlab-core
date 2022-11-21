/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 9.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "test_processstatescalculator.h"

#include <QThread>

TestProcessStatesCalculator::TestProcessStatesCalculator() :
    m_pInterval{"interval", "Interval", "Time until next state change",
                GtUnit::Time, 5},
    m_pIterations{"iterations", "Iterations",
                  "How many times should the state change",
                  GtUnit::NonDimensional, 2}
{
    setObjectName("Process State Changer");

    registerProperty(m_pInterval);
    registerProperty(m_pIterations);
}

bool
TestProcessStatesCalculator::run()
{
    // states to cycle
    QVector<STATE> states{
        CONNECTING,
        QUEUED,
        CONNECTING,
        SKIPPED,
        TERMINATION_REQUESTED,
    };

    for (int i = 0; i < m_pIterations - 1; ++i)
    {
        STATE state = states.at(i % states.size());

        gtInfo() << "Changing state to" << state;

        setState(state);

        gtInfo().nospace() << "Sleeping for " << m_pInterval() << "s...";

        QThread::currentThread()->msleep(m_pInterval * 1000);
    }

    setState(RUNNING);

    return true;
}