/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 16.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECTCHANGEDEVENT_H
#define GTPROJECTCHANGEDEVENT_H

#include <QPointer>

#include "gt_queuedmdievent.h"

class GtProject;

class GtProjectChangedEvent : public GtQueuedMdiEvent
{
    Q_OBJECT

public:
    GtProjectChangedEvent(GtMdiItem* item, GtProject* project);

    /**
     * @brief handle
     */
    void handle() override;

private:
    QPointer<GtProject> m_project;

};

#endif // GTPROJECTCHANGEDEVENT_H
