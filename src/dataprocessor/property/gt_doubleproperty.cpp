/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_doubleproperty.cpp
 *
 *  Created on: 14.10.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include "gt_doubleproperty.h"

#include <algorithm>
#include <cmath>

gt::PropertyConfiguration
GtDoubleProperty::configuration() const
{
    return {{QStringLiteral("unitCategory"), static_cast<int>(unitCategory())},
            {QStringLiteral("initialValue"), initialValue()},
            {QStringLiteral("lowBoundaryActive"), lowSideBoundaryActive()},
            {QStringLiteral("lowBoundary"), lowSideBoundary()},
            {QStringLiteral("highBoundaryActive"), highSideBoundaryActive()},
            {QStringLiteral("highBoundary"), highSideBoundary()}};
}

bool
GtDoubleProperty::applyConfiguration(const gt::PropertyConfiguration& config)
{
    static const QStringList keys{
        QStringLiteral("unitCategory"),       QStringLiteral("initialValue"),
        QStringLiteral("lowBoundaryActive"),  QStringLiteral("lowBoundary"),
        QStringLiteral("highBoundaryActive"), QStringLiteral("highBoundary")};
    if (config.size() != keys.size()) return false;
    if (!std::all_of(keys.cbegin(), keys.cend(), [&config](const QString& key) {
            return config.contains(key);
        }))
        return false;

    bool categoryOk = false;
    bool initialOk = false;
    bool lowOk = false;
    bool highOk = false;
    const int category =
        config.value(QStringLiteral("unitCategory")).toInt(&categoryOk);
    const double initial =
        config.value(QStringLiteral("initialValue")).toDouble(&initialOk);
    const double low =
        config.value(QStringLiteral("lowBoundary")).toDouble(&lowOk);
    const double high =
        config.value(QStringLiteral("highBoundary")).toDouble(&highOk);
    const auto lowActive = config.value(QStringLiteral("lowBoundaryActive"));
    const auto highActive = config.value(QStringLiteral("highBoundaryActive"));
    if (!categoryOk || !initialOk || !lowOk || !highOk ||
        lowActive.userType() != QMetaType::Bool ||
        highActive.userType() != QMetaType::Bool || !std::isfinite(initial) ||
        !std::isfinite(low) || !std::isfinite(high) ||
        (lowActive.toBool() && initial < low) ||
        (highActive.toBool() && initial > high) ||
        (lowActive.toBool() && highActive.toBool() && low > high))
    {
        return false;
    }

    // Category values are sparse; reject gaps and the deprecated Custom value.
    const bool knownCategory =
        (category >= GtUnit::Default && category <= GtUnit::Current) ||
        (category >= GtUnit::NonDimensionalPercentage &&
         category <= GtUnit::DataSize) ||
        category == GtUnit::None;
    if (!knownCategory) return false;

    m_unitCategory = static_cast<GtUnit::Category>(category);
    m_boundsCheckFlagLow = lowActive.toBool();
    m_boundsCheckFlagHi = highActive.toBool();
    m_boundLo = low;
    m_boundHi = high;
    m_value = initial;
    m_initValue = initial;
    return true;
}

GtDoubleProperty::GtDoubleProperty(const QString& ident,
                                   const QString& name,
                                   const QString& brief)
{
    setObjectName(name);

    m_id = ident;
    m_brief = brief;
    m_unitCategory = GtUnit::Category::None;
    m_value = 0.0;
    m_initValue = 0.0;
    m_boundsCheckFlagHi = false;
    m_boundsCheckFlagLow = false;
    m_boundHi = 0.0;
    m_boundLo = 0.0;
}

GtDoubleProperty::GtDoubleProperty(const QString& ident,
                                   const QString& name) :
    GtDoubleProperty(ident, name, QString())
{

}

GtDoubleProperty::GtDoubleProperty(const QString& ident,
                                   const QString& name,
                                   const QString& brief,
                                   const GtUnit::Category& unitCategory,
                                   const double& value) :
    GtDoubleProperty(ident, name, brief)
{
    m_unitCategory = unitCategory;
    m_value = value;
    m_initValue = value;
}


