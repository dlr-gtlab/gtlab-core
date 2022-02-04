/* GTlab - Gas Turbine laboratory
 * Source File: gt_downloader.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 16.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GT_DOWNLOADER_H
#define GT_DOWNLOADER_H

#include "gt_core_exports.h"

#include <QObject>

class GtDownloadItem;

class GT_CORE_EXPORT GtDownloader : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief GtDownloader
     * @param parent
     */
    explicit GtDownloader(QObject* parent = Q_NULLPTR);

    /**
     * @brief addFileDownload
     * @param url
     * @param downloadPath
     * @param targetFileName
     */
    void addFileDownload(const QString& url, const QString& downloadPath,
                         const QString& targetFileName = QString());

    /**
     * @brief GtDownloadItem
     * @param url
     * @param downloadPath
     * @param targetFileName
     */
    void addFileDownload(const QUrl& url, const QString& downloadPath,
                         const QString& targetFileName = QString());

    /**
     * @brief queueSize
     * @return
     */
    int queueSize();

public slots:
    /**
     * @brief exec
     */
    void exec();

    /**
     * @brief cancelDownload
     */
    void cancelDownload();

private:
    /// Queue.
    QList<GtDownloadItem*> m_queue;

    /// Current download item.
    GtDownloadItem* m_current;

    /// Request aborted flag.
    bool m_requestAborted;

    /// Number of files to download.
    int m_numberOfFiles;

    /**
     * @brief loadNextItem
     */
    void loadNextItem();

private slots:
    /**
     * @brief httpFinished
     */
    void onReplyFinished();

signals:
    /**
     * @brief finished
     */
    void finished();

    /**
     * @brief message
     * @param str
     */
    void message(QString str);

    /**
     * @brief queueChanged
     * @param finished
     * @param total
     */
    void queueChanged(int finished, int total);

};

#endif // GT_DOWNLOADER_H
