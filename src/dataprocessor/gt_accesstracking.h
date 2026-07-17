#pragma once

#include <gt_version.h>
#include "gt_datamodel_exports.h"

#include <QMap>
#include <QStack>
#include <QSet>
#include <QObject>
#include "gt_object.h"
#include <qdatetime.h>



struct GT_DATAMODEL_EXPORT AccessList
{
    struct AccessContext
    {
        QString uuid;
        QString parentUuid;
        QSet<QString> accessedObjects;
        QDateTime startTime;
        QDateTime endTime;
        bool isActive;

    };


    static AccessList & instance()
    {
        static AccessList s;
        return s;
    }

    AccessList(const AccessList &) = delete;
    AccessList & operator = (const AccessList &) = delete;

public:
    QStringList getList(QString & uuid);
    void clearList();
    void tracking(bool state);
    void addAccessedProperty(QString uuid);
    void
    startAccessTracking(GtObject& object);
    void
    endAccessTracking();
private:
    QStringList m_accessList{};
    bool m_tracking=false;

    QStack<QString> m_activeStack;                 // Current nesting path
    QMap<QString, AccessContext> m_contexts;       // All contexts by uuid


    AccessList() {}
    ~AccessList() {}

};
