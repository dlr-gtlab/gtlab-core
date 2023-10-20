/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_residualloop.cpp
 *
 *  Created on: 15.11.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#include "gt_residualloop.h"

GtResidualLoop::GtResidualLoop() :
    m_residual1("res1", "Residual 1", "Residual 1", GtUnit::None,
                0.0),
    m_residual2("res2", "Residual 2", "Residual 2", GtUnit::None,
                0.0),
    m_residual3("res3", "Residual 3", "Residual 3", GtUnit::None,
                0.0),
    m_residual4("res4", "Residual 4", "Residual 4", GtUnit::None,
                0.0),
    m_residual5("res5", "Residual 5", "Residual 5", GtUnit::None,
                0.0),
    m_precision("prec", "Precision", "Precision", GtUnit::None,
                0.0)
{
    registerProperty(m_precision, "Precision");
    registerProperty(m_residual1, "Redisuals");
    registerProperty(m_residual2, "Redisuals");
    registerProperty(m_residual3, "Redisuals");
    registerProperty(m_residual4, "Redisuals");
    registerProperty(m_residual5, "Redisuals");

    m_residual1.setOptional(true);
    m_residual2.setOptional(true);
    m_residual3.setOptional(true);
    m_residual4.setOptional(true);
    m_residual5.setOptional(true);

    setObjectName("Residual Loop");
}

GtTask::EVALUATION
GtResidualLoop::evaluate()
{
    if (m_currentIter < m_maxIter)
    {
        if (precisionReached())
        {
            return GtTask::EVAL_FINISHED;
        }
    }

    if (m_currentIter == m_maxIter)
    {
        if (precisionReached())
        {
            return GtTask::EVAL_FINISHED;
        }

        return GtTask::EVAL_FAILED;

    }

    return GtTask::EVAL_OK;
}

bool
GtResidualLoop::precisionReached()
{
    if (m_residual1.isActive())
    {
        if (m_residual1.getVal() > m_precision.getVal())
        {
            return false;
        }
    }

    if (m_residual2.isActive())
    {
        if (m_residual2.getVal() > m_precision.getVal())
        {
            return false;
        }
    }

    if (m_residual3.isActive())
    {
        if (m_residual3.getVal() > m_precision.getVal())
        {
            return false;
        }
    }

    if (m_residual4.isActive())
    {
        if (m_residual4.getVal() > m_precision.getVal())
        {
            return false;
        }
    }

    if (m_residual5.isActive())
    {
        if (m_residual5.getVal() > m_precision.getVal())
        {
            return false;
        }
    }

    return true;
}
