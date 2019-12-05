/* GTlab - Gas Turbine laboratory
 * Source File: gt_calculatorexecinterface
 * copyright 2009-2016 by DLR
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_CALCULATOREXECINTERFACE_H
#define GT_CALCULATOREXECINTERFACE_H

#include "gt_calculators_exports.h"

#include <QtPlugin>

class GtAbstractProperty;

/**
 * @brief The GtCalculatorExecInterface class
 */
class GT_CALCULATORS_EXPORT GtCalculatorExecInterface
{
public:
    /**
     * @brief Virtual destructor to keep compilers happy.
     */
    virtual ~GtCalculatorExecInterface() {}

    /**
     * @brief Returns executor identification string .
     * @return Executor identification string.
     */
    virtual QString executorId() = 0;

    /**
     * @brief Returns executor description.
     * @return Executor description.
     */
    virtual QString executorDescription() = 0;

    /**
     * @brief Returns meta object of executor.
     * @return Meta object of executor.
     */
    virtual QMetaObject executor() = 0;

    /**
     * @brief Returns list of executor settings.
     * @return Executor settings.
     */
    virtual QList<GtAbstractProperty*> executorSettings() = 0;

};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtCalculatorExecInterface,
                    "de.dlr.gtlab.GtCalculatorExecInterface/0.1")
QT_END_NAMESPACE

#endif // GT_CALCULATOREXECINTERFACE_H
