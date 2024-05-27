/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDir>
#include <QElapsedTimer>
#include <QEventLoop>

#include "gt_logging.h"
#include "gt_runnable.h"
#include "gt_project.h"
#include "gt_object.h"
#include "gt_objectmemento.h"
#include "gt_task.h"
#include "gt_objectmementodiff.h"
#include "gt_coreapplication.h"
#include "gt_taskrunner.h"

#include "gt_coreprocessexecutor.h"

const std::string GtCoreProcessExecutor::S_ID = "CoreProcessExecutor";

struct GtCoreProcessExecutor::Impl
{
    /// save results is used as standard
    /// set to false for batch processes with saving results false to
    /// spend less time
    bool save{true};

    /// inidcates whether executing a processes should block until finished
    /// (false = default behaviour for core)
    bool detached{false};

    /// custom project path for task execution
    QString customProjectPath;

    /// Pointer to current runnable
    QPointer<GtRunnable> currentRunnable;
};

GtCoreProcessExecutor::GtCoreProcessExecutor(QObject* parent, Flags flags) :
    QObject(parent),
    m_current(nullptr),
    m_source(nullptr),
    pimpl(std::make_unique<Impl>())
{
    setObjectName("CoreProcessExecutor");

    setCoreExecutorFlags(flags);
}

GtCoreProcessExecutor::~GtCoreProcessExecutor() = default;

void
GtCoreProcessExecutor::setCoreExecutorFlags(Flags flags)
{
    pimpl->save     = !flags.testFlag(gt::DryExecution);
    pimpl->detached =  flags.testFlag(gt::NonBlockingExecution);
}

bool
GtCoreProcessExecutor::runTask(GtTask* task)
{
    gtDebugId(GT_EXEC_ID).medium() << __FUNCTION__;

    if (!queueTask(task))
    {
        return false;
    }

    if (taskCurrentlyRunning())
    {
        return true;
    }

    return executeNextTask();
}

bool
GtCoreProcessExecutor::executeNextTask()
{
    // check whether a task is already running
    if (taskCurrentlyRunning())
    {
        gtErrorId(GT_EXEC_ID) << tr("A Task is already running!");
        return false;
    }

    // check whether queue is empty
    if (m_queue.isEmpty())
    {
        emit allTasksCompleted();
        return false;
    }

    // checkout next task in queue
    m_current = m_queue.first();

    // double check task
    if (!m_current || m_current->hasDummyChildren())
    {
        gtErrorId(GT_EXEC_ID) << tr("Cannot execute an invalid Task!");
        clearCurrentTask();
        return false;
    }

    // setup source
    if (!m_source)
    {
        // find project and put it into source
        m_source = m_current->findParent<GtProject*>();

        // check whether source is still a null pointer
        if (!m_source)
        {
            gtErrorId(GT_EXEC_ID) << tr("Source corrupted!");
            clearCurrentTask();
            return false;
        }
    }

    emit queueChanged();

    execute();

    return true;
}

bool
GtCoreProcessExecutor::terminateTask(GtTask* task)
{
    if (!m_current || m_current != task)
    {
        gtWarningId(GT_EXEC_ID) << tr("Invalid task to terminate!");
        return false;
    }

    return terminateCurrentTask();
}

bool
GtCoreProcessExecutor::terminateAllTasks()
{
    m_queue.clear();

    return !taskCurrentlyRunning() || terminateTask(m_current);
}

GtTask*
GtCoreProcessExecutor::currentRunningTask()
{
    return m_current;
}

bool
GtCoreProcessExecutor::taskCurrentlyRunning()
{
    return currentRunningTask();
}

bool
GtCoreProcessExecutor::taskQueued(GtTask* task) const
{
    return m_queue.contains(task);
}

const QList<QPointer<GtTask>>&
GtCoreProcessExecutor::queue() const
{
    return m_queue;
}

