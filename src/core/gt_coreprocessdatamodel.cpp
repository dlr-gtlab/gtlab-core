/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_coredatamodel.cpp
 *
 *  Created on: 02.09.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QMimeData>
#include <QDir>

#include "gt_session.h"
#include "gt_project.h"
#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_coreapplication.h"
#include <gt_logging.h>
#include "gt_state.h"
#include "gt_statehandler.h"
#include "gt_externalizationmanager.h"

#include "gt_coreprocessdatamodel.h"

GtCoreProcessDatamodel* GtCoreProcessDatamodel::m_self = 0;

GtCoreProcessDatamodel::GtCoreProcessDatamodel(QObject* parent)
    : QAbstractItemModel(parent),
    m_processData(nullptr)
{
    init();
}

Qt::ItemFlags
GtCoreProcessDatamodel::flags(const QModelIndex& index) const
{
    // check index
    if (!index.isValid())
    {
        return {};
    }

    // collect default flags
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    // get object
    GtObject* object = objectFromIndex(index);

    // check object
    if (object)
    {
        // check if object is renamable
        if (object->isRenamable() && index.column() == 0)
        {
            // add editable flag
            defaultFlags = defaultFlags | Qt::ItemIsEditable;
        }
    }

    // return flags
    return defaultFlags;
}

void
GtCoreProcessDatamodel::setProcessData(GtProcessData* processData)
{
    beginResetModel();
    m_processData = processData;
    endResetModel();
}

GtCoreProcessDatamodel*
GtCoreProcessDatamodel::instance()
{
    return m_self;
}

void
GtCoreProcessDatamodel::init()
{
    m_self = this;
}

void
GtCoreProcessDatamodel::initProjectStates(GtProject* project)
{
    // initialize last task group state
    gtStateHandler->initializeState(project,
                                    QStringLiteral("Project Settings"),
                                    QStringLiteral("Last Task Group"),
                                    project->objectPath() + ";lastTaskGroup",
                                    QString(), project);
}

GtProcessData*
GtCoreProcessDatamodel::session()
{
    return m_processData;
}

GtTaskGroup*
GtCoreProcessDatamodel::currentTaskGroup()
{
    // check session
    if (m_processData)
    {
        // return current project from session
        return m_processData->taskGroup();
    }

    // no session -> no project
    return nullptr;
}

int
GtCoreProcessDatamodel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

int
GtCoreProcessDatamodel::rowCount(const QModelIndex& parent) const
{
    // check column
    if (parent.column() > 0)
    {
        return 0;
    }

    if (!m_processData)
    {
        return 0;
    }

    if (!parent.isValid())
    {
        return m_processData->taskGroups().size();
    }

    // get parent item
    GtObject* parentItem = objectFromIndex(parent);

    if (!parentItem)
    {
        return 0;
    }

    // return number of child objects
    return parentItem->findDirectChildren<GtObject*>().size();
}

QModelIndex
GtCoreProcessDatamodel::index(int row, int col, const QModelIndex& parent) const
{
    // root
    if (!parent.isValid() && m_processData)
    {
        QList<const GtTaskGroup*> projects = m_processData->taskGroups();

        // check array size
        if (row >= projects.size()) return {};

        // get child object corresponding to row number
        const GtObject* childItem = projects[row];

        // check object
        if (!childItem) return {};

        GtObject* childItem2 = const_cast<GtObject*>(childItem);

        // create index
        return createIndex(row, col, childItem2);
    }

    // non root
    GtObject* parentItem = objectFromIndex(parent);

    // check parent item
    if (!parentItem) return {};

    QList<GtObject*> childItems =
            parentItem->findDirectChildren<GtObject*>();

    // check array size
    if (row >= childItems.size()) return {};

    // get child object corresponding to row number
    GtObject* childItem = childItems[row];

    // check object
    if (!childItem)
    {
        return {};
    }

    // create index
    return createIndex(row, col, childItem);
}

QModelIndex
GtCoreProcessDatamodel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return {};
    }

    // get object from index
    GtObject* childItem = objectFromIndex(index);

    // check object
    if (!childItem)
    {
        return {};
    }

    // get parent object
    GtObject* parentItem = childItem->parentObject();

    // check parent
    if (!parentItem)
    {
        return {};
    }

    // check whether parent object is a session
    if (qobject_cast<GtSession*>(parentItem))
    {
        return {};
    }

    // return index
    return indexFromObject(parentItem, 0);
}

