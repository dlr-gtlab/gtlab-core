/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 7.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_remoteprocessrunner.h"

#include <QTimer>
#include <QThread>
#include <QEventLoop>
#include <QTcpServer>
#include <QTcpSocket>

#include <iostream>

#include "gt_coreapplication.h"
#include "gt_eventloop.h"
#include "gt_task.h"
#include "gt_utilities.h"
#include "gt_coreprocessexecutor.h"
#include "gt_processrunnercommand.h"
#include "gt_remoteprocessrunnerstates.h"

#include <QLockFile>
#include <QStandardPaths>

static int S_SENDING_TIMEOUT = 3 * 1000;

static uint S_MAX_RETRIES = 3;

namespace
{

/**
 * @brief Helper function to send data. If writing fails retry x more times
 * @param connection COnnection strategy
 * @param data Data to write
 * @return Success
 */
template <typename Data>
inline GtEventLoop::State
sendData(GtProcessRunnerConnectionStrategy& connection, Data const& data)
{
    uint retries = 0;

    do {
        GtEventLoop loop{S_SENDING_TIMEOUT};

        connection.writeData(loop, data);

        if (loop.exec() == GtEventLoop::Success)
        {
            return GtEventLoop::Success;
        }

        // sending failed
        retries += 1;

        gtWarningId(GT_EXEC_ID) << QObject::tr("Failed to send data! (%1/%2)")
                              .arg(retries)
                              .arg(S_MAX_RETRIES);

        // connection may have been lost
        if (!connection.hasConnection())
        {
            gtDebugId(GT_EXEC_ID) << QObject::tr("No connection!");
            return GtEventLoop::Aborted;
        }
    }
    while(retries < S_MAX_RETRIES);

    return (retries < S_MAX_RETRIES) ? GtEventLoop::Success :
                                       GtEventLoop::Failed;
}

} // namespace


GtRemoteProcessRunner::GtRemoteProcessRunner() :
    m_connection{new GtProcessRunnerTcpConnection{this}}
{
    setObjectName("RemoteProcessRunner");

    setNextState(gt::process_runner::UninitializedState);

    // setup connections
    connect(m_connection.data(), &ConnectionStrategy::disconnected,
            this, &GtRemoteProcessRunner::onDisconnected,
            Qt::QueuedConnection);

    connect(m_connection.data(), &ConnectionStrategy::recievedCommand,
            this, &GtRemoteProcessRunner::onCommandRecieved,
            Qt::QueuedConnection);
}

void
GtRemoteProcessRunner::setNextState(gt::process_runner::State state)
{
    m_state = gt::process_runner::makeState(state, m_metaData, *this);

    connect(m_state.data(), &GtAbstractProcessRunnerState::stateFinished,
            this, &GtRemoteProcessRunner::onStateFinished);
}

void
GtRemoteProcessRunner::exit(ExitCode code)
{
    assert(code != Undefined);

    m_exitCode = code;

    m_connection->close();

    if (m_eventLoop)
    {
        return emit m_eventLoop->success();
    }
}

void
GtRemoteProcessRunner::onDisconnected()
{
    gtInfoId(GT_EXEC_ID) << "Connection closed!";

    exit(Disconnected);
}

void
GtRemoteProcessRunner::onStateFinished()
{
    gtDebugId(GT_EXEC_ID) << "On State Changed!" << m_state;
    assert(m_state);

    auto oldState = m_state;

    setNextState(m_state->nextState());

    oldState->deleteLater();
}

void
GtRemoteProcessRunner::onTaskFinished()
{
    gtInfoId(GT_EXEC_ID) << "Task finished!";

    // advance to next state
    onStateFinished();

    assert(m_metaData.task);

    // check success state of task
    bool success =
            m_metaData.task->currentState() == GtProcessComponent::FINISHED ||
            m_metaData.task->currentState() == GtProcessComponent::WARN_FINISHED;

    // task doesnt need to be collected
    if (!success)
    {
        onStateFinished();
    }

    auto notificationType =
            success ? gt::process_runner::TaskFinishedNotification :
                      gt::process_runner::TaskFailedNotification;

    auto notification = std::make_unique<GtProcessRunnerNotification>(
        m_metaData.sessionId,
        notificationType,
        true // non optional notification
    );

    sendNotification(std::move(notification));
}

