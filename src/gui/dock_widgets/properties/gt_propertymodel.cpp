/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_propertymodel.h"
#include "gt_object.h"
#include "gt_abstractproperty.h"
#include "gt_propertyitem.h"
#include "gt_propertycategoryitem.h"
#include "gt_modeproperty.h"
#include "gt_propertymodeitem.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"
#include "gt_application.h"

namespace
{
    constexpr const int MAIN_ORDER = 0;
    constexpr const int DEFAULT_ORDER = 1;

    int getOrder(const GtPropertyCategoryItem* item)
    {
        if (!item) return DEFAULT_ORDER;

        QString id = item->categoryId();

        // main is always first
        if (id == "Main") return MAIN_ORDER;

        // Find the index of the '|' character
        int pipeIndex = id.indexOf("||");
        // If there is no '|', order is 0
        if (pipeIndex == -1) return DEFAULT_ORDER;

        // Extract the part of the string before the '|'
        QString possibleNumber = id.left(pipeIndex).trimmed();

        bool ok = false;
        int order = possibleNumber.toInt(&ok);

        // If parsing failed, return 1; otherwise return the parsed integer
        return ok ? order : DEFAULT_ORDER;
    }

}

GtPropertyModel::GtPropertyModel(GtObject* scope,
                                 QObject* parent) :
    QAbstractItemModel(parent),
    m_obj(nullptr),
    m_scope(scope)
{
}

GtPropertyModel::~GtPropertyModel()
{
    qDeleteAll(m_properties);
}

int
GtPropertyModel::rowCount(const QModelIndex& parent) const
{
    if (!m_obj)
    {
        return 0;
    }

    if (!parent.isValid())
    {
        if (m_catFilter.isEmpty())
        {
            return m_properties.count();
        }

        return filteredCategories().count();
    }

    GtAbstractPropertyItem* parentItem = propertyFromIndex(parent);

    if (!parentItem)
    {
        return 0;
    }

    return parentItem->findDirectChildren<GtAbstractPropertyItem*>().count();
}

int
GtPropertyModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 3;
}

QVariant
GtPropertyModel::data(const QModelIndex& index, int role) const
{
    if (!m_obj)
    {
        return QVariant();
    }

    GtAbstractPropertyItem* item = propertyFromIndex(index);

    if (!item)
    {
        return QVariant();
    }

    if (item->data(index.column(), GtPropertyModel::ContainerRole).toBool())
    {
        if (role == Qt::DisplayRole)
        {
            return item->data(index.column(), role).toString() +
                    + " [" + QString::number(index.row()) + "]";
        }

        if (role == Qt::ToolTipRole)
        {
            auto* container = m_obj->findPropertyContainer(m_containerId);

            if (!container) return {};

            if (index.row() >= static_cast<int>(container->size())) return {};

            return container->at(index.row()).typeName();
        }
    }
    
    return item->data(index.column(), role);
}

Qt::ItemFlags
GtPropertyModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags baseFlags = QAbstractItemModel::flags(index);

    bool isCategory = index.data(GtPropertyModel::CategoryRole).toBool();

    if (index.column() == 0)
    {
        if (index.data(GtPropertyModel::OptionalRole).toBool())
        {
            baseFlags |= Qt::ItemIsUserCheckable;
        }
    }
    else if (index.column() == 1)
    {
        QVariant var = index.data(GtPropertyModel::UnitCategoryRole);
        GtUnit::Category cat = var.value<GtUnit::Category>();

        if (cat != GtUnit::Category::None && !isCategory &&
                cat != GtUnit::Category::NonDimensional)
        {
            if (index.data(GtPropertyModel::ActiveRole).toBool())
            {
                baseFlags |= Qt::ItemIsEditable;
            }
        }
    }
    else if (index.column() == 2)
    {
        QVariant var = index.data();

        if (gt::metaTypeId(var) == QMetaType::Bool)
        {
            if (!index.data(ReadOnlyRole).toBool())
            {
                baseFlags |= Qt::ItemIsUserCheckable;
            }
        }
        else
        {
            if (!index.data(GtPropertyModel::ReadOnlyRole).toBool() &&
                    !isCategory)
            {
                if (index.data(GtPropertyModel::ActiveRole).toBool())
                {
                    baseFlags |= Qt::ItemIsEditable;
                }

                baseFlags |= Qt::ItemIsDropEnabled;
            }
        }
    }

    return baseFlags;
}

