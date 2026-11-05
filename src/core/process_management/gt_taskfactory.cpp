/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 12.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>

#include "gt_task.h"
#include "gt_loop.h"
#include "gt_residualloop.h"
#include "gt_objectfactory.h"
#include "gt_processdata.h"
#include "gt_taskdata.h"
#include "gt_parameterloop.h"

#include "gt_taskfactory.h"

GtTaskFactory::GtTaskFactory(QObject* parent) : QObject(parent)
{
    // Default tasks
    GtTaskData simpleTask = GT_TASK_DATA(GtTask);
    simpleTask->id = QStringLiteral("Simple Task");
    simpleTask->version = GtVersionNumber(0,1);
    simpleTask->status = GtTaskDataImpl::RELEASE;
    GtTaskFactory::registerTaskData(simpleTask, builtinModuleId());

    GtTaskData loopTask = GT_TASK_DATA(GtLoop);
    loopTask->id = QStringLiteral("Loop");
    loopTask->version = GtVersionNumber(0,1);
    loopTask->description = tr("Repeats execution until maixmum "
                               "number of iterations is reached.");
    loopTask->status = GtTaskDataImpl::RELEASE;
    GtTaskFactory::registerTaskData(loopTask, builtinModuleId());

    GtTaskData residualLoopTask = GT_TASK_DATA(GtResidualLoop);
    residualLoopTask->id = QStringLiteral("Residual Loop");
    residualLoopTask->version = GtVersionNumber(0,1);
    residualLoopTask->description = tr("Repeats execution until residuals "
                                       "are less than defined value or "
                                       "maximum number of iterations "
                                       "is reached.");
    residualLoopTask->status = GtTaskDataImpl::RELEASE;
    GtTaskFactory::registerTaskData(residualLoopTask, builtinModuleId());

    GtTaskData parameterLoop = GT_TASK_DATA(GtParameterLoop);
    parameterLoop->id = QStringLiteral("Parameter Loop");
    parameterLoop->version = GtVersionNumber(0,1);
    parameterLoop->description = tr("Repeats execution for a number of steps."
                                    "Sets a parameter to a new value "
                                    "between start and end value");
    parameterLoop->status = GtTaskDataImpl::RELEASE;
    GtTaskFactory::registerTaskData(parameterLoop, builtinModuleId());
}


GtTaskFactory*
GtTaskFactory::instance()
{
    static GtTaskFactory* retval = nullptr;
    if (!retval)
    {
        retval = new GtTaskFactory(qApp);
    }
    return retval;
}

bool
GtTaskFactory::registerTaskData(GtTaskData data, const QString& module)
{
    if (!gtObjectFactory->registerClass(data->metaData(), module))
    {
        return false;
    }

    m_taskData.insert(data->metaData().className(), data);
    return true;
}

QList<GtTaskData>
GtTaskFactory::taskDataList() const
{
    return m_taskData.values();
}

GtTaskData
GtTaskFactory::taskData(const QString& className)
{
    if (m_taskData.contains(className))
    {
        return m_taskData.value(className);
    }

    return GtTaskData();
}

bool
GtTaskFactory::taskDataExists(const QString& className)
{
    return m_taskData.contains(className);
}

QString
GtTaskFactory::moduleId(const QString& className) const
{
    QString id = GtAbstractObjectFactory::moduleId(className);

    if (id.isEmpty())
    {
        id = gtObjectFactory->moduleId(className);
    }

    return id;
}
