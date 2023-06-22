/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_downloader.cpp
 *
 *  Created on: 16.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDir>

#include "gt_downloaditem.h"
#include "gt_accessmanager.h"
#include "gt_logging.h"

#include "gt_downloader.h"

GtDownloader::GtDownloader(QObject* parent) : QObject(parent),
    m_current(nullptr), m_requestAborted(false), m_numberOfFiles(0)
{

}

void
GtDownloader::addFileDownload(const QString& url, const QString& downloadPath,
                              const QString& targetFileName)
{
    auto downloadItem = new GtDownloadItem(this);

    downloadItem->m_url = QUrl(url);
    downloadItem->m_path = downloadPath;
    downloadItem->m_targetFileName = targetFileName;

    m_queue << downloadItem;
}

void
GtDownloader::addFileDownload(const QUrl& url, const QString& downloadPath,
                              const QString& targetFileName)
{
    auto downloadItem = new GtDownloadItem(this);

    downloadItem->m_url = url;
    downloadItem->m_path = downloadPath;
    downloadItem->m_targetFileName = targetFileName;

    m_queue << downloadItem;
}

int
GtDownloader::queueSize() const
{
    return m_queue.size();
}

void
GtDownloader::exec()
{
    if (m_current)
    {
        gtError() << tr("Could not start download proccess!") <<
                  QStringLiteral(" ") << tr("Download already running!");
        return;
    }

    m_numberOfFiles = m_queue.size();

    QString msg = QString::number(m_queue.size()) + QStringLiteral(" ") +
                  tr("items(s) in queue.");
    emit message(msg);

    loadNextItem();
}

void
GtDownloader::loadNextItem()
{
    emit queueChanged(m_numberOfFiles - m_queue.size(), m_numberOfFiles);

    if (m_queue.isEmpty())
    {
        gtInfo() << tr("All files downloaded!");
        emit message(tr("All files downloaded!"));
        emit finished();
        return;
    }

    if (m_requestAborted)
    {
        gtInfo() << tr("Download process aborted!");
        qDeleteAll(m_queue);
        m_queue.clear();
        m_current = nullptr;
        return;
    }

    m_current = m_queue.takeFirst();

    emit message(tr("downloading ") + m_current->url().toDisplayString() +
                 QStringLiteral("..."));

    QNetworkReply* reply =
        gtAccessManager->qnam()->get(QNetworkRequest(m_current->url()));

    connect(reply, SIGNAL(finished()), SLOT(onReplyFinished()));
}

void
GtDownloader::onReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply)
    {
        gtError() << tr("Invalid download reply!");
        return;
    }

    if (reply->error() || !m_current)
    {
        gtError() << tr("Could not download data!");
    }
    else
    {
        QDir dir(m_current->path());

        if (!dir.exists())
        {
            if (!dir.mkpath(m_current->path()))
            {
                gtError() << tr("Could not create directory!");
            }
        }

        QString filename = m_current->path() + QDir::separator()
                           + m_current->targetFileName();

        if (QFile::exists(filename))
        {
            gtWarning() << "...overwriting...";
            QFile::remove(filename);
        }

        QFile file(filename);

        if (!file.open(QIODevice::WriteOnly))
        {
            gtError() << tr("Could not save file!");
            gtError() << "  |-> " << filename;
        }
        else
        {
            file.write(reply->readAll());
            file.flush();
            file.close();
            gtInfo() << tr("done!");
            emit message(tr("done!"));
        }
    }

    delete m_current;
    m_current = nullptr;

    reply->deleteLater();

    loadNextItem();
}

void
GtDownloader::cancelDownload()
{
    if (!m_current)
    {
        gtInfo() << tr("Download process not active!");
        return;
    }

    m_requestAborted = true;
}
