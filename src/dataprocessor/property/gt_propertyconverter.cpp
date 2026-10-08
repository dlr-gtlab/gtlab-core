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
    QMetaObject from,
    QMetaObject to,
    gt::conversion::convert conversion,
    gt::conversion::canConnect canConnect) :
    m_from(from),
    m_to(to),
    m_conversion(conversion),
    m_canConnect(canConnect)
{

}

GtPropertyConverter::GtPropertyConverter(
    QMetaObject from,
    QMetaObject to,
    gt::conversion::convert conversion) :
    GtPropertyConverter(from, to, conversion, {})
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

gt::conversion::canConnect
GtPropertyConverter::canConnectFunc() const
{
    return m_canConnect;
}

bool
GtPropertyConverter::canConnectDefined() const
{
    if (m_canConnect) return true;
    return false;
}

bool
GtPropertyConverter::canConversionDefined() const
{
    if (m_conversion) return true;
    return false;
}



