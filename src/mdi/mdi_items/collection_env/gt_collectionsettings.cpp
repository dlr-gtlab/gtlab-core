/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectionsettings.cpp
 * copyright 2009-2017 by DLR
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

    if (wid == Q_NULLPTR)
    {
        return;
    }

    wid->setItems(items);
}

GtAbstractBrowserWidget*
GtCollectionSettings::browserWidget(QWidget* parent)
{
    return new GtBrowserWidget(parent);
}

void
GtCollectionSettings::setBrowserWidgetData(
        GtAbstractBrowserWidget* widget,
        QList<GtCollectionNetworkItem> installedItems,
        QList<GtCollectionNetworkItem> availableItems,
        QList<GtCollectionNetworkItem> updataAvailableItems)
{
    GtBrowserWidget* wid = qobject_cast<GtBrowserWidget*>(widget);

    if (wid == Q_NULLPTR)
    {
        return;
    }

    wid->setItems(installedItems, availableItems, updataAvailableItems);
}
