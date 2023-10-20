/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractcalculatorexecutor.h
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GTABSTRACTCALCULATOREXECUTOR_H
#define GTABSTRACTCALCULATOREXECUTOR_H

#include "gt_core_exports.h"

#include <QObject>

class GtCalculator;

/**
 * @brief The GtAbstractCalculatorExecutor class
 */
class GT_CORE_EXPORT GtAbstractCalculatorExecutor : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief exec
     * @return
     */
    virtual bool exec(GtCalculator* calculator) = 0;

protected:
    /**
     * @brief GtAbstractCalculatorExecutor
     */
    GtAbstractCalculatorExecutor();

};

#endif // GTABSTRACTCALCULATOREXECUTOR_H
