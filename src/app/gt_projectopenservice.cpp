/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_projectopenservice.h"

#include <QCryptographicHash>
#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalSocket>
#include <QThread>
#include <QTimer>
#include <QtEndian>

#include <utility>

namespace
{

    constexpr quint32 maxMessageSize = 1024 * 1024;
    constexpr int maxConnectionAttempts = 4;
    constexpr int connectionTimeoutMs = 200;
    constexpr int acknowledgementTimeoutMs = 600;
    constexpr int retryDelayMs = 100;

    QString serviceName()
    {
        QFileInfo executable(QCoreApplication::applicationFilePath());
        QString installation = executable.canonicalFilePath();
        if (installation.isEmpty())
        {
            installation = executable.absoluteFilePath();
        }

        const QByteArray identity = installation.toUtf8() + '\n' +
                                    QDir::homePath().toUtf8() +
                                    "\nproject-open-v1";
        const QByteArray digest =
            QCryptographicHash::hash(identity, QCryptographicHash::Sha256)
                .toHex();
        return QStringLiteral("gtlab-project-open-%1")
            .arg(QString::fromLatin1(digest));
    }

    QByteArray encodeFrame(const QJsonObject& object)
    {
        const QByteArray payload =
            QJsonDocument(object).toJson(QJsonDocument::Compact);
        QByteArray frame;
        const quint32 payloadSize =
            qToBigEndian(static_cast<quint32>(payload.size()));
        frame.append(reinterpret_cast<const char*>(&payloadSize),
                     sizeof(payloadSize));
        frame.append(payload);
        return frame;
    }

    bool decodeFrame(QByteArray& buffer, QByteArray& payload)
    {
        if (buffer.size() < static_cast<int>(sizeof(quint32)))
        {
            return false;
        }

        const quint32 payloadSize = qFromBigEndian<quint32>(
            reinterpret_cast<const uchar*>(buffer.constData()));
        if (payloadSize > maxMessageSize)
        {
            return false;
        }

        const int frameSize = static_cast<int>(sizeof(quint32) + payloadSize);
        if (buffer.size() < frameSize)
        {
            return false;
        }

        payload = buffer.mid(static_cast<int>(sizeof(quint32)),
                             static_cast<int>(payloadSize));
        buffer.remove(0, frameSize);
        return true;
    }

    bool writeFrame(QLocalSocket& socket, const QJsonObject& object)
    {
        const QByteArray frame = encodeFrame(object);
        return socket.write(frame) == frame.size() &&
               socket.waitForBytesWritten(acknowledgementTimeoutMs);
    }

    bool readFrame(QLocalSocket& socket, QByteArray& payload)
    {
        QByteArray buffer;
        QElapsedTimer timer;
        timer.start();

        while (timer.elapsed() < acknowledgementTimeoutMs)
        {
            buffer.append(socket.readAll());
            if (buffer.size() >= static_cast<int>(sizeof(quint32)))
            {
                const quint32 payloadSize = qFromBigEndian<quint32>(
                    reinterpret_cast<const uchar*>(buffer.constData()));
                if (payloadSize > maxMessageSize)
                {
                    return false;
                }

                const int frameSize =
                    static_cast<int>(sizeof(quint32) + payloadSize);
                if (buffer.size() >= frameSize)
                {
                    payload = buffer.mid(static_cast<int>(sizeof(quint32)),
                                         static_cast<int>(payloadSize));
                    return true;
                }
            }

            const int remaining =
                acknowledgementTimeoutMs - static_cast<int>(timer.elapsed());
            if (remaining <= 0 || !socket.waitForReadyRead(remaining))
            {
                return false;
            }
        }

        return false;
    }

} // namespace

GtProjectOpenService::GtProjectOpenService(QObject* parent) :
    QObject(parent), m_serverName(serviceName()),
    m_lockFile(QDir(QDir::tempPath())
                   .filePath(m_serverName + QStringLiteral(".lock"))),
    m_server()
{
    m_lockFile.setStaleLockTime(0);

    connect(&m_server, &QLocalServer::newConnection, this,
            [this]() { acceptConnections(); });
}

