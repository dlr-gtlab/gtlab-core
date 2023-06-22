/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_downloaditem.h
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
