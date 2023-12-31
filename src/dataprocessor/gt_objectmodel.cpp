/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_objectmodel.cpp
 *
 *  Created on: 28.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_object.h"

#include "gt_objectmodel.h"

GtObjectModel::GtObjectModel(QObject* parent) : QAbstractItemModel(parent)
{

}

GtObjectModel::GtObjectModel(GtObject* root, QObject* parent) :
    QAbstractItemModel(parent),
    m_root(root)
{

}

void
GtObjectModel::setRootObject(GtObject* obj)
{
    beginResetModel();
    m_root = obj;
    endResetModel();
}

int
GtObjectModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

int
GtObjectModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
    {
        if (!m_root)
        {
            return 0;
        }

        return 1;
    }

    GtObject* parentItem = objectFromIndex(parent);

    if (!parentItem)
    {
        return 0;
    }

    return parentItem->findDirectChildren<GtObject*>().size();
}

QModelIndex
GtObjectModel::index(int row, int col, const QModelIndex& parent) const
{
    if (!parent.isValid())
    {
        if (row >= 1 || row < 0)
        {
            return {};
        }

        if (!m_root)
        {
            return {};
        }

        return createIndex(row, col, m_root);
    }

    GtObject* parentItem = objectFromIndex(parent);

    if (!parentItem)
    {
        return {};
    }

    GtObject* childItem =
        parentItem->findDirectChildren<GtObject*>().value(row);

    if (!childItem)
    {
        return {};
    }

    return createIndex(row, col, childItem);
}

QModelIndex
GtObjectModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return {};
    }

    GtObject* childItem = objectFromIndex(index);

    if (!childItem)
    {
        return {};
    }

    GtObject* parentItem = childItem->parentObject();

    if (!parentItem)
    {
        return {};
    }

    return indexFromObject(parentItem);
}

QVariant
GtObjectModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (index.column() >0 )
    {
        return QVariant();
    }

    switch (role)
    {
        case Qt::DisplayRole:
        {
            GtObject* item = objectFromIndex(index);

            if (item)
            {
                return item->objectName();
            }

            break;
        }

        default:
            break;
    }

    return QVariant();
}

GtObject*
GtObjectModel::objectFromIndex(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return nullptr;
    }

    return static_cast<GtObject*>(index.internalPointer());
}

QModelIndex
GtObjectModel::indexFromObject(GtObject* obj) const
{
    if (!obj)
    {
        return {};
    }

    int row = -1;

    if (obj == m_root)
    {
        row = 0;
    }
    else
    {
        if (obj->parent())
        {
            row = obj->childNumber();
        }
    }

    if (row == -1)
    {
        return {};
    }

    return createIndex(row, 0, obj);
}