QVariant
GtCoreProcessDatamodel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            if (index.column() == 0)
            {
                // get object
                GtObject* item = objectFromIndex(index);

                // check object
                if (item)
                {
                    // return object identification string
                    return item->objectName();
                }
            }

            break;
        }

        case UuidRole:
        {
            // get object
            GtObject* item = objectFromIndex(index);

            // check object
            if (item)
            {
                // return object uuid
                return item->uuid();
            }
            break;
        }

        default:
            break;
    }

    return QVariant();
}

bool
GtCoreProcessDatamodel::setData(const QModelIndex& index, const QVariant& value,
                         int role)
{
    // check index
    if (!index.isValid())
    {
        return false;
    }

    // check column
    if (index.column() != 0)
    {
        return false;
    }

    // check role
    if (role != Qt::EditRole)
    {
        return false;
    }

    // get object
    GtObject* item = objectFromIndex(index);

    // check object
    if (!item) return false;
    {

    }

    // check renameable flag
    if (!item->isRenamable())
    {
        return false;
    }

    if (value.toString() == item->objectName())
    {
        return false;
    }

    // update identification string
    auto cmd = gtApp->makeCommand(item, item->objectName() + tr(" renamed"));
    Q_UNUSED(cmd)

    item->setObjectName(value.toString());

    return true;
}

QStringList
GtCoreProcessDatamodel::mimeTypes() const
{
    QStringList types;
    types << QStringLiteral("GtObject");
    return types;
}

QMimeData*
GtCoreProcessDatamodel::mimeData(const QModelIndexList& indexes) const
{
    if (!indexes.empty())
    {
        QModelIndex index = indexes.value(0);

        if (index.isValid())
        {
            GtObject* obj = objectFromIndex(index);

            return mimeDataFromObject(obj);
        }
    }

    return new QMimeData();
}

QMimeData*
GtCoreProcessDatamodel::mimeDataFromObject(GtObject* obj, bool newUuid) const
{
    // check object
    if (!obj) return nullptr;

    // create memento
    GtObjectMemento memento = obj->toMemento(!newUuid);

    QMimeData* mimeData = new QMimeData;

    // append memento to mime data
    mimeData->setData(QStringLiteral("GtObject"), memento.toByteArray());

    // return mime data
    return mimeData;
}

GtObject*
GtCoreProcessDatamodel::objectFromMimeData(const QMimeData* mime, bool newUuid,
                                    GtAbstractObjectFactory* factory)
{
    // check mime data
    if (!mime) return nullptr;

    // check mime data format
    if (!mime->hasFormat(QStringLiteral("GtObject")))
    {
        return nullptr;
    }

    // restore memento from mime data
    GtObjectMemento memento(mime->data(QStringLiteral("GtObject")));

    // check memento
    if (memento.isNull()) return nullptr;

    // check factory. if no factory was set, use default object factory
    if (!factory)
    {
        factory = gtObjectFactory;
    }

    // return object restored from memento
    return memento.restore(factory, newUuid);
}

GtObject*
GtCoreProcessDatamodel::objectFromIndex(const QModelIndex& index) const
{
    if (!index.isValid()) return nullptr;

    return static_cast<GtObject*>(index.internalPointer());
}

QModelIndex
GtCoreProcessDatamodel::indexFromObject(GtObject* obj, int col) const
{
    // initialize row
    int row = -1;

    if (qobject_cast<GtTaskGroup*>(obj) && m_processData)
    {
        // handle project object
        QList<const GtTaskGroup*> projects = m_processData->taskGroups();

        // get index from project
        row = projects.indexOf(qobject_cast<const GtTaskGroup*>(obj));
    }
    // handle object
    else if (obj->parent())
    {
        row = obj->childNumber();
    }

    // check row
    if (row < 0) return {};

    // create index
    return createIndex(row, col, obj);
}

