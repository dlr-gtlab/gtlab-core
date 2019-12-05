/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCOREPROCESSEXECUTOR_H
#define GTCOREPROCESSEXECUTOR_H

#include "gt_core_exports.h"

#include <QObject>
#include <QPointer>

#include "gt_object.h"

class GtTask;
class GtAbstractRunnable;
class GtRunnable;
class GtCalculator;
class GtTaskRunner;

class GtObjectLinkProperty;

#define gtProcessExecutor (GtCoreProcessExecutor::instance())

class GT_CORE_EXPORT GtCoreProcessExecutor : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief GtProcessExecutor
     * @param parent QObject
     */
    GtCoreProcessExecutor(QObject* parent = Q_NULLPTR, bool save = true);

    /**
     * @brief currentInstance
     * @return
     */
    static GtCoreProcessExecutor* instance();

    /**
     * @brief Runs a process if the queue is free
     * @param process GtdProcess
     */
    bool runTask(GtTask* task);

    /**
     * @brief Terminates current running task.
     * @param Pointer to task which should be terminated.
     * @return True if task was successfully marked for termination.
     */
    bool terminateTask(GtTask* task);

    /**
     * @brief Terminates the current task and clears the task queue.
     * @return True if all tasks are marked for termination.
     */
    bool terminateAllTasks();

    /**
     * @brief processQueued
     * @param process
     * @return
     */
    bool taskQueued(GtTask* task);

    /**
     * @brief setSource
     * @param source
     * @return
     */
    bool setSource(GtObject* source);

    /**
     * @brief currentRunningTask
     * @return
     */
    GtTask* currentRunningTask();

    /**
     * @brief Returns true if a task is currently running. Otherwise
     * false is returned.
     * @return Whether a task is currently running or not.
     */
    bool taskCurrentlyRunning();

    /**
     * @brief queue
     * @return
     */
    QList<QPointer<GtTask> > queue();

   /**
    * @brief removeFromQueue
    * @param task
    */
   void removeFromQueue(GtTask* task);

   /**
    * @brief moveToIndex
    * @param task
    * @param pos
    */
   void moveTaskUp(GtTask* task);

   /**
    * @brief moveTaskDown
    * @param task
    */
   void moveTaskDown(GtTask* task);

protected:
    /// Process queue
    QList<QPointer<GtTask> > m_queue;

    /// Current process
    GtTask* m_current;

    /// source
    QPointer<GtObject> m_source;

    /// Pointer to current runnable
    QPointer<GtRunnable> m_currentRunnable;

    /**
     * @brief handleTaskFinishedHelper
     * @param changedData
     * @param task
     */
    virtual void handleTaskFinishedHelper(QList<GtObjectMemento>& changedData,
                                          GtTask* task);

    /**
     * @brief executeHelper
     * @return
     */
    GtTaskRunner* executeHelper();

private:
    ///
    static GtCoreProcessExecutor* m_self;

    /**
     * @brief execute
     */
    virtual void execute();

    /**
     * @brief Initialization
     */
    void init();

    /// save results is used as standard
    /// set to false for batch processes with saving results false to
    /// spend less time
    bool m_save;

private slots:
    /**
     * @brief handleTaskFinished
     */
    void handleTaskFinished();

signals:
    /**
     * @brief Emitted on changes in the queue
     */
    void queueChanged();

    /**
     * @brief Emitted after all task completed.
     */
    void allTasksCompleted();

};

#endif // GTCOREPROCESSEXECUTOR_H
