/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 09.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_outputtester.h"
#include "gt_logging.h"

#include <QRandomGenerator>

GtOutputTester::GtOutputTester()
{ }

void
GtOutputTester::run()
{
    for (int i = 0; i < 1000; i++)
    {
        usleep(5);
        gtDebug() << "TEST:" << i
                  << QStringLiteral("test ")
                     .repeated(QRandomGenerator::global()->bounded(100));
    }
}

