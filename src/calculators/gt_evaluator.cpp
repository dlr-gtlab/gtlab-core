/* GTlab - Gas Turbine laboratory
 * Source File: gt_evaluator.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 07.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_evaluator.h"

GtEvaluator::GtEvaluator() : m_currentIter(0), m_maxIter(0)
{

}

bool
GtEvaluator::run()
{
    // Initialize individual evaluator setting
    if (!setUp())
    {
        return false;
    }

    // check max. iteration steps
    if (m_maxIter <= 0)
    {
        return false;
    }

    // reset evaluator variables
    m_currentIter = 1;

    // start iteration
    while (m_currentIter < m_maxIter)
    {
        // run calculators

        // evaluate current iteration step
        GtEvaluator::EVALUATION res = evaluate();

        // check evaluation result
        switch (res)
        {
            case GtEvaluator::EVAL_FINISHED:
            {
                // iteration finished successfully
                return true;
            }

            case GtEvaluator::EVAL_FAILED:
            {
                // iteration failed
                return false;
            }

            default:
                break;
        }

        // increment current iteration step and continue iteration
        m_currentIter++;
    }

    // max. number of iteration steps reached
    return true;
}
