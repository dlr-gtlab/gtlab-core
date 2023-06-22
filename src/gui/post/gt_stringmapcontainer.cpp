/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "gt_stringmapcontainer.h"

#include <gt_logging.h>

GtStringMapContainer::GtStringMapContainer()
{

}

QStringList
GtStringMapContainer::keys() const
{
    return m_keys;
}

void
GtStringMapContainer::setKeys(const QStringList& keys)
{
    m_keys = keys;
}

QStringList
GtStringMapContainer::values() const
{
    return m_values;
}

void
GtStringMapContainer::setValues(const QStringList& values)
{
    m_values = values;
}

void
GtStringMapContainer::addEntry(const QString& key, const QString& value)
{
    if (!key.isEmpty())
    {
        m_keys.append(key);
    }

    if (!value.isEmpty())
    {
        m_values.append(value);
    }

    if (!isValid())
    {
        gtWarning() << tr("GtStringMapContainer: Sizes of keys and"
                          "values do not match!") << objectName();
    }
}

bool
GtStringMapContainer::isValid()
{
    if (m_keys.size() != m_values.size())
    {
        gtWarning() << "string map container "
                    << ", m_keys.size() = " << m_keys.size()
                    << ", m_values.size() = " << m_values.size()
                    << ", m_keys = " << m_keys
                    << ", m_values = " << m_values;
        return false;
    }

    return true;
}

QMap<QString, QString>
GtStringMapContainer::stringMap()
{
    QMap<QString, QString> retval;

    if (!isValid())
    {
        gtWarning() << tr("Sizes of keys and values do not match!");

        return retval;
    }

    for (int i = 0; i < m_keys.size(); i++)
    {
        retval.insert(m_keys.at(i), m_values.at(i));
    }

    return retval;
}

void
GtStringMapContainer::clear()
{
    m_keys.clear();
    m_values.clear();
}

void
GtStringMapContainer::assignValuesFrom(GtStringMapContainer* from)
{
    m_keys = from->keys();
    m_values = from->values();
}
