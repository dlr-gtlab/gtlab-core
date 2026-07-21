/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 6.10.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_processrunnertransceiver.h"

#include "gt_logging.h"
#include "gt_application.h"
#include "gt_objectmementodiff.h"
#include "gt_project.h"
#include "gt_utilities.h"
#include "gt_eventloop.h"
#include "gt_exceptions.h"
#include "gt_hostinfo.h"
#include "gt_tcpsocket.h"
#include "gt_processrunnercommand.h"
#include "gt_processrunnerresponse.h"
#include "gt_processrunnernotification.h"
#include "gt_processrunnerconnectionstrategy.h"
#include "gt_environment.h"

#include <QUuid>
#include <QTimer>
#include <QRunnable>
#include <QRandomGenerator>
#include <QtConcurrent>
#include <QLockFile>

#include <array>

static const int S_SEND_COMMAND_TIMEOUT = 60 * 1000;
static const int S_CONNECTION_TIMEOUT   = 10 * 1000;
static const int S_ABORT_TIMEOUT        =  3 * 1000;

static const uint S_PORT_RANGE = 20;

static std::array<quint16, S_PORT_RANGE> S_PORTS{};

static const QString S_ENV_PROJECT_PATH =
        QStringLiteral("GT_REMOTE_PROJECT_PATH");

namespace
{

/**
 * @brief Attempts to reconnect to the remote rpocess runner and sends abort
 * command to terminate the process
 * @param sessionId Last session id
 * @param address Last address
 * @param port Last port
 * @param timeout Timeout
 * @return success
 */
bool terminateProcessRunner(QString const& sessionId,
                            QString const& address,
                            quint16 port,
                            int timeout);

/**
 * @brief Attempts to find an available port. Returns 0 if none is empty.
 * @return port
 */
quint16 findAvailablePort() noexcept(false);

/**
 * @brief Returns the project path to use for the execution
 * @param isLocalhost Whether the remote project path should be used
 * @return Project Path
 */
QString projectPath(bool isLocalhost);

} // namsespace

struct GtProcessRunnerTransceiver::Session : public QObject
{
    explicit Session(QString session, QObject* parent = {}) :
        QObject{parent},
        id{std::move(session)}
    {
        setObjectName(QStringLiteral("Session"));
    }

    /// session id. Used for authentification with the remote process runner
    const QString id;

    /// last known state of the remote process runner
    gt::process_runner::State state{gt::process_runner::UninitializedState};
};

GtProcessRunnerTransceiver::GtProcessRunnerTransceiver(
        std::unique_ptr<ConnectionStrategy> connection) :
    m_connection{connection.release()},
    m_diffs{nullptr}
{
    setObjectName("ProcessRunnerTransceiver");

    assert(m_connection);

    m_connection->setParent(this);

    // init port range
    std::iota(std::begin(S_PORTS), std::end(S_PORTS),
              gt::process_runner::S_DEFAULT_PORT);

    // register enum
    qRegisterMetaType<GtProcessRunnerTransceiver::ConnectionState>(
                "GtProcessRunnerTransceiver::ConnectionState");

    // connections
    connect(m_connection, &ConnectionStrategy::recievedNotification,
            this, &GtProcessRunnerTransceiver::onNotificationRecieved,
            Qt::QueuedConnection);

    connect(m_connection, &ConnectionStrategy::disconnected,
            this, &GtProcessRunnerTransceiver::onDisconnect,
            Qt::QueuedConnection);

    // register project path variable
    gtEnvironment->addEnvironmentVariable(S_ENV_PROJECT_PATH);

    // start thread
    moveToThread(&m_thread);
    m_thread.start();
}

GtProcessRunnerTransceiver::~GtProcessRunnerTransceiver()
{
    gtDebugId(GT_EXEC_ID).medium() << tr("Closing down Process Runner...");
    // join thread - should happen instantaneously
    m_thread.quit();
    m_thread.wait(std::chrono::seconds{1});
}

GtProcessRunnerTransceiver::ConnectionState
GtProcessRunnerTransceiver::connectionState() const
{
    return m_connectionState;
}

