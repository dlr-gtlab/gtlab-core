/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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
    void handle() Q_DECL_OVERRIDE;

private:
    QPointer<GtObject> m_obj;

};

#endif // GTOBJECTCHANGEDEVENT_H
