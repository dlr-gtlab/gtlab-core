/* GTlab - Gas Turbine laboratory
 * Source File: gt_doublemonitoringproperty.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 14.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_doublemonitoringproperty.h"

GtDoubleMonitoringProperty::GtDoubleMonitoringProperty(const QString& ident,
                                                       const QString& name) :
    GtDoubleProperty(ident, name)
{
    setReadOnly(true);
}