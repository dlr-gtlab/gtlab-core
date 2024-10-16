/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_parameterloop.h
 *
 *  Created on: 19.11.2018
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTPARAMETERLOOP_H
#define GTPARAMETERLOOP_H

#include "gt_core_exports.h"

#include "gt_task.h"

#include "gt_doubleproperty.h"
#include "gt_intproperty.h"

/**
 * @brief The GtParameterLoop class
 */
class GT_CORE_EXPORT GtParameterLoop : public GtTask
{
    Q_OBJECT
public:
    /**
     * @brief GtParameterLoop
     */
    Q_INVOKABLE GtParameterLoop();

    /**
     * @brief evaluate
     * @return
     */
    GtTask::EVALUATION evaluate() override;

    bool setUp() override;

private:
    bool runChildElements() override;

    GtDoubleProperty m_startVal;

    GtDoubleProperty m_endVal;

    GtIntProperty m_steps;

    GtDoubleProperty m_currentVal;

    /**
     * @brief value -  calculates the current value for the iteration step
     * @param iteration
     * @return
     */
    double value(int iteration);
};

#endif // GTPARAMETERLOOP_H
