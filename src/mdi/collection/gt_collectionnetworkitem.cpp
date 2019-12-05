/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectionnetworkitem.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 15.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_collectionnetworkitem.h"

GtCollectionNetworkItem::GtCollectionNetworkItem() :
    m_installedVersion(0.)
{

}

double
GtCollectionNetworkItem::installedVersion() const
{
    return m_installedVersion;
}

QUrl
GtCollectionNetworkItem::url() const
{
    return m_url;
}
