/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 16.12.2015
 *  Author: Richard Becker (AT-TW)
 *  Tel.: +49 2203 601 3493
 */

#include "gt_tablevalues.h"

QString
GtTableValues::description() const
{
    return m_brief;
}


void
GtTableValues::setDescription(const QString &description)
{
    m_brief = description;
}


QString
GtTableValues::unit() const
{
    return m_unit;
}


void
GtTableValues::setUnit(const QString &unit)
{
    m_unit = unit;
}


const QVector<double>&
GtTableValues::values() const
{
    return m_vals;
}


void
GtTableValues::setValues(const QVector<double> &values)
{
    m_vals = values;
}


void
GtTableValues::scale(double factor)
{
    for (int i = 0; i < m_vals.size(); ++i)
    {
        m_vals[i] = m_vals[i] * factor;
    }
}

void
GtTableValues::clearValues()
{
    m_vals.clear();
}

void
GtTableValues::prependValue(const double& val)
{
    m_vals.prepend(val);
}

void
GtTableValues::appendValue(const double& val)
{
    m_vals.append(val);
}

void
GtTableValues::insertValue(int index, const double& val)
{
    m_vals.insert(index, val);
}

void
GtTableValues::replaceValue(int index, const double& val)
{
    if (m_vals.size() > index)
    {
        m_vals[index] = val;
    }
}


