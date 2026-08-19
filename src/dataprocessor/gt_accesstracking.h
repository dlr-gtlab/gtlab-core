/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_runnable.cpp
 *
 *  Created on: 19.08.2026
 *  Author: Robert Marcenko (AT-TWK)
 */
#pragma once

#include "gt_datamodel_exports.h"

#include <QMap>
#include <QStack>
#include <QSet>
#include <quuid.h>


struct GT_DATAMODEL_EXPORT GtAccessTracker
{
    struct AccessContext
    {
        QSet<QUuid> childContextUuid;
        QSet<QUuid> accessedObjects;
    };


    static GtAccessTracker & instance()
    {
        static GtAccessTracker s;
        return s;
    }

    GtAccessTracker(const GtAccessTracker &) = delete;
    GtAccessTracker & operator = (const GtAccessTracker &) = delete;

public:
    QSet<QUuid>
    getAccessedObjects(const QUuid& contextUuid);
    QSet<QUuid> getChildContextUuid(const QUuid& contextUuid);
    void
    addAccessedProperty(const QUuid& uuid);
    bool
    clearContext(const QUuid& contextUuid);
    void
    startAccessTracking(const QUuid& contextUuid);
    void
    endAccessTracking();
    void pause(bool state);

private:
    QStack<QUuid> m_activeStack;
    QMap<QUuid, AccessContext> m_contexts;
    bool m_paused{false};
    GtAccessTracker() {}
    ~GtAccessTracker() {}
};
