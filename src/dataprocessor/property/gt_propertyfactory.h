/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTPROPERTYFACTORY_H
#define GTPROPERTYFACTORY_H

#include "gt_abstractproperty.h"

#include "gt_propertyconfiguration.h"

#include <memory>

#include <typeindex>

class GtAbstractObjectFactory;

/** A module-provided stable type ID and constructor. */
struct GT_DATAMODEL_EXPORT PropertyFactoryData
{
    QString typeId;
    const QMetaObject* propertyType{nullptr};
    gt::PropertyFactoryFunction factory;
    std::type_index cppType{typeid(void)};
};

/**
 * Registers a runtime-property constructor for a stable, namespaced type ID.
 * The registration is removed when this token is destroyed.
 */
class GT_DATAMODEL_EXPORT PropertyFactoryRegistration
{
public:
    PropertyFactoryRegistration(
        GtAbstractObjectFactory& objectFactory, QString typeId,
        const QMetaObject& propertyType, gt::PropertyFactoryFunction factory,
        std::type_index cppType = std::type_index(typeid(void)));
    ~PropertyFactoryRegistration();

    PropertyFactoryRegistration(PropertyFactoryRegistration&& other) noexcept;
    PropertyFactoryRegistration& operator=(
        PropertyFactoryRegistration&& other) noexcept;

    PropertyFactoryRegistration(const PropertyFactoryRegistration&) = delete;
    PropertyFactoryRegistration& operator=(const PropertyFactoryRegistration&) =
        delete;

    bool isValid() const;

private:
    GtAbstractObjectFactory* m_objectFactory{nullptr};
    QString m_typeId;
};

namespace gt
{
    GT_DATAMODEL_EXPORT
    std::unique_ptr<GtAbstractProperty> createProperty(
        const GtAbstractObjectFactory& factory, const QString& typeId,
        const QString& ident);

    GT_DATAMODEL_EXPORT
    bool isJsonLikePropertyConfiguration(
        const PropertyConfiguration& configuration);
} // namespace gt

#endif // GTPROPERTYFACTORY_H
