/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 22.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

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
    if (filterRegExp().isEmpty())
    {
        return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    }

    return filterAcceptsRow(source_row, source_parent, [](const GtObject*)
    {
        return true;
    });
}
