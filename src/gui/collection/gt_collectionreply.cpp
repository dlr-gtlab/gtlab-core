/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectionreply.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 12.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_logging.h"
#include "gt_collectionhelper.h"

#include "gt_collectionreply.h"

GtCollectionReply::~GtCollectionReply()
{
    //gtFatal() << "#### collection reply deleted!";
}

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
    gtDebug() << "helper finished!";
    GtCollectionHelper* helper = qobject_cast<GtCollectionHelper*>(sender());

    if (!helper)
    {
        // TODO error
        emit finished();
        return;
    }

    gtDebug() << "  |-> " << helper->collectionItems().size() <<
                 "items loaded!";

    helper->deleteLater();

    emit finished();
}
