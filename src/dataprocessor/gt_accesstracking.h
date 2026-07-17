#pragma once

#include <gt_version.h>
#include "gt_datamodel_exports.h"

#include <QMap>
#include <QStack>
#include <QSet>
#include "gt_object.h"

class GT_DATAMODEL_EXPORT GtAbstractRecorder
{
public:

    GtAbstractRecorder();

    virtual bool
    initLinkedObjects(QList<QPointer<GtObject> > linkedObjects)=0;

    virtual bool
    recordChanges()=0;

    virtual bool
    recordAccessObjects()=0;

};

struct GT_DATAMODEL_EXPORT GtAccessTracker
{
    struct AccessContext
    {
        QString uuid;
        QSet<QString> childContextUuid;
        QSet<QString> accessedObjects;
        QString startTime;
        QString endTime;
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
