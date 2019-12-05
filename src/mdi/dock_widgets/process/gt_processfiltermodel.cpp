/* GTlab - Gas Turbine laboratory
 * Source File: gt_processfiltermodel.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.07.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_task.h"
#include "gt_calculator.h"
#include "gt_processdata.h"

#include "gt_processfiltermodel.h"

GtProcessFilterModel::GtProcessFilterModel(QObject* parent) :
    GtTreeFilterModel(parent)
{

}

bool
GtProcessFilterModel::filterAcceptsRow(int source_row,
                                       const QModelIndex& source_parent) const
{
    bool retval = GtTreeFilterModel::filterAcceptsRow(source_row,
                  source_parent);

    if (retval == false)
    {
        return false;
    }

    QModelIndex source_index = sourceModel()->index(source_row,
                               this->filterKeyColumn(),
                               source_parent);

    if (source_index.isValid())
    {
        int i, nb = sourceModel()->rowCount(source_index) ;

        for (i = 0; i < nb; ++i)
        {
            if (filterAcceptsRow(i, source_index))
            {
                return true ;
            }
        }

        GtObject* obj = static_cast<GtObject*>(source_index.internalPointer());

        if (qobject_cast<GtTask*>(obj) || qobject_cast<GtCalculator*>(obj) ||
                qobject_cast<GtProcessData*>(obj))
        {
            return true;
        }

        return false;
    }

    return false;
}
