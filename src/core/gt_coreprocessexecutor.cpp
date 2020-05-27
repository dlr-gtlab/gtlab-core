/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>
#include <QString>
#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QEventLoop>

#include "gt_logging.h"
#include "gt_runnable.h"
#include "gt_project.h"
#include "gt_object.h"
#include "gt_calculator.h"
#include "gt_objectlinkproperty.h"
#include "gt_coredatamodel.h"
#include "gt_objectmemento.h"
#include "gt_task.h"
#include "gt_objectmementodiff.h"
#include "gt_coreapplication.h"
#include "gt_taskrunner.h"

#include "gt_coreprocessexecutor.h"

GtCoreProcessExecutor* GtCoreProcessExecutor::m_self = 0;

GtCoreProcessExecutor::GtCoreProcessExecutor(QObject* parent, bool save) :
    QObject(parent),
    m_save(save),
    m_current(Q_NULLPTR), m_source(Q_NULLPTR)
{
    // initialize
    init();

    qDebug() << "#### core process executor initialized!";
}

GtCoreProcessExecutor*
GtCoreProcessExecutor::instance()
{
    // return singleton instance
    return m_self;
}

bool
GtCoreProcessExecutor::runTask(GtTask* task)
{
    qDebug() << "GtCoreProcessExecutor::runTask";

    // check task
    if (task == Q_NULLPTR)
    {
        qDebug() << "process == NULL";
        return false;
    }

    if (task->hasDummyChildren())
    {
        gtError() << "Tasks with objects of unknown type cannot be executed!";
        return false;
    }

    // check whether queue already contains same task
    if (taskQueued(task))
    {
        gtError() << tr("process already in queue");
        return false;
    }

    qDebug() << "Appending Task to Queue:" << task->objectName();
    m_queue.append(task);

    // set state to QUEUED
    task->setStateRecursively(GtProcessComponent::QUEUED);

    // revert all monitoring properties
    task->resetMonitoringProperties();

    // run execution procedure
    execute();

    // emit queue chande
    emit queueChanged();

    // return success
    return true;
}

bool
GtCoreProcessExecutor::terminateTask(GtTask* task)
{
    if (task == Q_NULLPTR)
    {
        return false;
    }

    if (currentRunningTask() != task)
    {
        return false;
    }

    if (m_currentRunnable != Q_NULLPTR)
    {
        m_currentRunnable->requestInterruption();
    }

    return true;
}

bool
GtCoreProcessExecutor::terminateAllTasks()
{
    m_queue.clear();

    if (m_current != Q_NULLPTR)
    {
        terminateTask(m_current);
    }

    return true;
}

