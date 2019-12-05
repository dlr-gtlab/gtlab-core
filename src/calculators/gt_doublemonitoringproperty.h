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

#include "gt_calculators_exports.h"

#include "gt_doubleproperty.h"
#include "gt_monitoringproperty.h"

/**
 * @brief The GtDoubleMonitoringProperty class
 */
class GT_CALCULATORS_EXPORT GtDoubleMonitoringProperty :
        public GtDoubleProperty, public GtMonitoringProperty
{
    Q_OBJECT

public:
    /**
     * @brief GtDoubleMonitoringProperty
     * @param ident
     * @param name
     */
    Q_INVOKABLE GtDoubleMonitoringProperty(const QString& ident,
                                           const QString& name);

};

#endif // GTDOUBLEMONITORINGPROPERTY_H