bool
GtPropertyModel::setData(const QModelIndex& index,
                         const QVariant& value,
                         int role)
{
    GtAbstractPropertyItem* item = propertyFromIndex(index);

    if (!item)
    {
        return false;
    }

    if (GtPropertyModeItem* m = qobject_cast<GtPropertyModeItem*>(item))
    {
        if (index.column() == 0)
        {
            item->setData(index.column(), value, m_obj, role);
        }

        if (role == Qt::EditRole && index.column() == 2)
        {
            const QString& current = m->modeProperty()->get();
            const QString next = value.toString();

            if (current == next)
            {
                return false;
            }

            if (!m->modeProperty()->modeExists(next))
            {
                return false;
            }

            bool retval =  m->setData(index.column(), value, m_obj, role);

            updateModeItem(index);

            return retval;
        }
    }

    bool retval = item->setData(index.column(), value, m_obj, role);

    if (retval)
    {

        if (index.column() == 1)
        {
            QModelIndex tmpi = createIndex(index.row(), 2);
            emit dataChanged(tmpi, index, QVector<int>() << Qt::DisplayRole);
        }
        else
        {
            emit dataChanged(index, index, QVector<int>() << Qt::DisplayRole);
        }
    }

    if (index.column() == 0)
    {
        QModelIndex tmpi = createIndex(index.row(), 1);
        emit dataChanged(index, tmpi);
    }

    return true;
}

QModelIndex
GtPropertyModel::index(int row,
                       int col,
                       const QModelIndex& parent) const
{
    if (!parent.isValid())
    {
        QList<GtPropertyCategoryItem*> categories;

        if (m_catFilter.isEmpty())
        {
            categories =  m_properties;
        }
        else
        {
            categories = filteredCategories();
        }

        const int propCount = categories.count();

        if (row < 0 || row >= propCount)
        {
            return {};
        }

        return createIndex(row, col, categories[row]);
    }

    GtAbstractPropertyItem* parentItem = propertyFromIndex(parent);

    if (!parentItem)
    {
        return {};
    }

    GtAbstractPropertyItem* childItem =
        parentItem->findDirectChildren<GtAbstractPropertyItem*>().value(row);

    if (!childItem)
    {
        return {};
    }

    return createIndex(row, col, childItem);
}

QModelIndex
GtPropertyModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return {};
    }

    GtAbstractPropertyItem* childItem = propertyFromIndex(index);

    if (!childItem)
    {
        return {};
    }

    GtObject* parentItem = childItem->parentObject();

    if (!parentItem)
    {
        return {};
    }

    return indexFromProperty(qobject_cast<GtAbstractPropertyItem*>(parentItem));
}