bool
GtCoreProcessExecutor::taskQueued(GtTask* task)
{
    gtDebug() << "GtCoreProcessExecutor::taskQueued";

    if (m_queue.contains(task))
    {
        return true;
    }

    return false;
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

GtTask*
GtCoreProcessExecutor::currentRunningTask()
{
    return m_current;
}

bool
GtCoreProcessExecutor::taskCurrentlyRunning()
{
    if (currentRunningTask() == Q_NULLPTR)
    {
        return false;
    }

    return true;
}

QList<QPointer<GtTask> >
GtCoreProcessExecutor::queue()
{
    return m_queue;
}

void
GtCoreProcessExecutor::removeFromQueue(GtTask *task)
{
    if (task == Q_NULLPTR)
    {
        return;
    }

    if (m_queue.contains(task))
    {
        m_queue.removeAll(task);
    }

    task->setState(GtTask::NONE);

    foreach(GtProcessComponent* comp, task->findChildren<GtProcessComponent*>())
    {
        comp->setState(GtProcessComponent::NONE);
    }

    emit queueChanged();
}

void
GtCoreProcessExecutor::moveTaskUp(GtTask *task)
{
    if (task == Q_NULLPTR)
    {
        return;
    }

    if (!m_queue.contains(task))
    {
        return;
    }

    if (task == m_queue.first())
    {
        return;
    }

    int currentIndex = m_queue.indexOf(task);

    m_queue.move(currentIndex, currentIndex - 1);

    emit queueChanged();
}

void
GtCoreProcessExecutor::moveTaskDown(GtTask *task)
{
    if (task == Q_NULLPTR)
    {
        return;
    }

    if (!m_queue.contains(task))
    {
        return;
    }

    if (task == m_queue.last())
    {
        return;
    }

    int currentIndex = m_queue.indexOf(task);

    m_queue.move(currentIndex, currentIndex + 1);

    emit queueChanged();
}

void
GtCoreProcessExecutor::init()
{
    if (m_self)
    {
        return;
    }

    m_self = this;
}

void
GtCoreProcessExecutor::execute()
{
    qDebug() << "GtCoreProcessExecutor::execute()";

    GtTaskRunner* runner = executeHelper();

    if (runner == Q_NULLPTR)
    {
        return;
    }

    QEventLoop eventLoop;

    connect(runner, &GtTaskRunner::destroyed,
            &eventLoop, &QEventLoop::quit);

    // run
    runner->run();

    eventLoop.exec();
}

void
GtCoreProcessExecutor::handleTaskFinishedHelper(
    QList<GtObjectMemento>& changedData, GtTask* /*task*/)
{
    qDebug() << "handleTaskFinishedHelper batch mode!";
    qDebug() << "changed data = " << changedData.size();
    qDebug() << "source = " << m_source;

    // source may be NULL if there are no object links defined in the
    // calculators included in the task
    if (m_source != Q_NULLPTR)
    {
        QDir tempDir;

        tempDir = gtApp->applicationTempDir();

        GtObjectMementoDiff sumDiff;

        qDebug() << "generating sum diff...";

        foreach (GtObjectMemento memento, changedData)
        {
            qDebug() << "analysing changed data...";
            GtObject* target = m_source->getObjectByUuid(memento.uuid());

            if (target != Q_NULLPTR)
            {
                qDebug() << "target found = " << target->objectName();
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
                qDebug() << "ERROR: target not found!";
            }
        }

//        qDebug() << "saving sum diff...";

//        QString filename = QStringLiteral("sumDiff.xml");

//        QFile file(tempDir.absoluteFilePath(filename));

//        if (file.open(QFile::WriteOnly))
//        {
//            QTextStream TextStream(&file);
//            TextStream << sumDiff.toByteArray();
//            file.close();
//        }

        if (!m_source->applyDiff(sumDiff))
        {
            qDebug() << "Diff not succesfully applied!";
        }
    }

    qDebug() << "handleTaskFinishedHelper end!";
}

GtTaskRunner*
GtCoreProcessExecutor::executeHelper()
{
    // check whether a task is already running
    if (m_current != Q_NULLPTR)
    {
        qDebug() << tr("a task is already running");
        return Q_NULLPTR;
    }

    // check whether queue is empty
    if (m_queue.isEmpty())
    {
        qDebug() << tr("--> Queue is Empty...No more tasks to execute!");
        emit allTasksCompleted();
        return Q_NULLPTR;
    }

    // checkout next task in queue
    m_current = m_queue.first();

    // setup source
    if (m_source == Q_NULLPTR)
    {
        // find project and put it into source
        m_source = m_current->findParent<GtProject*>();

        // check whether source is still a null pointer
        if (m_source == Q_NULLPTR)
        {
            gtError() << tr("Source corrupted!");
            return Q_NULLPTR;
        }
    }

    gtInfo() << "----> Running Task:" << m_current->objectName() << " <----";
    gtDebug() << "  |-> " << m_source->objectName();

    // create new task runner
    GtTaskRunner* runner = new GtTaskRunner(m_current);

    // create new runnable
    m_currentRunnable = new GtRunnable;

    // setup task runner
    if (!runner->setUp(m_currentRunnable, m_source))
    {
        delete m_currentRunnable;
        m_queue.removeAll(m_current);
        m_current = Q_NULLPTR;
        execute();
        emit queueChanged();
        return Q_NULLPTR;
    }

    // connect task runner signals
    connect(runner, &GtTaskRunner::finished,
            this, &GtCoreProcessExecutor::handleTaskFinished);

    return runner;
}

void
GtCoreProcessExecutor::handleTaskFinished()
{
    qDebug() << "GtCoreProcessExecutor::handleTaskFinished()";

    // create timer
    QElapsedTimer timer;

    // start timer
    timer.start();

    // cast sender object to task runner
    GtTaskRunner* taskRunner = qobject_cast<GtTaskRunner*>(sender());

    // check task runne rpointer
    if (taskRunner == Q_NULLPTR)
    {
        gtFatal() << tr("Sender not a task runner object!");
        return;
    }

    // check current task
    if (m_current == Q_NULLPTR)
    {
        gtFatal() << tr("Current task corrupted!");
        return;
    }

    GtTask* finishedTask = m_current;

    // remove current from queue
    m_queue.removeAll(m_current);

    QList<GtObjectMemento> changedData = taskRunner->dataToMerge();

    // free current task pointer
    m_current = Q_NULLPTR;

    if (m_save)
    {
        qDebug() << "|-> Finished Task:"
                 << finishedTask->objectName()
                 << " <----";
        qDebug() << "|-> Data to merge: " << changedData.size();

        // handle task post processing if finished successfully
        if (finishedTask->currentState() == GtProcessComponent::FINISHED ||
            finishedTask->currentState() == GtProcessComponent::WARN_FINISHED)
        {
            handleTaskFinishedHelper(changedData, finishedTask);
        }
    }

    qDebug() << "----> Task finished <----";
    qDebug() << "   |-> " << timer.elapsed();
    qDebug() << "";

    // reset source
    m_source = Q_NULLPTR;

    emit queueChanged();

    execute();

    // delete task runner
    taskRunner->deleteLater();
}

