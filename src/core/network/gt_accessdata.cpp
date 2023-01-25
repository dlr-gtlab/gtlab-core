/* GTlab - Gas Turbine laboratory
 * Source File: gt_accessdata.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 17.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QUrl>

#include "gt_accessdata.h"
#include "gt_utilities.h"

GtAccessData::GtAccessData(const QString& host, int port,
                           const QString& user, const QString& pw) :
    m_host(host),
    m_port(port),
    m_user(user),
    m_pw(pw)
{
    if (!gt::checkNumericalLimits<quint16>(port))
    {
        m_port = -1;
    }
}

GtAccessData::GtAccessData(const QString& host, int port) :
    GtAccessData(host, port, {}, {})
{

}

GtAccessData::GtAccessData(const QString& host, const QString& user,
                           const QString& pw) :
    GtAccessData(host, -1, user, pw)
{

}

GtAccessData::GtAccessData(const QString& host) :
    GtAccessData(host, -1, {}, {})
{

}

const QString&
GtAccessData::host() const
{
    return m_host;
}

int
GtAccessData::port() const
{
    return m_port;
}

const QString&
GtAccessData::user() const
{
    return m_user;
}

const QString&
GtAccessData::password() const
{
    return m_pw;
}

QUrl
GtAccessData::data() const
{
    QUrl retval(m_host);

    if (m_port >= 0)
    {
        retval.setPort(m_port);
    }

    if (!m_user.isEmpty())
    {
        retval.setUserName(m_user);
    }

    if (!m_pw.isEmpty())
    {
        retval.setPassword(m_pw);
    }

    return retval;
}

bool
GtAccessData::isEmpty() const
{
    return m_host.isEmpty();
}
