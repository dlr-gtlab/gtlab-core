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