QVariant
GtPropertyModel::headerData(int section, Qt::Orientation orientation,
                            int role) const
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole))
    {
        switch (section)
        {
            case 0:
                return tr("Property");

            case 1:
                return tr("Unit");

            case 2:
                return tr("Value");

            default:
                break;
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

void
GtPropertyModel::setObject(GtObject* obj)
{
    if (m_obj)
    {
        disconnect(m_obj.data(), SIGNAL(destroyed(QObject*)), this,
                   SLOT(resetObject()));
    }

    beginResetModel();

    qDeleteAll(m_properties);
    m_properties.clear();
    m_containerId.clear();

    m_obj = obj;

    if (m_obj)
    {
        connect(m_obj.data(), SIGNAL(destroyed(QObject*)), SLOT(resetObject()));

        foreach (GtAbstractProperty* prop, m_obj->properties())
        {
            if (!prop->isHidden())
            {
                addProperty(prop);
            }
        }
    }

    std::stable_sort(m_properties.begin(), m_properties.end(),
        [](GtPropertyCategoryItem* p1, GtPropertyCategoryItem* p2) {
            return getOrder(p1) < getOrder(p2);
    });

    endResetModel();
}

void
GtPropertyModel::setObject(GtObject* obj, GtPropertyStructContainer& container)
{
    if (m_obj)
    {
        disconnect(m_obj.data(), SIGNAL(destroyed(QObject*)), this,
                   SLOT(resetObject()));

        // disconnect container slots if last
        // scope was container entry of object
        if (!m_containerId.isEmpty())
        {
            auto* lastContainer = m_obj->findPropertyContainer(m_containerId);

            assert(lastContainer);

            disconnect(lastContainer, SIGNAL(entryRemoved(int)), this,
                       SLOT(onContainerEntryRemoved(int)));
            disconnect(lastContainer, SIGNAL(entryAdded(int)), this,
                       SLOT(onContainerEntryAdded(int)));
        }

    }

    beginResetModel();
    auto finally = gt::finally([this](){
        endResetModel();
    });
    Q_UNUSED(finally)

    qDeleteAll(m_properties);
    m_properties.clear();

    m_obj = obj;
    m_containerId = container.ident();

    if (!m_obj)
    {
        return;
    }

    connect(m_obj.data(), SIGNAL(destroyed(QObject*)), SLOT(resetObject()));
    connect(&container, SIGNAL(entryRemoved(int)), this,
               SLOT(onContainerEntryRemoved(int)));
    connect(&container, SIGNAL(entryAdded(int)), this,
               SLOT(onContainerEntryAdded(int)));

    for (const auto& entry : container)
    {
        auto* cat = new GtPropertyCategoryItem(m_scope, container.entryPrefix(),
                                               this);
        cat->setIsContainer(true);

        if (container.getFlags() & GtPropertyStructContainer::ReadOnly)
        {
            cat->setIsReadOnly(true);
        }

        m_properties << cat;

        for (auto* pChild : entry.properties())
        {
            cat->addPropertyItem(pChild);
        }
    }
}

QModelIndex
GtPropertyModel::addNewStructContainerEntry(
        GtPropertyStructContainer& container,
        const QString& entryType)
{
    if (!container.allowedTypes().contains(entryType))
    {
        // invalid entry type
        return {};
    }

    beginInsertRows(QModelIndex(), m_properties.size(), m_properties.size());

    GtPropertyStructInstance* newEntry = nullptr;

    {
        auto cmd = gtApp->makeCommand(m_scope,
                                      gt::propertyItemCommandString(
                                          m_obj->objectName(),
                                          container.name(),
                                          QObject::tr("Entry added")));
        Q_UNUSED(cmd)

        newEntry = &container.newEntry(entryType);
    }

    auto* cat = new GtPropertyCategoryItem(m_scope,
                                           container.entryPrefix(),
                                           this);
    cat->setIsContainer(true);
    m_properties << cat;

    for (GtAbstractProperty* pChild : newEntry->properties())
    {
        cat->addPropertyItem(pChild);
    }

    endInsertRows();

    return indexFromProperty(cat);
}

void
GtPropertyModel::removeStructContainerEntry(const QModelIndex& index)
{
    if (!index.isValid() || index.model() != this)
    {
        gtWarning() << tr("invalid index");
        return;
    }

    if (!m_obj)
    {
        gtWarning() << tr("invalid object!");
        return;
    }

    auto* container = m_obj->findPropertyContainer(m_containerId);

    if (!container)
    {
        gtWarning() << tr("container not found!");
        return;
    }

    if (index.row() >= static_cast<int>(container->size()))
    {
        gtWarning() << tr("index out of bounds!");
        return;
    }

    auto& entry = container->at(index.row());


    auto iter = container->findEntry(entry.ident());

    if (iter == container->end())
    {
        // uuid not in container
        return;
    }

    auto cmd = gtApp->makeCommand(m_scope,
                                  gt::propertyItemCommandString(
                                      m_obj->objectName(),
                                      container->name(),
                                      QObject::tr("Entry deleted")));
    Q_UNUSED(cmd)

    container->removeEntry(iter);
}

GtObject*
GtPropertyModel::object()
{
    return m_obj;
}

GtAbstractPropertyItem*
GtPropertyModel::propertyFromIndex(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return nullptr;
    }

    GtAbstractPropertyItem* item =
        static_cast<GtAbstractPropertyItem*>(index.internalPointer());

    return item;
}

bool
GtPropertyModel::removeItem(GtAbstractPropertyItem* item)
{
    QModelIndex index = indexFromProperty(item);

    if (!index.isValid())
    {
        return false;
    }

    QModelIndex parentIndex = index.parent();

    if (!parentIndex.isValid())
    {
        return false;
    }

    beginRemoveRows(parentIndex, item->childNumber(), item->childNumber());

    delete item;

    endRemoveRows();

    return true;
}

void
GtPropertyModel::updateModeItem(const QModelIndex& index)
{
    GtAbstractPropertyItem* item = propertyFromIndex(index);

    if (!item)
    {
        return;
    }

    if (GtPropertyModeItem* m = qobject_cast<GtPropertyModeItem*>(item))
    {
        QList<GtAbstractPropertyItem*> list =
            item->findDirectChildren<GtAbstractPropertyItem*>();

        foreach (GtAbstractPropertyItem* i, list)
        {
            removeItem(i);
        }

        const int noc = m->modeProperty()->propertyCount(
                            m->modeProperty()->get());

        if (noc > 0)
        {
            QModelIndex tmp = indexFromProperty(item);

            beginInsertRows(tmp, 0, noc - 1);
            m->updateSubProperties();
            endInsertRows();
        }
    }
}

QModelIndex
GtPropertyModel::indexFromProperty(GtAbstractPropertyItem* obj) const
{
    if (!obj)
    {
        return {};
    }

    int row = -1;

    if (!obj->parent())
    {
        row = m_properties.indexOf(qobject_cast<GtPropertyCategoryItem*>(obj));
    }
    else
    {
        if (obj->parent())
        {
            row = obj->childNumber();
        }
        else
        {
            gtWarning().verbose()
                    << "WARNING (GtPropertyModel::indexFromObject): "
                    << "object has no parent!";
            gtWarning().medium().nospace()
                    << __FUNCTION__ << ": " << tr("Object has no parent!");
            gtWarning().medium() << " |-> obj =" << obj->objectName();
        }
    }

    if (row == -1)
    {
        gtWarning().medium().nospace() << __FUNCTION__ << ": row == -1!";
        return {};
    }

    return createIndex(row, 0, obj);
}

void
GtPropertyModel::updatePropertyData(GtAbstractPropertyItem* obj)
{
    QModelIndex topLeft = indexFromProperty(obj);

    if (!topLeft.isValid())
    {
        return;
    }

    if (qobject_cast<GtPropertyModeItem*>(obj))
    {
        updateModeItem(topLeft);
    }

    QModelIndex bottomRight = createIndex(topLeft.row(), 1,
                                          topLeft.internalPointer());

    emit dataChanged(topLeft, bottomRight, QVector<int>() << Qt::DisplayRole);
}

void
GtPropertyModel::setScope(GtObject* obj)
{
    m_scope = obj;

    foreach (GtPropertyCategoryItem* item, m_properties)
    {
        item->setScope(obj);
    }
}

void
GtPropertyModel::setCategoryFilter(const QStringList& filter)
{
    if (filter.isEmpty())
    {
        return;
    }

    beginResetModel();

    m_catFilter = filter;

    endResetModel();
}


bool
GtPropertyModel::canDropMimeData(const QMimeData* data,
                                 Qt::DropAction action,
                                 int row, int column,
                                 const QModelIndex& parent) const
{
    Q_UNUSED(row)
    Q_UNUSED(column)
    Q_UNUSED(action)


    if (!parent.isValid() || parent.data().isNull() )
    {
        return false;
    }

    GtAbstractPropertyItem* prop = propertyFromIndex(parent);

    return prop != nullptr && prop->acceptDrop(data);
}

bool
GtPropertyModel::dropMimeData(const QMimeData* data,
                              Qt::DropAction action,
                              int row, int column,
                              const QModelIndex& parent)
{
    Q_UNUSED(row)
    Q_UNUSED(column)
    Q_UNUSED(action)

    if (!parent.isValid() || parent.data().isNull() )
    {
        return false;
    }

    GtAbstractPropertyItem* prop = propertyFromIndex(parent);

    return prop != nullptr && prop->dropMimeData(data);
}

QStringList
GtPropertyModel::mimeTypes() const
{
    return {"GtObject"};
}

void
GtPropertyModel::addProperty(GtAbstractProperty* prop)
{
    using namespace std;

    QString catId = prop->categoryToString();
    GtPropertyCategoryItem* cat = nullptr;

    auto catIter = std::find_if(begin(m_properties), end(m_properties),
                                [&catId](const GtPropertyCategoryItem* c) {
                                    return c->categoryId() == catId;
                                });

    if (catIter == end(m_properties))
    {
        cat = new GtPropertyCategoryItem(m_scope, catId, this);
        m_properties << cat;
    }
    else
    {
        cat = *catIter;
    }

    cat->addPropertyItem(prop);
}

QList<GtPropertyCategoryItem*>
GtPropertyModel::filteredCategories() const
{
    QList<GtPropertyCategoryItem*> retval;

    foreach (GtPropertyCategoryItem* cat, m_properties)
    {
        if (m_catFilter.contains(cat->categoryId()))
        {
            retval << cat;
        }
    }

    return retval;
}

void
GtPropertyModel::resetObject()
{
    setObject(nullptr);
}

void
GtPropertyModel::onContainerEntryRemoved(int idx)
{
    beginRemoveRows(QModelIndex(), idx, idx);


    GtPropertyCategoryItem* ditem = m_properties.at(idx);
    m_properties.removeAt(idx);
    delete ditem;

    endRemoveRows();
}

void
GtPropertyModel::onContainerEntryAdded(int idx)
{
    assert(m_obj);

    auto* container = m_obj->findPropertyContainer(m_containerId);

    if (!container)
    {
        gtWarning() << tr("container not found!");
        return;
    }

    auto& entry = container->at(idx);

    beginInsertRows(QModelIndex(), idx, idx);


    GtPropertyCategoryItem* cat =
            new GtPropertyCategoryItem(m_scope,
                                       container->entryPrefix(),
                                       this);
    cat->setIsContainer(true);
    m_properties.insert(idx, cat);

    foreach (GtAbstractProperty* pChild, entry.properties())
    {
        cat->addPropertyItem(pChild);
    }

    endInsertRows();
}
