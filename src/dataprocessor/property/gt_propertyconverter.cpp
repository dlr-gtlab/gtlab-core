/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 08.10.2026
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_propertyconverter.h"

GtPropertyConverter::GtPropertyConverter(
    QMetaObject from, QMetaObject to, gt::conversion::convert conversion,
    gt::conversion::isCompatible isCompatible) :
    m_from(from), m_to(to), m_conversion(conversion),
    m_isCompatibleFunc(isCompatible)
{
}

QString
GtPropertyConverter::fromClassName() const
{
    return m_from.className();
}

QString
GtPropertyConverter::toClassName() const
{
    return m_to.className();
}

gt::conversion::isCompatible
GtPropertyConverter::isCompatibletFunc() const
{
    return m_isCompatibleFunc;
}

gt::conversion::convert
GtPropertyConverter::convertFunc() const
{
    return m_conversion;
}

bool
GtPropertyConverter::isCompatibleDefined() const
{
    if (m_isCompatibleFunc) return true;
    return false;
}

bool
GtPropertyConverter::conversionDefined() const
{
    if (m_conversion) return true;
    return false;
}
