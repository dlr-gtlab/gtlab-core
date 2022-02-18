/* GTlab - Gas Turbine laboratory
 * Source File: gt_versionnumber.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 05.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include <QVersionNumber>

#include "gt_versionnumber.h"

class GtVersionNumberImpl
{
public:
    /// version information
    QVersionNumber m_vers;

    /// additional version information (e.g. alpha, rc, etc.)
    QString m_additional;
};

GtVersionNumber::GtVersionNumber(int major, int minor, int patch,
                                 const QString& additional)
{
    m_pimpl = new GtVersionNumberImpl;
    m_pimpl->m_vers = QVersionNumber(major, minor, patch);
    m_pimpl->m_additional = additional;
}

GtVersionNumber::GtVersionNumber(int major, int minor,
                                 const QString& additional)
{
    m_pimpl = new GtVersionNumberImpl;
    m_pimpl->m_vers = QVersionNumber(major, minor);
    m_pimpl->m_additional = additional;
}

GtVersionNumber::GtVersionNumber(int major, const QString& additional)
{
    m_pimpl = new GtVersionNumberImpl;
    m_pimpl->m_vers = QVersionNumber(major);
    m_pimpl->m_additional = additional;
}

GtVersionNumber::GtVersionNumber()
{
    m_pimpl = new GtVersionNumberImpl;
}

GtVersionNumber::GtVersionNumber(const QString& versStr)
{
    m_pimpl = new GtVersionNumberImpl;

    int suffixIndex;
    m_pimpl->m_vers = QVersionNumber::fromString(versStr, &suffixIndex);

    if (suffixIndex < versStr.length())
    {
        m_pimpl->m_additional =
                versStr.right(versStr.length() - suffixIndex - 1);
    }
}

GtVersionNumber::GtVersionNumber(const GtVersionNumber& v2)
{
    m_pimpl = new GtVersionNumberImpl;
    m_pimpl->m_vers = v2.m_pimpl->m_vers;
    m_pimpl->m_additional = v2.m_pimpl->m_additional;
}

GtVersionNumber&
GtVersionNumber::operator =(const GtVersionNumber& v2)
{
    if (&v2 != this)
    {
        delete m_pimpl;
        m_pimpl = new GtVersionNumberImpl;
        m_pimpl->m_vers = v2.m_pimpl->m_vers;
        m_pimpl->m_additional = v2.m_pimpl->m_additional;
    }

    return *this;
}

GtVersionNumber::~GtVersionNumber()
{
    delete m_pimpl;
}

QString
GtVersionNumber::toString() const
{
    QString retval = m_pimpl->m_vers.toString();

    if (!m_pimpl->m_additional.isEmpty())
    {
        retval = retval + QStringLiteral("-") + m_pimpl->m_additional;
    }

    return retval;
}

bool
GtVersionNumber::isNull() const
{
    return m_pimpl->m_vers.isNull();
}

int
GtVersionNumber::compare(const GtVersionNumber& v1,
                         const GtVersionNumber& v2)
{
    return QVersionNumber::compare(v1.m_pimpl->m_vers,
                                   v2.m_pimpl->m_vers);
}

bool
operator==(const GtVersionNumber&v1, const GtVersionNumber& v2)
{
    return (GtVersionNumber::compare(v1, v2) == 0);
}

bool
operator!=(const GtVersionNumber& v1, const GtVersionNumber& v2)
{
    return (GtVersionNumber::compare(v1, v2) != 0);
}

bool
operator>(const GtVersionNumber& v1, const GtVersionNumber& v2)
{
    return (GtVersionNumber::compare(v1, v2) > 0);
}

bool
operator<(const GtVersionNumber& v1, const GtVersionNumber& v2)
{
    return (GtVersionNumber::compare(v1, v2) < 0);
}

bool
operator>=(const GtVersionNumber& v1, const GtVersionNumber& v2)
{
    return (GtVersionNumber::compare(v1, v2) >= 0);
}

bool
operator<=(const GtVersionNumber& v1, const GtVersionNumber& v2)
{
    return (GtVersionNumber::compare(v1, v2) <= 0);
}
