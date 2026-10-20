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
    const auto match = findBestConverterWithInheritance(
        from, to, LookupPurpose::Compatibility);

    if (!match.found) return {};

    if (!match.converter)
    {
        return gt::conversion::defaultCompatibility();
    }

    return match.converter->isCompatibleFunc();
}

gt::conversion::convert
GtPropertyConversionRegistry::convertFunction(const QMetaObject& from,
                                              const QMetaObject& to) const
{
    const auto match =
        findBestConverterWithInheritance(from, to, LookupPurpose::Conversion);

    if (!match.found) return {};

    if (!match.converter)
    {
        return gt::conversion::defaultConverter;
    }

    return match.converter->convertFunc();
}

const GtPropertyConverter*
GtPropertyConversionRegistry::findConverterWithInheritance(
    const QMetaObject& from, const QMetaObject& to) const
{
    const auto match = findBestConverterWithInheritance(
        from, to, LookupPurpose::AnyRegistered, false);

    return match.converter;
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

GtPropertyConversionRegistry::ConverterMatch
GtPropertyConversionRegistry::findBestConverterWithInheritance(
    const QMetaObject& from, const QMetaObject& to, LookupPurpose purpose,
    bool includeImplicit) const
{
    const auto fromChain = inheritanceChain(from);
    const auto toChain = inheritanceChain(to);

    const auto findTypeIndex = [](const auto& chain, const QString& className) {
        const auto it = std::find_if(chain.cbegin(), chain.cend(),
                                     [&className](const QMetaObject* type) {
                                         return className == type->className();
                                     });

        return static_cast<std::size_t>(std::distance(chain.cbegin(), it));
    };

    ConverterMatch best;

    const auto considerCandidate = [&best](const GtPropertyConverter* candidate,
                                           std::size_t fromIndex,
                                           std::size_t toIndex) {
        const auto score = fromIndex + toIndex;
        const auto bestScore = best.fromIndex + best.toIndex;

        const bool isBetter =
            !best.found || score < bestScore ||
            (score == bestScore && fromIndex < best.fromIndex) ||
            (score == bestScore && fromIndex == best.fromIndex &&
             toIndex == best.toIndex && candidate != nullptr &&
             best.converter == nullptr);

        if (isBetter)
        {
            best.converter = candidate;
            best.found = true;
            best.fromIndex = fromIndex;
            best.toIndex = toIndex;
        }
    };

    // 1. Consider registered converters.
    for (const GtPropertyConverter& converter : m_canConvertList)
    {
        const auto fromIndex =
            findTypeIndex(fromChain, converter.fromClassName());

        if (fromIndex == fromChain.size()) continue;

        const auto toIndex = findTypeIndex(toChain, converter.toClassName());

        if (toIndex == toChain.size()) continue;

        // Only consider converters that provide the requested function.
        if (purpose == LookupPurpose::Conversion && !converter.convertFunc())
        {
            continue;
        }

        if (purpose == LookupPurpose::Compatibility &&
            !converter.isCompatibleFunc())
        {
            continue;
        }

        considerCandidate(&converter, fromIndex, toIndex);
    }

    if (!includeImplicit)
    {
        return best;
    }

    // 2. Consider implicit candidates for common property classes.
    for (std::size_t i = 0; i < fromChain.size(); ++i)
    {
        for (std::size_t j = 0; j < toChain.size(); ++j)
        {
            const QMetaObject* commonType = fromChain[i];

            if (QString::fromLatin1(commonType->className()) !=
                QString::fromLatin1(toChain[j]->className()))
            {
                continue;
            }

            // Ignore common ancestors outside the property hierarchy,
            // such as QObject.
            // Ignore GtAbstractProperty itself as a common base.
            // Only more specific property classes may provide an
            // implicit conversion candidate.
            if (commonType == &GtAbstractProperty::staticMetaObject ||
                !commonType->inherits(&GtAbstractProperty::staticMetaObject))
            {
                continue;
            }

            // nullptr identifies the implicit default converter.
            considerCandidate(nullptr, i, j);
        }
    }

    return best;
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

    // converter from bool to int
    registerConnectionCompatibility(GtBoolProperty::staticMetaObject,
                                    GtIntProperty::staticMetaObject,
                                    gt::conversion::fromBool::toInt,
                                    gt::conversion::fromBool::canConnectToInt);
}
