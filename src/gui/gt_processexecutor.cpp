/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processexecutor.cpp
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
    GtCoreProcessExecutor(parent),
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
    mb.setWindowIcon(gt::gui::icon::stop());
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

    auto* helper = qobject_cast<GtFinishedProcessLoadingHelper*>(sender());

    if (!helper) return;

    auto cleanup = gt::finally(helper, &QObject::deleteLater);
    Q_UNUSED(cleanup)

    if (helper->sumDiff()->isNull()) return;

    const QString commandMsg = tr("Run '%1'").arg(m_task->objectName());

    if (!m_source)
    {
        gtErrorId(GT_EXEC_ID)
            << tr("Unable to find Root object to apply task results!");
        return;
    }

    auto command = gtApp->makeCommand(m_source, commandMsg);
    Q_UNUSED(command)

    if (!m_source->applyDiff(*helper->sumDiff()))
    {
            gtErrorId(GT_EXEC_ID)
                << tr("Data changes from the task '%1' could not be "
                      "merged back into datamodel!")
                       .arg(m_task->objectName());
            m_task->setState(GtProcessComponent::FAILED);
    }
}
