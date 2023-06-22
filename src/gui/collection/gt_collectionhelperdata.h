/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_collectionhelperdata.h
 *
 *  Created on: 16.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_COLLECTIONHELPERDATA_H
#define GT_COLLECTIONHELPERDATA_H

#include <QUrl>
#include <QJsonObject>

/**
 * @brief The GtCollectionHelperData class
 */
class GtCollectionHelperData
{
public:
    /**
     * @brief GtCollectionHelperData
     */
    GtCollectionHelperData(const QUrl& url, const QJsonObject& jsonObj);

    /**
     * @brief url
     * @return
     */
    QUrl url() const;

    /**
     * @brief data
     * @return
     */
    QJsonObject data() const;

private:
    /// Url.
    QUrl m_url;

    /// Collection item data.
    QJsonObject m_data;

};

#endif // GT_COLLECTIONHELPERDATA_H
