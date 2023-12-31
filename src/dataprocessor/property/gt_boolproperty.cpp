/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_boolproperty.cpp
 *
 *  Created on: 14.10.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include "gt_boolproperty.h"

GtBoolProperty::GtBoolProperty(const QString& ident, const QString& name)
{
    setObjectName(name);

    m_id = ident;
    m_brief = QString();
    m_unitCategory = GtUnit::Category::None;
    m_initValue = false;
    m_value = false;
}

GtBoolProperty::GtBoolProperty(const QString& ident,
                               const QString &name,
                               const QString &brief,
                               bool value)
{
    setObjectName(name);

    m_id = ident;
    m_brief = brief;
    m_unitCategory = GtUnit::Category::None;
    m_initValue = value;
    m_value = value;
}

QVariant
GtBoolProperty::valueToVariant(const QString& unit,
                               bool* success) const
{
    return getVal(unit, success);
}

bool
GtBoolProperty::setValueFromVariant(const QVariant& val,
                                    const QString& /*unit*/)
{
    setVal(val.toBool());

    return true;
}


gt::PropertyFactoryFunction
gt::makeBoolProperty(bool value)
{
    return makePropertyFactory<GtBoolProperty>(std::move(value));
}