bool
GtCoreProcessExecutor::queueTask(GtTask* task)
{
    if (!task)
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Null Task!");
        return false;
    }

    if (task->hasDummyChildren())
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Tasks with objects of unknown type cannot be queued!");
        return false;
    }

    if (taskQueued(task))
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Task has already been queued!");
        return false;
    }

    gtDebugId(GT_EXEC_ID)
            << tr("Appending Task '%1' to queue").arg(task->objectName());

    // setup task for queue
    task->setStateRecursively(GtProcessComponent::QUEUED);
    task->resetMonitoringProperties();

    m_queue.append(task);
    emit queueChanged();

    return true;
}

void
GtCoreProcessExecutor::removeFromQueue(GtTask *task)
{
    if (taskQueued(task))
    {
        m_queue.removeAll(task);

        if (task) task->setStateRecursively(GtTask::NONE);

        emit queueChanged();
    }
}

void
GtCoreProcessExecutor::moveTaskUp(GtTask *task)
{
    int idx = m_queue.indexOf(task);

    if (idx > 0)
    {
        m_queue[idx].swap(m_queue[idx - 1]);

        emit queueChanged();
    }
}

void
GtCoreProcessExecutor::moveTaskDown(GtTask *task)
{
    int idx = m_queue.indexOf(task);

    if (idx >= 0 && idx < m_queue.size() - 1)
    {
        m_queue[idx].swap(m_queue[idx + 1]);

        emit queueChanged();
    }
}

bool
GtCoreProcessExecutor::setSource(GtObject* source)
{
    if (!m_queue.isEmpty())
    {
        return false;
    }

    m_source = source;

    return true;
}

bool
GtCoreProcessExecutor::setCustomProjectPath(QString projectPath)
{
    if (!m_queue.isEmpty())
    {
        return false;
    }

    pimpl->customProjectPath = std::move(projectPath);
    return true;
}

bool
GtCoreProcessExecutor::terminateCurrentTask()
{
    if (!pimpl->currentRunnable)
    {
        return false;
    }

    pimpl->currentRunnable->requestInterruption();
    return true;
}

void
GtCoreProcessExecutor::execute()
{
    gtDebugId(GT_EXEC_ID).medium() << __FUNCTION__;

    if (auto* runner = setupTaskRunner())
    {
        QEventLoop eventLoop;

        connect(runner, &QObject::destroyed, &eventLoop, &QEventLoop::quit);

        // run
        runner->run();

        // wait for event loop to finish
        if (!pimpl->detached)
        {
            eventLoop.exec();
        }
    }
}

void
GtCoreProcessExecutor::handleTaskFinishedHelper(
        QList<GtObjectMemento>& changedData, GtTask* task)
{
    assert(task);

    gtDebugId(GT_EXEC_ID).medium() << __FUNCTION__ << "(batch mode)";
    gtDebugId(GT_EXEC_ID).medium() << "changed data =" << changedData.size();
    gtDebugId(GT_EXEC_ID).medium() << "source =" << m_source;

    bool ok = true;

    // source may be NULL if there are no object links defined in the
    // calculators included in the task
    if (m_source)
    {
        QDir tempDir;

        tempDir = gtApp->applicationTempDir();

        GtObjectMementoDiff sumDiff;

        gtDebugId(GT_EXEC_ID).medium() << "generating sum diff...";

        for (GtObjectMemento const& memento : qAsConst(changedData))
        {
            gtDebugId(GT_EXEC_ID).medium() << "analysing changed data...";

            GtObject* target = m_source->getObjectByUuid(memento.uuid());

            if (target)
            {
                gtDebugId(GT_EXEC_ID).medium()
                        << "target found =" << target->objectName();

                GtObjectMemento old = target->toMemento(true);
                GtObjectMementoDiff diff(old, memento);

                QString filename = target->objectName() +
                                   QStringLiteral(".xml");

                QFile file(tempDir.absoluteFilePath(filename));

                if (file.open(QFile::WriteOnly))
                {
                    QTextStream TextStream(&file);
                    TextStream << diff.toByteArray();
                    TextStream << "######### new";
                    TextStream << memento.toByteArray();
                    TextStream << "######### old";
                    TextStream << old.toByteArray();
                    file.close();
                }

                sumDiff << diff;
            }
            else
            {
                gtWarningId(GT_EXEC_ID) << tr("Target for memento diff not found");
                ok = false;
            }
        }

        if (!m_source->applyDiff(sumDiff))
        {
            gtWarningId(GT_EXEC_ID) << tr("Failed to apply memento diff!");
            ok = false;
        }
    }

    if (!ok)
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Data changes from the task '%1' could not be merged "
                      "back into datamodel!").arg(task->objectName());
        task->setState(GtProcessComponent::FAILED);
    }

    gtDebugId(GT_EXEC_ID).medium() << __FUNCTION__ << "end";
}

