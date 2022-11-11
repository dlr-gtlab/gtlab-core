/* GTlab - Gas Turbine laboratory
 * Source File: gt_objectfiltermodel.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>

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
    invalidate();
}

bool
GtObjectFilterModel::filterAcceptsRow(int source_row,
                                      const QModelIndex& source_parent) const
{
    if (!source_parent.isValid())
    {
        return true;
    }

    bool retval = GtTreeFilterModel::filterAcceptsRow(source_row,
                  source_parent);

    if (!retval)
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

        return acceptsRow(obj->metaObject()->className());
    }

    return false;
}

Qt::ItemFlags
GtObjectFilterModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return 0;
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
    return m_filterData.contains(ident);
}
