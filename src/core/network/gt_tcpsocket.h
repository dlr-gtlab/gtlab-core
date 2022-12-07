/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 *  Created on: 11.08.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef GTTCPSOCKET_H
#define GTTCPSOCKET_H

#include <QPointer>
#include <QObject>
#include <memory>

#include "gt_core_exports.h"

class QTcpSocket;
class GtEventLoop;
/**
 * @brief The GtTcpSocket class
 * Light wrapper class for the QTcpSocket.
 *
 * Wrapps the methods for connecting and writing to a socket by using the
 * GtEventLoop for more control, i.e. to make it blocking and
 * aborting/unblocking the call when desired.
 *
 * Implements a simple method to check for a valid connection.
 */
class GT_CORE_EXPORT GtTcpSocket : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief The DataFormat enum.
     * Indicates the format in which data should be written
     */
    enum DataFormat
    {
        RawData,        // Write the data as is
        PrependDataSize // preprend the length of the data to the data itself
    };

    /**
     * @brief Constructor
     * @param parent Parent bject
     **/
    explicit GtTcpSocket(QObject* parent = {});

    /**
     * @brief Returns whether the socket has a connection
     * @return has connection
     */
    bool hasConnection() const;

    /**
     * @brief Connects to the address specified (either ip or hostname).
     * Non blocking call
     * @param loop Event loop to wait for success or failure
     * @param address Address to connecto to
     * @param port Port to connect to
     */
    void connectTo(GtEventLoop& loop, const QString& address, quint16 port);

    /**
     * @brief CTakes ownership of the scket and connects to it.
     * Non blocking call
     * @param loop Socket Existing socket to connect to
     */
    void connectTo(GtEventLoop& loop, std::unique_ptr<QTcpSocket> socket);

    /**
     * @brief Writes the data to the socket. Non blocking call
     * @param loop Event loop to wait for success or failure
     * @param data Data to write
     * @param format Format to use
     */
    void write(GtEventLoop& loop, const QByteArray& data, DataFormat format={});

    /**
     * @brief Access to the socket
     */
    const QPointer<QTcpSocket>& socket() const { return m_socket; }

    /**
     * @brief Closes the socket
     */
    void close();

    /**
     * @brief Attempts to read all data
     * @return All data read
     */
    QByteArray readAll();

signals:

    /**
     * @brief Emitted once a connection has been established
     */
    void connected();

    /**
     * @brief Emitted ince a connection has been closed
     */
    void disconnected();

    /**
     * @brief Emitted once data has been written
     * @param bytes Bytes written
     */
    void bytesWritten(qint64 bytes);

    /**
     * @brief Emitted once data can be read
     */
    void readyRead();

private:

    QPointer<QTcpSocket> m_socket{};

    /**
     * @brief Helper function to setup connections
     */
    void setupConnections() const;
};

#endif // GTTCPSOCKET_H
