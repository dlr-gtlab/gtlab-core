/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 01.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_explorermodel.h"
#include "gt_object.h"

GtExplorerModel::GtExplorerModel(QObject* parent) :
    GtTreeFilterModel(parent)
{

}

GtExplorerModel::~GtExplorerModel()
{

}

GtObject*
GtExplorerModel::objectFromIndex(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return nullptr;
    }

    if (index.model() != this)
    {
        return nullptr;
    }

    QModelIndex tmp = mapToSource(index);

    if (!tmp.isValid())
    {
        return nullptr;
    }

    return static_cast<GtObject*>(tmp.internalPointer());
}

QModelIndexList
GtExplorerModel::getPersistentIndexList()
{
    return persistentIndexList();
}

void
GtExplorerModel::filterData(const QString& val)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    setFilterRegExp(val);
#else
    setFilterRegularExpression(val);
#endif
    invalidate();
}

Qt::DropActions
GtExplorerModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

bool
GtExplorerModel::filterAcceptsRow(int source_row,
                                  const QModelIndex& source_parent) const
{
    if (source_parent.isValid())
    {
        GtObject* obj = static_cast<GtObject*>(source_parent.internalPointer());

        if (obj)
        {
            GtObjectList childs = obj->findDirectChildren<GtObject*>();

            if (childs.size() > source_row)
            {
                GtObject* c = childs.at(source_row);

                if (c->isUserHidden())
                {
                    return false;
                }
            }
        }
    }

    return GtTreeFilterModel::filterAcceptsRow(source_row, source_parent);

}

