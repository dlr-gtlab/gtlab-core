/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_objectfiltermodel.cpp
 *
 *  Created on: 19.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_object.h"

#include "gt_objectfiltermodel.h"

GtObjectFilterModel::GtObjectFilterModel(QObject* parent) :
    GtTreeFilterModel(parent)
{

}

void
GtObjectFilterModel::setFilterData(const QStringList& val)
{
    m_filterData = val;
    m_filterLookup = QSet<QString>(m_filterData.begin(), m_filterData.end());
    invalidate();
}

bool
GtObjectFilterModel::filterAcceptsRow(int source_row,
                                      const QModelIndex& source_parent) const
{
    return GtTreeFilterModel::filterAcceptsRow(source_row, source_parent,
        [this](const GtObject* obj) {
          return acceptsRow(obj->metaObject()->className());
    });
}

Qt::ItemFlags
GtObjectFilterModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return {};
    }

    Qt::ItemFlags defaultFlags = GtTreeFilterModel::flags(index);

    QModelIndex source_index = mapToSource(index);

    if (source_index.isValid())
    {
        GtObject* obj = static_cast<GtObject*>(source_index.internalPointer());

        if (!acceptsRow(obj->metaObject()->className()))
        {
            return defaultFlags & ~Qt::ItemIsEnabled;
        }
    }

    return defaultFlags;
}

bool
GtObjectFilterModel::acceptsRow(const char* ident) const
{
    return m_filterLookup.contains(QString::fromLatin1(ident));
}
