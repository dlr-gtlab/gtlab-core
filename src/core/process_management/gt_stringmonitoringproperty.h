/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_stringmonitoringpropety.h
 *
 *  Created on: 30.07.2021
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#ifndef GTSTRINGMONITORINGPROPETY_H
#define GTSTRINGMONITORINGPROPETY_H

#include "gt_core_exports.h"

#include "gt_stringproperty.h"
#include "gt_monitoringproperty.h"

/**
 * @brief The GtStringMonitoringProperty class
 */
class GT_CORE_EXPORT GtStringMonitoringProperty :
        public GtStringProperty, public GtMonitoringProperty
{
    Q_OBJECT
public:
    using GtProperty<QString>::operator=;

    /**
     * @brief GtStringMonitoringProperty
     * @param ident for the datamodel
     * @param name shown in the GUI
     */
    Q_INVOKABLE GtStringMonitoringProperty(const QString& ident,
                                           const QString& name);
    /**
     * @brief GtStringMonitoringProperty
     * @param ident for the datamodel
     * @param name shown in the GUI
     * @param brief description text
     */
    Q_INVOKABLE GtStringMonitoringProperty(const QString& ident,
                                           const QString& name,
                                           const QString& brief);
};

namespace gt
{

/**
 * @brief Creates a property factory for monitoring strings with a default value
 */
GT_CORE_EXPORT
gt::PropertyFactoryFunction makeStringMonitoringProperty(QString value = "");

} // namespace gt

#endif // GTSTRINGMONITORINGPROPETY_H
