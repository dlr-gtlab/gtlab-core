/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 16.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_projectchangedevent.h"
#include "gt_mdiitem.h"
#include "gt_project.h"

GtProjectChangedEvent::GtProjectChangedEvent(GtMdiItem* item,
                                             GtProject* project) :
    GtQueuedMdiEvent(item), m_project(project)
{

}

void
GtProjectChangedEvent::handle()
{
    mdiItem()->projectChangedEvent(m_project);
}