void
GtCoreProcessExecutor::clearCurrentTask()
{
    gtDebugId(GT_EXEC_ID).medium() << __FUNCTION__;

    if (m_current)
    {
        using STATE =  GtProcessComponent::STATE;

        STATE state = m_current->currentState();

        // mark as terminated or failed if process element is not ready
        if (state == STATE::TERMINATION_REQUESTED) state = STATE::TERMINATED;
        else if (!m_current->isComponentReady()) state = STATE::FAILED;

        m_current->setState(state);

        auto const pcs = m_current->findChildren<GtProcessComponent*>();

        for (auto* pc : pcs)
        {
            // queued elements were not executed effectively skipping them
            if (pc->currentState() == STATE::QUEUED)
            {
                pc->setState(STATE::SKIPPED);
            }
            // mark any component, that is not ready as failed
            else if (!pc->isComponentReady())
            {
                pc->setState(state);
            }
        }
    }

    // remove from queue
    m_queue.removeAll(m_current);
    m_current = nullptr;
    emit queueChanged();
}

GtTaskRunner*
GtCoreProcessExecutor::setupTaskRunner()
{
    assert(m_current);
    assert(m_source);

    gtInfoId(GT_EXEC_ID)
        << tr("----> Running Task '%1' (source: %2) <----")
               .arg(m_current->objectName(),
                    m_source->objectName());

    // create new task runner
    auto* runner = new GtTaskRunner{m_current};

    // create new runnable
    pimpl->currentRunnable = new GtRunnable{pimpl->customProjectPath};

    // setup task runner
    if (!runner->setUp(pimpl->currentRunnable, m_source))
    {
        delete pimpl->currentRunnable;
        delete runner;
        clearCurrentTask();
        executeNextTask();
        return nullptr;
    }

    // connect task runner signals
    connect(runner, &GtTaskRunner::finished,
            this, &GtCoreProcessExecutor::onTaskRunnerFinished);

    return runner;
}

void
GtCoreProcessExecutor::onTaskRunnerFinished()
{
    gtTrace() << __FUNCTION__;

    // create timer
    QElapsedTimer timer;

    // start timer
    timer.start();

    // cast sender object to task runner
    auto* taskRunner = qobject_cast<GtTaskRunner*>(sender());

    // check task runne rpointer
    if (!taskRunner)
    {
        gtFatalId(GT_EXEC_ID) << tr("Sender not a task runner object!");
        return;
    }

    // check current task
    if (!m_current)
    {
        gtFatalId(GT_EXEC_ID) << tr("Current task corrupted!");
        taskRunner->deleteLater();
        return;
    }

    GtTask* finishedTask = m_current;

    clearCurrentTask();

    QList<GtObjectMemento> changedData = taskRunner->dataToMerge();

    if (pimpl->save)
    {
        gtInfoId(GT_EXEC_ID).medium()
            << tr("Finished task execution!")
            << gt::brackets(finishedTask->objectName());

        // handle task post processing if finished successfully
        if (finishedTask->currentState() == GtProcessComponent::FINISHED ||
            finishedTask->currentState() == GtProcessComponent::WARN_FINISHED)
        {
            handleTaskFinishedHelper(changedData, finishedTask);
        }
    }

    gtInfoId(GT_EXEC_ID).medium()
        << tr("----> Task finished (took %1 ms to merge) <----")
               .arg(timer.elapsed());

    // reset source
    m_source.clear();

    // delete task runner
    taskRunner->deleteLater();

    executeNextTask();
}
