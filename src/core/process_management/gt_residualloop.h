/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_residualloop.h
 *
 *  Created on: 15.11.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef GT_RESIDUALLOOP_H
#define GT_RESIDUALLOOP_H

#include "gt_loop.h"
#include "gt_doubleproperty.h"

/**
 * @brief The GtResidualLoop class
 */
class GT_CORE_EXPORT GtResidualLoop : public GtLoop
{
    Q_OBJECT

public:
    /**
     * @brief GtResidualLoop
     */
    Q_INVOKABLE GtResidualLoop();

    /**
     * @brief evaluate
     * @return
     */
    GtTask::EVALUATION evaluate() override;

private:
    /// residual value
    GtDoubleProperty m_residual1;

    /// residual value
    GtDoubleProperty m_residual2;

    /// residual value
    GtDoubleProperty m_residual3;

    /// residual value
    GtDoubleProperty m_residual4;

    /// residual value
    GtDoubleProperty m_residual5;

    /// residual value
    GtDoubleProperty m_precision;

    bool precisionReached();
};

#endif // GT_RESIDUALLOOP_H
