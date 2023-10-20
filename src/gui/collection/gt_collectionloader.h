/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_collectionloader.h
 *
 *  Created on: 09.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_COLLECTIONLOADER_H
#define GT_COLLECTIONLOADER_H

#include "gt_gui_exports.h"

#include <QObject>

class QUrl;
class GtCollectionItem;
class GtCollectionNetworkItem;
class GtCollectionInterface;
class GtCollectionReply;
class GtAccessData;

/**
 * @brief The GtCollectionLoader class
 */
class GT_GUI_EXPORT GtCollectionLoader : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Parent object.
     */
    explicit GtCollectionLoader(const QString &collectionId, QObject *parent = nullptr);

    /**
     * @brief Loads local collection items based on given collection.
     * @return List of collection items.
     */
    QList<GtCollectionItem> loadLocalCollection();

    /**
     * @brief loadNetworkCollection
     * @param accessDataList
     * @return
     */
    GtCollectionReply* loadNetworkCollection(
            const QList<GtAccessData>& accessDataList);

    /**
     * @brief downloadCollectionItems
     * @param items
     */
    void downloadCollectionItems(const QList<GtCollectionNetworkItem>& items);

private:
    /// Collection pointer.
    GtCollectionInterface* m_collection;

    /**
     * @brief Returns collection path based on given collection. If path
     * does not exists, a new directory will be created. If path could not
     * be created, empty string  will be returned.
     * @return Collection path.
     */
    QString collectionPath();

    /**
     * @brief loadLocalCollectionItem
     * @param path
     * @return
     */
    GtCollectionItem loadLocalCollectionItem(const QString& path);

    /**
     * @brief readItemInformation
     * @param json
     * @param item
     */
    GtCollectionItem readItemInformation(const QJsonObject& json);

    /**
     * @brief readNetworkItemInformation
     * @param url
     * @param json
     * @return
     */
    GtCollectionNetworkItem readNetworkItemInformation(const QUrl& url,
                                                       const QJsonObject& json);

    /**
     * @brief itemIsValid
     * @param json
     * @return
     */
    bool itemIsValid(const QJsonObject& json);

    /**
     * @brief sortItems
     * @param items
     * @param installedItems
     * @param availableItems
     * @param updateAvailableItems
     */
    void sortItems(const QList<GtCollectionNetworkItem>& items,
                   QList<GtCollectionNetworkItem>& installedItems,
                   QList<GtCollectionNetworkItem>& availableItems,
                   QList<GtCollectionNetworkItem>& updateAvailableItems);

private slots:
    /**
     * @brief onHelperFinished
     */
    void onHelperFinished();

};

#endif // GT_COLLECTIONLOADER_H
