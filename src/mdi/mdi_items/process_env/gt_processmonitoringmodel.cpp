/* GTlab - Gas Turbine laboratory
 * Source File: gt_processmonitoringmodel.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 14.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVariant>
#include <QDebug>

#include "gt_task.h"

#include "gt_processmonitoringmodel.h"

GtProcessMonitoringModel::GtProcessMonitoringModel(QObject* parent) :
    QAbstractItemModel(parent),
    m_root(Q_NULLPTR)
{

}

GtProcessMonitoringModel::~GtProcessMonitoringModel()
{
    // delete root item if exists
    if (m_root != Q_NULLPTR)
    {
        delete m_root;
    }
}

void
GtProcessMonitoringModel::clearModel()
{
    beginResetModel();
    if (m_root != Q_NULLPTR)
    {
        m_root = Q_NULLPTR;
    }
    endResetModel();
}


void
GtProcessMonitoringModel::setMonitoringData(GtTask* task)
{
    // notify model to begin reset procedure
    beginResetModel();

    // check root item
    if (m_root != Q_NULLPTR)
    {
        // root item exists - delete it
        delete m_root;

        // set root item to null
        m_root = Q_NULLPTR;
    }

    // check task
    if (task != Q_NULLPTR)
    {
        // create data tree
        createDataTree(task);
    }

    // notify model to end reset procedure
    endResetModel();
}

QList<GtProcessMonitoringItem*>
GtProcessMonitoringModel::selectedItems()
{
    // create empty list
    QList<GtProcessMonitoringItem*> retval;

    // check root item
    if (m_root != Q_NULLPTR)
    {
        // append selected items of root element recursively
        retval.append(m_root->selectedItems());
    }

    // return list of selected items
    return retval;
}

int
GtProcessMonitoringModel::rowCount(const QModelIndex& parent) const
{
    // check root
    if (m_root == Q_NULLPTR)
    {
        return 0;
    }

    // check parent index
    if (!parent.isValid())
    {
        return 1;
    }

    // get item corresponding to index
    GtProcessMonitoringItem* parentItem = itemFromIndex(parent);

    // check item
    if (parentItem == Q_NULLPTR)
    {
        return 0;
    }

    // return child count
    return parentItem->findDirectChildren<GtProcessMonitoringItem*>().count();
}

int
GtProcessMonitoringModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 4;
}

QVariant
GtProcessMonitoringModel::data(const QModelIndex& index, int role) const
{
    // check model index
    if (!index.isValid())
    {
        return QVariant();
    }

    // get item corresponding to index
    GtProcessMonitoringItem* item = itemFromIndex(index);

    // check item
    if (item == Q_NULLPTR)
    {
        return QVariant();
    }

    // return item specific data
    return item->data(index.column(), role);
}

bool
GtProcessMonitoringModel::setData(const QModelIndex& index,
                                  const QVariant& value, int role)
{
    // check model index
    if (!index.isValid())
    {
        return false;
    }

    // get item corresponding to index
    GtProcessMonitoringItem* item = itemFromIndex(index);

    // check item
    if (item == Q_NULLPTR)
    {
        return false;
    }

    // return item specific data
    bool retval = item->setData(index.column(), value, role);

    if (role == Qt::CheckStateRole && retval)
    {
        emit selectionChanged();
    }

    return retval;
}

QModelIndex
GtProcessMonitoringModel::index(int row, int col,
                                const QModelIndex& parent) const
{
    // check parent index
    if (!parent.isValid())
    {
        // root item
        if (m_root == Q_NULLPTR)
        {
            // no root set
            return QModelIndex();
        }

        // check row
        if (row != 0)
        {
            return QModelIndex();
        }

        // return new index for root
        return createIndex(row, col, m_root);
    }

    // get item corresponding to index
    GtProcessMonitoringItem* parentItem = itemFromIndex(parent);

    // check item
    if (parentItem == Q_NULLPTR)
    {
        return QModelIndex();
    }

    // get child monitoring item
    GtProcessMonitoringItem* childItem =
          parentItem->findDirectChildren<GtProcessMonitoringItem*>().value(row);

    // check child item
    if (childItem == Q_NULLPTR)
    {
        return QModelIndex();
    }

    // return new index for monitoring item
    return createIndex(row, col, childItem);
}

QModelIndex
GtProcessMonitoringModel::parent(const QModelIndex& index) const
{
    // check index
    if (!index.isValid())
    {
        return QModelIndex();
    }

    // get item from index
    GtProcessMonitoringItem* childItem = itemFromIndex(index);

    // check item
    if (childItem == Q_NULLPTR)
    {
        return QModelIndex();
    }

    // ger parent item
    GtObject* parentItem = childItem->parentObject();

    // check parent item
    if (parentItem == Q_NULLPTR)
    {
        return QModelIndex();
    }

    // return index corresponding to parent item
    return indexFromItem(qobject_cast<GtProcessMonitoringItem*>(parentItem));
}

QVariant
GtProcessMonitoringModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole))
    {
        switch (section) {
        case 0:
            return tr("Process Element");
            break;
        case 1:
            return tr("Value");
            break;
        case 2:
            return tr("Unit");
            break;
        case 3:
            return tr("State");
            break;
        default:
            break;
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags
GtProcessMonitoringModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags;

    if (index.isValid())
    {
        // get item corresponding to index
        GtProcessMonitoringItem* item = itemFromIndex(index);

        // check item
        if (item != Q_NULLPTR)
        {
            // check whether item is a property
            if (item->itemType() ==
                    GtProcessMonitoringItem::MONITORING_PROPERTY)
            {
                flags =  Qt::ItemIsSelectable | Qt::ItemIsEnabled |
                         Qt::ItemIsUserCheckable;
                return flags;
            }
        }
    }

    flags =  Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    return flags;
}

GtProcessMonitoringItem*
GtProcessMonitoringModel::itemFromIndex(const QModelIndex& index) const
{
    // check index
    if (!index.isValid())
    {
        return Q_NULLPTR;
    }

    // static cast item
    GtProcessMonitoringItem* item =
            static_cast<GtProcessMonitoringItem*>(index.internalPointer());

    // return item
    return item;
}

QModelIndex
GtProcessMonitoringModel::indexFromItem(GtProcessMonitoringItem* item) const
{
    // check item
    if (item == Q_NULLPTR)
    {
        return QModelIndex();
    }

    // initialize row
    int row = -1;

    // check parent item
    if (item->parent() == Q_NULLPTR)
    {
        // check whether item is root
        if (m_root != item)
        {
            return QModelIndex();
        }

        // item is root
        row = 0;
    }
    else
    {
        // non root item
        row = item->childNumber();
    }

    // check row
    if (row == -1)
    {
        qWarning() << "WARNING (GtProcessMonitoringModel::indexFromItem): " <<
                      "row == -1!";
        return QModelIndex();
    }

    // return index for given item
    return createIndex(row, 0, item);
}

void
GtProcessMonitoringModel::createDataTree(GtTask* task)
{
    // check task
    if (task == Q_NULLPTR)
    {
        return;
    }

    // check root
    if (m_root != Q_NULLPTR)
    {
        // root element already exists
        return;
    }

    // create root monitoring item
    m_root = new GtProcessMonitoringItem(task);

    // make connections
    connect(m_root, SIGNAL(dataChanged(GtObject*)),
            SLOT(onObjectDataChanged(GtObject*)));

    // fill root item
    createDataTree(task, m_root);
}

void
GtProcessMonitoringModel::createDataTree(GtProcessComponent* component,
                                         GtProcessMonitoringItem* item)
{
    // check process component
    if (component == Q_NULLPTR)
    {
        return;
    }

    // check process monitoring item
    if (item == Q_NULLPTR)
    {
        return;
    }

    // parse monitoring properties
    foreach (GtAbstractProperty* prop, component->monitoringProperties())
    {
        // create new monitoring item for each property
        GtProcessMonitoringItem* childItem = new GtProcessMonitoringItem(prop);

        // append to parent item
        item->appendChild(childItem);
    }

    // parse children
    foreach (GtProcessComponent* child,
             component->findDirectChildren<GtProcessComponent*>())
    {
        // create new monitoring item for each child
        GtProcessMonitoringItem* childItem = new GtProcessMonitoringItem(child);

        // append to parent item
        item->appendChild(childItem);

        // continue to create data tree recursively
        createDataTree(child, childItem);
    }
}

void
GtProcessMonitoringModel::dataChangedToRoot(const QModelIndex& index)
{
    // check index
    if (!index.isValid())
    {
        return;
    }

    // temporary index for second column
    QModelIndex tmpIndex = createIndex(index.row(), 3,
                                       index.internalPointer());

    // notify model
    emit dataChanged(index, tmpIndex);

    // check parent index
    if (index.parent().isValid())
    {
        // notify model recursively
        dataChangedToRoot(index.parent());
    }
}

void
GtProcessMonitoringModel::onObjectDataChanged(GtObject* obj)
{
    // check object
    if (obj == Q_NULLPTR)
    {
        return;
    }

    // cast object to process monitoring item
    GtProcessMonitoringItem* item = qobject_cast<GtProcessMonitoringItem*>(obj);

    // check casted process monitoring item
    if (item == Q_NULLPTR)
    {
        return;
    }

    // get index of object
    QModelIndex index = indexFromItem(item);

    // check index
    if (!index.isValid())
    {
        return;
    }

    // notify model up to root
    dataChangedToRoot(index);
}
