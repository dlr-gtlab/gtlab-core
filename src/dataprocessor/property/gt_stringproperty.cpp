/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_stringproperties.cpp
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
    return gt::valueSuccess(getVal(), success);
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
