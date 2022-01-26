/* GTlab - Gas Turbine laboratory
 * Source File: gt_stringproperties.cpp
 * copyright 2009-2015 by DLR
 *
 *  Created on: 14.10.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include "gt_stringproperty.h"
#include <QValidator>
#include "gt_regexp.h"

GtStringProperty::GtStringProperty(const QString& ident, const QString& name)
{
    setObjectName(name);

    m_id = ident;
    m_brief = QString();
    m_unitCategory = GtUnit::Category::None;
    m_value = QString();
    m_initValue = QString();

    m_validator = new QRegExpValidator(
                GtRegExp::forExpressions(), this);
}

GtStringProperty::GtStringProperty(const QString& ident,
                                   const QString& name,
                                   const QString& brief,
                                   const QString& value,
                                   QValidator* validator)
{
    setObjectName(name);

    m_id = ident;
    m_brief = brief;
    m_unitCategory = GtUnit::Category::None;
    m_value = value;
    m_initValue = value;

    if (validator != nullptr)
    {
        m_validator = validator;
        m_validator->setParent(this);
    }
    else
    {
        m_validator = new QRegExpValidator(
                    GtRegExp::forExpressions(), this);
    }
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

    if (success != nullptr)
    {
        retval = *success;
    }

    return retval;
}

GtStringProperty::~GtStringProperty()
{
    if (m_validator != nullptr)
    {
        delete m_validator;
    }
}

QValidator*
GtStringProperty::validator()
{
    return m_validator;
}
