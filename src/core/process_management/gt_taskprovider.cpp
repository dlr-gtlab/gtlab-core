/* GTlab - Gas Turbine laboratory
 * Source File: gt_taskprovider.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_task.h"

#include "gt_taskprovider.h"

GtTaskProvider::GtTaskProvider(GtTask* initTask)
{
    if (initTask != Q_NULLPTR)
    {
        if (initComponent(initTask->metaObject()))
        {
            setComponentData(initTask->toMemento());
        }
    }
}
