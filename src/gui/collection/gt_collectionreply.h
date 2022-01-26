/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectionreply.h
 * copyright 2009-2017 by DLR
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
     * @brief ~GtCollectionReply
     */
    virtual ~GtCollectionReply();

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
    GtCollectionReply(QObject* parent = Q_NULLPTR);

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
