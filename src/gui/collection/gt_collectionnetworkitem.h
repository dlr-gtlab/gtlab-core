/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_collectionnetworkitem.h
 *
 *  Created on: 15.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_COLLECTIONNETWORKITEM_H
#define GT_COLLECTIONNETWORKITEM_H

#include "gt_gui_exports.h"

#include <QUrl>

#include "gt_collectionitem.h"

/**
 * @brief The GtCollectionNetworkItem class
 */
class GT_GUI_EXPORT GtCollectionNetworkItem : public GtCollectionItem
{
    friend class GtCollectionLoader;

public:
    /**
     * @brief Constructor.
     */
    GtCollectionNetworkItem();

    /**
     * @brief installedVersion
     * @return
     */
    double installedVersion() const;

    /**
     * @brief url
     * @return
     */
    QUrl url() const;

private:
    /// Installed version.
    double m_installedVersion;

    /// Network access data to collection item
    QUrl m_url;

};

#endif // GT_COLLECTIONNETWORKITEM_H
