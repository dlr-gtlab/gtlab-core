/* GTlab - Gas Turbine laboratory
 * Source File: gt_loop.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 13.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_loop.h"

GtLoop::GtLoop()
{
    setObjectName(QStringLiteral("Loop"));

    enableMaxIterationProperty();
    enableCurrentIterationMonitoring();
}
