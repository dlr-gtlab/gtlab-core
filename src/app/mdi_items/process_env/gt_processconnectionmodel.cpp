/* GTlab - Gas Turbine laboratory
 * Source File: gt_processconnectionmodel.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVariant>

#include "gt_task.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"

#include "gt_processconnectionmodel.h"

GtProcessConnectionModel::GtProcessConnectionModel(QObject* parent) :
    QAbstractItemModel(parent),
    m_root(nullptr),
    m_mode(READ_WRITE)
{

}

GtProcessConnectionModel::~GtProcessConnectionModel()
{
    // delete root item
    delete m_root;
}

void
GtProcessConnectionModel::setRoot(GtTask* task)
{
    // notify model to begin reset procedure
    beginResetModel();

    // check root item
    if (m_root)
    {
        // root item exists - delete it
        delete m_root;

        // set root item to null
        m_root = nullptr;
    }

    // check task
    if (task)
    {
        // create data tree
        createDataTree(task);
    }

    // notify model to end reset procedure
    endResetModel();
}

void
GtProcessConnectionModel::setMode(GtProcessConnectionModel::Modes mode)
{
    // check whether mode is already active
    if (m_mode == mode)
    {
        return;
    }

    m_mode = mode;

    // check root item. if exists reset entire model
    if (m_root)
    {
        setRoot(qobject_cast<GtTask*>(m_root));
    }
}

int
GtProcessConnectionModel::rowCount(const QModelIndex& parent) const
{
    // check root
    if (!m_root)
    {
        return 0;
    }

    // check parent index
    if (!parent.isValid())
    {
        return 1;
    }

    // get item corresponding to index
    GtProcessConnectionItem* parentItem = itemFromIndex(parent);

    // check item
    if (!parentItem)
    {
        return 0;
    }

    // return child count
    return parentItem->findDirectChildren<GtProcessConnectionItem*>().count();
}

int
GtProcessConnectionModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

QVariant
GtProcessConnectionModel::data(const QModelIndex& index, int role) const
{
    // check model index
    if (!index.isValid())
    {
        return QVariant();
    }

    // get item corresponding to index
    GtProcessConnectionItem* item = itemFromIndex(index);

    // check item
    if (!item)
    {
        return QVariant();
    }

    // return item specific data
    return item->data(index.column(), role);
}

Qt::ItemFlags
GtProcessConnectionModel::flags(const QModelIndex& /*index*/) const
{
    Qt::ItemFlags flags;

    flags =  Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    return flags;
}

QModelIndex
GtProcessConnectionModel::index(int row, int col,
                                const QModelIndex& parent) const
{
    // check parent index
    if (!parent.isValid())
    {
        // root item
        if (!m_root)
        {
            // no root set
            return {};
        }

        // check row
        if (row != 0)
        {
            return {};
        }

        // return new index for root
        return createIndex(row, col, m_root);
    }

    // get item corresponding to index
    GtProcessConnectionItem* parentItem = itemFromIndex(parent);

    // check item
    if (!parentItem)
    {
        return {};
    }

    // get child monitoring item
    GtProcessConnectionItem* childItem =
          parentItem->findDirectChildren<GtProcessConnectionItem*>().value(row);

    // check child item
    if (!childItem)
    {
        return {};
    }

    // return new index for monitoring item
    return createIndex(row, col, childItem);
}

QModelIndex
GtProcessConnectionModel::parent(const QModelIndex& index) const
{
    // check index
    if (!index.isValid())
    {
        return {};
    }

    // get item from index
    GtProcessConnectionItem* childItem = itemFromIndex(index);

    // check item
    if (!childItem)
    {
        return {};
    }

    // ger parent item
    GtObject* parentItem = childItem->parentObject();

    // check parent item
    if (!parentItem)
    {
        return {};
    }

    // return index corresponding to parent item
    return indexFromItem(qobject_cast<GtProcessConnectionItem*>(parentItem));
}

GtProcessConnectionItem*
GtProcessConnectionModel::itemFromIndex(const QModelIndex& index) const
{
    // check index
    if (!index.isValid())
    {
        return nullptr;
    }

    // static cast item
    GtProcessConnectionItem* item =
            static_cast<GtProcessConnectionItem*>(index.internalPointer());

    // return item
    return item;
}

