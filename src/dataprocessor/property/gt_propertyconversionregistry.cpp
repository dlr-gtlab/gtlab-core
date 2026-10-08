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

GtPropertyConversionRegistry&
GtPropertyConversionRegistry::getInstance()
{
    static GtPropertyConversionRegistry instance;
    return instance;
}

GtPropertyConversionRegistry&
gtPropConversion()
{
    return GtPropertyConversionRegistry::getInstance();
}

void
GtPropertyConversionRegistry::registerConnectionCompatibility(
    QMetaObject from, QMetaObject to,
    std::function<bool(GtAbstractProperty&, GtAbstractProperty&)> f)
{
    if (f) getInstance().canConvertHash.insert(from.className(), {to, f});
}

using CanConnectFunction =
    std::function<bool(GtAbstractProperty& from, GtAbstractProperty& to)>;
QVector<CanConnectFunction>
GtPropertyConversionRegistry::canConnectFunctions(const QMetaObject& from,
                                        const QMetaObject& to) const
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
            auto range = gtPropConversion().canConvertHash.equal_range(fromType->className());

            QVector<CanConnectFunction> result;

            for (auto it = range.first; it != range.second; ++it)
            {
                if (it.value().to.className() == toType->className())
                {
                    result.append(it.value().f);
                }
            }

            if (!result.empty())
            {
                return result;
            }
        }
    }

    return {};
}
