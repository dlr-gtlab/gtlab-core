/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 11.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSINTERFACE_H
#define GTPROCESSINTERFACE_H

#include "gt_core_exports.h"

#include <QtPlugin>

#include "gt_calculatordata.h"
#include "gt_taskdata.h"

class GtCalculatorDataImpl;

/**
 * @brief The GtProcessInterface class
 */
class GT_CORE_EXPORT GtProcessInterface
{
public:
    /** Virtual destructor to keep compilers happy */
    virtual ~GtProcessInterface() {}

    /**
     * @brief Returns static meta objects of calculator classes.
     * @return list including meta objects
     */
    virtual QList<GtCalculatorData> calculators() {
        return {};
    }

    /**
     * @brief Returns static meta objects of task classes.
     * @return list including meta objects
     */
    virtual QList<GtTaskData> tasks() {
        return {};
    }

};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtProcessInterface,
                    "de.dlr.gtlab.GtProcessInterface/0.2")
QT_END_NAMESPACE

#endif // GTPROCESSINTERFACE_H