bool
GtProjectOpenService::forwardProjectOpen(const QString& projectFilePath) const
{
    for (int attempt = 0; attempt < maxConnectionAttempts; ++attempt)
    {
        QLocalSocket socket;
        socket.connectToServer(m_serverName);
        if (socket.waitForConnected(connectionTimeoutMs) &&
            writeFrame(socket,
                       {{QStringLiteral("type"), QStringLiteral("OpenProject")},
                        {QStringLiteral("path"), projectFilePath}}))
        {
            QByteArray payload;
            if (readFrame(socket, payload))
            {
                const QJsonObject acknowledgement =
                    QJsonDocument::fromJson(payload).object();
                if (acknowledgement.value(QStringLiteral("accepted")).toBool())
                {
                    return true;
                }
            }
        }

        if (attempt + 1 < maxConnectionAttempts)
        {
            QThread::msleep(retryDelayMs);
        }
    }

    return false;
}

bool
GtProjectOpenService::startServer()
{
    if (!m_lockFile.tryLock(0))
    {
        return false;
    }

    // Another process may have registered the endpoint while this process
    // was waiting for the ownership lock.
    if (isServiceReachable())
    {
        m_lockFile.unlock();
        return false;
    }

    m_server.setSocketOptions(QLocalServer::UserAccessOption);
    if (m_server.listen(m_serverName))
    {
        return true;
    }

    // The endpoint can be stale after a crash. Only its lock owner may remove
    // it, and only after verifying that no service is reachable.
    if (isServiceReachable())
    {
        m_lockFile.unlock();
        return false;
    }

    QLocalServer::removeServer(m_serverName);
    if (!m_server.listen(m_serverName))
    {
        m_lockFile.unlock();
        return false;
    }

    return true;
}

void
GtProjectOpenService::enqueueProjectOpen(const QString& projectFilePath)
{
    m_pendingPaths.enqueue(projectFilePath);
    if (m_ready)
    {
        QTimer::singleShot(0, this, [this]() { processPendingRequests(); });
    }
}

void
GtProjectOpenService::setProjectOpenHandler(
    std::function<void(const QString&)> handler)
{
    m_projectOpenHandler = std::move(handler);
}

void
GtProjectOpenService::setReady()
{
    m_ready = true;
    QTimer::singleShot(0, this, [this]() { processPendingRequests(); });
}

void
GtProjectOpenService::acceptConnections()
{
    while (m_server.hasPendingConnections())
    {
        QLocalSocket* socket = m_server.nextPendingConnection();
        if (!socket)
        {
            continue;
        }

        connect(socket, &QLocalSocket::disconnected, socket,
                &QObject::deleteLater);
        connect(socket, &QLocalSocket::readyRead, this, [this, socket]() {
            QByteArray buffer =
                socket->property("projectOpenBuffer").toByteArray();
            buffer.append(socket->readAll());

            QByteArray payload;
            if (!decodeFrame(buffer, payload))
            {
                if (buffer.size() >= static_cast<int>(sizeof(quint32)))
                {
                    const quint32 payloadSize = qFromBigEndian<quint32>(
                        reinterpret_cast<const uchar*>(buffer.constData()));
                    if (payloadSize > maxMessageSize)
                    {
                        writeFrame(*socket,
                                   {{QStringLiteral("accepted"), false}});
                        socket->disconnectFromServer();
                        return;
                    }
                }
                socket->setProperty("projectOpenBuffer", buffer);
                return;
            }

            const QJsonObject request =
                QJsonDocument::fromJson(payload).object();
            const QString path =
                request.value(QStringLiteral("path")).toString();
            const bool accepted =
                request.value(QStringLiteral("type")).toString() ==
                    QStringLiteral("OpenProject") &&
                !path.isEmpty();
            if (accepted)
            {
                // Queue before acknowledging so the sender can safely exit.
                enqueueProjectOpen(path);
            }

            writeFrame(*socket, {{QStringLiteral("accepted"), accepted}});
            socket->disconnectFromServer();
        });
    }
}

void
GtProjectOpenService::processPendingRequests()
{
    if (!m_ready || !m_projectOpenHandler)
    {
        return;
    }

    while (!m_pendingPaths.isEmpty())
    {
        m_projectOpenHandler(m_pendingPaths.dequeue());
    }
}

bool
GtProjectOpenService::isServiceReachable() const
{
    QLocalSocket socket;
    socket.connectToServer(m_serverName);
    return socket.waitForConnected(connectionTimeoutMs);
}
