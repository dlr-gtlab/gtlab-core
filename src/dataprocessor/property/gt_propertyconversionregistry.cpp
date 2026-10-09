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
        QStringList avails = converterAvailable(from.className());

        if (!avails.contains(to.className()))
        {
            canConvertHash.append(GtPropertyConverter(from, to, {}, canConnect));
        }
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
        QStringList avails = converterAvailable(from.className());

        if (!avails.contains(to.className()))
        {
            canConvertHash.append(
                GtPropertyConverter(from, to, convert, canConnect));
        }
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
        for (const GtPropertyConverter& converter : canConvertHash)
        {
            if (converter.fromClassName() == fromType->className())
            {
                for (const auto* toType : toChain)
                {
                    if (converter.toClassName() == toType->className())
                    {
                        return converter.canConnectFunc();
                    }
                }
            }
        }
    }

    return {};
}

QStringList
GtPropertyConversionRegistry::converterAvailable(QString const& from) const
{
    QStringList retVal;

    for (const GtPropertyConverter& converter : canConvertHash)
    {
        if (converter.fromClassName() == from)
        {
            retVal.append(converter.toClassName());
        }
    }

    return retVal;
}

GtPropertyConversionRegistry::GtPropertyConversionRegistry()
{
    gt::conversion::registerBasicPropertyConverters();
}
