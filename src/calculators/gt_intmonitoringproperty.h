/* GTlab - Gas Turbine laboratory
 * Source File: gt_intmonitoringproperty.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 14.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTINTMONITORINGPROPERTY_H
#define GTINTMONITORINGPROPERTY_H

#include "gt_calculators_exports.h"

#include "gt_intproperty.h"
#include "gt_monitoringproperty.h"

/**
 * @brief The GtIntMonitoringProperty class
 */
class GT_CALCULATORS_EXPORT GtIntMonitoringProperty :
        public GtIntProperty, public GtMonitoringProperty
{
    Q_OBJECT

public:
    /**
     * @brief GtIntMonitoringProperty
     * @param ident
     * @param name
     */
    Q_INVOKABLE GtIntMonitoringProperty(const QString& ident,
                                        const QString& name);

};

#endif // GTINTMONITORINGPROPERTY_H
