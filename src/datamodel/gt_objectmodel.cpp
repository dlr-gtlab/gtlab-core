/* GTlab - Gas Turbine laboratory
 * Source File: gt_objectmodel.cpp
 * copyright 2009-2016 by DLR
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
        if (m_root == Q_NULLPTR)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }

    GtObject* parentItem = objectFromIndex(parent);

    if (parentItem == Q_NULLPTR)
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
            return QModelIndex();
        }

        if (m_root == Q_NULLPTR)
        {
            return QModelIndex();
        }

        return createIndex(row, col, m_root);
    }

    GtObject* parentItem = objectFromIndex(parent);

    if (parentItem == Q_NULLPTR)
    {
        return QModelIndex();
    }

    GtObject* childItem =
        parentItem->findDirectChildren<GtObject*>().value(row);

    if (!childItem)
    {
        return QModelIndex();
    }

    return createIndex(row, col, childItem);
}

QModelIndex
GtObjectModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    GtObject* childItem = objectFromIndex(index);

    if (childItem == Q_NULLPTR)
    {
        return QModelIndex();
    }

    GtObject* parentItem = childItem->parentObject();

    if (parentItem == Q_NULLPTR)
    {
        return QModelIndex();
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

            if (item != Q_NULLPTR)
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
        return NULL;
    }

    return static_cast<GtObject*>(index.internalPointer());
}

QModelIndex
GtObjectModel::indexFromObject(GtObject* obj) const
{
    if (obj == Q_NULLPTR)
    {
        return QModelIndex();
    }

    int row = -1;

    if (obj == m_root)
    {
        row = 0;
    }
    else
    {
        if (obj->parent() != Q_NULLPTR)
        {
            row = obj->childNumber();
        }
    }

    if (row == -1)
    {
        return QModelIndex();
    }

    return createIndex(row, 0, obj);
}
