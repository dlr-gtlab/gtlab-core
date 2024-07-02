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

gt::PropertyFactoryFunction
gt::makeStringMonitoringProperty(QString value)
{
    auto makeString = [=](QString const& id)
    {
        auto* p = new GtStringProperty(id, id);      
        p->setMonitoring(true);
        p->setValueFromVariant(std::move(value), "");
        return p;
    };

    return makeString;
}
