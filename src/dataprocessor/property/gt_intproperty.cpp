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

#include <algorithm>

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

GtIntProperty::GtIntProperty(const QString& ident,
                             const QString& name,
                             const QString& brief,
                             gt::Boundaries<int> bounds,
                             int value) :
    GtIntProperty(ident, name, brief)
{
    m_boundsCheckFlagHi = bounds.high().has_value();
    m_boundsCheckFlagLow = bounds.low().has_value();

    if (bounds.high())
    {
        m_boundHi = bounds.high().value();
    }

    if (bounds.low())
    {
        m_boundLo = bounds.low().value();
    }

    m_value = gt::clamp(value, m_boundLo, m_boundHi);
    m_initValue = m_value;
}

GtIntProperty::GtIntProperty(const QString& ident,
                             const QString& name,
                             const QString& brief,
                             const GtUnit::Category &unitCategory,
                             const int& value) :
    GtIntProperty(ident, name, brief, value)
{ }


GtIntProperty::GtIntProperty(const QString& ident,
                             const QString& name,
                             const QString& brief,
                             const GtUnit::Category &unitCategory,
                             const int lowSideBoundary,
                             const int highSideBoundary,
                             const int& value) :
    GtIntProperty(ident, name, brief,
                  gt::Boundaries<int>::makeNormalized(lowSideBoundary, highSideBoundary), 
                  value)
{ }

GtIntProperty::GtIntProperty(const QString& ident,
                             const QString& name,
                             const QString& brief,
                             const GtUnit::Category& unitCategory,
                             GtIntProperty::BoundType boundType,
                             const int boundary,
                             const int& value) :
    GtIntProperty(ident, name, brief,
        gt::Boundaries<int>::makeNormalized(
            boundType == BoundLow ? boundary : std::numeric_limits<int>::min(),
            boundType == BoundHigh ? boundary : std::numeric_limits<int>::max()),
        value)
{ 

}

void
GtIntProperty::operator+=(int b)
{
    int newVal = getVal() + b;
    setVal(newVal);
}

void
GtIntProperty::operator-=(int b)
{
    int newVal = getVal() - b;
    setVal(newVal);
}

void
GtIntProperty::operator*=(int b)
{
    int newVal = getVal() * b;
    setVal(newVal);
}

void
GtIntProperty::operator/=(int b)
{
    assert(b != 0);
    int newVal = getVal() / b;
    setVal(newVal);
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

bool
GtIntProperty::lowSideBoundaryActive() const
{
    return m_boundsCheckFlagLow;
}

bool
GtIntProperty::highSideBoundaryActive() const
{
    return m_boundsCheckFlagHi;
}

gt::PropertyConfiguration
GtIntProperty::configuration() const
{
    return {{QStringLiteral("initialValue"), m_initValue},
            {QStringLiteral("lowBoundaryActive"), m_boundsCheckFlagLow},
            {QStringLiteral("lowBoundary"), m_boundLo},
            {QStringLiteral("highBoundaryActive"), m_boundsCheckFlagHi},
            {QStringLiteral("highBoundary"), m_boundHi}};
}

bool
GtIntProperty::applyConfiguration(const gt::PropertyConfiguration& config)
{
    static const QStringList keys{
        QStringLiteral("initialValue"), QStringLiteral("lowBoundaryActive"),
        QStringLiteral("lowBoundary"), QStringLiteral("highBoundaryActive"),
        QStringLiteral("highBoundary")};
    if (config.size() != keys.size()) return false;
    if (!std::all_of(keys.cbegin(), keys.cend(), [&config](const QString& key) {
            return config.contains(key);
        }))
        return false;
    const auto initial = config.value(QStringLiteral("initialValue"));
    const auto lowActive = config.value(QStringLiteral("lowBoundaryActive"));
    const auto low = config.value(QStringLiteral("lowBoundary"));
    const auto highActive = config.value(QStringLiteral("highBoundaryActive"));
    const auto high = config.value(QStringLiteral("highBoundary"));
    if (initial.userType() != QMetaType::Int ||
        lowActive.userType() != QMetaType::Bool ||
        low.userType() != QMetaType::Int ||
        highActive.userType() != QMetaType::Bool ||
        high.userType() != QMetaType::Int ||
        (lowActive.toBool() && initial.toInt() < low.toInt()) ||
        (highActive.toBool() && initial.toInt() > high.toInt()) ||
        (lowActive.toBool() && highActive.toBool() &&
         low.toInt() > high.toInt()))
        return false;

    m_boundsCheckFlagLow = lowActive.toBool();
    m_boundLo = low.toInt();
    m_boundsCheckFlagHi = highActive.toBool();
    m_boundHi = high.toInt();
    m_initValue = initial.toInt();
    m_value = m_initValue;
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
        return new GtIntProperty(id, id, QString{},
                      gt::Boundaries<int>::makeNormalized(
                          boundaryType == GtIntProperty::BoundLow ? boundary : std::numeric_limits<int>::min(),
                          boundaryType == GtIntProperty::BoundHigh ? boundary : std::numeric_limits<int>::max()),
                          value);
    };
}

gt::PropertyFactoryFunction
gt::makeIntProperty(int lowSideBoundary, int highSideBoundary, int value)
{
    return [=](QString const& id){
        return new GtIntProperty(id, id, QString{},
                                 gt::Boundaries<int>::makeNormalized(
                                     lowSideBoundary, highSideBoundary),
                                 value);
    };
}

gt::PropertyFactoryFunction
gt::makeIntProperty(gt::Boundaries<int> bounds, int value)
{
    return [=](QString const& id){
        return new GtIntProperty(id, id, QString{}, std::move(bounds), value);
    };
}