void
GtRemoteProcessRunner::onTaskStateChanged(GtProcessComponent::STATE)
{
    auto* pc = qobject_cast<GtProcessComponent*>(sender());
    if (!pc)
    {
        return;
    }

    // state changed notification
    auto notifyTaskChange = std::make_unique<GtProcessRunnerNotification>(
        m_metaData.sessionId,
        gt::process_runner::TaskStateChangeNotification
    );

    // notification will take ownership
    notifyTaskChange->appendPayload(
                new gt::process_runner::ProcessComponentStatePayload{pc});

    sendNotification(std::move(notifyTaskChange));

    // update monitoring property once process component finished
    if (pc->isComponentReady() && !pc->monitoringProperties().isEmpty())
    {
        // monitoring property change notification
        auto notifyPropChange = std::make_unique<GtProcessRunnerNotification>(
            m_metaData.sessionId,
            gt::process_runner::TaskPropertyChangeNotification
        );

        // notification will take ownership
        notifyPropChange->appendPayload(pc->clone());

        sendNotification(std::move(notifyPropChange));
    }
}

void
GtRemoteProcessRunner::sendResponse(GtProcessRunnerResponse& response)
{
    if (m_exitCode != Undefined)
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Not sending command, execution has been stopped!");
        return;
    }

    // make sure command was finalized
    if (response.sessionId().isEmpty() || response.commandUuid().isEmpty())
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Response was not finalized!");
        return exit(SocketWriteError);
    }

    gtDebugId(GT_EXEC_ID).verbose() << "Sending response...";

    // debug output for an unsuccessful reponse
    if (!response.success())
    {
        gtWarningId(GT_EXEC_ID).verbose().nospace()
                << tr("Error occured in state ") << m_state->state()
                << ": " << gt::quoted(response.error());
    }

    // send data
    auto res = sendData(*m_connection, response.toMemento().toByteArray());

    // Aborted
    if (res != GtEventLoop::Success)
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Failed to send non-optional response")
                << tr("aborting execution!");
        return exit(SocketWriteError);
    }
}

void
GtRemoteProcessRunner::sendNotification(
        std::unique_ptr<Notification> notification)
{
    m_pendingNotifications.emplace(std::move(notification));

    processPendingNotifications();
}

void
GtRemoteProcessRunner::processPendingNotifications()
{
    // While data is beeing send other notifications may call this function
    // However each notification should be written one by one as they are in
    // the queue. Therefore we use this mutex like flag.
    static bool isBeeingProcessed = false;

    // exit if notifications are beeing processed
    if (isBeeingProcessed)
    {
        return;
    }

    if (!m_connection->hasConnection())
    {
        return;
    }

    isBeeingProcessed = true;

    // process pending notifications
    while (!m_pendingNotifications.empty())
    {
        auto& notification = *m_pendingNotifications.front();

        gtDebugId(GT_EXEC_ID).verbose()
                << "Sending notification..." << notification.notificationType();

        // send data
        auto res = sendData(
                       *m_connection, notification.toMemento().toByteArray());

        // abort (e.g. connection lost)
        if (res == GtEventLoop::Aborted)
        {
            break;
        }

        bool optional = notification.isOptional();

        // unqueue (notification is now invalid)
        m_pendingNotifications.pop();

        // exit if non-optional
        if (!optional && res == GtEventLoop::Failed)
        {
            gtErrorId(GT_EXEC_ID) << tr("Failed to send non-optional notification!")
                             << tr("aborting execution!");
            exit(SocketWriteError);
            break;
        }
    }

    isBeeingProcessed = false;
}
void
GtRemoteProcessRunner::onCommandRecieved()
{
    auto command = m_connection->nextCommand();
    if (!command)
    {
        gtWarningId(GT_EXEC_ID) << "Invalid command!";
        return;
    }

    gtDebugId(GT_EXEC_ID)
            << "Recieved command: " << command->objectName() << command->uuid();

    gtDebugId(GT_EXEC_ID) << "Current state:     " << m_state;
    gtDebugId(GT_EXEC_ID) << "Session id:        " << m_metaData.sessionId;
    gtDebugId(GT_EXEC_ID) << "Command session id:" << command->sessionId();

    auto response = m_state->handleCommand(*command);

    // command was not handled
    if (!response)
    {
        response = m_state->makeResponse(*command,
            gt::process_runner::UnkownCommandError,
            tr("Command could not be handled by current state!")
        );
    }

    return sendResponse(*response);
}