GtProcessRunnerTransceiver::CollectedDiffs
GtProcessRunnerTransceiver::collectDiffs()
{
    QMutexLocker locker{&m_diffMutex};

    return std::move(m_diffs);
}

bool
GtProcessRunnerTransceiver::abort()
{
    // local event loop to make this call blocking
    GtEventLoop loop{S_ABORT_TIMEOUT};
    loop.connectSuccess(this, &GtProcessRunnerTransceiver::aborted);

    // stop any running event loop that may block our next signal
    {
        QMutexLocker locker{&m_eventLoopMutex};
        if (m_eventLoop)
        {
            emit m_eventLoop->abort();
        }
    }

    // abort transveiver
    // use meta object call to invoke the method in the objects thread
    bool success = QMetaObject::invokeMethod(
                       this, &GtProcessRunnerTransceiver::doAbort);
    assert(success);

    return success && loop.exec() == GtEventLoop::Success;
}

void
GtProcessRunnerTransceiver::close()
{
    bool success = QMetaObject::invokeMethod(
                       this, &GtProcessRunnerTransceiver::doClose);
    assert(success);
}

void
GtProcessRunnerTransceiver::triggerReconnect()
{
    // use meta object call to invoke the method in the objects thread
    bool success = QMetaObject::invokeMethod(
                       this, &GtProcessRunnerTransceiver::doReconnect);
    assert(success);
}

void
GtProcessRunnerTransceiver::triggerRunTask(GtTask* task)
{
    // use meta object call to invoke the method in the objects thread
    bool success = QMetaObject::invokeMethod(this, "doRunTask",
                                             Q_ARG(GtTask*, task));
    assert(success);
}

void
GtProcessRunnerTransceiver::triggerCollectTask(GtTask* task)
{
    // use meta object call to invoke the method in the objects thread
    bool success = QMetaObject::invokeMethod(this, "doCollectTask",
                                             Q_ARG(GtTask*, task));
    assert(success);
}

void
GtProcessRunnerTransceiver::setTargetAddress(
        gt::process_runner::ConnectionAddress data, bool autostart)
{
    if (m_connection->hasConnection() || m_session)
    {
        gtWarningId(GT_EXEC_ID)
                << tr("Cannot change connection data while a session is "
                      "still active!");
        return;
    }

    m_connection->setTargetAddress(std::move(data));
    m_autostart = autostart;

    // clear localhost flag
    m_isLocalhost = false;
}

void
GtProcessRunnerTransceiver::onDisconnect()
{
    // check if the connection was lost unexpectedly
    if (m_connectionState != ConnectionClosed)
    {
        setConnectionState(ConnectionLost);
    }
}

void
GtProcessRunnerTransceiver::onNotificationRecieved()
{
    auto notification = m_connection->nextNotification();
    if (!notification)
    {
        gtWarningId(GT_EXEC_ID) << tr("Null notifcation recieved!");
        return;
    }

    gtDebugId(GT_EXEC_ID).verbose()
            << tr("Recieved notification") << notification->notificationType();

    if (notification->sessionId() != m_sessionId)
    {
        gtWarningId(GT_EXEC_ID) << tr("Recieved notification with an invalid "
                                 "session id!");
        return;
    }

    // change owner
    auto n = notification.release();
    n->setParent(this);
    emit notificationRecieved(n);
}

void
GtProcessRunnerTransceiver::doClose()
{
    gtDebugId(GT_EXEC_ID).verbose() << "closing...";

    setConnectionState(ConnectionClosed);
    m_connection->close();

    if (m_session)
    {
        m_session->deleteLater();
    }
}

