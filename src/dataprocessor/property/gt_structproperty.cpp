/* GTlab - Gas Turbine laboratory
 * copyright 2022 by DLR
 *
 *  Author: Martin Siggel (AT-TW)
 *  Tel.: +49 2203 601 2264
 */

#include "gt_structproperty.h"

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
                                         gt::PropertyFactoryFunction f)
{
    members.push_back(MemberType{id, std::move(f)});
}

std::unique_ptr<GtPropertyStructInstance>
GtPropertyStructDefinition::newInstance(QString name) const
{
    gt::PolyVector<GtAbstractProperty> subProps;

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
    gt::PolyVector<GtAbstractProperty>&& childProps) :
    GtGroupProperty(typeName, ""),
    m_childs(std::move(childProps))
{
    m_id = id;
    m_storeMemento = true;

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
        gtError() << tr("Cannot set member value. Member '%1' does not exist")
                     .arg(memberId);
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
        gtError() << tr("Cannot get member value. Member '%1' does not exist")
                     .arg(memberId);
        return gt::valueError(QVariant{}, okay);
    }

    return gt::valueSuccess(prop->valueToVariant(), okay);
}
