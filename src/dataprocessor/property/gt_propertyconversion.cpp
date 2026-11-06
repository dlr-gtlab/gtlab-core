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
#include "gt_boolproperty.h"
#include "gt_intproperty.h"
#include "gt_doubleproperty.h"

gt::conversion::ConversionResult<int>
gt::conversion::fromDouble::toInt(const GtAbstractProperty& source,
                                  const GtAbstractProperty& target)
{
    const auto& from = static_cast<const GtDoubleProperty&>(source);
    auto& to = static_cast<const GtIntProperty&>(target);

    double baseValue = from.getVal();

    if (qIsNaN(baseValue) || qIsInf(baseValue))
    {
        return ConversionResult<int>::failed();
    }

    const double rounded = std::round(baseValue);

    if (rounded < std::numeric_limits<int>::lowest() ||
        rounded > std::numeric_limits<int>::max())
    {
        return ConversionResult<int>::failed();
    }

    int result = static_cast<int>(rounded);

    if (to.lowSideBoundaryActive() && result < to.lowSideBoundary())
    {
        return ConversionResult<int>::failed();
    }

    if (to.highSideBoundaryActive() && result > to.highSideBoundary())
    {
        return ConversionResult<int>::failed();
    }

    // Difference to original double
    const double error = std::abs(baseValue - rounded);

    // Tolerance in relation to magnitute of property value
    const double tolerance = std::numeric_limits<double>::epsilon() *
                             std::max(1.0, std::abs(baseValue));

    if (error > tolerance)
    {
        return ConversionResult<int>::lossy(result);
    }

    if (from.unitCategory() != GtUnit::None &&
        from.unitCategory() != GtUnit::NonDimensional)
    {
        gtWarning() << QObject::tr("Conversion from %1 to %2 indicates a loss "
                                   "of the unit information (%3)")
                           .arg(from.objectName(), to.objectName(),
                                from.unitCategoryToString());

        return ConversionResult<int>::lossy(result);
    }

    return ConversionResult<int>::success(result);
}

bool
gt::conversion::fromDouble::canConnectToInt(const GtAbstractProperty& source,
                                            const GtAbstractProperty& target)
{
    return true;
}

gt::conversion::ConversionResult<double>
gt::conversion::fromInt::toDouble(const GtAbstractProperty& source,
                                  const GtAbstractProperty& target)
{
    const auto& from = static_cast<const GtIntProperty&>(source);
    //auto& to = static_cast<const GtDoubleProperty&>(target);

    double res = double(from.getVal());

    return gt::conversion::ConversionResult<double>::success(res);
}

bool
gt::conversion::fromInt::canConnectToDouble(const GtAbstractProperty& source,
                                            const GtAbstractProperty& target)
{
    return true;
}

gt::conversion::ConversionResult<QVariant>
gt::conversion::defaultConverter(const GtAbstractProperty& source,
                                 const GtAbstractProperty& target)
{
    const QVariant sourceVal = source.valueToVariant();
    const QVariant targetVal = target.valueToVariant();

    if (!sourceVal.isValid() || !targetVal.isValid())
    {
        return gt::conversion::ConversionResult<QVariant>::failed();
    }

    // Both properties must contain the same metatype.
    if (sourceVal.userType() != targetVal.userType())
    {
        return gt::conversion::ConversionResult<QVariant>::failed();
    }

    return gt::conversion::ConversionResult<QVariant>::success(sourceVal);
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

    if (!isUnitFree(from.unitCategory()) && !isUnitFree(to.unitCategory()) &&
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

bool
gt::conversion::fromBool::canConnectToInt(const GtAbstractProperty& source,
                                          const GtAbstractProperty& target)
{
    return true;
}

bool
gt::conversion::fromInt::canConnectToBool(const GtAbstractProperty& source,
                                          const GtAbstractProperty& target)
{
    return true;
}

gt::conversion::ConversionResult<bool>
gt::conversion::fromInt::toBool(const GtAbstractProperty& source,
                                const GtAbstractProperty& target)
{
    const auto& from = static_cast<const GtIntProperty&>(source);

    bool converted = from.getVal() != 0;

    if (from.getVal() != 1 && from.getVal() != 0)
    {
        return gt::conversion::ConversionResult<bool>::lossy(converted);
    }

    return gt::conversion::ConversionResult<bool>::success(converted);
}

gt::conversion::ConversionResult<int>
gt::conversion::fromBool::toInt(const GtAbstractProperty& source,
                                const GtAbstractProperty& target)
{
    const auto& from = static_cast<const GtBoolProperty&>(source);
    auto& to = static_cast<const GtIntProperty&>(target);

    int result = 0;

    if (from.get())
    {
        result = 1;
    }

    return gt::conversion::ConversionResult<int>::success(result);
}

gt::conversion::isCompatible
gt::conversion::defaultCompatibility()
{
    return
        [](const GtAbstractProperty& source, const GtAbstractProperty& target) {
            const QVariant sourceValue = source.valueToVariant();
            const QVariant targetValue = target.valueToVariant();

            return sourceValue.isValid() && targetValue.isValid() &&
                   sourceValue.userType() == targetValue.userType();
        };
}