void
GtProcessRunnerTransceiver::doAbort()
{
    // nothing to do here
    if (m_connectionState == ConnectionClosed)
    {
        return;
    }

    gtDebugId(GT_EXEC_ID).verbose() << "aborting...";

    bool hadConnection = m_connection->hasConnection();

    // disconnect to gurantee that any stalling send/recieve action has
    // been stopped
    doClose();

    emit aborted();

    // terminate the remote process runner by reconnecting and
    // sending the abort command (in a separate thread)
    if (hadConnection)
    {
        auto future = QtConcurrent::run(
            terminateProcessRunner,
            m_sessionId,
            m_connection->targetAddress().address,
            m_connection->targetAddress().port,
            S_ABORT_TIMEOUT
        );
        Q_UNUSED(future);
    }
}

void
GtProcessRunnerTransceiver::doReconnect()
{
    if (!m_session)
    {
        gtErrorId(GT_EXEC_ID) << tr("No active session, cannot reconnect!");
        setConnectionState(NotActive);
        return;
    }

    if (!restoreSession())
    {
        setConnectionState(ConnectionLost);
    }
}

void
GtProcessRunnerTransceiver::doRunTask(GtTask* task)
{
    if (!task)
    {
        gtErrorId(GT_EXEC_ID) << tr("Null task!");
        return doAbort();
    }

    if (!startProcessRunner(*task))
    {
        return doAbort();
    }

    Session* session = initSession();
    if (!session)
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Setting up session with Process Runner failed!");
        return doAbort();
    }

    auto pPath = projectPath(m_isLocalhost);
    gtDebugId(GT_EXEC_ID) << tr("Using project path: %1").arg(pPath);

    // make run command
    GtProcessRunnerRunTaskCommand cmd{task, pPath};

    // send run command and check response
    auto response = sendCommand(session, cmd);

    if (!response || !response->success())
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Triggering task execution in the Process Runner failed!")
                << gt::process_runner::errorMessage(response.get());
        return doAbort();
    }

    setConnectionState(NotActive);
}

void
GtProcessRunnerTransceiver::doCollectTask(GtTask* task)
{
    auto* project = gtApp->currentProject();
    assert(project);

    Session* session = restoreSession();
    if (!session)
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Restoring session with Process Runner failed!");
        return doAbort();
    }

    GtProcessRunnerCollectTaskCommand cmd{task->uuid()};

    auto response = sendCommand(session, cmd);

    if (!response || !response->success())
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Collecting task data from the Process Runner failed!")
                << gt::process_runner::errorMessage(response.get());
        return doAbort();
    }

    setConnectionState(NotActive);

    // data diff
    auto* dataPayload =
            response->payload<gt::process_runner::MementoDiffPayload*>(
                project->objectName());

    // task diff
    auto* taskPayload =
            response->payload<gt::process_runner::MementoDiffPayload*>(
                task->objectName());

    if (!dataPayload || !taskPayload)
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Collecting task data from the Process Runner failed! "
                      "(Payload corrupted)");
        return doAbort();
    }

    { // block for mutex locking
        QMutexLocker locker{&m_diffMutex};
        m_diffs = {};
        m_diffs.data = std::make_unique<GtObjectMementoDiff>(
                           dataPayload->mementoDiff());
        m_diffs.task = std::make_unique<GtObjectMementoDiff>(
                           taskPayload->mementoDiff());
    }
    emit taskCollected();

    return doClose();
}

GtProcessRunnerTransceiver::Session*
GtProcessRunnerTransceiver::initSession()
{
    // check if a session already exists
    if (m_session)
    {
        gtWarningId(GT_EXEC_ID)
                << tr("A session with a Process Runner is already active!");

        gtDebugId(GT_EXEC_ID) << "Session state:     " << m_session->state;
        gtDebugId(GT_EXEC_ID) << "Session connection:" << m_connection->hasConnection();
        return nullptr;
    }

    if (!connectWithProcessRunner())
    {
        return nullptr;
    }

    // create new session id
    QString sessionId = QUuid::createUuid().toString();

    // create session object
    return initSessionHelper(sessionId, true);
}

