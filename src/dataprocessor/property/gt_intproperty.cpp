/* GTlab - Gas Turbine laboratory
 * Source File: gt_intproperty.cpp
 * copyright 2009-2015 by DLR
 *
 *  Created on: 14.10.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include "gt_intproperty.h"

GtIntProperty::GtIntProperty(const QString& ident, const QString& name,
                             const QString& brief)
{
    setObjectName(name);

    m_id = ident;
    m_brief = brief;
    m_unitCategory = GtUnit::Category::None;
    m_value = 0;
    m_initValue = 0;
    m_boundsCheckFlagHi = false;
    m_boundsCheckFlagLow = false;
    m_boundHi = 0;
    m_boundLo = 0;
}

GtIntProperty::GtIntProperty(const QString& ident, const QString& name) :
    GtIntProperty(ident, name, QString())
{

}

GtIntProperty::GtIntProperty(const QString& ident,
                             const QString& name,
                             const QString& brief,
                             const GtUnit::Category &unitCategory,
                             const int& value)
{
    setObjectName(name);

    m_id = ident;
    m_brief = brief;
    m_unitCategory = unitCategory;
    m_value = value;
    m_initValue = value;
    m_boundsCheckFlagHi = false;
    m_boundsCheckFlagLow = false;
    m_boundHi = 0;
    m_boundLo = 0;
}


GtIntProperty::GtIntProperty(const QString& ident,
                             const QString& name,
                             const QString& brief,
                             const GtUnit::Category &unitCategory,
                             const int lowSideBoundary,
                             const int highSideBoundary,
                             const int& value)
{
    setObjectName(name);

    m_id = ident;
    m_brief = brief;
    m_unitCategory = unitCategory;
    m_value = 0;
    m_initValue = 0;

    if (lowSideBoundary >= highSideBoundary)
    {
        m_boundsCheckFlagHi = false;
        m_boundsCheckFlagLow = false;
        m_boundHi = 0.0;
        m_boundLo = 0.0;
    }
    else
    {
        m_boundsCheckFlagLow = true;
        m_boundsCheckFlagHi = true;
        m_boundLo = lowSideBoundary;
        m_boundHi = highSideBoundary;
    }

    bool success = false;

    setVal(value, &success);

    if (success)
    {
        m_initValue = value;
    }
}

GtIntProperty::GtIntProperty(const QString& ident,
                             const QString& name,
                             const QString& brief,
                             const GtUnit::Category& unitCategory,
                             GtIntProperty::BoundType boundType,
                             const int boundary,
                             const int& value)
{
    setObjectName(name);

    m_id = ident;
    m_brief = brief;
    m_unitCategory = unitCategory;
    m_value = 0;
    m_initValue = 0;

    if (boundType == GtIntProperty::BoundLow)
    {
        m_boundsCheckFlagLow = true;
        m_boundsCheckFlagHi = false;
        m_boundLo = boundary;
        m_boundHi = 0;
    }
    else
    {
        m_boundsCheckFlagLow = false;
        m_boundsCheckFlagHi = true;
        m_boundLo = 0;
        m_boundHi = boundary;
    }

    bool success = false;

    setVal(value, &success);

    if (success)
    {
        m_initValue = value;
    }
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
                        << tr(". Value has to be bigger than")
                        << m_boundLo;
            return false;
        }
    }

    if (m_boundsCheckFlagHi)
    {
        if (value > m_boundHi)
        {
            gtWarning() << tr("Invalid value for ") << objectName()
                        << tr(". Value has to be lower than")
                        << m_boundHi;
            return false;
        }
    }

    return true;
}

gt::PropertyFactoryFunction
gt::makeIntProperty(int value)
{
    return makePropertyFactory<GtIntProperty>(std::move(value));
}
