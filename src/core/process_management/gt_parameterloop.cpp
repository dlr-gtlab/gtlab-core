/* GTlab - Gas Turbine laboratory
 * Source File: gt_parameterloop.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 19.11.2018
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_calculator.h"

#include "gt_parameterloop.h"

GtParameterLoop::GtParameterLoop() :
    m_startVal("start", "start"),
    m_endVal("end", "end"),
    m_steps("steps", "steps"),
    m_currentVal("currentVal", "currentVal")
{
    setObjectName("Parameter Loop");

    registerProperty(m_startVal);
    registerProperty(m_endVal);
    registerProperty(m_steps);

    registerMonitoringProperty(m_currentVal);
}

GtTask::EVALUATION
GtParameterLoop::evaluate()
{
    gtDebug() << "evaluate for step:" << m_currentIter;

    if (m_currentIter < m_maxIter)
    {
        gtDebug() << "current Value:" << value(m_currentIter);
        m_currentVal.setVal(value(m_currentIter));
    }
    else
    {
        return GtTask::EVAL_FINISHED;
    }

    return GtTask::EVAL_OK;
}

bool
GtParameterLoop::setUp()
{
    gtDebug() << "initial run with value:" << value(m_currentIter);
    m_currentVal.setVal(value(m_currentIter));

    m_maxIter.setVal(m_steps + 1);

    return true;
}

double
GtParameterLoop::value(int iteration)
{
    double stepSize = (m_endVal - m_startVal) / m_steps;

    return m_startVal + stepSize * iteration;

}

bool
GtParameterLoop::runChildElements()
{
    QList<GtProcessComponent*> childs = processComponents();

    // increment current iteration step and continue iteration
    m_currentIter.setVal(m_currentIter.getVal() + 1);

    // trigger transfer of monitoring properties before running calculators
    emit transferMonitoringProperties();

    // reset state of child prcess elements
    foreach (GtProcessComponent* comp, childs)
    {
        comp->setStateRecursively(GtProcessComponent::QUEUED);
    }

    // run calculators
    foreach (GtProcessComponent* comp, childs)
    {
        GtCalculator* calc = qobject_cast<GtCalculator*>(comp);

        if (!comp->exec())
        {

            if (calc)
            {
            // calculator run failed
            //setState(GtProcessComponent::FAILED);
            //
            //return false;
            gtWarning() << "Current Step failed";
            break;
            }

        }

        if (isInterruptionRequested())
        {
            gtWarning() << "task terminated!";
            setState(GtProcessComponent::TERMINATED);
            return false;
        }

        //GtCalculator* calc = qobject_cast<GtCalculator*>(comp);

        if (calc && calc->runFailsOnWarning())
        {
            if (calc->currentState() == GtProcessComponent::WARN_FINISHED)
            {
                calc->setState(FAILED);
                setState(GtProcessComponent::FAILED);
                return false;
            }
        }
    }

    // evaluate current iteration step
    m_lastEval = evaluate();

    // trigger transfer of monitoring properties after evaluation
    emit transferMonitoringProperties();

    // collect monitoring data for entire task
    GtMonitoringDataSet monData = collectMonitoringData();

    // check whether monitoring data has entries
    if (!monData.isEmpty())
    {
        // monitoring data available - emit signal
        emit monitoringDataTransfer(m_currentIter, monData);
    }

    return true;
}
