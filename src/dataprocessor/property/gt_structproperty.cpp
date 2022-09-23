/* GTlab - Gas Turbine laboratory
 * copyright 2022 by DLR
 *
 *  Author: Martin Siggel (AT-TW)
 *  Tel.: +49 2203 601 2264
 */

#include "gt_structproperty.h"
#include "gt_propertyfactory.h"

GtPropertyStructDefinition::GtPropertyStructDefinition(QString typeName) :
    instanceTypeName(std::move(typeName))
{
}

QString
GtPropertyStructDefinition::typeName() const
{
    return instanceTypeName;
}

void
GtPropertyStructDefinition::defineMember(const QString& id,
                                         const QString& propertyClassName,
                                         QVariant defaultVal)
{
    PropertyFactoryFunction f = [=](const QString& localId) {
        auto p = GtPropertyFactory::instance()->newProperty(propertyClassName,
                                                            localId, localId);
        if (!p)
        {
            gtError().noquote().nospace() << "Cannot create property of type '"
                      << propertyClassName << "'.";
            return static_cast<GtAbstractProperty*>(nullptr);
        }

        p->setValueFromVariant(defaultVal);
        return p;
    };

    members.push_back(MemberType{id, f});
}

void
GtPropertyStructDefinition::defineMember(const QString& id,
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
        auto newProp =
            std::unique_ptr<GtAbstractProperty>(member.makeProperty(member.id));
        subProps.push_back(std::move(newProp));
    }

    return std::make_unique<GtPropertyStructInstance>(name, instanceTypeName,
                                                      std::move(subProps));
}

GtPropertyStructInstance::GtPropertyStructInstance(
    const QString& id, const QString& typeName,
    gt::poly_vector<GtAbstractProperty>&& childProps) :
    GtGroupProperty(typeName, ""),
    m_childs(std::move(childProps))
{
    m_id = id;

    for (auto& child : m_childs)
    {
        registerSubProperty(child);
    }
}

bool
GtPropertyStructInstance::setMemberVal(const QString& memberId,
                                       const QVariant& value)
{
    auto* prop = findProperty(memberId);
    if (!prop)
    {
        gtError().noquote().nospace() << "Cannot set member value. Member '"
                                      << memberId << "' does not exist";
        return false;
    }

    return prop->setValueFromVariant(value);
}

QVariant
GtPropertyStructInstance::getMemberValToVariant(const QString& memberId,
                                                bool* okay) const
{
    auto* prop = findProperty(memberId);
    if (!prop)
    {
        gtError().noquote().nospace() << "Cannot get member value. Member '"
                                      << memberId << "' does not exist";


        if (okay != nullptr)
            *okay = false;
        return QVariant{};
    }

    if (okay != nullptr)
        *okay = true;

    return prop->valueToVariant();
}
