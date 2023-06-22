/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 16.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTQUEUEDMDIEVENT_H
#define GTQUEUEDMDIEVENT_H

#include <QObject>

class GtMdiItem;

/**
 * @brief The GtQueuedMdiEvent class
 */
class GtQueuedMdiEvent : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief GtQueuedMdiEvent
     */
    explicit GtQueuedMdiEvent(GtMdiItem* item) { m_mdiItem = item; }

    /**
     * @brief handle
     */
    virtual void handle() = 0;

protected:
    GtMdiItem* mdiItem() { return m_mdiItem; }

private:
    GtMdiItem* m_mdiItem;

};

#endif // GTQUEUEDMDIEVENT_H
