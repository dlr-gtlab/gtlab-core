/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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
    gtObjectFactory->registerClass(GT_METADATA(GtProcessData));

    // Default tasks
    GtTaskData simpleTask = GT_TASK_DATA(GtTask);
    simpleTask->id = QStringLiteral("Simple Task");
    simpleTask->version = GtVersionNumber(0,1);
    simpleTask->status = GtTaskDataImpl::RELEASE;
    registerTaskData(simpleTask);

    GtTaskData loopTask = GT_TASK_DATA(GtLoop);
    loopTask->id = QStringLiteral("Loop");
    loopTask->version = GtVersionNumber(0,1);
    loopTask->description = tr("Repeats execution until maixmum "
                               "number of iterations is reached.");
    loopTask->status = GtTaskDataImpl::RELEASE;
    registerTaskData(loopTask);

    GtTaskData residualLoopTask = GT_TASK_DATA(GtResidualLoop);
    residualLoopTask->id = QStringLiteral("Residual Loop");
    residualLoopTask->version = GtVersionNumber(0,1);
    residualLoopTask->description = tr("Repeats execution until residuals "
                                       "are less than defined value or "
                                       "maximum number of iterations "
                                       "is reached.");
    residualLoopTask->status = GtTaskDataImpl::RELEASE;
    registerTaskData(residualLoopTask);

    GtTaskData parameterLoop = GT_TASK_DATA(GtParameterLoop);
    parameterLoop->id = QStringLiteral("Parameter Loop");
    parameterLoop->version = GtVersionNumber(0,1);
    parameterLoop->description = tr("Repeats execution for a number of steps."
                                    "Sets a parameter to a new value "
                                    "between start and end value");
    parameterLoop->status = GtTaskDataImpl::RELEASE;
    registerTaskData(parameterLoop);
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
GtTaskFactory::registerTaskData(GtTaskData taskData)
{
    if (!gtObjectFactory->registerClass(taskData->metaData()))
    {
        return false;
    }

    m_taskData.insert(taskData->metaData().className(), taskData);

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
