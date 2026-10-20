/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_propertyfactory.h"
#include "gt_abstractobjectfactory.h"

#include <algorithm>
#include <cmath>

namespace
{
    bool isJsonLike(const QVariant& value, int depth)
    {
        if (depth > gt::PropertyConfigurationMaxDepth) return false;
        if (!value.isValid() || value.isNull()) return true;

        switch (value.userType())
        {
        case QMetaType::Bool:
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
        case QMetaType::QString:
            return true;
        case QMetaType::Double:
            return std::isfinite(value.toDouble());
        case QMetaType::QVariantList: {
            const auto list = value.toList();
            return std::all_of(list.cbegin(), list.cend(),
                               [depth](const auto& item) {
                                   return isJsonLike(item, depth + 1);
                               });
        }
        case QMetaType::QVariantMap: {
            const auto map = value.toMap();
            return std::all_of(map.cbegin(), map.cend(),
                               [depth](const auto& entry) {
                                   return isJsonLike(entry, depth + 1);
                               });
        }
        default:
            return false;
        }
    }
} // namespace

PropertyFactoryRegistration::PropertyFactoryRegistration(
    GtAbstractObjectFactory& objectFactory, QString typeId,
    const QMetaObject& propertyType, gt::PropertyFactoryFunction factory,
    std::type_index cppType) :
    m_objectFactory(&objectFactory), m_typeId(std::move(typeId))
{
    if (!m_objectFactory->registerPropertyFactory(
            m_typeId, propertyType, std::move(factory), {}, cppType))
    {
        m_objectFactory = nullptr;
        m_typeId.clear();
    }
}

PropertyFactoryRegistration::~PropertyFactoryRegistration()
{
    if (m_objectFactory && !m_typeId.isEmpty())
    {
        m_objectFactory->unregisterPropertyFactory(m_typeId);
    }
}

PropertyFactoryRegistration::PropertyFactoryRegistration(
    PropertyFactoryRegistration&& other) noexcept :
    m_objectFactory(other.m_objectFactory), m_typeId(std::move(other.m_typeId))
{
    other.m_objectFactory = nullptr;
    other.m_typeId.clear();
}

PropertyFactoryRegistration&
PropertyFactoryRegistration::operator=(
    PropertyFactoryRegistration&& other) noexcept
{
    if (this == &other) return *this;
    if (m_objectFactory && !m_typeId.isEmpty())
    {
        m_objectFactory->unregisterPropertyFactory(m_typeId);
    }
    m_objectFactory = other.m_objectFactory;
    m_typeId = std::move(other.m_typeId);
    other.m_objectFactory = nullptr;
    other.m_typeId.clear();
    return *this;
}

bool
PropertyFactoryRegistration::isValid() const
{
    return m_objectFactory && !m_typeId.isEmpty();
}

namespace gt
{
    std::unique_ptr<GtAbstractProperty> createProperty(
        const GtAbstractObjectFactory& factory, const QString& typeId,
        const QString& ident)
    {
        return factory.createProperty(typeId, ident);
    }

    bool isJsonLikePropertyConfiguration(
        const PropertyConfiguration& configuration)
    {
        return std::all_of(
            configuration.cbegin(), configuration.cend(),
            [](const auto& entry) { return isJsonLike(entry, 1); });
    }
} // namespace gt
