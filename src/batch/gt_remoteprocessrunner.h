/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 7.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTREMOTEPROCESSRUNNER_H
#define GTREMOTEPROCESSRUNNER_H

#include <QObject>
#include <queue>

#include "gt_processcomponent.h"
#include "gt_processrunnertcpconnection.h"
#include "gt_processrunnernotification.h"
#include "gt_processrunnerresponse.h"
#include "gt_remoteprocessrunnerstates.h"

class GtProcessRunnerCommand;
class GtProcessRunnerRunTaskCommand;
class GtProcessRunnerCollectTaskCommand;
class GtProcessRunnerInitSessionCommand;
class QTcpServer;
class QHostAddress;

class GtRemoteProcessRunner : public QObject
{
    Q_OBJECT

    using ConnectionStrategy = GtProcessRunnerTcpConnection;
    using Notification = GtProcessRunnerNotification;

public:

    enum ExitCode
    {
        Success = 0,            // successfull
        PortUnavailableError,   // port is already in use
        UninitializedError,     // process runner was not initialized in time
        SocketWriteError,       // critical socket write error
        UnexpectedExit,         // Error in execution
        AbortExit,              // process runner was aborted

        // internally used only
        Undefined = -900,       // exit code not used
        Disconnected = -901,    // connection has been closed
    };

    /**
     * @brief Constructor
     */
    GtRemoteProcessRunner();

    /**
     * @brief Main exec function. Blocking call.
     * Executes the Remote Proess Runner. Runner will open a tcp server and
     * listen to the address and port specified.
     *
     * If no connection was established with in the timeout the runner
     * will exit.
     *
     * Process Runner will exit once an Error occured or the Connection is
     * closed and the process has been collected.
     *
     * @param client Client address to listen to
     * @param port Port to listen to
     * @param timeout Timeout for the initial connection
     * @return Exit code
     */
    ExitCode exec(const QHostAddress& client, quint16 port, int timeout);

    /**
     * @brief Exits the execution.
     * @param code Exit code
     */
    void exit(ExitCode code);

public slots:

    /**
     * @brief Notifies the client, that the task has finished executing
     */
    void onTaskFinished();

    /**
     * @brief Notifies the client about an state or monitoring property change
     */
    void onTaskStateChanged(GtProcessComponent::STATE);

private slots:

    /**
     * @brief Handles disconnects
     */
    void onDisconnected();

    /**
     * @brief Advances to the next state
     */
    void onStateFinished();

    /**
     * @brief Forwards command recieved to the state handlers
     */
    void onCommandRecieved();

private:

    /// pointer to current state handler
    QPointer<GtAbstractProcessRunnerState> m_state;

    /// meta data for the state handlers
    gt::process_runner::MetaData m_metaData;

    /// Connection strategy
    QPointer<ConnectionStrategy> m_connection;

    /// Main event loop
    QPointer<GtEventLoop> m_eventLoop;

    /// Current exit code
    ExitCode m_exitCode{Undefined};

    /// Queue for pending notifications
    std::queue<std::unique_ptr<Notification>> m_pendingNotifications;

    /**
     * @brief Advances tothe next state
     * @param state Next state
     */
    void setNextState(gt::process_runner::State state);

    /**
     * @brief Helper fucntion for sending responses
     * @param response Resonse to send
     */
    void sendResponse(GtProcessRunnerResponse& response);

    /**
     * @brief Helper function for sending notifications
     * @param notification Notification to send
     */
    void sendNotification(std::unique_ptr<Notification> notification);

    /**
     * @brief Process the notifications queue
     */
    void processPendingNotifications();

    /**
     * @brief Helper function for executing the main event loop of the
     * process runner
     * @param server Tcp server
     * @return Exit code
     */
    ExitCode execMainLoop(QTcpServer& server);
};

#endif // GTREMOTEPROCESSRUNNER_H
