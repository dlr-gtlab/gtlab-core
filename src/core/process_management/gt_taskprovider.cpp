/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_taskprovider.cpp
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_task.h"

#include "gt_taskprovider.h"

GtTaskProvider::GtTaskProvider(GtTask* initTask)
{
    if (initTask)
    {
        if (initComponent(initTask->metaObject()))
        {
            setComponentData(initTask->toMemento());
        }
    }
}
