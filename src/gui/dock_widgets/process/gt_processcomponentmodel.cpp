/* GTlab - Gas Turbine laboratory
 * Source File: gt_processcomponentmodel.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 05.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QMimeData>
#include <QFont>

#include "gt_application.h"
#include "gt_objectlinkproperty.h"
#include "gt_extendedcalculatordata.h"
#include "gt_extendedtaskdata.h"
#include "gt_calculator.h"
#include "gt_calculatorfactory.h"
#include "gt_datamodel.h"
#include "gt_processfactory.h"
#include "gt_task.h"
#include "gt_taskfactory.h"
#include "gt_propertyconnectionfunctions.h"
#include "gt_propertyconnection.h"
#include "gt_processcomponentmodel.h"
#include "gt_icons.h"
#include "gt_taskgroup.h"

using namespace gt::gui;

GtProcessComponentModel::GtProcessComponentModel(QObject* parent) :
    GtStyledModel(parent)
{
}

QVariant
GtProcessComponentModel::data(const QModelIndex& index, int role) const
{
    // read column
    const int col = index.column();

    // switch roles
    switch (role)
    {
        case Qt::DisplayRole:
        {
            if (col != 0)
            {
                return QVariant();
            }

            GtObject* item = static_cast<GtObject*>(
                                 index.internalPointer());

            GtProcessComponent* pc =
                qobject_cast<GtProcessComponent*>(item);

            if (!pc)
            {
                return item->objectName();
            }

            if (pc->currentState() == GtProcessComponent::TERMINATION_REQUESTED)
            {
                QVariant var = GtStyledModel::data(index, role);

                return var.toString() + QStringLiteral(" (terminating...)");
            }

            if (pc->currentState() == GtProcessComponent::TERMINATED)
            {
                QVariant var = GtStyledModel::data(index, role);

                return var.toString() + QStringLiteral(" (terminated)");
            }

        }
        break;

        case Qt::DecorationRole:
        {
            GtObject* item = static_cast<GtObject*>(index.internalPointer());

            GtProcessComponent* pc = qobject_cast<GtProcessComponent*>(item);

            if (!pc)
            {
                return GtStyledModel::data(index, role);
            }

            if (col == 0)
            {
                if (qobject_cast<GtCalculator*>(pc))
                {
                    QString className = pc->metaObject()->className();
                    GtCalculatorData calcData =
                        gtCalculatorFactory->calculatorData(className);

                    GtExtendedCalculatorDataImpl* eData =
                        dynamic_cast<GtExtendedCalculatorDataImpl*>(
                            calcData.get());

                    if (eData)
                    {
                        return eData->icon;
                    }

                    return gt::gui::icon::calculator16();

                }

                if (qobject_cast<GtTask*>(pc))
                {
                    QString className = pc->metaObject()->className();
                    GtTaskData taskData =
                        gtTaskFactory->taskData(className);

                    GtExtendedTaskDataImpl* eData =
                        dynamic_cast<GtExtendedTaskDataImpl*>(
                            taskData.get());

                    if (eData)
                    {
                        return eData->icon;
                    }
                }
            }
            else if (col == 1)
            {
                return stateToIcon(pc->currentState(), pc->currentProgress());
            }
        }
        break;

        case Qt::FontRole:
        {
            GtObject* item = static_cast<GtObject*>(
                                 index.internalPointer());

            GtProcessComponent* pc =
                qobject_cast<GtProcessComponent*>(item);

            if (!pc)
            {
                return GtStyledModel::data(index, role);
            }

            if (col == 0 && pc->isSkipped())
            {
                QFont font =
                    GtStyledModel::data(index, role).value<QFont>();
                font.setItalic(true);
                font.setStrikeOut(true);
                return font;
            }
        }
        break;

        case Qt::ToolTipRole:
        {
            GtObject* item = static_cast<GtObject*>(index.internalPointer());

            auto* pc = qobject_cast<GtProcessComponent*>(item);

            if (pc && col == 1)
            {
                return stateToString(pc->currentState(), pc->currentProgress());
            }

        }
        break;

        default:
            break;
    }

    return GtStyledModel::data(index, role);
}

QIcon
GtProcessComponentModel::stateToIcon(GtProcessComponent::STATE state,
                                     int progress)
{
    switch (state)
    {
        case GtProcessComponent::RUNNING:
            return gt::gui::icon::processRunningIcon(progress);

        case GtProcessComponent::QUEUED:
            return gt::gui::icon::sleep16();

        case GtProcessComponent::FAILED:
            return gt::gui::icon::error16();

        case GtProcessComponent::FINISHED:
            return gt::gui::icon::check16();

        case GtProcessComponent::CONNECTING:
            return gt::gui::icon::network16();

        case GtProcessComponent::SKIPPED:
            return gt::gui::icon::skip16();

        case GtProcessComponent::TERMINATION_REQUESTED:
            return gt::gui::icon::stopRequest16();

        case GtProcessComponent::TERMINATED:
            return gt::gui::icon::stop16();

        case GtProcessComponent::WARN_FINISHED:
            return gt::gui::icon::processFailed16();

        default:
            break;
    }

    return QIcon();
}

QIcon
GtProcessComponentModel::stateToIcon(GtProcessComponent::STATE state)
{
    return stateToIcon(state, 0.0);
}

QString
GtProcessComponentModel::stateToString(GtProcessComponent::STATE state,
                                       int progress)
{
    switch (state)
    {
        case GtProcessComponent::RUNNING:
            if (progress <= 0)
            {
                return tr("Running");
            }
            else
            {
                return tr("Progress: ") + QString::number(progress) + "%";
            }

        case GtProcessComponent::QUEUED:
            return tr("Queued");

        case GtProcessComponent::FAILED:
            return tr("Failed");

        case GtProcessComponent::FINISHED:
            return tr("Finished");

        case GtProcessComponent::CONNECTING:
            return tr("Connecting");

        case GtProcessComponent::SKIPPED:
            return tr("Skipped");

        case GtProcessComponent::TERMINATION_REQUESTED:
            return tr("Termination requested");

        case GtProcessComponent::TERMINATED:
            return tr("Terminated");

        case GtProcessComponent::WARN_FINISHED:
            return tr("Finished with warning");

        default:
            break;
    }

    return {};
}

bool
GtProcessComponentModel::moveRows(const QModelIndex& sourceParent,
                                  int sourceRow, int count,
                                  const QModelIndex& destinationParent,
                                  int destinationChild)
{
    // check model index
    if (!sourceParent.isValid() || !destinationParent.isValid())
    {
        qWarning() << "invalid model indices!";
        return false;
    }

    // get source parent
    GtObject* srcParentObj =
        static_cast<GtObject*>(sourceParent.internalPointer());

    // get destination parent
    GtObject* dstParentObj =
        static_cast<GtObject*>(destinationParent.internalPointer());

    // get source children
    QList<GtObject*> srcChilds = srcParentObj->findDirectChildren<GtObject*>();

    // number of source children
    const int nosc = srcChilds.size();

    // check source indices
    if (sourceRow < 0)
    {
        qWarning() << "source row < 0!";
        return false;
    }

    if (count <= 0)
    {
        qWarning() << "count <= 0!";
        return false;
    }

    if (sourceRow >= nosc || (sourceRow + count) > nosc)
    {
        qWarning() << "invalid source row!";
        return false;
    }

    // number of destination children
    const int nodc = dstParentObj->childCount<GtObject*>();

    if (destinationChild > nodc)
    {
        qWarning() << "destinationChild out of index!";
        return false;
    }

    QList<GtObject*> objsToMove;

    for (int i = sourceRow; i < (sourceRow + count); i++)
    {
        objsToMove << srcChilds[i];
    }



    if (destinationChild == -1)
    {
        // append children

        // handle source == destination
        if (srcParentObj == dstParentObj)
        {
            foreach (GtObject* child, objsToMove)
            {
                child->disconnectFromParent();
                dstParentObj->appendChild(child);
            }
        }
        else
        {
            foreach (GtObject* child, objsToMove)
            {
                dstParentObj->appendChild(child);
            }
        }
    }
    else
    {
        // insert children

        // handle source == destination
        if (srcParentObj == dstParentObj)
        {
            if (sourceRow == destinationChild ||
                    sourceRow == (destinationChild - 1))
            {
                return false;
            }

            if (sourceRow < destinationChild)
            {
                destinationChild = destinationChild - count;
            }

            foreach (GtObject* child, objsToMove)
            {
                child->disconnectFromParent();
                dstParentObj->insertChild(destinationChild, child);
                destinationChild++;
            }
        }
        else
        {
            foreach (GtObject* child, objsToMove)
            {
                child->disconnectFromParent();
                dstParentObj->insertChild(destinationChild, child);
                destinationChild++;
            }
        }
    }

    return true;
}

bool
GtProcessComponentModel::canDropMimeData(const QMimeData* data,
        Qt::DropAction /*action*/,
        int /*row*/, int column,
        const QModelIndex& parent) const
{
    if (column > 0)
    {
        return false;
    }

    if (parent.column() > 0)
    {
        return false;
    }

    // recreate object from mimedata
    GtObject* obj = gtDataModel->objectFromMimeData(data, true,
                    gtProcessFactory);

    // check object
    if (!obj)
    {
        gtError() << tr("Could not restore object from mimedata!");
        return false;
    }

    // get parent object
    GtObject* parentObj = static_cast<GtObject*>(parent.internalPointer());

    if (qobject_cast<GtCalculator*>(obj))
    {
        // handle calculator drop
        if (qobject_cast<GtTask*>(parentObj))
        {
            return true;
        }
    }
    else if (const GtTask* task = qobject_cast<GtTask*>(obj))
    {
        // handle task drop
        if (qobject_cast<GtTaskGroup*>(parentObj))
        {
            return true;
        }

        if (const GtTask* parentTask = qobject_cast<GtTask*>(parentObj))
        {
            if (parentTask != task)
            {
                return true;
            }
        }
    }

    return false;
}

