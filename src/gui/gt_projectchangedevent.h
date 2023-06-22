/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
