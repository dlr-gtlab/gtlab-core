/* GTlab - Gas Turbine laboratory
 * Source File: gt_doublemonitoringproperty.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 14.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTDOUBLEMONITORINGPROPERTY_H
#define GTDOUBLEMONITORINGPROPERTY_H

#include "gt_core_exports.h"

#include "gt_doubleproperty.h"
#include "gt_monitoringproperty.h"

/**
 * @brief The GtDoubleMonitoringProperty class
 */
class GT_CORE_EXPORT GtDoubleMonitoringProperty :
        public GtDoubleProperty, public GtMonitoringProperty
{
    Q_OBJECT

public:

    using GtProperty<double>::operator=;

    /**
     * @brief GtDoubleMonitoringProperty
     * @param ident
     * @param name
     */
    Q_INVOKABLE GtDoubleMonitoringProperty(const QString& ident,
                                           const QString& name);

    /**
     * @brief GtDoubleMonitoringProperty
     * @param ident for the datamodel
     * @param name shown in the GUI
     * @param brief description text
     */
    Q_INVOKABLE GtDoubleMonitoringProperty(const QString& ident,
                                           const QString& name,
                                           const QString& brief);

};

namespace gt
{

/**
 * @brief Creates a property factory for monitoring doubles with a default value
 */
GT_CORE_EXPORT
gt::PropertyFactoryFunction makeDoubleMonitoringProperty(double value);

} // namespace gt

#endif // GTDOUBLEMONITORINGPROPERTY_H
