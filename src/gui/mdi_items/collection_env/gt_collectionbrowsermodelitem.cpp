/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_collectionbrowsermodelitem.cpp
 *
 *  Created on: 16.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_collectionbrowsermodelitem.h"

GtCollectionBrowserModelItem::GtCollectionBrowserModelItem(
        const GtCollectionNetworkItem& item) : m_item(item), m_selected(false)
{

}

GtCollectionNetworkItem
GtCollectionBrowserModelItem::item() const
{
    return m_item;
}

bool
GtCollectionBrowserModelItem::isSelected() const
{
    return m_selected;
}

void
GtCollectionBrowserModelItem::setSelected(bool selected)
{
    m_selected = selected;
}
