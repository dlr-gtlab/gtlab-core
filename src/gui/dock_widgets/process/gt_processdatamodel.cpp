/* GTlab - Gas Turbine laboratory
 * Source File: gt_processdatamodel.cpp
 *
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * Created on: 20.09.2024
 * Author: Marvin Noethen (DLR AT-TWK)
 */

#include "gt_datamodel.h"
#include "gt_processdata.h"
#include "gt_project.h"
#include "gt_application.h"

#include "gt_processdatamodel.h"

GtProcessDataModel::GtProcessDataModel(QObject* parent) :
    QAbstractItemModel(parent)
{
//    connect(gtDataModel, SIGNAL(triggerEndResetDataModelView()),
//            SLOT(resetModel()), Qt::DirectConnection);
}

QModelIndex
GtProcessDataModel::index(int row, int col, const QModelIndex& parent) const
{
    // determine parent item
    GtObject* parentItem = nullptr;

    if (!parent.isValid())
        parentItem = currentProcessData();
    else
        parentItem = objectFromIndex(parent);

    if (!parentItem)
        return {};

    // get child items
    auto childItems = parentItem->findDirectChildren<GtObject*>();

    if (row >= childItems.size())
        return {};

    // get the child item for the current row
    auto childItem = childItems[row];

    if (!childItem)
        return {};

    return createIndex(row, col, childItem);
}

QModelIndex
GtProcessDataModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return {};

    // get child item
    auto childItem = objectFromIndex(index);

    if (!childItem)
        return {};

    // get parent item
    auto parentItem = childItem->parentObject();

    // check if the parent item is valid and not a process data object
    if (!parentItem || qobject_cast<GtProcessData*>(parentItem))
        return {};

    // return index
    return indexFromObject(parentItem, 0);
}

int
GtProcessDataModel::rowCount(const QModelIndex& parent) const
{
    // determine parent item
    GtObject* parentItem = nullptr;

    if (!parent.isValid())
        parentItem = currentProcessData();
    else
        parentItem = objectFromIndex(parent);

    // return number of child objects
    return parentItem ? parentItem->findDirectChildren<GtObject*>().size() : 0;
}

int
GtProcessDataModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 2;
}

QVariant
GtProcessDataModel::data(const QModelIndex& index, int role) const
{
    if (!gtDataModel)
        return {};

    return gtDataModel->data(mapToGtDataModel(index), role);

//    if (!index.isValid())
//        return {};

//    switch (role)
//    {
//        case Qt::DisplayRole:
//        case Qt::EditRole:
//        {
//            if (index.column() == 0)
//            {
//                if (auto item = objectFromIndex(index))
//                    return item->objectName();
//            }

//            break;
//        }

//        case GtCoreDatamodel::UuidRole:
//        {
//            if (auto item = objectFromIndex(index))
//                return item->uuid();

//            break;
//        }

//        default:
//            break;
//    }

//    return {};
}

bool
GtProcessDataModel::setData(const QModelIndex& index, const QVariant& value,
                            int role)
{
    if (!gtDataModel)
        return false;

    return gtDataModel->setData(mapToGtDataModel(index), value, role);

//    // check index
//    if (!index.isValid())
//        return false;

//    // check column
//    if (index.column() != 0)
//        return false;

//    // check role
//    if (role != Qt::EditRole)
//        return false;

//    // get object
//    auto item = objectFromIndex(index);

//    // check object
//    if (!item)
//        return false;

//    // check renameable flag
//    if (!item->isRenamable())
//        return false;

//    if (value.toString() == item->objectName())
//        return false;

//    // update identification string
//    auto cmd = gtApp->makeCommand(item, item->objectName() + tr(" renamed"));
//    Q_UNUSED(cmd)

//    item->setObjectName(value.toString());

//    return true;
}

Qt::ItemFlags
GtProcessDataModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return {};

    // collect default flags
    auto defaultFlags = QAbstractItemModel::flags(index);

    // check object
    if (auto object = objectFromIndex(index))
    {
        defaultFlags = defaultFlags | Qt::ItemIsDropEnabled;

        // check if object is renamable
        if (object->isRenamable() && index.column() == 0)
        {
            // add editable flag
            defaultFlags = defaultFlags | Qt::ItemIsEditable;
        }
    }

    return Qt::ItemIsDragEnabled | defaultFlags;
}

