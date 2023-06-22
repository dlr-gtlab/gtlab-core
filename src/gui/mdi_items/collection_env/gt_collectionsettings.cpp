/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_collectionsettings.cpp
 *
 *  Created on: 07.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_localcollectionwidget.h"
#include "gt_browserwidget.h"
#include "gt_collectionnetworkitem.h"

#include "gt_collectionsettings.h"

GtCollectionSettings::GtCollectionSettings()
{

}

QWidget*
GtCollectionSettings::localCollectionWidget(QWidget* parent)
{
    return new GtLocalCollectionWidget(parent);
}

void
GtCollectionSettings::setLocalCollectionData(QWidget* widget,
                                         QList<GtCollectionItem> items)
{
    GtLocalCollectionWidget* wid =
            qobject_cast<GtLocalCollectionWidget*>(widget);

    if (!wid)
    {
        return;
    }

    wid->setItems(std::move(items));
}

GtAbstractBrowserWidget*
GtCollectionSettings::browserWidget(QWidget* parent)
{
    return new GtBrowserWidget(parent);
}

void
GtCollectionSettings::setBrowserWidgetData(
        GtAbstractBrowserWidget* widget,
        const QList<GtCollectionNetworkItem>& installedItems,
        const QList<GtCollectionNetworkItem>& availableItems,
        const QList<GtCollectionNetworkItem>& updataAvailableItems)
{
    GtBrowserWidget* wid = qobject_cast<GtBrowserWidget*>(widget);

    if (!wid)
    {
        return;
    }

    wid->setItems(installedItems, availableItems, updataAvailableItems);
}
