/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectionhelperdata.cpp
 * copyright 2009-2017 by DLR
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
