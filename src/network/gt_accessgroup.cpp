/* GTlab - Gas Turbine laboratory
 * Source File: gt_accessgroup.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 24.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_accessgroup.h"

GtAccessGroup::GtAccessGroup(const QString& id, const QMetaObject& connection,
                             QObject* parent) :
    QObject(parent),
    m_connectionMetaData(connection)
{
    setObjectName(id);
}

bool
GtAccessGroup::addAccessData(const QString& host,int port,
                       const QString& user, const QString& pw)
{
    if (host.isEmpty())
    {
        return false;
    }

    if (!hostExists(host))
    {
        m_data << GtAccessData(host, port, user, pw);
        return true;
    }

    return false;
}

const QList<GtAccessData>&
GtAccessGroup::accessData()
{
    return m_data;
}

int
GtAccessGroup::numberOfAccessData() const
{
    return m_data.size();
}

bool
GtAccessGroup::hostExists(const QString& host)
{
    foreach (GtAccessData data, m_data)
    {
        if (data.host() == host)
        {
            return true;
        }
    }

    return false;
}

bool
GtAccessGroup::removeAccessData(int index)
{
    if (index < 0 || index >= m_data.size())
    {
        return false;
    }

    m_data.removeAt(index);

    return true;
}

bool
GtAccessGroup::setAccessData(const GtAccessData& data, int index)
{
    if (index < 0 || index >= m_data.size())
    {
        return false;
    }

    m_data[index] = data;

    return true;
}

const QMetaObject&
GtAccessGroup::connectionMetaData()
{
    return m_connectionMetaData;
}
