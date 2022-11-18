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
class GtRunnable;
class GtCalculator;
class GtTaskRunner;

class GtObjectLinkProperty;

#define gtProcessExecutor (GtProcessExecutorManager::instance())

namespace gt
{

/**
 * @brief The CoreProcessExecutorFlag enum
 * Only relevant for the core process executor
 */
enum CoreProcessExecutorFlag
{
    /// Do not save process results on successful execution
    DryExecution = 1,
    /// Whether as task runTask method should be blocking or not
    NonBlockingExecution = 2
};

}

class GT_CORE_EXPORT GtCoreProcessExecutor : public QObject
{
    Q_OBJECT

public:

    /// Id of this executor
    static const std::string S_ID;

    Q_DECLARE_FLAGS(Flags, gt::CoreProcessExecutorFlag)

    /**
     * @brief Constructor
     * @param id unique id to register and identify the executor
     * @param parent Parent object
     * @param flags Flags for the core process executor.
     */
    explicit GtCoreProcessExecutor(std::string id = S_ID,
                                   QObject* parent = {},
                                   Flags flags = {});

    /**
     * @brief Constructor
     * @param parent Parent object
     * @param flags Flags for the core process executor.
     */
    explicit GtCoreProcessExecutor(QObject* parent, Flags flags = {});

    /**
     * @brief Constructor
     * @param flags Flags for the core process executor.
     */
    explicit GtCoreProcessExecutor(Flags flags);

    /**
     * @brief Destructor
     */
    ~GtCoreProcessExecutor();

    /**
     * @brief Instance
     * @return instace
     */
    static GtCoreProcessExecutor* instance();

    /**
     * @brief Runs a process if the queue is free
     * @param process GtdProcess
     */
    bool runTask(GtTask* task);

    /**
     * @brief Executes the next task in the queue. No task must be running.
     * @return Whether task execution was successfully triggered
     */
    bool executeNextTask();

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
     * @brief Returns whether the task is queued
     * @param task Task to check
     * @return is task queued
     */
    bool taskQueued(GtTask* task) const;

    /**
     * @brief Returns the queue
     * @return queue
     */
    const QList<QPointer<GtTask>>& queue() const;

    /**
     * @brief Queues the task
     * @param task Task to queue
     * @return Success
     */
    bool queueTask(GtTask* task);

    /**
     * @brief Removes the task from the queue
     * @param task Task to unqueue
     * @return Success
     */
    void removeFromQueue(GtTask* task);

    /**
     * @brief Moves the task in the queue up
     * @param task Task to move up
     */
    void moveTaskUp(GtTask* task);

    /**
     * @brief Moves the task in the queue down
     * @param task Task to move down
     */
    void moveTaskDown(GtTask* task);

    /**
    * @brief setSource
    * @param source
    * @return success
    */
    bool setSource(GtObject* source);

    /**
    * @brief Sets a custom project path for the task execution.
    * @param projectPath Project path
    * @return success
    */
    bool setCustomProjectPath(QString projectPath);

signals:

    /**
     * @brief Emitted on changes in the queue
     */
    void queueChanged();

    /**
     * @brief Emitted after all task have been executed
     */
    void allTasksCompleted();

protected:

    /// Process queue
    QList<QPointer<GtTask>> m_queue;

    /// Current process
    QPointer<GtTask> m_current;

    /// Source/root object of data objects
    QPointer<GtObject> m_source;

    /**
     * @brief Executes the current task. Depending on the flags used when
     * instancing this executor, this call is Blocking by default.
     */
    virtual void execute();

    /**
     * @brief Virtual method to implement for terminating the current task.
     * Current task is guaranteed to be valid (not null)
     * @return success
     */
    virtual bool terminateCurrentTask();

    /**
     * @brief Virtual method to merge memento data
     * @param changedData Memento to merge
     * @param task Finished task
     */
    virtual void handleTaskFinishedHelper(QList<GtObjectMemento>& changedData,
                                          GtTask* task);

    void clearCurrentTask();

    /**
     * @brief Setup a task runner
     * @return Task runner pointer (null if setup failed)
     */
    GtTaskRunner* setupTaskRunner();

private:

    ///pimpl
    struct Impl;
    std::unique_ptr<Impl> pimpl;

private slots:

    /**
     * @brief Executed once the task runner has finished
     */
    void onTaskRunnerFinished();
};

/**
 * @brief Helper class for adding and selecting a Process Executor
 */
class GT_CORE_EXPORT GtProcessExecutorManager : public QObject
{
    Q_OBJECT

    /// private ctor
    GtProcessExecutorManager();

public:

    /**
     * @brief instance
     */
    static GtProcessExecutorManager& instance();

    /**
     * @brief Getter for the current selected process executor
     * @return executor
     */
    GtCoreProcessExecutor* currentExecutor();
    GtCoreProcessExecutor const* currentExecutor() const;

    /**
     * @brief operator GtCoreProcessExecutor*
     */
    operator GtCoreProcessExecutor*() { return currentExecutor(); }
    operator GtCoreProcessExecutor const*() const { return currentExecutor(); }

    /**
     * @brief operator ->
     * @return executor
     */
    GtCoreProcessExecutor* operator->() { return currentExecutor(); }
    GtCoreProcessExecutor const* operator->() const { return currentExecutor();}

    /**
     * @brief operator bool
     */
    explicit operator bool() const { return currentExecutor(); }

    /**
     * @brief Sets the current executor using its id. The Executor must be
     * present previously. If the current executor is not ready (e.g a task
     * is running) the function returns false and the executor is not switched
     * @param id Id of the executor to set as active
     * @return success
     */
    bool setCurrentExecutor(std::string const& id);

    /**
     * @brief Registers the executor specified. If no executor is
     * currently active, the selected executor will be updated as well.
     * An Executor can only be set once
     * @param id Id of the executor. Must be unique.
     * @param exec Executor to add
     * @return success
     */
    bool registerExecutor(std::string id, GtCoreProcessExecutor& exec);

    /**
     * @brief Removes the executor by its id. Executor must not be active.
     * @param id Id of the executor to remove
     * @return success
     */
    bool removeExecutor(std::string const& id);

    /**
     * @brief Returns whether the executor is already present
     * @param id Id of the executor
     * @return Is executor registered
     */
    bool hasExecutor(std::string const& id);

signals:

    /**
     * @brief Emitted once the process executor has changed.
     * @param New executor
     */
    void executorChanged(GtCoreProcessExecutor*);
};

#endif // GTCOREPROCESSEXECUTOR_H
