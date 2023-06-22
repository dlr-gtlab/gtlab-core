/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 2.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_processrunner.h"

#include "gt_processrunnernotification.h"
#include "gt_processrunnertcpconnection.h"

#include "gt_accessdata.h"
#include "gt_application.h"
#include "gt_settings.h"
#include "gt_command.h"
#include "gt_objectgroup.h"
#include "gt_objectmementodiff.h"
#include "gt_project.h"
#include "gt_utilities.h"
#include "gt_task.h"
#include "gt_package.h"

#include <QMessageBox>

const std::string GtProcessRunner::S_ID = "ProcessRunner";

struct GtProcessRunner::Impl
{
    /// transceiver that handles the communication to remote process runner
    std::unique_ptr<GtProcessRunnerTransceiver> transceiver{
        std::make_unique<GtProcessRunnerTransceiver>(
                    std::make_unique<GtProcessRunnerTcpConnection>())
    };
};

GtProcessRunner::GtProcessRunner() :
    pimpl(std::make_unique<Impl>())
{
    setObjectName("ProcessRunner");

    // intercepts notifications
    connect(pimpl->transceiver.get(),
            &GtProcessRunnerTransceiver::notificationRecieved,
            this, &GtProcessRunner::onNotificationRecieved);

    // intercept task collection signal
    connect(pimpl->transceiver.get(), &GtProcessRunnerTransceiver::taskCollected,
            this, &GtProcessRunner::onTaskCollected);

    // intercept cnnection state changed signal
    connect(pimpl->transceiver.get(),
            &GtProcessRunnerTransceiver::connectionStateChanged,
            this, &GtProcessRunner::onConnectionStateChanged);

    // intercept abort signal
    connect(pimpl->transceiver.get(), &GtProcessRunnerTransceiver::aborted,
            this, &GtProcessRunner::onTransceiverAborted);
}

GtProcessRunner::~GtProcessRunner() = default;

GtProcessRunner::ConnectionState
GtProcessRunner::connectionState() const
{
    return pimpl->transceiver->connectionState();
}

bool
GtProcessRunner::terminateCurrentTask()
{
    assert(m_current);

    auto tasks = m_current->findChildren<GtTask*>() << m_current;

    // will do nothing except change state as the tasks arent running in this
    // application instance
    for (auto task : qAsConst(tasks))
    {
        task->requestInterruption();
    }

    return pimpl->transceiver->abort();
}

void
GtProcessRunner::execute()
{
    assert(m_current);

    m_current->setState(GtProcessComponent::CONNECTING);

    auto* settings = gtApp->settings();
    pimpl->transceiver->setTargetAddress(getConnectionAddress(),
                                         settings->autostartProcessRunner());

    // async call
    pimpl->transceiver->triggerRunTask(m_current);
}

void
GtProcessRunner::onNotificationRecieved(GtProcessRunnerNotification* n)
{
    if (!n)
    {
        gtWarningId(GT_EXEC_ID) << tr("Null notification recieved!");
        return;
    }

    switch (n->notificationType())
    {
    case gt::process_runner::TaskFailedNotification:
        onTaskFinishedUnsuccessfully();
        break;
    case gt::process_runner::TaskFinishedNotification:
        onTaskFinishedSuccessfully();
        break;
    case gt::process_runner::TaskStateChangeNotification:
    {
        auto* payload = n->payload<ProcessComponentStatePayload*>();
        onTaskStateChange(payload);
        break;
    }
    case gt::process_runner::TaskPropertyChangeNotification:
        onTaskPropertyChange(n->payload<GtProcessComponent*>());
        break;
    default:
        gtWarningId(GT_EXEC_ID) << tr("Unkown notification recieved!");
        break;
    }

    n->deleteLater();
}

void
GtProcessRunner::onTaskFinishedSuccessfully()
{
    gtDebugId(GT_EXEC_ID) << "Collecting task...";

    if (!taskCurrentlyRunning())
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Failed to collect task data!")
                << tr("(No running task registered)");
        return;
    }

    // async call
    pimpl->transceiver->triggerCollectTask(m_current);
}

void
GtProcessRunner::onTaskFinishedUnsuccessfully()
{
    gtWarningId(GT_EXEC_ID) << "Task execution failed!";
    pimpl->transceiver->close();

    onTransceiverAborted();
}

