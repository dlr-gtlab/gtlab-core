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
#include "gt_stringmonitoringproperty.h"

GtStringMonitoringProperty::GtStringMonitoringProperty(const QString& ident,
                                                       const QString& name,
                                                       const QString& brief) :
    GtStringProperty(ident, name, brief)
{
    setMonitoring(true);

    // this additional registration is needed as long
    // as string monitoring properties are used in GTlab
    static auto initOnce = []() {
        GtAbstractProperty::registerConnectionCompatibility(
            GtStringMonitoringProperty::staticMetaObject,
            GtStringProperty::staticMetaObject,
            [](GtAbstractProperty const&,           // LCOV_EXCL_LINE
               GtAbstractProperty const&) -> bool { // LCOV_EXCL_LINE
                return true;
            });

        return 0;
    }();
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
