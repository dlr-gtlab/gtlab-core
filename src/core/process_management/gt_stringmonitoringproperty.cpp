/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_stringmonitoringpropety.cpp
 *
 *  Created on: 30.07.2021
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include <QValidator>

#include "gt_stringmonitoringproperty.h"

GtStringMonitoringProperty::GtStringMonitoringProperty(const QString& ident,
                                                       const QString& name,
                                                       const QString& brief) :
    GtStringProperty(ident, name, brief)
{
    setMonitoring(true);
}

GtStringMonitoringProperty::GtStringMonitoringProperty(const QString& ident,
                                                       const QString& name) :
    GtStringMonitoringProperty(ident, name, QString())
{

}

gt::PropertyFactoryFunction
gt::makeStringMonitoringProperty(QString value)
{
    return makeMonitoring(makeStringProperty(value));
}
