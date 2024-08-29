/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processfiltermodel.cpp
 *
 *  Created on: 19.07.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_task.h"
#include "gt_calculator.h"
#include "gt_taskgroup.h"

#include "gt_processfiltermodel.h"

GtProcessFilterModel::GtProcessFilterModel(QObject* parent) :
    GtTreeFilterModel(parent)
{

}

bool
GtProcessFilterModel::filterAcceptsRow(int source_row,
                                       const QModelIndex& source_parent) const
{
    return GtTreeFilterModel::filterAcceptsRow(source_row, source_parent,
                                               [](GtObject* obj)
    {
        return qobject_cast<GtTask*>(obj) || qobject_cast<GtCalculator*>(obj) ||
               qobject_cast<GtTaskGroup*>(obj) || obj->isDummy();
    });
}