GtProcessRunnerTransceiver::Session*
GtProcessRunnerTransceiver::restoreSession()
{
    if (m_sessionId.isEmpty())
    {
        gtWarningId(GT_EXEC_ID) << tr("No session to restore!");
        return nullptr;
    }

    // check if session already exists
    if (m_session)
    {
        gtDebugId(GT_EXEC_ID) << "Restoring session...";

        if (m_session->id != m_sessionId)
        {
            gtWarningId(GT_EXEC_ID) << tr("Another Process Runner session is "
                                     "already active!");

            gtDebugId(GT_EXEC_ID) << "Session id:" << m_session->id;
            gtDebugId(GT_EXEC_ID) << "Session state:" << m_session->state;
            gtDebugId(GT_EXEC_ID) << "Session connection:"
                              << m_connection->hasConnection();
            return nullptr;
        }

        if (m_connection->hasConnection())
        {
            gtDebugId(GT_EXEC_ID) << "Session is still valid!";
            return m_session;
        }

        gtDebugId(GT_EXEC_ID) << "Overwriting session...";
    }

    if (!connectWithProcessRunner())
    {
        return nullptr;
    }

    return initSessionHelper(m_sessionId, false);
}

GtProcessRunnerTransceiver::Session*
GtProcessRunnerTransceiver::initSessionHelper(const QString& sessionId,
                                              bool initSession)
{
    auto session = std::make_unique<Session>(sessionId, this);

    if (initSession)
    {
        // init session with remote instance
        GtProcessRunnerInitSessionCommand sessionCommand;

        auto response = sendCommand(session.get(), sessionCommand);

        if (!response)
        {
            gtWarningId(GT_EXEC_ID) << tr("Setting up the session with the "
                                     "Process Runner timed out!");
            return nullptr;
        }

        if (!response->success())
        {
            gtWarningId(GT_EXEC_ID)
                    << tr("Process Runner declined session request!")
                    << gt::process_runner::errorMessage(response.get());
            return nullptr;
        }
    }

    setConnectionState(NotActive);

    // take ownership of session
    if (m_session)
    {
        m_session->deleteLater();
    }
    m_sessionId = session->id;
    m_session = session.release(); // session is null now!
    m_session->setParent(this);

    return m_session;
}

std::unique_ptr<GtProcessRunnerResponse>
GtProcessRunnerTransceiver::sendCommand(Session* session, Command& command)
{
    if (!m_connection->hasConnection() || !session)
    {
        gtWarningId(GT_EXEC_ID) << tr("Invalid session!");
        return nullptr;
    }

    // set session id
    command.setSessionId(session->id);

    gtDebugId(GT_EXEC_ID)
            << tr("Sending command")
            << gt::squoted(command.objectName()) + "...";


    // make sure command was finalized
    if (!command.isFinalized())
    {
        gtWarningId(GT_EXEC_ID) << tr("Command was not finalized!");
        return nullptr;
    }

    { // limit lifetime of loop

        GtEventLoop loop{S_SEND_COMMAND_TIMEOUT};

        // set loop
        {
            QMutexLocker locker{&m_eventLoopMutex};
            m_eventLoop = &loop;
        }

        setConnectionState(Transmitting);

        // async call
        m_connection->writeData(loop, command.toMemento().toByteArray());

        auto res = loop.exec();

        // clear loop
        {
            QMutexLocker locker{&m_eventLoopMutex};
            m_eventLoop.clear();
        }

        // wait for exit
        if (res != GtEventLoop::Success)
        {
            gtWarningId(GT_EXEC_ID) << tr("Failed to send command!");
            return nullptr;
        }
    }


    { // limit lifetime of loop
        GtEventLoop loop{S_SEND_COMMAND_TIMEOUT};

        // set loop
        {
            QMutexLocker locker{&m_eventLoopMutex};
            m_eventLoop = &loop;
        }

        setConnectionState(Recieving);

        auto res = m_connection->waitFor(loop, SIGNAL(recievedResponse()));

        // clear loop
        {
            QMutexLocker locker{&m_eventLoopMutex};
            m_eventLoop.clear();
        }

        // wait for exit
        if (res != GtEventLoop::Success)
        {
            gtWarningId(GT_EXEC_ID) << tr("Recieving response timed out!");
            return nullptr;
        }
    }

    // TODO: Make sure response was not recieved before
    // fetch and check response
    auto response = m_connection->nextResponse();

    if (!response)
    {
        gtWarningId(GT_EXEC_ID)
                << tr("Recieved null response!");
        return nullptr;
    }
    if (response->sessionId() != session->id)
    {
        gtWarningId(GT_EXEC_ID)
                << tr("Recieved a response with an invalid session id!");
        return nullptr;
    }
    if (response->commandUuid() != command.uuid())
    {
        gtWarningId(GT_EXEC_ID)
                << tr("Recieved a response with an invalid command id!");
        return nullptr;
    }

    // update executor state
    session->state = response->runnerState();

    return response;
}

