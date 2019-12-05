/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectionbrowsermodelitem.cpp
 * copyright 2009-2017 by DLR
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