bool
GtProcessComponentModel::dropMimeData(const QMimeData* mimeData,
                                      Qt::DropAction action,
                                      int row, int column,
                                      const QModelIndex& destinationIndex)
{
    if (column > 0)
    {
        return false;
    }

    // check action -> only move action is supported
    if (action != Qt::MoveAction)
    {
        return false;
    }

    if (!destinationIndex.isValid())
    {
        return false;
    }

    // get destination object
    GtObject* destinationObject = static_cast<GtObject*>
                                  (destinationIndex.internalPointer());

    // get task croup
    GtTaskGroup* taskGroup =
            destinationObject->findParent<GtTaskGroup*>();

    if (!taskGroup)
    {
        taskGroup = qobject_cast<GtTaskGroup*>(destinationObject);
    }

    // check process data
    if (!taskGroup)
    {
        return false;
    }

    // check mime data format
    if (!mimeData->hasFormat(QStringLiteral("GtObject")))
    {
        return false;
    }

    // restore memento from mime data
    GtObjectMemento memento(mimeData->data(QStringLiteral("GtObject")));

    // check memento
    if (memento.isNull())
    {
        return false;
    }

    QString mimeDataUUID = memento.uuid();

    // find object
    GtObject* droppedObject = taskGroup->getObjectByUuid(mimeDataUUID);

    // check object
    if (!droppedObject)
    {
        return false;
    }

    // find parent object
    GtObject* droppedObjectParent = droppedObject->parentObject();

    // check parent object
    if (!droppedObjectParent)
    {
        return false;
    }

    QList<GtPropertyConnection*> internalCons;
    QList<GtPropertyConnection*> lostCons;
    GtTask* newHighestParent = nullptr;
    GtTask* highestParent = nullptr;

    if (GtProcessComponent* pComp = qobject_cast<GtProcessComponent*>
                                    (droppedObject))
    {
        internalCons = detail::internalPropertyConnections(pComp);
        lostCons = detail::lostPropertyConnections(pComp);
    }

    if (GtProcessComponent* pComp = qobject_cast<GtProcessComponent*>
                                    (destinationObject))
    {
        newHighestParent = detail::highestParentTask(pComp);
    }
    else if (qobject_cast<GtTaskGroup*>(destinationObject))
    {
        newHighestParent = qobject_cast<GtTask*>(droppedObject);
    }

    if (!newHighestParent)
    {
        return false;
    }

    if (GtProcessComponent* pComp = qobject_cast<GtProcessComponent*>
                                    (droppedObject))
    {
        highestParent = detail::highestParentTask(pComp);
    }

    if (!highestParent)
    {
        return false;
    }

    // get source model index for object
    QModelIndex droppedObjectParentIndexSRC = gtDataModel->indexFromObject(
                droppedObjectParent);

    // check source index
    if (!droppedObjectParentIndexSRC.isValid())
    {
        return false;
    }

    // map index from source model
    QModelIndex droppedObjectParentIndex = mapFromSource(
            droppedObjectParentIndexSRC);

    // check index
    if (!droppedObjectParentIndex.isValid())
    {
        return false;
    }

    // get row of source object
    const int droppedObjectSrcRow = droppedObject->childNumber();

    // check row
    if (droppedObjectSrcRow < 0)
    {
        return false;
    }


    QList<GtObject*> toDelete;

    foreach (GtPropertyConnection* propCon, lostCons)
    {
        toDelete.append(propCon);
    }

    detail::setOffLostConnectionWarnings(lostCons, highestParent);

    if (row >= 0)
    {
        beginMoveRows(droppedObjectParentIndex, droppedObjectSrcRow,
                      droppedObjectSrcRow, destinationIndex, row);

        if (!moveRow(droppedObjectParentIndex, droppedObjectSrcRow,
                     destinationIndex, row))
        {
            if (gtApp->devMode())
            {
                gtFatal() << "move rows failed!";
            }

            return false;
        }

    }
    else
    {
        const int destinationRowCount = rowCount(destinationIndex);

        beginMoveRows(droppedObjectParentIndex, droppedObjectSrcRow,
                      droppedObjectSrcRow, destinationIndex,
                      destinationRowCount);

        if (!moveRow(droppedObjectParentIndex, droppedObjectSrcRow,
                     destinationIndex, destinationRowCount))
        {
            if (gtApp->devMode())
            {
                // more checks in canDropMimeData function to prevent moveRow
                // function return false.
                gtFatal() << "move rows failed!";
            }

            return false;
        }
    }

    foreach (GtPropertyConnection* propCon, internalCons)
    {
        newHighestParent->appendChild(propCon);
    }

    gtDataModel->deleteFromModel(toDelete);

    endMoveRows();

    return true;
}

