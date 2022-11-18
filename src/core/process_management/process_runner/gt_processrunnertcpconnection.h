/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 29.8.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTPROCESSRUNNERTCPCONNECTION_H
#define GTPROCESSRUNNERTCPCONNECTION_H

#include "gt_processrunnerconnectionstrategy.h"
#include "gt_tcpsocket.h"

#include <QTimer>

/**
 * @brief The GtProcessRunnerTcpConnection class
 * TCP Connection strategy
 */
class GT_CORE_EXPORT GtProcessRunnerTcpConnection :
        public GtProcessRunnerConnectionStrategy
{
    Q_OBJECT

public:

    /**
     * @brief Constructor
     * @param parent Parent object
     */
    GtProcessRunnerTcpConnection(QObject* parent = {});

    /**
     * @brief Writes the data to the tcp connection. Non blocking. Use event
     * loop to make this call blocking
     * @param loop Event loop to setup
     * @param data Data to write
     */
    void writeData(GtEventLoop& loop, const QByteArray& data) override;

    /**
     * @brief Connects to the tcp socket. Blocking call
     * @param socket Socket to connect to
     */
    void connectTo(std::unique_ptr<QTcpSocket> socket);

    /**
     * @brief Returns whether there is a valid tcp connetion
     * @return
     */
    bool hasConnection() const override;

    void close() override;

protected:

    /**
     * @brief Connects to the address specified. Non blocking. Use event
     * loop to make this call blocking
     * @param loop Event loop to setup
     * @param address Target address
     * @param port Target port
     */
    void connectTo(GtEventLoop& loop,
                   const QString& address,
                   quint16 port) override;

private:

    /// Pointer to socket (owned by this)
    QPointer<GtTcpSocket> m_socket;

    /// Buffer for reading incompleete packages
    QByteArray m_buffer{};

    /// Timeout timer for reading incomplete packages
    QTimer m_readTimer;

signals:

    /**
     * @brief Emitted once the read timeout timer should be cleared
     */
    void clearReadTimeout();

private slots:

    /**
     * @brief Reads the incoming data.
     */
    void onRead();

    /**
     * @brief Handles read timeout.
     */
    void onReadTimeout();
};

#endif // GTPROCESSRUNNERTCPCONNECTION_H