void
GtProcessRunner::onTaskCollected()
{
    gtDebugId(GT_EXEC_ID) << "Merging task...";

    // sanity check
    if (!taskCurrentlyRunning())
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Failed to merge task data!")
                << tr("(No running task registered)");
        return;
    }

    // collect diff
    auto diffs = pimpl->transceiver->collectDiffs();
    if (!diffs.data || !diffs.task)
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Failed to merge task data!")
                << tr("(Null diff)");
        return;
    }

    // find project and parent of task to merge diffs with
    auto* project = m_current->findParent<GtProject*>();
    auto* processData = m_current->parentObject();
    if (!project || !processData)
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Failed to merge task data!")
                << tr("(Null project or process data)");
        return;
    }

    // create command
    QString commandMsg = QStringLiteral("Merge Task '%1'")
                            .arg(m_current->objectName());

    GtCommand command = gtApp->startCommand(project, commandMsg);

    // apply diffs
    bool ok = true;
    ok &= project->applyDiff(*diffs.data);
    ok &= processData->applyDiff(*diffs.task);

    if (!ok)
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Data changes from the task '%1' could not be merged "
                      "into the datamodel").arg(m_current->objectName());

        m_current->setStateRecursively(GtProcessComponent::FAILED);
    }

    gtApp->endCommand(command);

    clearCurrentTask();

    // execute next task
    executeNextTask();
}

void
GtProcessRunner::onTaskStateChange(ProcessComponentStatePayload* payload)
{
    if (!payload)
    {
        gtWarningId(GT_EXEC_ID) << tr("Invalid notification payload!");
        return;
    }

    if (!taskCurrentlyRunning())
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Failed to change task state!")
                << tr("(No running task registered)");
        return;
    }

    // find object to update
    auto* pc = qobject_cast<GtProcessComponent*>(
                   m_current->getObjectByUuid(payload->componentUuid()));

    if (!pc)
    {
        gtWarningId(GT_EXEC_ID) << tr("Failed to find process component by uuid!");
        return;
    }

    // update property
    pc->setState(payload->state());
}

void
GtProcessRunner::onTaskPropertyChange(GtProcessComponent* other)
{
    if (!other)
    {
        gtWarningId(GT_EXEC_ID) << tr("Invalid notification payload!");
        return;
    }

    if (!taskCurrentlyRunning())
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Failed to update monitoring property of task!")
                << tr("(No running task registered)");
        return;
    }

    // find object to update
    auto* local = qobject_cast<GtProcessComponent*>(
                        m_current->getObjectByUuid(other->uuid()));

    if (!local)
    {
        gtWarningId(GT_EXEC_ID) << tr("Failed to find process component by uuid!");
        return;
    }

    // monitoring props
    auto otherProps = other->monitoringProperties();
    auto localProps = local->monitoringProperties();

    int size = localProps.size();

    // check if all uuids match, order of objects must match!
    bool canMergeData =
            (size == otherProps.size()) &&
            std::equal(std::cbegin(otherProps), std::cend(otherProps),
                       std::cbegin(localProps), [](auto* a, auto* b){
        return a && b && a->ident() == b->ident();
    });

    if (!canMergeData)
    {
        gtWarningId(GT_EXEC_ID)
               << tr("Cannot update the monitoring properties, data mismatch!");
        gtDebug() << "Remote:" << otherProps;
        gtDebug() << "Local: " << localProps;
        return;
    }

    // update properties
    for (int j = 0; j < size; ++j)
    {
        localProps[j]->setValueFromVariant(otherProps[j]->valueToVariant());
    }
}

void
GtProcessRunner::onConnectionStateChanged(ConnectionState state)
{
    emit connectionStateChanged();

    if (state == ConnectionState::ConnectionLost)
    {
        gtWarningId(GT_EXEC_ID) << tr("Lost connection with the Process Runner!");

        auto button = QMessageBox::warning(
            nullptr, tr("Lost connection with Process Runner"),
            tr("The connection to the Process Runner has been "
               "lost unexpectedly.\n"
               "The process may "
               "have crashed.\n\n"
               "Would you like to try reconnecting?"),
            QMessageBox::Retry, QMessageBox::Abort
        );

        // reconnect
        if (button == QMessageBox::Retry)
        {
            return pimpl->transceiver->triggerReconnect();
        }

        // abort execution
        terminateTask(m_current);
    }
}

void
GtProcessRunner::onTransceiverAborted()
{
    gtDebug().medium() << __FUNCTION__;

    clearCurrentTask();

    // execute next task
    executeNextTask();
}

gt::process_runner::ConnectionAddress
GtProcessRunner::getConnectionAddress() const
{
    // connection address
    auto addr = gt::process_runner::S_DEFAULT_HOST;
    auto port = gt::process_runner::S_DEFAULT_PORT;

    assert(m_current);

    auto accessData = m_current->selectedProcessRunner();
    if (!accessData.isEmpty())
    {
        addr = accessData.host();

        if (gt::checkNumericalLimits<quint16>(accessData.port()))
        {
            port = accessData.port();
        }
        else
        {
            gtWarningId(GT_EXEC_ID)
                    << tr("Invalid port specified, defaulting to") << port;
        }
    }
    else
    {
        gtDebugId(GT_EXEC_ID)
                << tr("No process runner selected, defaulting to localhost!");
    }

    return { addr, port };
}
