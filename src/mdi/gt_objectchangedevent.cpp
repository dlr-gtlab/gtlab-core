/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 16.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_objectchangedevent.h"
#include "gt_mdiitem.h"
#include "gt_object.h"

GtObjectChangedEvent::GtObjectChangedEvent(GtMdiItem* item, GtObject* obj) :
    GtQueuedMdiEvent(item), m_obj(obj)
{

}

void
GtObjectChangedEvent::handle()
{
    mdiItem()->objectChangedEvent(m_obj);
}

