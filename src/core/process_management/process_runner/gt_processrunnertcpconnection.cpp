/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 29.8.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_processrunnertcpconnection.h"

#include "gt_objectmemento.h"
#include "gt_utilities.h"
#include "gt_eventloop.h"
#include "gt_processrunnercommand.h"
#include "gt_processrunnerresponse.h"
#include "gt_processrunnercommandfactory.h"

#include <QTcpSocket>
#include <QRegularExpression>
#include <QDomDocument>

GtProcessRunnerTcpConnection::GtProcessRunnerTcpConnection(QObject* parent) :
    GtProcessRunnerConnectionStrategy(parent),
    m_socket{new GtTcpSocket{this}}
{
    setObjectName("ProcessRunnerTcpConnection");

    connect(m_socket.data(), &GtTcpSocket::readyRead,
            this, &GtProcessRunnerTcpConnection::onRead,
            Qt::QueuedConnection);
    connect(m_socket.data(), &GtTcpSocket::disconnected,
            this, &GtProcessRunnerTcpConnection::disconnected,
            Qt::QueuedConnection);

    m_readTimer.setSingleShot(true);
    m_readTimer.setInterval(5 * 1000); // timeout
    connect(m_socket.data(), SIGNAL(readyRead()), &m_readTimer, SLOT(start()));
    connect(this, SIGNAL(clearReadTimeout()), &m_readTimer, SLOT(stop()));
}

void
GtProcessRunnerTcpConnection::writeData(GtEventLoop& loop,
                                        const QByteArray& data)
{
    m_socket->write(loop, data, GtTcpSocket::PrependDataSize);
}

void
GtProcessRunnerTcpConnection::connectTo(GtEventLoop& loop,
                                        const QString& address,
                                        quint16 port)
{
    return m_socket->connectTo(loop, address, port);
}

void
GtProcessRunnerTcpConnection::connectTo(std::unique_ptr<QTcpSocket> socket)
{
    GtEventLoop loop{3 * 1000};
    m_socket->connectTo(loop, std::move(socket));
    loop.exec();
}

bool
GtProcessRunnerTcpConnection::hasConnection() const
{
    return m_socket->hasConnection();
}

void
GtProcessRunnerTcpConnection::close()
{
    m_socket->close();
}

void
GtProcessRunnerTcpConnection::onRead()
{
    static QRegularExpression regex{R"(^\d+)"};

    m_buffer += m_socket->readAll();

    if (m_buffer.isEmpty())
    {
        // all data must have been parsed successfully
        return emit clearReadTimeout();
    }

    // fetch size of data
    QRegularExpressionMatch match = regex.match(m_buffer);

    bool ok = false;
    auto captured = match.captured();
    int size = captured.toInt(&ok);

    if (!match.hasMatch() || !ok)
    {
        gtWarningId("TCP")
                << tr("Failed to parse data!")
                << gt::brackets(tr("Data size could not be parsed"));
        return m_buffer.clear();
    }

    // read data
    auto data = m_buffer.mid(captured.size(), size);
    if (data.size() != size)
    {
        gtDebugId("TCP")
                << tr("Incomplete data! (%1/%2)")
                       .arg(data.size()).arg(size);
        return;
    }

    // remove data from buffer
    m_buffer.remove(0, captured.size() + size);

    // data may not be complete yet
    QDomDocument doc;
    QString errMsg;
    int errRow = 0;
    int errCol = 0;

    // rudimental check if XML data is complete
    if (!doc.setContent(data, &errMsg, &errRow, &errCol))
    {
        gtWarningId("TCP")
                << tr("Parsing error: row (%1) col (%2) error (%3)")
                       .arg(errRow).arg(errCol).arg(errMsg);
        qWarning().noquote() << "TCP: Data:\n"   << data;
        qWarning().noquote() << "TCP: Buffer:\n" << m_buffer;
        return;
    }

    GtObjectMemento memento{doc.documentElement()};
    GtObject* obj = memento.restore(&gtProcessRunnerCommandFactory);

    queueData(std::unique_ptr<GtObject>(obj));

    // parse pending commands in buffer
    onRead();
}

void
GtProcessRunnerTcpConnection::onReadTimeout()
{
    gtErrorId("TCP") << tr("Reading incomplete data timed out!");
    m_buffer.clear();
}
