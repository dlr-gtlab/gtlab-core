/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 29.8.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTPROCESSRUNNERCONNECTIONSTRATEGY_H
#define GTPROCESSRUNNERCONNECTIONSTRATEGY_H

#include "gt_eventloop.h"
#include "gt_core_exports.h"
#include "gt_processrunnerglobals.h"

#include <memory>
#include <QQueue>
#include <QMutex>
#include <QObject>

class GtObject;
class GtProcessRunnerResponse;
class GtProcessRunnerCommand;
class GtProcessRunnerNotification;
/**
 * @brief The GtProcessRunnerConnectionStrategy class
 * Abstract interface for the process runner connection.
 */
class GT_CORE_EXPORT GtProcessRunnerConnectionStrategy : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Method to implement to write the data to the connection.
     * Should be non-blocking.
     * @param loop Event loop to setup, which may be used to make this call
     * blocking.
     * @param data Data to write
     */
    virtual void writeData(GtEventLoop& loop, const QByteArray& data) = 0;

    /**
     * @brief Returns whether a connection has been established
     * @return has connection
     */
    virtual bool hasConnection() const = 0;

    /**
     * @brief Closes the connection (i.e. disconnects)
     */
    virtual void close() = 0;

    /**
     * @brief Connects to the target address set. Non-blocking.
     * @param loop Event loop to setup, can be used to make call blocking.
     */
    void connectToTarget(GtEventLoop& loop);

    /**
     * @brief Returns the target address set.
     * @return target address
     */
    gt::process_runner::ConnectionAddress const& targetAddress() const;

    /**
     * @brief Sets the target address
     * @param address Address to set
     */
    void setTargetAddress(gt::process_runner::ConnectionAddress address);

    /**
     * @brief Whether the connection has pending responses
     * @return has responses
     */
    bool hasResponse() const;

    /**
     * @brief Whether the connection has pending commands
     * @return has commands
     */
    bool hasCommand() const;

    /**
     * @brief Whether the connection has pending notifications
     * @return has notifications
     */
    bool hasNotification() const;

    /**
     * @brief Returns the next response. Null if no response was available.
     * @return next response
     */
    std::unique_ptr<GtProcessRunnerResponse> nextResponse();

    /**
     * @brief Returns the next command. Null if no command was available.
     * @return next command
     */
    std::unique_ptr<GtProcessRunnerCommand> nextCommand();

    /**
     * @brief Returns the next notification. Null if no notification was available.
     * @return next notification
     */
    std::unique_ptr<GtProcessRunnerNotification> nextNotification();

    /**
     * @brief Setups the event loop and wait for the signal specified.
     * Blocking call.
     * @param loop Event loop to setup
     * @param signal Success signal
     * @return Result
     */
    template <typename Signal>
    GtEventLoop::State waitFor(GtEventLoop& loop, Signal signal) const;

protected:

    /**
     * @brief Constructor
     * @param parent Parent object
     */
    GtProcessRunnerConnectionStrategy(QObject* parent = {});

    /**
     * @brief Method to implement for connecting to the address specified.
     * Should be non-blocking.
     * @param loop Event loop to setup, which may be used to make this call
     * blocking.
     * @param address Target address
     * @param port Target port
     */
    virtual void connectTo(GtEventLoop& loop,
                           const QString& address,
                           quint16 port) = 0;

    /**
     * @brief Checks and queues the object if its either a command, response or
     * notification. Emits a signal accordingly
     * @param obj
     */
    void queueData(std::unique_ptr<GtObject> obj);

signals:

    /**
     * @brief Emiited once a command was recieved
     */
    void recievedCommand();

    /**
     * @brief Emiited once a response was recieved
     */
    void recievedResponse();

    /**
     * @brief Emiited once a notification was recieved
     */
    void recievedNotification();

    /**
     * @brief Emiited once the connection was closed
     */
    void disconnected();

private:

    /// Queue for the responses
    QQueue<GtProcessRunnerResponse*> m_responses{};
    /// Queue for the commands
    QQueue<GtProcessRunnerCommand*> m_commands{};
    /// Queue for the notifications
    QQueue<GtProcessRunnerNotification*> m_notifications{};
    /// Mutex for appending new commands
    mutable QMutex m_mutex{};
    /// Data for the next/current connection
    gt::process_runner::ConnectionAddress m_targetAddress;

    /**
     * @brief Queues the response
     * @param o Object
     */
    void queueResponse(GtProcessRunnerResponse* o);

    /**
     * @brief Queues the command
     * @param o Object
     */
    void queueCommand(GtProcessRunnerCommand* o);

    /**
     * @brief Queues the notification
     * @param o Object
     */
    void queueNotification(GtProcessRunnerNotification* o);
};



template <typename Signal>
inline GtEventLoop::State
GtProcessRunnerConnectionStrategy::waitFor(GtEventLoop& loop,
                                           Signal signal) const
{
    loop.connectAbort(this, &GtProcessRunnerConnectionStrategy::disconnected);
    loop.connectSuccess(this, signal);
    return loop.exec();
}

#endif // GTPROCESSRUNNERCONNECTIONSTRATEGY_H
