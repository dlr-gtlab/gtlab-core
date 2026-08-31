/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 16.07.2026
 *  Author: Code Generation
 */

#include "gt_doetask.h"
#include "gt_runnable.h"
#include <QThreadPool>
#include "gt_abstractrunnable.h"
#include "gt_calculator.h"
#include "gt_processcomponent.h"

GtDOETask::GtDOETask()
    : m_doeRunCount(QStringLiteral("doeRunCount"),
                   tr("Number of DOE Runs"),
                   tr("Total number of parallel DOE execution runs"),
                   gt::Boundaries<int>::makeLower(1), 100)
    , m_maxParallelRuns(QStringLiteral("maxParallelRuns"),
                        tr("Maximum Parallel Runs"),
                        tr("Maximum number of parallel execution channels"),
                        gt::Boundaries<int>::makeLower(1),
                        QThreadPool::globalInstance()->maxThreadCount())
{
    setObjectName(QStringLiteral("DOE Task"));

    enableMaxIterationProperty();
    enableCurrentIterationMonitoring();

    registerProperty(m_doeRunCount, tr("DOE Settings"));
    registerProperty(m_maxParallelRuns, tr("Execution"));
}

int
GtDOETask::doeRunCount() const
{
    return m_doeRunCount.getVal();
}

void
GtDOETask::setDoeRunCount(int count)
{
    m_doeRunCount.setVal(count);
}

int
GtDOETask::maxParallelRuns() const
{
    return m_maxParallelRuns.getVal();
}

void
GtDOETask::setMaxParallelRuns(int count)
{
    m_maxParallelRuns.setVal(count);
}

bool
GtDOETask::runIteration()
{
    return runChildElementsParallel();
}

bool
GtDOETask::runChildElementsParallel()
{
    int iteration = currentIterationStep();
    int totalRuns = doeRunCount();
    int maxParallel = maxParallelRuns();

    // increment current iteration step
    m_currentIter.setVal(m_currentIter.getVal() + 1);

    if (totalRuns <= 0)
    {
        return false;
    }

    QList<GtAbstractRunnable*> runnables = prepareChildRunnables(iteration, totalRuns, maxParallel);

    if (runnables.isEmpty())
    {
        return false;
    }

    bool success = executeRunnablesParallel(runnables, maxParallel);

    foreach (GtAbstractRunnable* r, runnables)
    {
        delete r;
    }

    return success;
}

QList<GtAbstractRunnable*>
GtDOETask::prepareChildRunnables(int iteration, int totalRuns, int maxParallel)
{
    Q_UNUSED(maxParallel)

    QList<GtAbstractRunnable*> runnables;

    QList<GtProcessComponent*> childs = processComponents();

    for (int i = 0; i < totalRuns; ++i)
    {
        auto* runnable = new GtRunnable();
        setupChildContext(runnable, iteration, i, totalRuns);

        foreach (GtProcessComponent* comp, childs)
        {
            auto* compCopy = qobject_cast<GtProcessComponent*>(comp->clone());
            if (compCopy)
            {
                runnable->appendProcessComponent(compCopy);
            }
        }

        runnable->setAutoDelete(false);
        runnables.append(runnable);
    }

    return runnables;
}

bool
GtDOETask::executeRunnablesParallel(QList<GtAbstractRunnable*>& runnables, int maxParallel)
{
    QThreadPool* threadPool = QThreadPool::globalInstance();

    if (!threadPool)
    {
        return false;
    }

    threadPool->setMaxThreadCount(qMin(maxParallel, runnables.size()));

    foreach (GtAbstractRunnable* r, runnables)
    {
        threadPool->start(r);
    }

    threadPool->waitForDone();

    bool success = true;

    foreach (GtAbstractRunnable* r, runnables)
    {
        if (!r->successful())
        {
            success = false;
            break;
        }
    }

    return success;
}

void
GtDOETask::setupChildContext(GtAbstractRunnable* runnable, int iteration, int runIndex, int totalRuns)
{
    if (!runnable)
    {
        return;
    }

    GtAbstractRunnable::DOEContext context;
    context.iteration = iteration;
    context.runIndex = runIndex;
    context.totalRuns = totalRuns;

    runnable->setDOEContext(context);
}
