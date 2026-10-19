/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTPROJECTOPENSERVICE_H
#define GTPROJECTOPENSERVICE_H

#include <functional>

#include <QLocalServer>
#include <QLockFile>
#include <QObject>
#include <QQueue>

class GtProjectOpenService : public QObject
{
public:
    explicit GtProjectOpenService(QObject* parent = nullptr);

    /**
     * @brief Forwards a project-open request to the current service owner.
     * @param projectFilePath Project file path to open.
     * @return True only after the receiver acknowledges that it queued the request.
     */
    bool forwardProjectOpen(const QString& projectFilePath) const;

    /**
     * @brief Tries to become the local project-open service owner.
     * @return Whether this instance owns and is listening on the service.
     */
    bool startServer();

    /**
     * @brief Queues a project path for opening after the GUI is ready.
     * @param projectFilePath Project file path to open.
     */
    void enqueueProjectOpen(const QString& projectFilePath);

    /**
     * @brief Sets the handler used to open queued project paths.
     * @param handler Project-open callback.
     */
    void setProjectOpenHandler(std::function<void(const QString&)> handler);

    /**
     * @brief Allows queued requests to be processed after startup completes.
     */
    void setReady();

private:
    void acceptConnections();
    void processPendingRequests();
    bool isServiceReachable() const;

    QString m_serverName;
    QLockFile m_lockFile;
    QLocalServer m_server;
    QQueue<QString> m_pendingPaths;
    std::function<void(const QString&)> m_projectOpenHandler;
    bool m_ready{false};
};

#endif // GTPROJECTOPENSERVICE_H
