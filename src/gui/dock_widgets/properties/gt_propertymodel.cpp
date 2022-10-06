/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>

#include "gt_propertymodel.h"
#include "gt_object.h"
#include "gt_abstractproperty.h"
#include "gt_propertyitem.h"
#include "gt_propertycategoryitem.h"
#include "gt_modeproperty.h"
#include "gt_propertymodeitem.h"

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

        if (var.type() == QVariant::Bool)
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

    endResetModel();
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
            qWarning() << "WARNING (GtPropertyModel::indexFromObject): " <<
                       "object has no parent!";
            qWarning() << " |-> obj = " << obj->objectName();
        }
    }

    if (row == -1)
    {
        qWarning() << "WARNING (GtPropertyModel::indexFromObject): " <<
                   "row == -1!";
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
    QString catId = prop->categoryToString();
    GtPropertyCategoryItem* cat = nullptr;

    using namespace std;
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
