/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 *  Created on: 11.08.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_tcpsocket.h"

#include <QTcpSocket>
#include <QHostAddress>

#include "gt_logging.h"
#include "gt_eventloop.h"

#include <QHostInfo>
#include <QHostAddress>

GtTcpSocket::GtTcpSocket(QObject* parent) :
    QObject{parent}
{
    setObjectName("GtTcpSocket");
}

bool
GtTcpSocket::hasConnection() const
{
    return m_socket && (m_socket->state() == QTcpSocket::ConnectedState);
}

void
GtTcpSocket::connectTo(GtEventLoop& loop,
                       const QString& address,
                       quint16 port)
{
    // delete the old socket
    if (m_socket)
    {
        m_socket->deleteLater();
    }
    // create a new socket (and take ownership by setting the parent)
    m_socket = new QTcpSocket{this};
    setupConnections();

    loop.connectSuccess(m_socket.data(), &QTcpSocket::connected);
    loop.connectAbort(m_socket.data(), &QTcpSocket::disconnected);

    QObject::connect(&loop, &GtEventLoop::failed,
                     this, [&](){
        if (m_socket && m_socket->error() != QTcpSocket::UnknownSocketError)
        {
            gtWarningId("TCP").nospace()
                    << tr("Error:") << " " << m_socket->errorString()
                    << " (" << m_socket->error() << ")";
        }
    }, Qt::QueuedConnection);

    gtDebugId("TCP").nospace()
        << "Connecting to " << address << ":" << port << "...";

    // this is a non blocking call
    m_socket->connectToHost(address, port);
}

void
GtTcpSocket::connectTo(GtEventLoop& loop, std::unique_ptr<QTcpSocket> socket)
{
    // delete the old socket
    if (m_socket)
    {
        m_socket->deleteLater();
    }

    gtDebugId("TCP").nospace()
            << "Connecting to '" << socket.get() << "'";

    if (!socket)
    {
        return emit loop.failed();
    }

    loop.connectSuccess(m_socket.data(), &QTcpSocket::connected);
    loop.connectAbort(m_socket.data(), &QTcpSocket::disconnected);

    // take ownership by setting the parent
    m_socket = socket.release();
    m_socket->setParent(this);
    setupConnections();

    // socket may already be connected
    if (hasConnection())
    {
        emit loop.success();
    }
}

void
GtTcpSocket::write(GtEventLoop& loop, const QByteArray& data, DataFormat format)
{
    if (!m_socket)
    {
        gtWarningId("TCP") << "Attempted to write to a null scoket!";
        return emit loop.failed();
    }
    loop.connectSuccess(socket().data(), &QTcpSocket::bytesWritten);
    loop.connectAbort(socket().data(), &QTcpSocket::disconnected);

    switch (format)
    {
    // preprend the data size to the data itself
    case PrependDataSize:
        m_socket->write(QByteArray::number(data.size()) + data);
        break;
    // by default write data as raw
    default:
        m_socket->write(data);
        break;
    }
}

void
GtTcpSocket::close()
{
    if (m_socket)
    {
        m_socket->close();
    }
}

QByteArray
GtTcpSocket::readAll()
{
    if (m_socket)
    {
        return m_socket->readAll();
    }
    return {};
}

void
GtTcpSocket::setupConnections() const
{
    if (!m_socket)
    {
        return;
    }

    QObject::connect(m_socket.data(), &QTcpSocket::connected, this, [](){
        gtDebugId("TCP").medium() << "Connected!";
    });
    QObject::connect(m_socket.data(), &QTcpSocket::connected,
                     this, &GtTcpSocket::connected);

    QObject::connect(m_socket.data(), &QTcpSocket::disconnected, this, [](){
        gtDebugId("TCP").medium() << "Disconnected!";
    });
    QObject::connect(m_socket.data(), &QTcpSocket::disconnected,
                     this, &GtTcpSocket::disconnected);

    QObject::connect(m_socket.data(), &QTcpSocket::bytesWritten,
                     this, &GtTcpSocket::bytesWritten);
    QObject::connect(m_socket.data(), &QTcpSocket::readyRead,
                     this, &GtTcpSocket::readyRead);
}
