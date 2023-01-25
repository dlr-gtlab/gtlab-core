/* GTlab - Gas Turbine laboratory
 * Source File: gt_processexecutor.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 26.08.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QMessageBox>

#include "gt_application.h"
#include "gt_settings.h"
#include "gt_task.h"
#include "gt_command.h"
#include "gt_objectmementodiff.h"
#include "gt_taskrunner.h"
#include "gt_finishedprocessloadinghelper.h"
#include "gt_icons.h"

#include "gt_processexecutor.h"

const std::string GtProcessExecutor::S_ID = "ProcessExecutor";

GtProcessExecutor::GtProcessExecutor(QObject* parent) :
    GtCoreProcessExecutor(S_ID, parent),
    m_task(nullptr)
{
    setObjectName("ProcessEecutor");
}

void
GtProcessExecutor::handleTaskFinishedHelper(QList<GtObjectMemento>& changedData,
                                            GtTask* task)
{
    gtDebugId(GT_EXEC_ID).medium() << __FUNCTION__ << "(gui mode)";

    // source may be NULL if there are no object links defined in the
    // calculators included in the task
    if (m_source)
    {
        m_task = task;

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
    gtDebugId(GT_EXEC_ID).medium() << __FUNCTION__;

    if (auto* runner = setupTaskRunner())
    {
        runner->run();
    }
}

bool
GtProcessExecutor::terminateCurrentTask()
{
    assert(m_current);

    QMessageBox mb;
    mb.setIcon(QMessageBox::Question);
    mb.setWindowTitle(tr("Stop Running Task"));
    mb.setWindowIcon(gt::gui::icon::stop16());
    mb.setText(tr("Stop the execution of") + " " +
               m_current->objectName() + "?\n\n" +
               tr("Note: The process cannot be aborted while a "
                  "calculator is running. \n"
                  "The process will be marked for termination "
                  "and will be stopped after completion of the "
                  "calculator."));
    mb.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    mb.setDefaultButton(QMessageBox::Cancel);
    int ret = mb.exec();

    switch (ret)
    {
        case QMessageBox::Yes:
        {
            return GtCoreProcessExecutor::terminateCurrentTask();
        }
        default:
            return false;
    }
}

void
GtProcessExecutor::onHelperFinished()
{
    assert(m_task);

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

    const QString commandMsg = tr("Run '%1'").arg(m_task->objectName());

    if (m_source)
    {

        GtCommand command = gtApp->startCommand(m_source, commandMsg);

        if (!m_source->applyDiff(*helper->sumDiff()))
        {
            gtErrorId(GT_EXEC_ID)
                    << tr("Data changes from the task '%1' could not be "
                           "merged back into datamodel!")
                       .arg(m_task->objectName());
            m_task->setState(GtProcessComponent::FAILED);
        }

        gtApp->endCommand(command);
    }
    else
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Unable to find Root object to apply task results!");
    }

    helper->deleteLater();
}
