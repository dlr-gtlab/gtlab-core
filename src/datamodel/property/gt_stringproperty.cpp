/* GTlab - Gas Turbine laboratory
 * Source File: gt_stringproperties.cpp
 * copyright 2009-2015 by DLR
 *
 *  Created on: 14.10.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include "gt_stringproperty.h"

GtStringProperty::GtStringProperty(const QString& ident, const QString& name)
{
    setObjectName(name);

    m_id = ident;
    m_brief = QString();
    m_unitCategory = GtUnit::Category::None;
    m_value = QString();
    m_initValue = QString();
}

GtStringProperty::GtStringProperty(const QString& ident,
                                   const QString& name,
                                   const QString& brief,
                                   const QString& value)
{
    setObjectName(name);

    m_id = ident;
    m_brief = brief;
    m_unitCategory = GtUnit::Category::None;
    m_value = value;
    m_initValue = value;
}

QVariant
GtStringProperty::valueToVariant(const QString& /*unit*/,
                                 bool* success) const
{
    if (success)
    {
        *success = true;
    }

    return getVal();
}

bool
GtStringProperty::setValueFromVariant(const QVariant& val,
                                      const QString& /*unit*/,
                                      bool* success)
{
    bool retval = true;

    setVal(val.toString(), success);

    if (success != 0)
    {
        retval = *success;
    }

    return retval;
}

GtStringProperty::~GtStringProperty()
{
}
