/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 06.12.2024
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_colorproperty.h"

GtColorProperty::GtColorProperty(const QString &ident,
                             const QString &name,
                             const QString &brief,
                             const QColor &color)
{
    m_id = ident;
    setObjectName(name);
    m_brief = brief;
    m_initValue = color;
    m_value = color;
}

QVariant
GtColorProperty::valueToVariant(const QString &unit, bool *ok) const
{
    return gt::valueSuccess(QVariant::fromValue(m_value.name()), ok);
}

bool
GtColorProperty::setValueFromVariant(const QVariant &val, const QString &unit)
{
    QString name = val.toString();

    if (name.isEmpty()) return false;

    setVal(name);

    return true;
}

GtColorProperty&
GtColorProperty::operator=(const QColor& val)
{
    setVal(val);
    emit changed();
    return *this;
}
