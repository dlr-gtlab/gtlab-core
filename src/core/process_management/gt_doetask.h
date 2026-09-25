/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 16.07.2026
 *  Author: Code Generation
 */

#ifndef GTDOETASK_H
#define GTDOETASK_H

#include "gt_core_exports.h"
#include "gt_task.h"
#include "gt_intproperty.h"

class GT_CORE_EXPORT GtDOETask : public GtTask
{
    Q_OBJECT

public:
    Q_INVOKABLE GtDOETask();

    bool runIteration() override;

    int doeRunCount() const;
    void setDoeRunCount(int count);

    int maxParallelRuns() const;
    void setMaxParallelRuns(int count);

    bool runChildElementsParallel();

private:
    GtIntProperty m_doeRunCount;
    GtIntProperty m_maxParallelRuns;

    QList<GtAbstractRunnable*> prepareChildRunnables(int iteration, int totalRuns, int maxParallel);
    bool executeRunnablesParallel(QList<GtAbstractRunnable*>& runnables, int maxParallel);
    void setupChildContext(GtAbstractRunnable* runnable, int iteration, int runIndex, int totalRuns);
};

#endif // GTDOETASK_H
