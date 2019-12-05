#include "gt_doublemapcontainer.h"
#include <QsLog.h>

GtDoubleMapContainer::GtDoubleMapContainer()
{

}

QStringList
GtDoubleMapContainer::keys() const
{
    return m_keys;
}

void
GtDoubleMapContainer::setKeys(const QStringList& keys)
{
    m_keys = keys;
}

QList<double>
GtDoubleMapContainer::values() const
{
    return m_values;
}

QStringList
GtDoubleMapContainer::valuesString() const
{
    QStringList retval;

    foreach (double val, m_values)
    {
        retval.append(QString::number(val));
    }

    return retval;
}

void
GtDoubleMapContainer::setValues(const QList<double>& values)
{
    m_values = values;
}

void
GtDoubleMapContainer::setValuesString(const QStringList& values)
{
    foreach (QString val, values)
    {
        m_values.append(val.toDouble());
    }
}

void
GtDoubleMapContainer::addEntry(QString key, double value)
{
    if (!key.isEmpty())
    {
        m_keys.append(key);
    }

    m_values.append(value);

    if (!isValid())
    {
        gtWarning() << tr("GtDoubleMapContainer: Sizes of keys and "
                          "values do not match!");
    }
}

bool
GtDoubleMapContainer::isValid()
{
    if (m_keys.size() != m_values.size())
    {
        gtWarning() << "double map container "
                    << ", m_keys.size() = " << m_keys.size()
                    << ", m_values.size() = " << m_values.size();
        return false;
    }

    return true;
}

QMap<QString, double>
GtDoubleMapContainer::doubleMap()
{
    QMap<QString, double> retval;

    if (!isValid())
    {
        gtWarning() << tr("Sizes of keys and values do not match!");

        return retval;
    }

    for (int i = 0; i < m_keys.size(); i++)
    {
        qDebug() << "m_keys.at(i) = " << m_keys.at(i) << ", m_values.at(i)"
                 << m_values.at(i);
        retval.insert(m_keys.at(i), m_values.at(i));
    }

    return retval;
}

void
GtDoubleMapContainer::clear()
{
    m_keys.clear();
    m_values.clear();
}

void
GtDoubleMapContainer::assignValuesFrom(GtDoubleMapContainer* from)
{
    m_keys = from->keys();
    m_values = from->values();
}
