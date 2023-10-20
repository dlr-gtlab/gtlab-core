/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
    QVector<STATE> states{
        NONE,
        RUNNING,
        QUEUED,
        FAILED,
        FINISHED,
        WARN_FINISHED,
        CONNECTING,
        SKIPPED,
        TERMINATION_REQUESTED,
        TERMINATED
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
