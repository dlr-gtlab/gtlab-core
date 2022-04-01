/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectionhelper.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 13.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_COLLECTIONHELPER_H
#define GT_COLLECTIONHELPER_H

#include <QObject>

#include <QJsonObject>
#include <QMutex>

#include "gt_accessdata.h"
#include "gt_collectionhelperdata.h"

class QNetworkAccessManager;
class GtCollectionItem;
class GtCollectionReply;

/**
 * @brief The GtCollectionHelper class
 */
class GtCollectionHelper : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param List of access data objects.
     * @param Pointer to collection reply.
     * @param Parent object.
     */
    GtCollectionHelper(const QList<GtAccessData>& accessDataList,
                       GtCollectionReply* reply,
                       QObject* parent = nullptr);

    /**
     * @brief Returns loaded collection items.
     * @return List of loaded collection items.
     */
    QList<GtCollectionHelperData> collectionItems() const;

    /**
     * @brief Returns pointer to collection reply.
     * @return Collection reply pointer.
     */
    GtCollectionReply* reply();

public slots:
    /**
     * @brief Executes download process of collection items.
     */
    void exec();

private:
    /// Host queue.
    QList<GtAccessData> m_hostQueue;

    /// List of loaded collection items.
    QList<GtCollectionHelperData> m_items;

    /// Network access manager.
    QNetworkAccessManager* m_qnam;

    /// Item queue.
    QList<QUrl> m_itemQueue;

    /// Mutex.
    QMutex m_mutex;

    /// Collection reply
    GtCollectionReply* m_reply;

    /**
     * @brief readAccessData
     * @param accessData
     */
    void readAccessData(const GtAccessData& accessData);

    /**
     * @brief processListData
     * @param byteArray
     * @param url
     */
    void processListData(const QByteArray& byteArray, QUrl url);

    /**
     * @brief processItemData
     * @param url
     * @param byteArray
     */
    void processItemData(const QUrl& url, const QByteArray& byteArray);

private slots:
    /**
     * @brief onListFetched
     */
    void onListFetched();

    /**
     * @brief onItemDataLoaded
     */
    void onItemDataLoaded();

    /**
     * @brief loadNextItem
     */
    void loadNextItem();

signals:
    /**
     * @brief finished
     */
    void finished();

    /**
     * @brief allHostsLoaded
     */
    void accessDataFinished();

};

#endif // GT_COLLECTIONHELPER_H
