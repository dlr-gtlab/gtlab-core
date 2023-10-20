/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 07.01.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_tasklink.h"
#include "gt_processdata.h"
#include "gt_task.h"
#include "gt_taskgroup.h"

GtTaskLink::GtTaskLink()
{
    setObjectName(QStringLiteral("Task Link"));
}

GtTask*
GtTaskLink::target()
{
    GtTaskGroup* group = findParent<GtTaskGroup*>();

    if (!group)
    {
        return nullptr;
    }

    return group->findDirectChild<GtTask*>(objectName());
}

bool
GtTaskLink::run()
{
    return true;
}

