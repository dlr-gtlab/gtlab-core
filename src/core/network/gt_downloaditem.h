/* GTlab - Gas Turbine laboratory
 * Source File: gt_downloaditem.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 16.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_DOWNLOADITEM_H
#define GT_DOWNLOADITEM_H

#include <QObject>

#include <QUrl>

/**
 * @brief The GtDownloadItem class
 */
class GtDownloadItem : public QObject
{
    Q_OBJECT

    friend class GtDownloader;

public:
    /**
     * @brief url
     * @return
     */
    const QUrl& url() const;

    /**
     * @brief path
     * @return
     */
    const QString& path() const;

    /**
     * @brief targetFileName
     * @return
     */
    const QString& targetFileName() const;

private:
    /// Url.
    QUrl m_url;

    /// Target path.
    QString m_path;

    /// Target filename.
    QString m_targetFileName;

    /**
     * @brief GtDownloadItem
     * @param parent
     */
    explicit GtDownloadItem(QObject* parent = nullptr);

};

#endif // GT_DOWNLOADITEM_H
