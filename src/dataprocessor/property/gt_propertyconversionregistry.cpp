/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 08.10.2026
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_propertyconversionregistry.h"

#include "gt_doubleproperty.h"
#include "gt_intproperty.h"

GtPropertyConversionRegistry&
gtPropConversion()
{
    return GtPropertyConversionRegistry::getInstance();
}

GtPropertyConversionRegistry&
GtPropertyConversionRegistry::getInstance()
{
    static GtPropertyConversionRegistry instance;
    return instance;
}

void
GtPropertyConversionRegistry::registerConnectionCompatibility(
    QMetaObject from, QMetaObject to,
    gt::conversion::canConnect canConnect)
{
    if (canConnect)
    {
        gtPropConversion().canConvertHash.append(
            GtPropertyConverter(from, to, {}, canConnect));
    }
}

void
GtPropertyConversionRegistry::registerConnectionCompatibility(
    QMetaObject from, QMetaObject to,
    gt::conversion::convert convert,
    gt::conversion::canConnect canConnect)
{
    if (convert)
    {
        gtPropConversion().canConvertHash.append(
            GtPropertyConverter(from, to, convert, canConnect));
    }
}

gt::conversion::canConnect
GtPropertyConversionRegistry::canConnectFunction(
    const QMetaObject& from, const QMetaObject& to) const
{
    const auto inheritanceChain = [](const QMetaObject& metaObject)
    {
        QVector<const QMetaObject*> result;

        for (auto* current = &metaObject;
             current != nullptr;
             current = current->superClass())
        {
            result.append(current);
        }

        return result;
    };

    const auto fromChain = inheritanceChain(from);
    const auto toChain = inheritanceChain(to);

    for (const auto* fromType : fromChain)
    {
        for (const auto* toType : toChain)
        {
            for (const GtPropertyConverter& converter : canConvertHash)
            {
                if (converter.fromClassName() == fromType->className() &&
                    converter.toClassName() == toType->className())
                {
                    return converter.canConnectFunc();
                }
            }
        }
    }

    return {};
}

GtPropertyConversionRegistry::GtPropertyConversionRegistry()
{
    // add the basic converter registrations here later

    // converter from double to int
    GtPropertyConversionRegistry::registerConnectionCompatibility(
        GtDoubleProperty::staticMetaObject,
        GtIntProperty::staticMetaObject,
        [](GtAbstractProperty const& a, // LCOV_EXCL_LINE
           GtAbstractProperty& b) {
            const auto& from = static_cast<const GtDoubleProperty&>(a);
            auto& to = static_cast<GtIntProperty&>(b);

            double baseValue = from.getVal();

            const double rounded = std::round(baseValue);

            int result = static_cast<int>(rounded);

            // Abweichung vom ursprünglichen double
            const double error = std::abs(baseValue - rounded);

            // Toleranz entsprechend der Größenordnung des Wertes
            const double tolerance =
                std::numeric_limits<double>::epsilon() *
                std::max(1.0, std::abs(baseValue));

            to.setVal(result);

            if (error <= tolerance)
            {
                return gt::conversion::conversionSuccess::Success;
            }

            return gt::conversion::conversionSuccess::Lossy;

        },
        // can connect
        [](GtAbstractProperty const&, // LCOV_EXCL_LINE
           GtAbstractProperty const&) { return true; });

    // converter from int to double
    GtPropertyConversionRegistry::registerConnectionCompatibility(
        GtIntProperty::staticMetaObject,
        GtDoubleProperty::staticMetaObject,
        [](GtAbstractProperty const& a, // LCOV_EXCL_LINE
           GtAbstractProperty& b) {
            const auto& from = static_cast<const GtIntProperty&>(a);
            auto& to = static_cast<GtDoubleProperty&>(b);

            double res = double(from.getVal());

            to.setVal(res);

            return gt::conversion::conversionSuccess::Success;
        },
        // can connect
        [](GtAbstractProperty const&, // LCOV_EXCL_LINE
           GtAbstractProperty const&) { return true; });
}