QModelIndex
GtProcessDataModel::indexFromObject(GtObject* obj, int col) const
{
    if (!obj)
        return {};

    auto pd = currentProcessData();

    if (!pd)
        return {};

    // check if obj is a descendant of the current process data object
    auto parentProcessData = obj->findParent<GtProcessData*>();

    if (!parentProcessData || parentProcessData != pd)
        return {};

    // get the row of the object
    int row = obj->childNumber();

    if (row < 0)
        return {};

    return createIndex(row, col, obj);
}

GtObject*
GtProcessDataModel::objectFromIndex(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return nullptr;
    }

    return static_cast<GtObject*>(index.internalPointer());
}

QModelIndex
GtProcessDataModel::appendChild(GtObject* child, GtObject* parent)
{    
    if (!gtDataModel || !parent)
        return {};

    // check if the parent belongs to the process data
    if (!parent->findParent<GtProcessData*>())
        return {};

    QModelIndex parentIndex = indexFromObject(parent);

    if (!parentIndex.isValid())
        return {};

    int childCount = parent->childCount<GtObject*>();

    // append child
    beginInsertRows(parentIndex, childCount, childCount + 1);

    auto index = gtDataModel->appendChild(child, parent);

    endInsertRows();

    // convert index from GtDataModel index to GtProcessDataModel index
    return indexFromObject(objectFromIndex(index));
}

bool
GtProcessDataModel::deleteFromModel(QList<GtObject*> objects)
{
    if (!gtDataModel)
        return false;

    // delete each object separately
    foreach (auto* obj, objects)
    {
        // get object model index
        QModelIndex objIndex = indexFromObject(obj);

        // check model index
        if (!objIndex.isValid())
            return false;

        // get parent index
        QModelIndex parentIndex = objIndex.parent();

        // check parent index
        if (!parentIndex.isValid())
            return false;

        beginRemoveRows(parentIndex, obj->childNumber(), obj->childNumber());

        // delete object
        gtDataModel->deleteFromModel(obj);

        endRemoveRows();
    }

    // return success state
    return true;
}

QStringList
GtProcessDataModel::mimeTypes() const
{
    if (!gtDataModel)
        return {};

    return gtDataModel->mimeTypes();
}

QMimeData*
GtProcessDataModel::mimeData(const QModelIndexList& indexes) const
{
    if (!gtDataModel)
        return nullptr;

    return gtDataModel->mimeData(mapToGtDataModel(indexes));

//    if (!indexes.empty())
//    {
//        QModelIndex index = indexes.value(0);

//        if (index.isValid())
//        {
//            GtObject* obj = objectFromIndex(index);

//            return gtDataModel->mimeDataFromObject(obj);
//        }
//    }

//    return {};
}

Qt::DropActions
GtProcessDataModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

Qt::DropActions
GtProcessDataModel::supportedDragActions() const
{
    return Qt::MoveAction;
}

//void
//GtProcessDataModel::reduceToParents(QList<GtObject*>& toReduce)
//{
//    gtDataModel->reduceToParents(toReduce);
//}

//QMimeData*
//GtProcessDataModel::mimeDataFromObject(GtObject* obj, bool newUuid) const
//{
//    return gtDataModel->mimeDataFromObject(obj, newUuid);
//}

//GtObject*
//GtProcessDataModel::objectFromMimeData(const QMimeData* mime, bool newUuid,
//                                       GtAbstractObjectFactory* factory)
//{
//    return gtDataModel->objectFromMimeData(mime, newUuid, factory);
//}

GtProcessData*
GtProcessDataModel::currentProcessData() const
{
    if (!gtDataModel)
        return nullptr;

    auto project = gtDataModel->currentProject();
    return project ? project->processData() : nullptr;
}

QModelIndex
GtProcessDataModel::mapToGtDataModel(const QModelIndex& index) const
{
    if (!gtDataModel)
        return {};

    if (auto obj = objectFromIndex(index))
        return gtDataModel->indexFromObject(obj);

    return {};
}

QModelIndexList
GtProcessDataModel::mapToGtDataModel(const QModelIndexList& indexes) const
{
    QModelIndexList mappedIndexes{};

    for (auto& index : indexes)
    {
        mappedIndexes << mapToGtDataModel(index);
    }

    return mappedIndexes;
}

void
GtProcessDataModel::resetModel()
{
    beginResetModel();

    endResetModel();
}

