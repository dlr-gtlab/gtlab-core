/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectionbrowsermodelitem.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 16.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_COLLECTIONBROWSERMODELITEM_H
#define GT_COLLECTIONBROWSERMODELITEM_H

#include "gt_collectionnetworkitem.h"

/**
 * @brief The GtCollectionBrowserModelItem class
 */
class GtCollectionBrowserModelItem
{
public:
    /**
     * @brief GtCollectionBrowserModelItem
     * @param item
     */
    explicit GtCollectionBrowserModelItem(const GtCollectionNetworkItem& item);

    /**
     * @brief item
     * @return
     */
    GtCollectionNetworkItem item() const;

    /**
     * @brief isSelected
     * @return
     */
    bool isSelected() const;

    /**
     * @brief setSelected
     * @param selected
     */
    void setSelected(bool selected);

private:
    /// Collection network item.
    GtCollectionNetworkItem m_item;

    /// Item selected indicator
    bool m_selected;

};

#endif // GT_COLLECTIONBROWSERMODELITEM_H
