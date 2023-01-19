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

GtStringProperty::GtStringProperty(const QString& ident, const QString& name) :
    m_validator(std::make_unique<QRegExpValidator>(gt::re::forExpressions()))
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
                                   const QString& value,
                                   QValidator* validator)
{
    setObjectName(name);

    m_id = ident;
    m_brief = brief;
    m_unitCategory = GtUnit::Category::None;
    m_value = value;
    m_initValue = value;

    if (validator)
    {
        m_validator.reset(validator);
    }
    else
    {
        m_validator = std::make_unique<QRegExpValidator>(
                    gt::re::forExpressions());
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
                                      const QString& /*unit*/)
{
    bool ok = true;
    setVal(val.toString(), &ok);

    return ok;
}


QValidator*
GtStringProperty::validator()
{
    return m_validator.get();
}

GtStringProperty::~GtStringProperty() = default;

gt::PropertyFactoryFunction
gt::makeStringProperty(QString value)
{
    return makePropertyFactory<GtStringProperty>(std::move(value));
}
