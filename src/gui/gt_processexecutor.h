/* GTlab - Gas Turbine laboratory
 * Source File: gt_processexecutor.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 26.08.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSEXECUTOR_H
#define GTPROCESSEXECUTOR_H

#include "gt_gui_exports.h"
#include "gt_coreprocessexecutor.h"

/**
 * @brief The GtProcessExecutor class
 */
class GtProcessExecutor : public GtCoreProcessExecutor
{
    Q_OBJECT

public:

    /// Id of this executor
    GT_GUI_EXPORT static const std::string S_ID;

    explicit GtProcessExecutor(QObject* parent = nullptr);

protected:

    /**
     * @brief handleTaskFinishedHelper
     * Uses a loading helper to analyze and apply diff
     *
     * @param changedData
     * @param task
     */
    void handleTaskFinishedHelper(QList<GtObjectMemento>& changedData,
                                  GtTask* task) override;

    /**
     * @brief Executes the current task. Non-blocking.
     */
    void execute() override;

    /**
     * @brief Terminates current running task.
     * @return Success
     */
    bool terminateCurrentTask() override;

private slots:

    /**
     * @brief onHelperFinished - reaction on finished signal of loading helper
     *
     * deletes the loading helper
     */
    void onHelperFinished();

private:

    /// Task
    GtTask* m_task;
};

#endif // GTPROCESSEXECUTOR_H
