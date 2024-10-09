/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_intmonitoringproperty.cpp
 *
 *  Created on: 14.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_intmonitoringproperty.h"


GtIntMonitoringProperty::GtIntMonitoringProperty(
        const QString& ident, const QString& name) :
    GtIntProperty(ident, name)
{
    setMonitoring(true);
}

GtIntMonitoringProperty::GtIntMonitoringProperty(
        const QString& ident, const QString& name, const QString& brief) :
    GtIntProperty(ident, name, brief)
{
    setMonitoring(true);
}

gt::PropertyFactoryFunction
gt::makeIntMonitoringProperty(int value)
{
    auto makeInt = [=](QString const& id)
    {
        auto* p = new GtIntProperty(id, id);
        p->setMonitoring(true);
        p->setValueFromVariant(std::move(value), "");
        return p;
    };

    return makeInt;
}
