/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_collectionreply.h
 *
 *  Created on: 12.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_COLLECTIONREPLY_H
#define GT_COLLECTIONREPLY_H

#include <QObject>
#include <QMutex>

#include "gt_collectionnetworkitem.h"

class GtCollectionLoader;

/**
 * @brief The GtCollectionReply class
 */
class GtCollectionReply : public QObject
{
    Q_OBJECT

    friend class GtCollectionLoader;

public:
    /**
     * @brief installedItems
     * @return
     */
    QList<GtCollectionNetworkItem> installedItems() const;

    /**
     * @brief avilableItems
     * @return
     */
    QList<GtCollectionNetworkItem> avilableItems() const;

    /**
     * @brief updateAvailableItems
     * @return
     */
    QList<GtCollectionNetworkItem> updateAvailableItems() const;

private:
    // Installed collection items.
    QList<GtCollectionNetworkItem> m_installedItems;

    // Collection items available for installation.
    QList<GtCollectionNetworkItem> m_availableItems;

    // Collection items with update available.
    QList<GtCollectionNetworkItem> m_updateAvailableItems;

    /// Mutex.
    QMutex m_mutex;

    /**
     * @brief GtCollectionReply
     * @param parent
     */
    explicit GtCollectionReply(QObject* parent = nullptr);

    /**
     * @brief replyReady
     */
    void replyReady();

private slots:
    /**
     * @brief onHelperFinished
     */
    void onHelperFinished();

signals:
    /**
     * @brief finished
     */
    void finished();

};

#endif // GT_COLLECTIONREPLY_H
