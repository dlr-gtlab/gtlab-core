/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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

