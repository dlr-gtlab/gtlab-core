/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 16.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOBJECTCHANGEDEVENT_H
#define GTOBJECTCHANGEDEVENT_H

#include <QPointer>

#include "gt_queuedmdievent.h"

class GtObject;

class GtObjectChangedEvent : public GtQueuedMdiEvent
{
    Q_OBJECT

public:
    GtObjectChangedEvent(GtMdiItem* item, GtObject* obj);

    /**
     * @brief handle
     */
    void handle() override;

private:
    QPointer<GtObject> m_obj;

};

#endif // GTOBJECTCHANGEDEVENT_H
