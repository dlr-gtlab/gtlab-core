/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_intmonitoringproperty.h
 *
 *  Created on: 14.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTINTMONITORINGPROPERTY_H
#define GTINTMONITORINGPROPERTY_H

#include "gt_core_exports.h"

#include "gt_intproperty.h"

/**
 * @brief The GtIntMonitoringProperty class
 */
class GT_CORE_EXPORT GtIntMonitoringProperty : public GtIntProperty
{
    Q_OBJECT

public:
    using GtProperty<int>::operator=;

    /**
     * @brief GtIntMonitoringProperty
     * @param ident
     * @param name
     */
    [[deprecated ("Use GtIntProperty and set the monitoring flag")]]
    Q_INVOKABLE GtIntMonitoringProperty(const QString& ident,
                                        const QString& name);

    /**
     * @brief GtIntMonitoringProperty
     * @param ident for the datamodel
     * @param name shown in the GUI
     * @param brief description text
     */
    [[deprecated ("Use GtIntProperty and set the monitoring flag")]]
    Q_INVOKABLE GtIntMonitoringProperty(const QString& ident,
                                        const QString& name,
                                        const QString& brief);
};


namespace gt
{

/**
 * @brief Creates a property factory for monitoring ints with a default value
 */
[[deprecated("Use makeMonitoring(makeIntProperty(value)) instead")]]
GT_CORE_EXPORT
gt::PropertyFactoryFunction makeIntMonitoringProperty(int value);

} // namespace gt

#endif // GTINTMONITORINGPROPERTY_H