QModelIndex
GtCoreProcessDatamodel::appendChild(GtObject* child, GtObject* parent)
{
    // check child object
    if (!child)
    {
        gtWarning() << tr("Could not append child!") <<
                    QStringLiteral(" - ") <<
                    QStringLiteral("child == NULL");
        return {};
    }

    if (!child->factory())
    {
        child->setFactory(gtObjectFactory);
    }

    // check parent object
    if (!parent)
    {
        gtWarning() << tr("Could not append child!") <<
                    QStringLiteral(" - ") <<
                    QStringLiteral("parent == NULL");
        return {};
    }

    // use multi append function
    QModelIndexList indexList = appendChildren(GtObjectList() << child, parent);

    // check whether child was appended
    if (indexList.isEmpty())
    {
        return {};
    }

    // return model index
    return indexList.first();
}

QModelIndexList
GtCoreProcessDatamodel::appendChildren(QList<GtObject*> const& children,
                                GtObject* parent)
{
    // check parent object
    if (!parent)
    {
        gtWarning() << tr("Could not append children!") <<
                    QStringLiteral(" - ") <<
                    QStringLiteral("parent == NULL");
        return QModelIndexList();
    }

    // get index of parent object
    QModelIndex parentIndex = indexFromObject(parent);

    // check parent index
    if (!parentIndex.isValid())
    {
        gtWarning() << tr("Could not append children!") <<
                    QStringLiteral(" - ") <<
                    QStringLiteral("parent index invalid");
        return QModelIndexList();
    }

    // TODO: check whether children can be appended or not before appending

    beginInsertRows(parentIndex, parent->childCount<GtObject*>(),
                    parent->childCount<GtObject*>() + children.size() - 1);

    if (!parent->appendChildren(children))
    {
        return QModelIndexList();
    }

    endInsertRows();

    QModelIndexList retval;

    foreach (GtObject* obj, children)
    {
        retval << indexFromObject(obj);
    }

    // return model index list
    return retval;
}

QModelIndex
GtCoreProcessDatamodel::insertChild(GtObject* child, GtObject* parent, int row)
{
    // check child object
    if (!child)
    {
        gtWarning() << tr("Could not insert child!") <<
                    QStringLiteral(" - ") <<
                    QStringLiteral("child == NULL");
        return {};
    }

    // check parent object
    if (!parent)
    {
        gtWarning() << tr("Could not insert child!") <<
                    QStringLiteral(" - ") <<
                    QStringLiteral("parent == NULL");
        return {};
    }

    // get index of parent object
    QModelIndex parentIndex = indexFromObject(parent);

    // check parent index
    if (!parentIndex.isValid())
    {
        gtWarning() << tr("Could not insert children!") <<
                    QStringLiteral(" - ") <<
                    QStringLiteral("parent index invalid");
        return {};
    }

    // TODO: check whether child can be appended or not before appending

    beginInsertRows(parentIndex, row, row);

    if (!parent->insertChild(row, child))
    {
        return {};
    }

    endInsertRows();

    // return model index
    return indexFromObject(child);
}

bool
GtCoreProcessDatamodel::deleteFromModel(GtObject* obj)
{
    // check object
    if (!obj)
    {
        gtWarning() << tr("Could not delete object!") <<
                    QStringLiteral(" - ") <<
                    QStringLiteral("object == NULL");
        return false;
    }

    // use multi delete function
    return deleteFromModel(QList<GtObject*>() << obj);

}

bool
GtCoreProcessDatamodel::deleteFromModel(QList<GtObject*> objects)
{
    // check object list
    if (objects.isEmpty())
    {
        return true;
    }

    // delete each object separately
    foreach (GtObject* obj, objects)
    {
        // get object model index
        QModelIndex objIndex = indexFromObject(obj);
        // check model index
        if (!objIndex.isValid())
        {
            return false;
        }

        // get parent index
        QModelIndex parentIndex = objIndex.parent();

        // check parent index
        if (!parentIndex.isValid())
        {
            return false;
        }

        beginRemoveRows(parentIndex, obj->childNumber(), obj->childNumber());

        // delete object
        delete obj;

        endRemoveRows();
    }

    // return success state
    return true;
}

void
GtCoreProcessDatamodel::reduceToParents(QList<GtObject *> &toReduce)
{
    foreach (GtObject* obj, toReduce)
    {
        GtObject* parObj = obj->parentObject();

        while (parObj != nullptr)
        {
            if (toReduce.contains(parObj))
            {
                toReduce.removeAll(obj);
            }

            parObj = parObj->parentObject();
        }
    }
}
