/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2023 by DLR
 *
 *  Created on: 18.01.2023
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "test_progresscalculator.h"

#include <QThread>

TestProgressCalculator::TestProgressCalculator()
{
    setObjectName("ProgressTest-Calculator");
}

bool
TestProgressCalculator::run()
{
    int sleep = 3;

    gtInfo().nospace() << "A - Sleeping for " << sleep << "s...";

    QThread::currentThread()->msleep(sleep * 1000);

    setProgress(20);

    gtInfo().nospace() << "B - Sleeping for " << sleep << "s...";

    QThread::currentThread()->msleep(sleep * 1000);

    setProgress(40);

    gtInfo().nospace() << "C - Sleeping for " << sleep << "s...";

    QThread::currentThread()->msleep(sleep * 1000);

    setProgress(60);

    gtInfo().nospace() << "D - Sleeping for " << sleep << "s...";

    QThread::currentThread()->msleep(sleep * 1000);

    setProgress(80);

    gtInfo().nospace() << "E - Sleeping for " << sleep << "s...";

    QThread::currentThread()->msleep(sleep * 1000);

    setProgress(100);

    gtInfo().nospace() << "Finised";

    return true;
}
