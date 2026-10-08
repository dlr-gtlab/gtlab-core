/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_doublemonitoringproperty.cpp
 *
 *  Created on: 14.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_doublemonitoringproperty.h"
#include "gt_propertyconversionregistry.h"

GtDoubleMonitoringProperty::GtDoubleMonitoringProperty(const QString& ident,
                                                       const QString& name,
                                                       const QString& brief) :
    GtDoubleProperty(ident, name, brief)
{
    setMonitoring(true);

    // this additional registration is needed as long
    // as double monitoring properties are used in GTlab
    // This old implementation of monitoring properties does not support units
    static auto initOnce = []() {
        GtPropertyConversionRegistry::registerConnectionCompatibility(
            GtDoubleMonitoringProperty::staticMetaObject,
            GtDoubleProperty::staticMetaObject,
            [](GtAbstractProperty const&, // LCOV_EXCL_LINE
               GtAbstractProperty const&) -> bool { return true; });

        return 0;
    }();
}

GtDoubleMonitoringProperty::GtDoubleMonitoringProperty(const QString& ident,
                                                       const QString& name) :
    GtDoubleMonitoringProperty(ident, name, QString())
{
    setMonitoring(true);
}

gt::PropertyFactoryFunction
gt::makeDoubleMonitoringProperty(double value)
{
    return makeMonitoring(makeDoubleProperty(value));
}
