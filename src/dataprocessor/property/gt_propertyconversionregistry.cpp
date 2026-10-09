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
#include "gt_propertyconversion.h"
#include "gt_intproperty.h"
#include "gt_doubleproperty.h"

namespace {
    const auto inheritanceChain = [](const QMetaObject& metaObject) {
        QVector<const QMetaObject*> result;

        for (auto* current = &metaObject; current != nullptr;
             current = current->superClass())
        {
            result.append(current);
        }

        return result;
    };
}

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
    QMetaObject from, QMetaObject to, gt::conversion::canConnect canConnect)
{
    if (canConnect)
    {
        QStringList avails = converterAvailable(from.className());

        if (!avails.contains(to.className()))
        {
            m_canConvertList.append(
                GtPropertyConverter(from, to, {}, canConnect));
        }
    }
}

void
GtPropertyConversionRegistry::registerConnectionCompatibility(
    QMetaObject from, QMetaObject to, gt::conversion::convert convert,
    gt::conversion::canConnect canConnect)
{
    if (convert)
    {
        QStringList avails = converterAvailable(from.className());

        if (!avails.contains(to.className()))
        {
            m_canConvertList.append(
                GtPropertyConverter(from, to, convert, canConnect));
        }
    }
}

gt::conversion::canConnect
GtPropertyConversionRegistry::canConnectFunction(const QMetaObject& from,
                                                 const QMetaObject& to) const
{
    const GtPropertyConverter* converter = findConverterWithInheritance(from,
                                                                        to);

    if (!converter) return {};

    return converter->canConnectFunc();
}

gt::conversion::convert
GtPropertyConversionRegistry::convertFunction(const QMetaObject& from,
                                              const QMetaObject& to) const
{
    const GtPropertyConverter* converter = findConverterWithInheritance(from,
                                                                        to);

    if (!converter) return {};

    return converter->convertFunc();
}

const GtPropertyConverter*
GtPropertyConversionRegistry::findConverterWithInheritance(
    const QMetaObject& from, const QMetaObject& to) const
{
    const auto fromChain = inheritanceChain(from);
    const auto toChain = inheritanceChain(to);

    for (const auto* fromType : fromChain)
    {
        const auto converterIt = std::find_if(
            m_canConvertList.cbegin(), m_canConvertList.cend(),
            [&](const GtPropertyConverter& converter) {
                if (converter.fromClassName() != fromType->className())
                {
                    return false;
                }

                return std::any_of(
                    toChain.cbegin(), toChain.cend(),
                    [&](const auto* toType) {
                        return converter.toClassName() == toType->className();
                    });
            });

        if (converterIt != m_canConvertList.cend())
        {
            return &*converterIt;
        }
    }

    return nullptr;
}

QStringList
GtPropertyConversionRegistry::converterAvailable(QString const& from) const
{
    QStringList retVal;

    for (const GtPropertyConverter& converter : m_canConvertList)
    {
        if (converter.fromClassName() == from)
        {
            retVal.append(converter.toClassName());
        }
    }

    return retVal;
}

bool
GtPropertyConversionRegistry::canConnect(const GtAbstractProperty& a,
                                         const GtAbstractProperty& b)
{
    auto function = canConnectFunction(*a.metaObject(), *b.metaObject());

    // if no connectionCheck function is available check based
    if (!function)
    {
        return a.metaObject()->className() == b.metaObject()->className();
    }

    return function(a, b);
}

GtPropertyConversionRegistry::GtPropertyConversionRegistry()
{
    registerBasicPropertyConvertersImpl();
}

void
GtPropertyConversionRegistry::registerBasicPropertyConvertersImpl()
{
    // add the basic converter registrations here

    // converter from double to int
    registerConnectionCompatibility(
        GtDoubleProperty::staticMetaObject, GtIntProperty::staticMetaObject,
        // conversion
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
            const double tolerance = std::numeric_limits<double>::epsilon() *
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
    registerConnectionCompatibility(
        GtIntProperty::staticMetaObject, GtDoubleProperty::staticMetaObject,
        // conversion
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