bool
GtProcessRunnerTransceiver::connectWithProcessRunner()
{
    if (m_connection->hasConnection())
    {
        gtDebugId(GT_EXEC_ID) << "Already connected with a Process Runner!";
        return true;
    }

    gtInfoId(GT_EXEC_ID) << tr("Connecting with Process Runner...");

    // check if connection data is valid
    if (!m_connection->targetAddress().isValid())
    {
        gtWarningId(GT_EXEC_ID)
                << tr("Invalid connection data, cannot connect to the "
                      "Process Runner!");
        return false;
    }

    // connect with remote instance
    GtEventLoop loop{S_CONNECTION_TIMEOUT};

    // set loop
    {
        QMutexLocker locker{&m_eventLoopMutex};
        m_eventLoop = &loop;
    }

    setConnectionState(Connecting);

    // async call
    m_connection->connectToTarget(loop);

    auto res = loop.exec();

    setConnectionState(NotActive);

    // clear loop
    {
        QMutexLocker locker{&m_eventLoopMutex};
        m_eventLoop.clear();
    }

    // wait for exit
    if (res != GtEventLoop::Success)
    {
        gtWarningId(GT_EXEC_ID) << tr("Failed to connect to the Process Runner!");
        return false;
    }
    return true;
}

bool
GtProcessRunnerTransceiver::startProcessRunner(GtTask& task)
{
    // update localhost flag
    if (!m_isLocalhost)
    {
        // blocking call
        QHostAddress host = GtHostInfo::lookupHost(
                                m_connection->targetAddress().address);
        if (host.isNull())
        {
            gtWarningId(GT_EXEC_ID)
                    << tr("Failed to resolve whether target address is "
                          "pointing to localhost!");
        }
        m_isLocalhost = host.isLoopback();
    }

    // nothing to do here
    if (!m_autostart)
    {
        gtDebugId(GT_EXEC_ID) << tr("Process Runner must be started manually!");
        return true;
    }

    // autostart currently only supported on localhost
    if (!m_isLocalhost)
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Starting the Process Runner on a remote machine is "
                      "currently not supported! Please start the process "
                      "runner manually via the Console Application.");
        return false;
    }

    // find avialable port
    quint16 port{};
    try
    {
        port = findAvailablePort();
    }
    catch (GTlabException const& /*e*/)
    {
        return false;
    }

    // update port
    auto address = m_connection->targetAddress();
    address.port = port;
    m_connection->setTargetAddress(address);

    // process args
    QStringList args{
        QStringLiteral("process_runner"),
        QStringLiteral("--port=%1").arg(port)
    };
    // append dev mode
    if (gtApp->devMode())
    {
        args << QStringLiteral("--dev");
    }

    // build process call
    QProcess process;

    QDir appDir = qApp->applicationDirPath();
    process.setProgram(appDir.absoluteFilePath(QStringLiteral("GTlabConsole")));
    process.setArguments(args);

    // log file
    QDir tempDir = task.tempDir();
    if (tempDir.path().isEmpty())
    {
        tempDir = gtApp->applicationTempDir();
    }

    QString logfile =
            tempDir.absoluteFilePath(QStringLiteral("process_runner.log"));
    process.setStandardOutputFile(logfile);
    process.setStandardErrorFile(logfile);

    gtDebugId(GT_EXEC_ID) <<tr("Logfile is located at %1").arg(logfile);

    // start detached
    qint64 pid;
    if (!process.startDetached(&pid))
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Failed to start the Process Runner!")
                << tr("Error:") << process.errorString();
        return false;
    }
    return true;
}