GtRemoteProcessRunner::ExitCode
GtRemoteProcessRunner::exec(const QHostAddress& client,
                            quint16 port,
                            int timeout)
{
    // for nicer debug output
    auto printOutputSeparator = [](auto msg){
        qDebug().noquote() << "";
        qDebug().noquote() << QByteArray{"#"}.repeated(30) << msg;
        qDebug().noquote() << "";
    };

    // lock file destination
    QString lockFilePath = gt::process_runner::portLockFilePath(port);

    gtDebugId(GT_EXEC_ID) << "Lockfile:" << lockFilePath;

    QLockFile portLockFile{lockFilePath};
    // unlock file on exit
    auto cleanup = gt::finally(&portLockFile, &QLockFile::unlock);

    // we will lock this file indefinetly
    portLockFile.setStaleLockTime(0);

    // lock the port if available
    if (!portLockFile.tryLock())
    {
        gtFatalId(GT_EXEC_ID)
                << tr("Port %1 is already in use. If you think this is an "
                      "error, remove the lock file manually (%2)")
                       .arg(port)
                       .arg(lockFilePath);
        return PortUnavailableError;
    }

    // initialize executor
    gt::currentProcessExecutor().setCoreExecutorFlags(gt::NonBlockingExecution);

    // initialize server
    QTcpServer server;
    server.setMaxPendingConnections(2);

    // start
    std::cout << "Executing process runner. Listening to "
              << client.toString().toStdString() << " on port "
              << port << "..." << std::endl;

    // start listening
    server.listen(client, port);

    // blocking call
    if (!server.waitForNewConnection(timeout))
    {
        std::cout << "Failed to connect to a client!";
        std::cout << "Error: " << server.errorString().toStdString()
                  << std::endl;
        return UninitializedError;
    }

    // connect to client
    m_connection->connectTo(std::unique_ptr<QTcpSocket>(
                                server.nextPendingConnection()));
    std::cout << "Client connected!" << std::endl;

    printOutputSeparator("START");

    auto code = execMainLoop(server);

    // unlock file
    cleanup.finalize();

    switch (code)
    {
    case Success:
        printOutputSeparator(QStringLiteral("EXT SUCCESS"));
        break;
    case AbortExit:
        gtFatalId(GT_EXEC_ID) << "Aborting execution!";
        printOutputSeparator(QStringLiteral("EXT FAILURE (Aborted with %1)")
                             .arg(code));
        std::exit(code);
        break;
    default:
        printOutputSeparator(QStringLiteral("EXT FAILURE (%1)")
                             .arg(code));
    }

    return code;
}

GtRemoteProcessRunner::ExitCode
GtRemoteProcessRunner::execMainLoop(QTcpServer& server)
{
    // main loop
    while (m_connection->hasConnection() && m_exitCode == Undefined)
    {
        // execution event loop
        GtEventLoop loop{-1};
        m_eventLoop = &loop;

        processPendingNotifications();

        // eventloop will be canceld by exit function
        loop.exec();

        switch (m_exitCode)
        {
        case Disconnected:
        {
            auto type = m_state->stateType();
            // process runner is finished
            if (type == gt::process_runner::FinishStateType)
            {
                return Success;
            }
            // process runner was not initialized
            if (type == gt::process_runner::StartStateType)
            {
                gtErrorId(GT_EXEC_ID)
                        << tr("Failed to initialize Process Runner in time!");
                return UninitializedError;
            }
            // dont yet return, continue process execution
            break;
        }
        default:
            return m_exitCode;
        }

        // clear exit code
        m_exitCode = Undefined;

        // reconnect
        gtInfoId(GT_EXEC_ID) << "Waiting for a new connection...";

        // wait for new connection
        GtEventLoop waitForConnection{-1};
        // QPointer will take care of "dangling life time"
        // cppcheck-suppress danglingLifetime
        m_eventLoop = &waitForConnection;

        waitForConnection.connectSuccess(&server, &QTcpServer::newConnection);
        waitForConnection.connectFailed(&server, &QTcpServer::acceptError);

        // there might be a pending execution
        if (!server.hasPendingConnections())
        {
            waitForConnection.exec();
        }

        gtInfoId(GT_EXEC_ID) << "Connecting...";
        m_connection->connectTo(std::unique_ptr<QTcpSocket>(
                                    server.nextPendingConnection()));
    }

    gtErrorId(GT_EXEC_ID) << tr("Unexpected exit (no connection/invalid state)");
    return UnexpectedExit;
}

