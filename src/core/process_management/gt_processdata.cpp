/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 05.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_processdata.h"
#include "gt_task.h"

GtProcessData::GtProcessData()
{
    setObjectName(QStringLiteral("Process Data"));

    setUserHidden(true);
}

QList<GtTask*>
GtProcessData::processList()
{
    return findDirectChildren<GtTask*>();
}

GtTask*
GtProcessData::findProcess(const QString& val)
{
    return findDirectChild<GtTask*>(val);
}

