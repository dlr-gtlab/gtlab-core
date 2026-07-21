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
private:
    QStack<QUuid> m_activeStack;
    QMap<QUuid, AccessContext> m_contexts;

    GtAccessTracker() {}
    ~GtAccessTracker() {}
};
