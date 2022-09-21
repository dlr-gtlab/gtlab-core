/* GTlab - Gas Turbine laboratory
 * Source File: gt_structproperty.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.06.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVariant>

#include "gt_dynamicpropertycontainer.h"
#include "gt_propertyfactory.h"
#include "gt_exceptions.h"

#include <utility>

GtDynamicPropertyContainer::GtDynamicPropertyContainer(const QString& name)
{
    setObjectName(name);

    m_id = name;
    m_unitCategory = GtUnit::Category::None;
}

QVariant
GtDynamicPropertyContainer::valueToVariant(const QString& /*unit*/,
                                 bool* success) const
{
    if (success)
    {
        *success = false;
    }

    return QVariant();
}

bool
GtDynamicPropertyContainer::setValueFromVariant(const QVariant& /*val*/,
                                      const QString& /*unit*/,
                                      bool* success)
{
    if (success)
    {
        *success = false;
    }

    return false;
}

void
GtDynamicPropertyContainer::registerSubProperty(GtAbstractProperty& prop)
{
    GtAbstractProperty::registerSubProperty(prop);
}

void
GtDynamicPropertyContainer::deleteSubProperty(GtAbstractProperty *property)
{
    if (!m_subProperties.contains(property))
    {
        return;
    }

    m_subProperties.removeAll(property);
    delete property;

}

GtPropertyStructDefinition::GtPropertyStructDefinition(QString typeName)
    : instanceTypeName(std::move(typeName))
{
}

void
GtPropertyStructDefinition::defineMember(const QString &id,
                                         const QString &propertyClassName,
                                         QVariant defaultVal)
{
    PropertyFactoryFunction f = [=](const QString& localId) {
        auto p = GtPropertyFactory::instance()->newProperty(propertyClassName,
                                                          localId, localId);
        if (!p)
        {
            gtError() << "Cannot create property of type '" << propertyClassName << "'.";
            return static_cast<GtAbstractProperty*>(nullptr);
        }

        p->setValueFromVariant(defaultVal);
        return p;
    };

    members.push_back(MemberType{id, f});
}

void
GtPropertyStructDefinition::defineMember(const QString &id,
                                         PropertyFactoryFunction f)
{
    members.push_back(MemberType{id, std::move(f)});
}

std::unique_ptr<GtPropertyStructInstance>
GtPropertyStructDefinition::newInstance(QString name) const
{
    gt::poly_vector<GtAbstractProperty> subProps;

    for (const auto& member : members)
    {
        auto newProp = std::unique_ptr<GtAbstractProperty>(
            member.makeProperty(member.id));
        subProps.push_back(std::move(newProp));
    }

    return std::make_unique<GtPropertyStructInstance>(
        name, instanceTypeName, std::move(subProps));
}

void GtPropertyStructContainer::registerAllowedType(QString typeID, GtPropertyStructDefinition f)
{
    allowedTypes.insert(std::make_pair(typeID, f));
}

GtPropertyStructInstance &GtPropertyStructContainer::newEntry(QString name, QString typeID)
{
    const auto iter = allowedTypes.find(typeID);
    if (iter == allowedTypes.end())
    {
        throw GTlabException(
            "GtPropertyStructInstance::newEntry",
            QString("Cannot create a structure of type '%1'.").arg(typeID));
    }

    const auto& structureDefinition = iter->second;

    m_entries.push_back(structureDefinition.newInstance(name));

    return m_entries[m_entries.size() - 1];
}
