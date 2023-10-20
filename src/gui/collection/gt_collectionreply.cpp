/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_collectionreply.cpp
 *
 *  Created on: 12.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_logging.h"
#include "gt_collectionhelper.h"

#include "gt_collectionreply.h"

QList<GtCollectionNetworkItem>
GtCollectionReply::installedItems() const
{
    return m_installedItems;
}

QList<GtCollectionNetworkItem>
GtCollectionReply::avilableItems() const
{
    return m_availableItems;
}

QList<GtCollectionNetworkItem>
GtCollectionReply::updateAvailableItems() const
{
    return m_updateAvailableItems;
}

GtCollectionReply::GtCollectionReply(QObject* parent) : QObject(parent)
{

}

void
GtCollectionReply::replyReady()
{
    emit finished();
}

void
GtCollectionReply::onHelperFinished()
{
    gtDebug().medium() << tr("helper finished!");
    GtCollectionHelper* helper = qobject_cast<GtCollectionHelper*>(sender());

    if (!helper)
    {
        // TODO error
        emit finished();
        return;
    }

    gtDebug() << "  |-> " << helper->collectionItems().size()
              << tr("items loaded!");

    helper->deleteLater();

    emit finished();
}
