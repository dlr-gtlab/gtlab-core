/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_collectionhelperdata.cpp
 *
 *  Created on: 16.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_collectionhelperdata.h"

GtCollectionHelperData::GtCollectionHelperData(const QUrl& url,
                                               const QJsonObject& jsonObj) :
    m_url(url), m_data(jsonObj)
{

}

QUrl
GtCollectionHelperData::url() const
{
    return m_url;
}

QJsonObject
GtCollectionHelperData::data() const
{
    return m_data;
}
