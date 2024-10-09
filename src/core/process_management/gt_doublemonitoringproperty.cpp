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


GtDoubleMonitoringProperty::GtDoubleMonitoringProperty(
        const QString& ident, const QString& name) :
    GtDoubleProperty(ident, name)
{
    setMonitoring(true);
}

GtDoubleMonitoringProperty::GtDoubleMonitoringProperty(
        const QString& ident, const QString& name, const QString& brief) :
    GtDoubleProperty(ident, name, brief)
{
    setMonitoring(true);
}

gt::PropertyFactoryFunction
gt::makeDoubleMonitoringProperty(double value)
{
    auto makeDouble = [=](QString const& id)
    {
        auto* p = new GtDoubleProperty(id, id);
        p->setMonitoring(true);
        p->setValueFromVariant(std::move(value), "");
        return p;
    };

    return makeDouble;
}
