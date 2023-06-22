/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 6.10.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTPROCESSRUNNERTRANSCEIVER_H
#define GTPROCESSRUNNERTRANSCEIVER_H

#include <QObject>
#include <QThread>
#include <QPointer>
#include <QMutex>

#include "gt_processrunnerglobals.h"
#include "gt_task.h"
#include "gt_objectmementodiff.h"

class GtEventLoop;
class GtProcessRunnerCommand;
class GtProcessRunnerResponse;
class GtProcessRunnerNotification;
class GtProcessRunnerConnectionStrategy;
/**
 * @brief The GtProcessRunnerTransceiver class
 * Abstracts the communication and handling of the remote process runner.
 */
class GtProcessRunnerTransceiver : public QObject
{
    Q_OBJECT

    using ConnectionStrategy = GtProcessRunnerConnectionStrategy;

    using Command  = GtProcessRunnerCommand;
    using Response  = GtProcessRunnerResponse;

public:

    struct CollectedDiffs
    {
        /// Pointer to memento diff from collected data objects. May be null
        std::unique_ptr<GtObjectMementoDiff> data{};
        /// Pointer to memento diff from collected task. May be null
        std::unique_ptr<GtObjectMementoDiff> task{};
    };

    /**
     * @brief The ConnectionState enum.
     * Inidcates wther the transceiver is active
     */
    enum ConnectionState
    {
        NotActive = 0,      // No transmission active
        StartingRunner,     // Starting workflow engine
        Connecting,         // Connecting and creating session
        Transmitting,       // Transmitting data
        Recieving,          // Recieving data
        ConnectionClosed,   // Connection was closed explicitly
        ConnectionLost,     // Connection was lost unexpectedly
    };
    Q_ENUM(ConnectionState)

    /**
     * @brief constructor
     * @param connection Connection strategy
     */
    explicit GtProcessRunnerTransceiver(
            std::unique_ptr<ConnectionStrategy> connection);

    /// destructor
    ~GtProcessRunnerTransceiver();

    /**
     * @brief Closes the current session and connection peacefully.
     * Non blocking.
     */
    void close();

    /**
     * @brief Aborts any further activity of the transceiver. Closes current
     * session. Blocking call.
     */
    bool abort();

    /**
     * @brief Will try to reconnect to the process runner. Non blocking.
     */
    void triggerReconnect();

    /**
     * @brief Triggers the execution of the task specified. Non blocking.
     * @param task Task to execute
     */
    void triggerRunTask(GtTask* task);

    /**
     * @brief Triggers the collection of the task specified. Non blocking.
     * @param task Task to collect.
     */
    void triggerCollectTask(GtTask* task);

    /**
     * @brief Updates the connection address. Must be called while no
     * session is active.
     * @param data
     * @param autostart Whether to autostart process runner
     */
    void setTargetAddress(gt::process_runner::ConnectionAddress data,
                          bool autostart);

    /**
     * @brief Returns the collected diff. May be null if no task was collected.
     * @return Pointer to collected memento diff
     */
    CollectedDiffs collectDiffs();

    /**
     * @brief Getter for the current connection state
     * @return Connection state
     */
    ConnectionState connectionState() const;

signals:

    /**
     * @brief Emitted once the connection state changes
     * @param New state
     */
    void connectionStateChanged(GtProcessRunnerTransceiver::ConnectionState);

    /**
     * @brief Emitted once the transceiver has been aborted
     */
    void aborted();

    /**
     * @brief Emitted once a new notification was recieved. Notification should
     * be deleted once its used to reduce memory overhead.
     */
    void notificationRecieved(GtProcessRunnerNotification*);

    /**
     * @brief Emitted once a task has been collected
     */
    void taskCollected();

private slots:

    /**
     * @brief Forwards the notification recieved
     */
    void onNotificationRecieved();

    /**
     * @brief Handles a disconnect
     */
    void onDisconnect();

private:

    /// Thread to run transmitt and recieve data in
    QThread m_thread;

    /// Mutex for accessing event loop
    QMutex m_diffMutex;

    /// Mutex for accessing
    QMutex m_eventLoopMutex;

    /// Current/last session id
    QString m_sessionId;

    /// Representing an active session (frwd decl)
    struct Session;
    /// Pointer to session helper object
    QPointer<Session> m_session;

    /// Pointer to event loop that handels recieving/transmitting.
    /// May be null (indicates no active transmission)
    QPointer<GtEventLoop> m_eventLoop;

    /// Pointer to the connection strategy
    QPointer<ConnectionStrategy> m_connection;

    /// Current state of the conection
    std::atomic<ConnectionState> m_connectionState{};

    /// Pointer to memento diff from collected data objects. May be null
    CollectedDiffs m_diffs;

    /// whether to autostart remote process runner
    bool m_autostart{false};

    /// whether the connection points to localhost
    bool m_isLocalhost{false};

    /**
     * @brief Actual close function. Will disconnect and delete the current
     * session if any existed
     */
    Q_INVOKABLE void doClose();

    /**
     * @brief Actual abort function. Will send an abort command if a connection
     * existed
     */
    Q_INVOKABLE void doAbort();

    /**
     * @brief Actual reconnect function. Restores the last session.
     */
    Q_INVOKABLE void doReconnect();

    /**
     * @brief Actuual function which starts the process runner
     * (if specified), establishes a new session and send the run task command
     * @param task Task to execute
     */
    Q_INVOKABLE void doRunTask(GtTask* task);

    /**
     * @brief Actal function which communicates with the process runner
     * and collects the task diff
     * @param uuid Task to collect
     */
    Q_INVOKABLE void doCollectTask(GtTask* task);

    /**
     * @brief Initializes a new session
     * @return Session pointer. Null if setting up session failed
     */
    Session* initSession();

    /**
     * @brief Retores the last session
     * @return Session pointer. Null if restoring session failed
     */
    Session* restoreSession();

    /**
     * @brief Helper method for initializng/restoring sessions
     * @param sessionId Session id
     * @param initSession Whether to init or restore the session
     * @return Session pointer.
     */
    Session* initSessionHelper(const QString& sessionId,
                               bool initSession = true);

    /**
     * @brief Sends the command sepcified and waits for the response
     * @param command Command to send
     * @return response. May be null
     */
    std::unique_ptr<Response> sendCommand(Session* session, Command& command);

    /**
     * @brief Helper function, which establishes a connection with the process
     * runner
     * @return success
     */
    bool connectWithProcessRunner();

    /**
     * @brief Helper function, which starts the process runner (if specified)
     * @return success
     */
    bool startProcessRunner(GtTask& task);

    /**
     * @brief Setter for the connection state
     * @param state New state
     */
    void setConnectionState(ConnectionState state);
};

#endif // GTPROCESSRUNNERTRANSCEIVER_H
