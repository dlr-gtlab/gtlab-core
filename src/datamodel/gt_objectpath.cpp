/* GTlab - Gas Turbine laboratory
 * Source File: gt_objectpath.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 18.12.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_object.h"

#include "gt_objectpath.h"

GtObjectPath::GtObjectPath()
{

}

GtObjectPath::GtObjectPath(const QString& str)
{
    makePath(str);
}

GtObjectPath::GtObjectPath(GtObject* obj)
{
    makePath(obj->objectPath());
}

QChar
GtObjectPath::separator()
{
    return QChar(';');
}

GtObject*
GtObjectPath::getObject(GtObject* src) const
{
    if (src == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    QStringList list = toStringList();

    return src->getObjectByPath(list);
}

QString
GtObjectPath::toString() const
{
    if (m_data.isEmpty())
    {
        return QString();
    }

    QString retval = m_data.first();

    if (m_data.size() > 1)
    {
        for (int i = 1; i < m_data.size(); ++i)
        {
            retval = retval + GtObjectPath::separator() + m_data[i];
        }
    }

    return retval;
}

QStringList
GtObjectPath::toStringList() const
{
    return m_data;
}

bool
GtObjectPath::isEmpty() const
{
    return m_data.isEmpty();
}

void
GtObjectPath::makePath(const QString& str)
{
    // clear internal data
    m_data.clear();

    // fill internal data
    m_data = str.split(GtObjectPath::separator());
}
