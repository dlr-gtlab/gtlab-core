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
#include "gt_regularexpression.h"

GtStringProperty::GtStringProperty(const QString& ident, const QString& name)
{
    setObjectName(name);

    m_id = ident;
    m_brief = QString();
    m_unitCategory = GtUnit::Category::None;
    m_value = QString();
    m_initValue = QString();
    m_validatorPattern = gt::rex::forExpressions();
}

GtStringProperty::GtStringProperty(const QString& ident,
                                   const QString& name,
                                   const QString& brief,
                                   const QString& value,
                                   const QRegularExpression& validationPattern)
{
    setObjectName(name);

    m_id = ident;
    m_brief = brief;
    m_unitCategory = GtUnit::Category::None;
    m_value = value;
    m_initValue = value;

    m_validatorPattern = validationPattern;
}

void
GtStringProperty::operator+=(const QString& b)
{
    QString newVal = getVal() + b;
    setVal(newVal);
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


QRegularExpression const&
GtStringProperty::validator()
{
    return m_validatorPattern;
}

GtStringProperty::~GtStringProperty() = default;

gt::PropertyFactoryFunction
gt::makeStringProperty(QString value)
{
    return makePropertyFactory<GtStringProperty>(std::move(value));
}
