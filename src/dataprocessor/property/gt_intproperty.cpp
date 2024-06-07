/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_intproperty.cpp
 *
 *  Created on: 14.10.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include "gt_intproperty.h"

GtIntProperty::GtIntProperty(const QString& ident,
                             const QString& name) :
    GtIntProperty(ident, name, name)
{ }

GtIntProperty::GtIntProperty(const QString& ident,
                             const QString& name,
                             const QString& brief) :
    GtIntProperty(ident, name, brief, 0)
{ }

GtIntProperty::GtIntProperty(const QString& ident,
                             const QString& name,
                             const QString& brief,
                             int value) :
    m_boundsCheckFlagLow(false),
    m_boundsCheckFlagHi(false),
    m_boundLo(std::numeric_limits<int>::min()),
    m_boundHi(std::numeric_limits<int>::max())
{
    setObjectName(name);

    m_id = ident;
    m_brief = brief;
    m_unitCategory = GtUnit::Category::NonDimensional;
    m_value = value;
    m_initValue = value;
}

GtIntProperty::GtIntProperty(const QString& ident,
                             const QString& name,
                             const QString& brief,
                             int lowSideBoundary,
                             int highSideBoundary,
                             int value) :
    GtIntProperty(ident, name, brief, gt::clamp(value, lowSideBoundary, highSideBoundary))
{
    if (lowSideBoundary < highSideBoundary)
    {
        m_boundsCheckFlagLow = true;
        m_boundsCheckFlagHi = true;
        m_boundLo = lowSideBoundary;
        m_boundHi = highSideBoundary;
    }
}

GtIntProperty::GtIntProperty(const QString& ident,
                             const QString& name,
                             const QString& brief,
                             BoundType boundType,
                             int boundary,
                             int value):
    GtIntProperty(ident, name, brief, value)
{
    switch(boundType)
    {
    case BoundLow:
        m_boundsCheckFlagLow = true;
        m_boundLo = boundary;
        break;
    case BoundHigh:
        m_boundsCheckFlagHi = true;
        m_boundHi = boundary;
        break;
    }

    m_value = gt::clamp(value, m_boundLo, m_boundHi);
    m_initValue = m_value;
}

QVariant
GtIntProperty::valueToVariant(const QString& unit,
                              bool* success) const
{
    return getVal(unit, success);
}

bool
GtIntProperty::setValueFromVariant(const QVariant& val,
                                   const QString& unit)
{
    bool ok = false;
    const int intVal = val.toInt(&ok);

    if (!ok) return false;

    setVal(intVal, unit, &ok);

    return ok;
}

int
GtIntProperty::lowSideBoundary() const
{
    return m_boundLo;
}

int
GtIntProperty::highSideBoundary() const
{
    return m_boundHi;
}

bool
GtIntProperty::validateValue(const int& value)
{
    if (m_boundsCheckFlagLow)
    {
        if (value < m_boundLo)
        {
            gtWarning() << tr("Invalid value for ") << objectName()
                        << tr(". Value has to be bigger or equal than")
                        << m_boundLo;
            return false;
        }
    }

    if (m_boundsCheckFlagHi)
    {
        if (value > m_boundHi)
        {
            gtWarning() << tr("Invalid value for ") << objectName()
                        << tr(". Value has to be lower or equal than")
                        << m_boundHi;
            return false;
        }
    }

    return true;
}

gt::PropertyFactoryFunction
gt::makeIntProperty(int value)
{
    return makePropertyFactory<GtIntProperty>(value);
}

gt::PropertyFactoryFunction
gt::makeIntProperty(GtIntProperty::BoundType boundaryType, int boundary, int value)
{
    return [=](QString const& id){
        return new GtIntProperty(id, id, QString{}, boundaryType, boundary, value);
    };
}

gt::PropertyFactoryFunction
gt::makeIntProperty(int lowSideBoundary, int highSideBoundary, int value)
{
    return [=](QString const& id){
        return new GtIntProperty(id, id, QString{}, lowSideBoundary, highSideBoundary, value);
    };
}
