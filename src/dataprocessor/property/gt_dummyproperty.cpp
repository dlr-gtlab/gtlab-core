/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_dummyproperty.h"

GtDummyProperty::GtDummyProperty(
    const GtObjectMemento::DynamicPropertyData& definition) :
    m_definition(definition)
{
    m_id = definition.ident;
    setObjectName(definition.name);
    setBrief(definition.brief);
    setReadOnly(true);
    setOptional(
        definition.hasFlag(GtObjectMemento::DynamicPropertyData::Optional));
    hide(false);
    setMonitoring(
        definition.hasFlag(GtObjectMemento::DynamicPropertyData::Monitoring));
    setCategory(definition.category);
}

QVariant
GtDummyProperty::valueToVariant(const QString&, bool* success) const
{
    if (success) *success = true;
    return tr("Unavailable");
}

bool
GtDummyProperty::setValueFromVariant(const QVariant& value, const QString&)
{
    Q_UNUSED(value);
    return false;
}

gt::PropertyConfiguration
GtDummyProperty::configuration() const
{
    return m_definition.configuration;
}

bool
GtDummyProperty::applyConfiguration(const gt::PropertyConfiguration& config)
{
    return config == m_definition.configuration;
}

const GtObjectMemento::DynamicPropertyData&
GtDummyProperty::originalDefinition() const
{
    return m_definition;
}

QVariant
GtDummyProperty::storedValue() const
{
    return m_definition.propertyData.data();
}