QModelIndex
GtProcessConnectionModel::indexFromItem(GtProcessConnectionItem* item) const
{
    // check item
    if (!item)
    {
        return {};
    }

    // initialize row
    int row = -1;

    // check parent item
    if (!item->parent())
    {
        // check whether item is root
        if (m_root != item)
        {
            return {};
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
        qWarning() << "WARNING (GtProcessConnectionModel::indexFromItem): " <<
                      "row == -1!";
        return {};
    }

    // return index for given item
    return createIndex(row, 0, item);
}

GtProcessConnectionItem*
GtProcessConnectionModel::itemById(const QString& uuid, const QString& propId)
{
    // check root
    if (!m_root)
    {
        return nullptr;
    }

    // find item by id
    return m_root->itemById(uuid, propId);
}

void
GtProcessConnectionModel::createDataTree(GtTask* task)
{
    // check task
    if (!task)
    {
        return;
    }

    // check root
    if (m_root)
    {
        // root element already exists
        return;
    }

    // create root monitoring item
    m_root = new GtProcessConnectionItem(task);

    // make connections
    connect(m_root, SIGNAL(dataChanged(GtObject*)),
            SLOT(onObjectDataChanged(GtObject*)));

    // fill root item
    createDataTree(task, m_root);
}

void
GtProcessConnectionModel::createDataTree(GtProcessComponent* component,
                                         GtProcessConnectionItem* item)
{
    // check process component
    if (!component)
    {
        return;
    }

    // check process monitoring item
    if (!item)
    {
        return;
    }

    // parse properties
    QList<GtAbstractProperty*> defaultProperties;

    if (m_mode == GtProcessConnectionModel::READ_WRITE)
    {
        // get read-write properties
        defaultProperties = component->readWriteProperties();
    }
    else
    {
        // get write only properties
        defaultProperties = component->nonReadOnlyProperties();
    }

    // parse monitoring properties (read write mode)
    if (m_mode == GtProcessConnectionModel::READ_WRITE)
    {
        foreach (GtAbstractProperty* prop, component->monitoringProperties())
        {
            defaultProperties << prop;
        }
    }

    // generate connection items
    foreach (GtAbstractProperty* prop, defaultProperties)
    {
        // check whether property type is accepted
        if (GtProcessConnectionItem::propertyTypeAccepted(prop))
        {
            // create new connection item for each property
            GtProcessConnectionItem* childItem =
                    new GtProcessConnectionItem(prop);

            // append to parent item
            item->appendChild(childItem);
        }
    }

    // parse container
    for (GtPropertyStructContainer& container: component->propertyContainers())
    {
        // create new monitoring item for each child
        GtProcessConnectionItem* childItem = new GtProcessConnectionItem(container);

        // append to parent item
        item->appendChild(childItem);

        for (auto& entry: container)
        {
            GtProcessConnectionItem* childChildItem = new GtProcessConnectionItem(container, entry);

            // append to parent item
            childItem->appendChild(childChildItem);

            // generate connection items
            foreach (GtAbstractProperty* prop, entry.fullProperties())
            {
                // check whether property type is accepted
                if (GtProcessConnectionItem::propertyTypeAccepted(prop))
                {
                    // create new connection item for each property
                    GtProcessConnectionItem* propItem =
                            new GtProcessConnectionItem(*component, container,
                                                        entry, *prop);

                    // append to parent item
                    childChildItem->appendChild(propItem);
                }
            }
        }
    }

    // parse children
    foreach (GtProcessComponent* child,
             component->findDirectChildren<GtProcessComponent*>())
    {
        // create new monitoring item for each child
        GtProcessConnectionItem* childItem = new GtProcessConnectionItem(child);

        // append to parent item
        item->appendChild(childItem);

        // continue to create data tree recursively
        createDataTree(child, childItem);
    }
}

void
GtProcessConnectionModel::dataChangedToRoot(const QModelIndex& index)
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
GtProcessConnectionModel::onObjectDataChanged(GtObject* obj)
{
    // check object
    if (!obj)
    {
        return;
    }

    // cast object to process monitoring item
    GtProcessConnectionItem* item = qobject_cast<GtProcessConnectionItem*>(obj);

    // check casted process monitoring item
    if (!item)
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
