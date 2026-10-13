/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 08.10.2026
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_propertyconversion.h"
#include "gt_intproperty.h"
#include "gt_doubleproperty.h"

gt::conversion::conversionSuccess
gt::conversion::fromDouble::toInt(const GtAbstractProperty& source,
                                  GtAbstractProperty& target)
{
    const auto& from = static_cast<const GtDoubleProperty&>(source);
    auto& to = static_cast<GtIntProperty&>(target);

    double baseValue = from.getVal();

    if (qIsNaN(baseValue) || qIsInf(baseValue))
    {
        return gt::conversion::conversionSuccess::Failed;
    }

    const double rounded = std::round(baseValue);

    int result = static_cast<int>(rounded);

    if (to.lowSideBoundaryActive() && result < to.lowSideBoundary())
    {
        return gt::conversion::conversionSuccess::Failed;
    }

    if (to.highSideBoundaryActive() && result > to.highSideBoundary())
    {
        return gt::conversion::conversionSuccess::Failed;
    }

    // Abweichung vom ursprünglichen double
    const double error = std::abs(baseValue - rounded);

    // Toleranz entsprechend der Größenordnung des Wertes
    const double tolerance = std::numeric_limits<double>::epsilon() *
                             std::max(1.0, std::abs(baseValue));

    bool success = true;

    to.setVal(result, &success);

    if (!success)
    {
        return gt::conversion::conversionSuccess::Failed;
    }

    if (error > tolerance)
    {
        return gt::conversion::conversionSuccess::Lossy;
    }

    if (from.unitCategory() != GtUnit::None &&
        from.unitCategory() != GtUnit::NonDimensional)
    {
        gtWarning() << QObject::tr("Conversion from %1 to %2 indicates a loss "
                                   "of the unit information (%3)").arg(
                               from.objectName(), to.objectName(),
                               from.unitCategoryToString());

        return gt::conversion::conversionSuccess::Lossy;
    }

    return gt::conversion::conversionSuccess::Success;
}

bool
gt::conversion::fromDouble::canConnectToInt(const GtAbstractProperty& source,
                                            const GtAbstractProperty& target)
{
    return true;
}

gt::conversion::conversionSuccess
gt::conversion::fromInt::toDouble(const GtAbstractProperty& source,
                                  GtAbstractProperty& target)
{
    const auto& from = static_cast<const GtIntProperty&>(source);
    auto& to = static_cast<GtDoubleProperty&>(target);

    double res = double(from.getVal());

    to.setVal(res);

    return gt::conversion::conversionSuccess::Success;
}

bool
gt::conversion::fromInt::canConnectToDouble(const GtAbstractProperty& source,
                                            const GtAbstractProperty& target)
{
    return true;
}

gt::conversion::conversionSuccess
gt::conversion::defaultConverter(const GtAbstractProperty& source,
                                 GtAbstractProperty& target)
{
    QVariant sourceVal = source.valueToVariant();
    QVariant targetVal = target.valueToVariant();

    if (sourceVal.typeName() == targetVal.typeName())
    {
        target.setValueFromVariant(sourceVal);
        return gt::conversion::conversionSuccess::Success;
    }

    return gt::conversion::conversionSuccess::Failed;
}

bool
gt::conversion::fromInt::canConnectToInt(const GtAbstractProperty& source,
                                         const GtAbstractProperty& target)
{
    const auto& from = static_cast<const GtIntProperty&>(source);
    const auto& to = static_cast<const GtIntProperty&>(target);

    // check of the bounds
    // if bounds are active it should be checked if the valid
    // ranges of the properties do at least match in pieces
    if (from.highSideBoundaryActive() && to.lowSideBoundaryActive())
    {
        if (from.highSideBoundary() < to.lowSideBoundary())
        {
            return false;
        }
    }

    if (from.lowSideBoundaryActive() && to.highSideBoundaryActive())
    {
        if (from.lowSideBoundary() > to.highSideBoundary())
        {
            return false;
        }
    }

    return true;
}

bool
gt::conversion::fromDouble::canConnectToDouble(const GtAbstractProperty& source,
                                               const GtAbstractProperty& target)
{

    const auto& from = static_cast<const GtDoubleProperty&>(source);
    auto& to = static_cast<const GtDoubleProperty&>(target);

    // check of the units:
    // only identical units and nondimensional should be connected
    const auto isUnitFree = [](auto category) {
        return category == GtUnit::None;
    };

    if (!isUnitFree(from.unitCategory()) &&
        !isUnitFree(to.unitCategory()) &&
        from.unitCategory() != to.unitCategory())
    {
        return false;
    }

    // check of the bounds
    // if bounds are active it should be checked if the valid
    // ranges of the properties do at least match in pieces
    if (from.highSideBoundaryActive() && to.lowSideBoundaryActive())
    {
        if (from.highSideBoundary() < to.lowSideBoundary())
        {
            return false;
        }
    }

    if (from.lowSideBoundaryActive() && to.highSideBoundaryActive())
    {
        if (from.lowSideBoundary() > to.highSideBoundary())
        {
            return false;
        }
    }

    return true;
}
