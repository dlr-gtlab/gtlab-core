/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_collectionnetworkitem.cpp
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