Qt::ItemFlags
GtProcessComponentModel::flags(const QModelIndex& index) const
{
    // check index
    if (!index.isValid())
    {
        return {};
    }

    // collect default flags
    Qt::ItemFlags defaultFlags = GtStyledModel::flags(index);

    // get object
    GtObject* object = static_cast<GtObject*>(index.internalPointer());

    GtProcessComponent* pc = qobject_cast<GtProcessComponent*>(object);

    if (pc && !pc->isReady())
    {
        defaultFlags = defaultFlags & ~Qt::ItemIsEditable;
    }

    // add drop enabled flag
    defaultFlags = defaultFlags | Qt::ItemIsDropEnabled;

    // add drag enabled flag
    defaultFlags = defaultFlags | Qt::ItemIsDragEnabled;

    // return flags
    return defaultFlags;
}

QString
GtProcessComponentModel::toolTip(GtObject* obj) const
{
    if (!obj)
    {
        return QString();
    }

    QString retval;

    QList<GtAbstractProperty*> props = obj->properties();

    foreach (GtAbstractProperty* prop, props)
    {
        GtObjectLinkProperty* objLink =
            qobject_cast<GtObjectLinkProperty*>(prop);

        if (objLink)
        {
            GtObject* linkedObj = objLink->linkedObject();

            if (retval.isEmpty())
            {
                retval = retval + objectLinkToolTip(objLink->objectName(),
                                                    linkedObj,
                                                    true);
            }
            else
            {
                retval = retval + objectLinkToolTip(objLink->objectName(),
                                                    linkedObj);
            }
        }
    }

    return retval;
}

QString
GtProcessComponentModel::objectLinkToolTip(const QString& id, GtObject* obj,
        bool first) const
{
    if (id.isEmpty())
    {
        return QString();
    }

    QString retval;

    if (!first)
    {
        retval = QStringLiteral("<br>");
    }

    retval = retval + QStringLiteral("<b>") + id + QStringLiteral("<b><br>");

    if (!obj)
    {
        retval = retval + QStringLiteral("<font color=\"#ff0000\">") +
                 tr("Linked object not found!") + QStringLiteral("</font>");
    }
    else
    {
        retval = retval + obj->objectPath();
    }

    return retval;
}
