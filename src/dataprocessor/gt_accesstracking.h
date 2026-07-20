#pragma once

#include "gt_datamodel_exports.h"

#include <QMap>
#include <QStack>
#include <QSet>


struct GT_DATAMODEL_EXPORT GtAccessTracker
{
    struct AccessContext
    {
        QString uuid;
        QSet<QString> childContextUuid;
        QSet<QString> accessedObjects;
    };


    static GtAccessTracker & instance()
    {
        static GtAccessTracker s;
        return s;
    }

    GtAccessTracker(const GtAccessTracker &) = delete;
    GtAccessTracker & operator = (const GtAccessTracker &) = delete;

public:
    QSet<QString>
    getAccessedObjects(const QString& contextUuid);
    QSet<QString>
    getChildContextUuid(const QString& contextUuid);
    void
    addAccessedProperty(const QString& uuid);
    bool
    clearContext(const QString& contextUuid);
    void
    startAccessTracking(const QString& contextUuid);
    void
    endAccessTracking();
private:
    QStack<QString> m_activeStack;
    QMap<QString, AccessContext> m_contexts;


    GtAccessTracker() {}
    ~GtAccessTracker() {}

};
