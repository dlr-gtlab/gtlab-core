/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_taskoverviewmodel.cpp
 *
 *  Created on: 03.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>

#include "gt_application.h"
#include "gt_taskfactory.h"
#include "gt_processcategoryitem.h"
#include "gt_processtaskitem.h"
#include "gt_extendedtaskdata.h"
#include "gt_objectui.h"
#include "gt_icons.h"

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
    if (!item) return {};

    GtProcessTaskItem* taskItem = qobject_cast<GtProcessTaskItem*>(item);

    if (!taskItem) return {};

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
    if (!item) return {};

    GtProcessTaskItem* taskItem = qobject_cast<GtProcessTaskItem*>(item);

    if (!taskItem) return {};

    GtTaskData taskData = taskItem->taskData();

    return taskData->version.toString();
}

QIcon
GtTaskOverviewModel::icon(GtAbstractProcessItem* item) const
{
    if (!item) return {};

    GtProcessTaskItem* taskItem = qobject_cast<GtProcessTaskItem*>(item);

    if (!taskItem) return {};

    GtTaskData taskData = taskItem->taskData();

    auto* extendedData = dynamic_cast<GtExtendedTaskDataImpl*>(taskData.get());

    if (!extendedData || extendedData->icon.isNull())
    {
        if (GtObjectUI* oui = gtApp->defaultObjectUI(
                taskData->metaData().className()))
        {
            QIcon icn = oui->icon(nullptr);

            if (!icn.isNull())
            {
                return icn;
            }
        }

        return gt::gui::icon::process();
    }

    return extendedData->icon;
}

QString
GtTaskOverviewModel::description(GtAbstractProcessItem* item) const
{
    if (!item) return {};

    auto* taskItem = qobject_cast<GtProcessTaskItem*>(item);

    if (!taskItem) return {};

    GtTaskData taskData = taskItem->taskData();

    if (taskData->description.isNull())
    {
        return taskData->id;
    }

    return taskData->description;
}

gt::ProcessInformation
GtTaskOverviewModel::processElementInformation(GtAbstractProcessItem* item) const
{
    if (!item) return {};

    auto* taskItem = qobject_cast<GtProcessTaskItem*>(item);

    if (!taskItem) return {};

    GtTaskData taskData = taskItem->taskData();

    gt::ProcessInformation info;
    info.id = taskData->id;
    info.version = taskData->version;

    QString descriptionVal = taskData->description;
    if (!descriptionVal.isEmpty())
    {
        info.description = descriptionVal;
    }

    QString authorVal = taskData->author;
    if (!authorVal.isEmpty())
    {
        info.author = authorVal;
    }

    QString contactVal = taskData->contact;
    if (!contactVal.isEmpty())
    {
        info.contact = contactVal;
    }

    QString companyVal = taskData->company;
    if (!companyVal.isEmpty())
    {
        info.company = companyVal;
    }

    return info;
}
