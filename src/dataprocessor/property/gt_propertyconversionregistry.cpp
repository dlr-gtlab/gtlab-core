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
#include "gt_propertyconversion.h"

namespace
{
    const auto inheritanceChain = [](const QMetaObject& metaObject) {
        QVector<const QMetaObject*> result;

        for (auto* current = &metaObject; current != nullptr;
             current = current->superClass())
        {
            result.append(current);
        }

        return result;
    };
} // namespace

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
        else
        {
            gtWarning() << QObject::tr("Registration for a property converter "
                                       "from %1 to %2 is ignored as there is an"
                                       " existing one.")
                               .arg(from.className(), to.className());
        }
    }
}

gt::conversion::canConnect
GtPropertyConversionRegistry::canConnectFunction(const QMetaObject& from,
                                                 const QMetaObject& to) const
{
    const GtPropertyConverter* converter =
        findConverterWithInheritance(from, to);

    if (!converter) return {};

    return converter->canConnectFunc();
}

gt::conversion::convert
GtPropertyConversionRegistry::convertFunction(const QMetaObject& from,
                                              const QMetaObject& to) const
{
    const GtPropertyConverter* converter =
        findConverterWithInheritance(from, to);

    if (!converter) return {};

    gt::conversion::convert func = converter->convertFunc();

    if (func) return func;

    // default behaviour for identical metatypes
    if (from.className() == to.className())
    {
        return gt::conversion::defaultConverter;
    }

    return {};
}

const GtPropertyConverter*
GtPropertyConversionRegistry::findConverterWithInheritance(
    const QMetaObject& from, const QMetaObject& to) const
{
    const auto fromChain = inheritanceChain(from);
    const auto toChain = inheritanceChain(to);

    // Smaller index means a more specific type.
    const auto findTypeIndex =
        [](const auto& chain, const QString& className)
    {
        const auto it = std::find_if(
            chain.cbegin(),
            chain.cend(),
            [&className](const QMetaObject* type)
            {
                return className == type->className();
            });

        return static_cast<std::size_t>(
            std::distance(chain.cbegin(), it));
    };

    const GtPropertyConverter* bestConverter = nullptr;

    std::size_t bestFromIndex = fromChain.size();
    std::size_t bestToIndex = toChain.size();

    for (const GtPropertyConverter& converter : m_canConvertList)
    {
        const auto fromIndex = findTypeIndex(
            fromChain, converter.fromClassName());

        // Source type is not part of the inheritance chain.
        if (fromIndex == fromChain.size()) continue;

        const auto toIndex = findTypeIndex(
            toChain, converter.toClassName());

        // Target type is not part of the inheritance chain.
        if (toIndex == toChain.size()) continue;

        // Lexicographic ranking:
        // 1. Most specific source type
        // 2. Most specific target type
        const bool isBetter =
            fromIndex < bestFromIndex ||
            (fromIndex == bestFromIndex &&
             toIndex < bestToIndex);

        if (isBetter)
        {
            bestConverter = &converter;
            bestFromIndex = fromIndex;
            bestToIndex = toIndex;
        }
    }

    return bestConverter;
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
GtPropertyConversionRegistry::connectionCheckAvailable(
    const GtAbstractProperty& a, const GtAbstractProperty& b)
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
        gt::conversion::fromDouble::toInt,
        gt::conversion::fromDouble::canConnectToInt);


    // converter from int to double
    registerConnectionCompatibility(
        GtIntProperty::staticMetaObject, GtDoubleProperty::staticMetaObject,
        gt::conversion::fromInt::toDouble,
        gt::conversion::fromInt::canConnectToDouble);
}
