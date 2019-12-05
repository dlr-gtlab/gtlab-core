/* GTlab - Gas Turbine laboratory
 * Source File: gt_taskoverviewmodel.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 03.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QColor>
#include <QIcon>

#include "gt_application.h"
#include "gt_taskfactory.h"
#include "gt_processcategoryitem.h"
#include "gt_processtaskitem.h"
#include "gt_extendedtaskdata.h"
#include "gt_objectui.h"

#include "gt_taskoverviewmodel.h"

GtTaskOverviewModel::GtTaskOverviewModel(QObject* parent) :
    GtProcessOverviewModel(parent)
{

}

void
GtTaskOverviewModel::setUpModel()
{
    foreach (GtTaskData taskData,
             gtTaskFactory->taskDataList())
    {
        if (!gtApp->devMode() &&
            taskData->status != GtTaskDataImpl::RELEASE)
        {
            continue;
        }

        QString catId = taskData->category;

        if (catId.isEmpty())
        {
            catId = tr("Uncategorized");
        }

        GtProcessCategoryItem* catItem = categoryItem(catId);

        GtProcessTaskItem* pItem = new GtProcessTaskItem(taskData,
                                                         catItem);

        catItem->addItem(pItem);
    }
}

QString
GtTaskOverviewModel::id(GtAbstractProcessItem* item) const
{
    if (item == Q_NULLPTR)
    {
        return QString();
    }

    GtProcessTaskItem* taskItem = qobject_cast<GtProcessTaskItem*>(item);

    if (taskItem == Q_NULLPTR)
    {
        return QString();
    }

    GtTaskData taskData = taskItem->taskData();

    QString tmpStr = taskData->id;

    if (gtApp->devMode())
    {
        tmpStr = tmpStr + QStringLiteral(" [") +
                 taskData->statusToString(taskData->status) +
                 QStringLiteral("]");
    }

    return tmpStr;
}

QString
GtTaskOverviewModel::version(GtAbstractProcessItem* item) const
{
    if (item == Q_NULLPTR)
    {
        return QString();
    }

    GtProcessTaskItem* taskItem = qobject_cast<GtProcessTaskItem*>(item);

    if (taskItem == Q_NULLPTR)
    {
        return QString();
    }

    GtTaskData taskData = taskItem->taskData();

    return QString::number(taskData->version);
}

QIcon
GtTaskOverviewModel::icon(GtAbstractProcessItem* item) const
{
    if (item == Q_NULLPTR)
    {
        return QIcon();
    }

    GtProcessTaskItem* taskItem = qobject_cast<GtProcessTaskItem*>(item);

    if (taskItem == Q_NULLPTR)
    {
        return QIcon();
    }

    GtTaskData taskData = taskItem->taskData();

    GtExtendedTaskDataImpl* extendedData =
            dynamic_cast<GtExtendedTaskDataImpl*>(taskData.get());

    if (extendedData == Q_NULLPTR || extendedData->icon.isNull())
    {
        GtObjectUI* oui = gtApp->objectUI(taskData->metaData().className());

        if (oui != Q_NULLPTR)
        {
            QIcon icn = oui->icon(Q_NULLPTR);

            if (!icn.isNull())
            {
                return icn;
            }
        }

        return gtApp->icon(QStringLiteral("processIcon_16.png"));
    }

    return extendedData->icon;
}

QString
GtTaskOverviewModel::description(GtAbstractProcessItem* item) const
{
    if (item == Q_NULLPTR)
    {
        return QString();
    }

    GtProcessTaskItem* taskItem = qobject_cast<GtProcessTaskItem*>(item);

    if (taskItem == Q_NULLPTR)
    {
        return QString();
    }

    GtTaskData taskData = taskItem->taskData();

    if (taskData->description.isNull())
    {
        return taskData->id;
    }

    return taskData->description;
}