void
GtProcessRunnerTransceiver::setConnectionState(GtProcessRunnerTransceiver::ConnectionState state)
{
    m_connectionState = state;
    emit connectionStateChanged(state);
}

namespace
{

bool
terminateProcessRunner(QString const& sessionId,
                       QString const& address,
                       quint16 port,
                       int timeout)
{
    static auto msg = QObject::tr("Failed to terminate the Process Runner, "
                                  "it may still be running!");

    // defer execution
    QThread::currentThread()->msleep(100);

    GtTcpSocket socket;
    GtEventLoop loop{timeout};

    // connect
    socket.connectTo(loop, address, port);

    if (loop.exec() != GtEventLoop::Success)
    {
        // assuming no previous connection could be established either
        gtWarningId(GT_EXEC_ID)
                << msg << QObject::tr("(Connection refused)");
        return false;
    }

    // abort command
    GtProcessRunnerAbortCommand cmd;
    cmd.setSessionId(sessionId);

    // event loop to wait for disconnect
    GtEventLoop waitForDisconnect{timeout};
    waitForDisconnect.connectSuccess(&socket, &GtTcpSocket::disconnected);

    // write command
    loop.clear();
    socket.write(loop, cmd.toMemento().toByteArray(),
                 GtTcpSocket::PrependDataSize);

    // wait until written
    if (loop.exec() != GtEventLoop::Success)
    {
        gtErrorId(GT_EXEC_ID)
                << msg << QObject::tr("(Sending Abort Command failed)");
        return false;
    }

    // wait until connection is lost
    if (waitForDisconnect.exec() != GtEventLoop::Success)
    {
        gtErrorId(GT_EXEC_ID)
                << msg << QObject::tr("(Timeout)");
        return false;
    }

    gtDebugId(GT_EXEC_ID) << "Successfully terminated process runner!";
    return true;
}

quint16 findAvailablePort() noexcept(false)
{
    using namespace gt::process_runner;

    // local copy of ports
    std::vector<quint16> ports{S_PORTS.cbegin(), S_PORTS.cend()};

    while (!ports.empty())
    {
        // randomly select a port
        size_t idx = QRandomGenerator::global()->bounded(
                         0, static_cast<int>(ports.size()));
        quint16 port = ports[idx];

        // check lock file
        QString lockFilePath = portLockFilePath(port);

        // lockfile
        QLockFile portLockFile{lockFilePath};
        // unlock file on exit
        auto finally = gt::finally(&portLockFile, &QLockFile::unlock);

        // we wont lock the file for long
        portLockFile.setStaleLockTime(1);

        // check if available
        if (portLockFile.tryLock())
        {
            return port;
        }

        gtDebugId(GT_EXEC_ID)
                << QObject::tr("Port %1 is already in use!").arg(port);
        // remove port
        ports.erase(std::begin(ports) + idx);
    }

    // error message
    gtErrorId(GT_EXEC_ID)
            << QObject::tr("All resvered ports are currently in use (%1-%2). "
                           "If you think this is an error, remove the "
                           "lock files manually (%3)")
                        .arg(S_DEFAULT_PORT)
                        .arg(S_DEFAULT_PORT + S_PORT_RANGE - 1)
                        .arg(portLockFilePath(S_DEFAULT_PORT));

    throw GTlabException{"ProcessRunner::findAvailabePort",
                         "No port available!"};
}

QString
projectPath(bool isLocalhost)
{
    if (isLocalhost)
    {
        auto* project = gtApp->currentProject();
        assert(project);
        return project->path();
    }

    QString path = gtEnvironment->value(S_ENV_PROJECT_PATH).toString();

    if (path.isEmpty())
    {
        gtWarningId(GT_EXEC_ID)
                << QObject::tr("Invalid project path for remote execution!");
    }

    return path;
}

} // namespace
