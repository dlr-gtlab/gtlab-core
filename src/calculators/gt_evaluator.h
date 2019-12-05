/* GTlab - Gas Turbine laboratory
 * Source File: gt_evaluator.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 07.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTEVALUATOR_H
#define GTEVALUATOR_H

#include "gt_calculators_exports.h"

#include "gt_processcomponent.h"

/**
 * @brief The GtEvaluator class
 */
class GT_CALCULATORS_EXPORT GtEvaluator : public GtProcessComponent
{
    Q_OBJECT

public:
    enum EVALUATION
    {
        EVAL_OK,
        EVAL_FAILED,
        EVAL_FINISHED
    };

    /**
     * @brief Runs the iteration process.
     * @return Returns true if iteration was successful, otherwise returns
     * false
     */
    bool run();

    /**
     * @brief Virtual method to implement an initial setup for the iteration
     * process.
     * @return Returns true if setup was successful, otherwise returns false
     */
    virtual bool setUp() = 0;

    /**
     * @brief Virtual method to implement an evaluation method for each
     * iteration step.
     * @return Evaluation state for current iteration step
     */
    virtual GtEvaluator::EVALUATION evaluate() = 0;

protected:
    /**
     * @brief Constructor.
     */
    GtEvaluator();

private:
    /// Current iteration
    int m_currentIter;

    /// Maximal number of iteration steps
    int m_maxIter;

};

#endif // GTEVALUATOR_H
