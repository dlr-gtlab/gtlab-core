/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 04.12.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_variantproperty.h"

GtVariantProperty::GtVariantProperty(const QString& name,
                                     const QString& brief,
                                     const GtUnit::Category& unitCategory,
                                     const QVariant& value) :
    GtVariantProperty({}, name, brief, unitCategory, value)
{ }

GtVariantProperty::GtVariantProperty(const QString& id,
                                     const QString& name,
                                     const QString& brief,
                                     const GtUnit::Category& unitCategory,
                                     const QVariant& value)
{
    setObjectName(name);

    m_id = id;
    m_brief = brief;
    m_unitCategory = unitCategory;
    m_value = value;
    m_initValue = value;
    m_storeMemento = false;
}

QVariant
GtVariantProperty::valueToVariant(const QString& unit,
                                  bool* success) const
{
    return getVal(unit, success);
}

bool
GtVariantProperty::setValueFromVariant(const QVariant& val,
                                       const QString& unit,
                                       bool* success)
{
    bool retval = true;

    setVal(val, unit, success);

    if (success)
    {
        retval = *success;
    }

    return retval;
}


gt::PropertyFactoryFunction
gt::makeVariantProperty(const GtUnit::Category &unitCategory)
{
    return [=](const QString& id) {
        return new GtVariantProperty(id, "", "", unitCategory);
    };
}