GtDoubleProperty::GtDoubleProperty(const QString& ident,
                                   const QString& name,
                                   const QString& brief,
                                   const GtUnit::Category& unitCategory,
                                   const double lowSideBoundary,
                                   const double highSideBoundary,
                                   const double& value) :
    GtDoubleProperty(ident, name, brief)
{
    m_unitCategory = unitCategory;

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

GtDoubleProperty::GtDoubleProperty(const QString& ident,
                                   const QString& name,
                                   const QString& brief,
                                   const GtUnit::Category& unitCategory,
                                   GtDoubleProperty::BoundType boundType,
                                   const double boundary,
                                   const double& value) :
    GtDoubleProperty(ident, name, brief)
{
    m_unitCategory = unitCategory;

    if (boundType == GtDoubleProperty::BoundLow)
    {
        m_boundsCheckFlagLow = true;
        m_boundsCheckFlagHi = false;
        m_boundLo = boundary;
        m_boundHi = 0.0;
    }
    else
    {
        m_boundsCheckFlagLow = false;
        m_boundsCheckFlagHi = true;
        m_boundLo = 0.0;
        m_boundHi = boundary;
    }

    bool success = false;

    setVal(value, &success);

    if (success)
    {
        m_initValue = value;
    }
}

GtDoubleProperty::GtDoubleProperty(const QString& ident, const QString& name,
                                   const QString& brief,
                                   const GtUnit::Category& unitCategory,
                                   gt::Boundaries<double> bounds,
                                   const double& value) :
    GtDoubleProperty(ident, name, brief)
{
    m_unitCategory = unitCategory;
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

    bool success = false;
    setVal(value, &success);

    if (success)
    {
        m_initValue = value;
    }
}

void
GtDoubleProperty::operator+=(const double& b)
{
    double newVal = getVal() + b;
    setVal(newVal);
}

void
GtDoubleProperty::operator-=(const double& b)
{
    double newVal = getVal() - b;
    setVal(newVal);
}

void
GtDoubleProperty::operator*=(const double& b)
{
    double newVal = getVal() * b;
    setVal(newVal);
}

void
GtDoubleProperty::operator/=(const double& b)
{
    assert(b != 0.0);
    double newVal = getVal() / b;
    setVal(newVal);
}

QVariant
GtDoubleProperty::valueToVariant(const QString& unit,
                                 bool* success) const
{
    return getVal(unit, success);
}

bool
GtDoubleProperty::setValueFromVariant(const QVariant& val,
                                      const QString& unit)
{
    bool ok = false;
    const double doubleVal = val.toDouble(&ok);

    if (!ok) return false;

    setVal(doubleVal, unit, &ok);

    return ok;
}

double
GtDoubleProperty::lowSideBoundary() const
{
    return m_boundLo;
}

double
GtDoubleProperty::highSideBoundary() const
{
    return m_boundHi;
}

double
GtDoubleProperty::convertFrom(const double &value, const QString& unit,
                              bool* success)
{
    GtUnit::Category cat = GtProperty<double>::m_unitCategory;
    GtUnitConverter<double>* conv = GtProperty<double>::m_unitConverter;

    // TODO: test vor invalidation
    return conv->from(cat, unit, value, success);
}

double
GtDoubleProperty::convertTo(const QString& unit, bool* success) const
{
    GtUnit::Category cat = GtProperty<double>::m_unitCategory;
    GtUnitConverter<double>* conv = GtProperty<double>::m_unitConverter;

    return conv->To(cat, unit, m_value, success);
}

bool
GtDoubleProperty::validateValue(const double& value)
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

double
GtDoubleProperty::initialValue() const
{
    return m_initValue;
}

bool
GtDoubleProperty::lowSideBoundaryActive() const
{
    return m_boundsCheckFlagLow;
}

bool
GtDoubleProperty::highSideBoundaryActive() const
{
    return m_boundsCheckFlagHi;
}

gt::PropertyFactoryFunction
gt::makeDoubleProperty(double value)
{
    return makePropertyFactory<GtDoubleProperty>(std::move(value));
}

gt::PropertyFactoryFunction
gt::makeDoubleProperty(const QString& name, const QString& brief,
                       const GtUnit::Category& unitCategory,
                       const double& value)
{
    return [=](QString const& id){
        return new GtDoubleProperty(id, name, brief, unitCategory, value);
    };
}

gt::PropertyFactoryFunction
gt::makeDoubleProperty(const QString& name, const QString& brief,
                       const GtUnit::Category& unitCategory,
                       gt::Boundaries<double> boundaries,
                       const double& value)
{
    return [=](QString const& id){
        return new GtDoubleProperty(id, name, brief, unitCategory,
                                    std::move(boundaries), value);
    };
}
