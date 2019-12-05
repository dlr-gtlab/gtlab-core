/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 22.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>

#include "gt_treefiltermodel.h"

GtTreeFilterModel::GtTreeFilterModel(QObject* parent) :
    QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void
GtTreeFilterModel::filterData(const QString& val)
{
    setFilterRegExp(val);
    invalidate();
}

bool
GtTreeFilterModel::filterAcceptsRow(int source_row,
                                    const QModelIndex& source_parent) const
{
    if (filterRegExp().isEmpty() == false)
    {
        QModelIndex source_index =
                sourceModel()->index(source_row,
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
            QString key = sourceModel()->data(source_index,
                                              filterRole()).toString();

            return key.contains(filterRegExp()) ;
        }
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row,
                                                   source_parent);
}
