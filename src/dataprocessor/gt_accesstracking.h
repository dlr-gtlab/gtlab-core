#pragma once

#include <gt_version.h>
#include "gt_datamodel_exports.h"

#include <QObject>



struct GT_DATAMODEL_EXPORT AccessList
{

    static AccessList & instance()
    {
        static AccessList s;
        return s;
    }

    AccessList(const AccessList &) = delete;
    AccessList & operator = (const AccessList &) = delete;

public:
    QStringList getList();
    void clearList();
    void tracking(bool state);
    void addAccessedProperty(QString uuid);
private:
    QStringList m_accessList{};
    bool m_tracking=false;

    AccessList() {}
    ~AccessList() {}

};
