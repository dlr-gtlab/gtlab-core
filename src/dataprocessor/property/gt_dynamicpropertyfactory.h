/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTDYNAMICPROPERTYFACTORY_H
#define GTDYNAMICPROPERTYFACTORY_H

#include "gt_datamodel_exports.h"
#include "gt_abstractproperty.h"

#include <QVariantMap>
#include <functional>
#include <memory>

/** Registry for reconstructible runtime property definitions. */
class GT_DATAMODEL_EXPORT GtDynamicPropertyFactory
{
public:
    using DefinitionWriter =
        std::function<QVariantMap(const GtAbstractProperty&)>;
    using Matcher = std::function<bool(const GtAbstractProperty&)>;
    using Creator = std::function<std::unique_ptr<GtAbstractProperty>(
        const QString&, const QString&, const QVariantMap&)>;

    class GT_DATAMODEL_EXPORT Registration
    {
    public:
        Registration() = default;
        Registration(Registration&& other) noexcept;
        Registration& operator=(Registration&& other) noexcept;
        ~Registration();
        Registration(const Registration&) = delete;
        Registration& operator=(const Registration&) = delete;
        explicit operator bool() const
        {
            return m_token != 0;
        }

    private:
        friend class GtDynamicPropertyFactory;
        explicit Registration(quint64 token) : m_token(token)
        {
        }
        quint64 m_token{0};
    };

    static Registration registerType(const QString& typeId, Matcher matcher,
                                     DefinitionWriter writer, Creator creator);
    static bool definition(const QString& typeId,
                           const GtAbstractProperty& property,
                           QVariantMap& result);
    static bool definition(const GtAbstractProperty& property, QString& typeId,
                           QVariantMap& result);
    static std::unique_ptr<GtAbstractProperty> create(
        const QString& typeId, const QString& ident, const QString& name,
        const QVariantMap& definition);

private:
    static void unregister(quint64 token);
};

#endif
