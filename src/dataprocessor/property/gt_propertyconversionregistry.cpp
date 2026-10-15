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
#include "gt_boolproperty.h"
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
    QMetaObject from, QMetaObject to, gt::conversion::convert convert,
    gt::conversion::isCompatible canConnect)
{
    if (!convert)
    {
        gtWarning() << QObject::tr("Registration of a converter is invalid if "
                                   "no conversion function is given");
        return;
    }

    QStringList avails = converterAvailable(from.className());

    if (avails.contains(to.className()))
    {
        gtWarning() << QObject::tr("Registration for a property converter "
                                   "from %1 to %2 is ignored as there is an"
                                   " existing one.")
                           .arg(from.className(), to.className());
        return;
    }

    m_canConvertList.append(GtPropertyConverter(from, to, convert, canConnect));
}

gt::conversion::isCompatible
GtPropertyConversionRegistry::isCompatibleFunction(const QMetaObject& from,
                                                   const QMetaObject& to) const
{
    const GtPropertyConverter* converter =
        findConverterWithInheritance(from, to);

    if (!converter) return {};

    return converter->isCompatibletFunc();
}

gt::conversion::convert
GtPropertyConversionRegistry::convertFunction(const QMetaObject& from,
                                              const QMetaObject& to) const
{
    const GtPropertyConverter* converter =
        findConverterWithInheritance(from, to);

    // A specifically registered converter takes precedence.
    if (converter)
    {
        const auto func = converter->convertFunc();

        if (func)
        {
            return func;
        }
    }

    // Identical property types can use the default converter,
    // even if no explicit registration exists.
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
    const auto findTypeIndex = [](const auto& chain, const QString& className) {
        const auto it = std::find_if(chain.cbegin(), chain.cend(),
                                     [&className](const QMetaObject* type) {
                                         return className == type->className();
                                     });

        return static_cast<std::size_t>(std::distance(chain.cbegin(), it));
    };

    const GtPropertyConverter* bestConverter = nullptr;

    std::size_t bestFromIndex = fromChain.size();
    std::size_t bestToIndex = toChain.size();

    for (const GtPropertyConverter& converter : m_canConvertList)
    {
        const auto fromIndex =
            findTypeIndex(fromChain, converter.fromClassName());

        // Source type is not part of the inheritance chain.
        if (fromIndex == fromChain.size()) continue;

        const auto toIndex = findTypeIndex(toChain, converter.toClassName());

        // Target type is not part of the inheritance chain.
        if (toIndex == toChain.size()) continue;

        // Lexicographic ranking:
        // 1. Most specific source type
        // 2. Most specific target type
        const bool isBetter =
            fromIndex < bestFromIndex ||
            (fromIndex == bestFromIndex && toIndex < bestToIndex);

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
    auto function = isCompatibleFunction(*a.metaObject(), *b.metaObject());

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

    // converter from int to bool
    registerConnectionCompatibility(GtIntProperty::staticMetaObject,
                                    GtBoolProperty::staticMetaObject,
                                    gt::conversion::fromInt::toBool,
                                    gt::conversion::fromInt::canConnectToBool);
}
