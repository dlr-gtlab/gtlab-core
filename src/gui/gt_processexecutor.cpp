/* GTlab - Gas Turbine laboratory
 * Source File: gt_processexecutor.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 26.08.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>

#include "gt_application.h"
#include "gt_task.h"
#include "gt_command.h"
#include "gt_objectmementodiff.h"
#include "gt_taskrunner.h"
#include "gt_finishedprocessloadinghelper.h"

#include "gt_processexecutor.h"

GtProcessExecutor::GtProcessExecutor(QObject* parent) :
    GtCoreProcessExecutor(parent),
    m_task(nullptr)
{
}

void
GtProcessExecutor::handleTaskFinishedHelper(QList<GtObjectMemento>& changedData,
                                            GtTask* task)
{
    qDebug() << "handleTaskFinishedHelper gui mode!";

    // source may be NULL if there are no object links defined in the
    // calculators included in the task
    if (m_source)
    {
        m_task = task;
        // create timer
        //QElapsedTimer timer;

        // start timer
        //timer.start();

        GtFinishedProcessLoadingHelper* helper =
                new GtFinishedProcessLoadingHelper(&changedData, m_source,
                                                   m_task->objectName());

        connect(helper, SIGNAL(finished()), SLOT(onHelperFinished()));

        gtApp->loadingProcedure(helper);
    }
}

void
GtProcessExecutor::execute()
{
    qDebug() << "GtProcessExecutor::execute()";

    GtTaskRunner* runner = executeHelper();

    if (!runner)
    {
        return;
    }

    // run
    runner->run();
}

void
GtProcessExecutor::onHelperFinished()
{
    GtFinishedProcessLoadingHelper* helper =
            qobject_cast<GtFinishedProcessLoadingHelper*>(sender());

    if (!helper)
    {
        return;
    }

    if (helper->sumDiff()->isNull())
    {
        helper->deleteLater();

        return;
    }

    const QString commandMsg = m_task->objectName() +
                               QStringLiteral(" ") +
                               tr("run");

    GtCommand command = gtApp->startCommand(m_source, commandMsg);

    if (m_source != nullptr)
    {
        if (!m_source->applyDiff(*helper->sumDiff()))
        {
            gtError() << tr("Data changes from the task")
                      << m_task->objectName()
                      << tr("could not feed back to datamodel");

            m_task->setState(GtProcessComponent::FAILED);
            qDebug() << "Diff not succesfully applied!";
        }
        else
        {
//            const QString commandMsg = task->objectName() +
//                                       QStringLiteral(" ") +
//                                       tr("run");

//            GtMementoChangeCommand* command =
//                    new GtMementoChangeCommand(sumDiff, commandMsg, m_source);
//            gtApp->undoStack()->push(command);
        }
    }
    else
    {
        gtError() << tr("Unable to find Root object to apply task results");
    }


    gtApp->endCommand(command);

    helper->deleteLater();

    //qDebug() << "   |-> diff applied" << timer.elapsed();
}
