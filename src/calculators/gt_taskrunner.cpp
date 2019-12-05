/* GTlab - Gas Turbine laboratory
 * Source File: gt_taskrunner.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 11.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QThreadPool>

#include "gt_logging.h"
#include "gt_processcomponent.h"
#include "gt_task.h"
#include "gt_abstractrunnable.h"
#include "gt_processdata.h"
#include "gt_objectlinkproperty.h"
#include "gt_processdata.h"
#include "gt_labeldata.h"

#include "gt_taskrunner.h"

GtTaskRunner::GtTaskRunner(GtTask* task) : m_task(task), m_runnable(Q_NULLPTR),
    m_source(Q_NULLPTR)
{
}

GtTaskRunner::~GtTaskRunner()
{
    qDebug() << "task runner deleted!";
}

bool
GtTaskRunner::setUp(GtAbstractRunnable* runnable, GtObject* source)
{
    // check runnable
    if (m_runnable != Q_NULLPTR)
    {
        // runable already exists
        gtFatal() << tr("Runnable already exists!");
        return false;
    }

    // check source
    if (source == Q_NULLPTR)
    {
        // no source set
        gtFatal() << tr("Source corrupted!");
        return false;
    }

    // check task
    if (m_task == Q_NULLPTR)
    {
        // no task set
        gtFatal() << tr("No task set!");
        return false;
    }

    // setup runnable
    m_runnable = runnable;

    // setup source
    m_source = source;

    // free data to merge list
    m_dataToMerge.clear();

    // clear component mapping
    m_componentMap.clear();

    // transfer source meta data to runnable
    foreach (GtObject* srcObj, m_source->findDirectChildren<GtObject*>())
    {
        if (qobject_cast<GtProcessData*>(srcObj))
        {
            continue;
        }

        if (qobject_cast<GtLabelData*>(srcObj))
        {
            continue;
        }

        m_runnable->appendSourceData(srcObj->toMemento());
    }

    GtTask* taskCopy = cloneTask();

    if (taskCopy == Q_NULLPTR)
    {
        m_task->setState(GtProcessComponent::FAILED);
        // could not cast task
        //        gtFatal() << tr("Task copy corrupted!");
        return false;
    }

    // append task to runnable
    m_runnable->appendProcessComponent(taskCopy);

    // make runnable not delete herself
    m_runnable->setAutoDelete(false);

    // connect runnable signals to task runner slots
    connect(m_runnable.data(), &GtAbstractRunnable::runnableFinished,
            this, &GtTaskRunner::handleRunnableFinished);

    return true;
}

void
GtTaskRunner::run()
{
    // check runnable
    if (m_runnable == Q_NULLPTR)
    {
        // runable already exists
        gtFatal() << tr("Runnable corrupted!");
        return;
    }

    // check source
    if (m_source == Q_NULLPTR)
    {
        // no source set
        gtFatal() << tr("Source corrupted!");
        return;
    }

    // get global thread pool instance
    QThreadPool* tp = QThreadPool::globalInstance();

    // start runnable
    tp->start(m_runnable);
}

const QList<GtObjectMemento>&
GtTaskRunner::dataToMerge()
{
    return m_dataToMerge;
}

GtTask*
GtTaskRunner::cloneTask()
{
    GtTask* retval = Q_NULLPTR;

    GtObject* obj = m_task->clone();

    if (obj == Q_NULLPTR)
    {
        // could not copy task
        gtFatal() << tr("Could not copy task!");
        return retval;
    }

    // cast copied object
    retval = qobject_cast<GtTask*>(obj);

    // check copy
    if (retval == Q_NULLPTR)
    {
        // delete object
        delete obj;
    }
    else
    {
        retval->setParent(m_runnable);

        // setup all elements
        if (!setupElements(m_task, retval))
        {
            // delete obj
            delete retval;
            retval = Q_NULLPTR;
        }
    }

    // return cloned task
    return retval;
}

bool
GtTaskRunner::setupElements(GtProcessComponent* orig,
                            GtProcessComponent* cloned)
{
    // check original process component
    if (orig == Q_NULLPTR)
    {
        return false;
    }

    // check cloned process component
    if (cloned == Q_NULLPTR)
    {
        return false;
    }

    // get full child list of original process component
    QList<GtProcessComponent*> origChilds =
        orig->findDirectChildren<GtProcessComponent*>();

    // get full child list of cloned process component
    QList<GtProcessComponent*> clonedChilds =
        cloned->findDirectChildren<GtProcessComponent*>();

    // compare size of both lists
    if (origChilds.size() != clonedChilds.size())
    {
        orig->setState(GtProcessComponent::FAILED);
        gtError() << tr("Task corrupted!") << QStringLiteral(" ") <<
                  tr("Could not connect elements!");
        return false;
    }

    // connect signals
    connect(cloned, &GtProcessComponent::stateChanged,
            orig, &GtProcessComponent::handleStateChanged);
    connect(cloned, &GtProcessComponent::transferMonitoringProperties,
            this, &GtTaskRunner::transferMonitoringProperties);

    // check whether process component is a task
    GtTask* tmpTaskOrig = qobject_cast<GtTask*>(orig);

    if (tmpTaskOrig != Q_NULLPTR)
    {
        // cast cloned process component to task
        GtTask* tmpTaskCloned = qobject_cast<GtTask*>(cloned);

        // check cloned task object
        if (tmpTaskCloned != Q_NULLPTR)
        {
            // connect task specific signals
            connect(tmpTaskCloned, &GtTask::monitoringDataTransfer,
                    tmpTaskOrig, &GtTask::onMonitoringDataAvailable);
            connect(tmpTaskCloned, &GtTask::triggerClearMonitoringData,
                    tmpTaskOrig, &GtTask::clearMonitoringData);
        }
    }

    // append to component mapping structure
    m_componentMap.insert(cloned, orig);

    // loop over children and setup them recursively
    for (int i = 0; i < origChilds.size(); i++)
    {
        if (!setupElements(origChilds[i], clonedChilds[i]))
        {
            return false;
        }
    }

    return true;
}

void
GtTaskRunner::handleRunnableFinished()
{
    qDebug() << "GtTaskRunner::handleRunnableFinished()";

    // check runnable
    if (m_runnable == Q_NULLPTR)
    {
        gtFatal() << tr("Runnable corrupted!");
        return;
    }

    disconnect(m_runnable.data(), &GtAbstractRunnable::runnableFinished,
               this, &GtTaskRunner::handleRunnableFinished);

    m_dataToMerge.append(m_runnable->outputData());

    delete m_runnable;

    if (m_task != Q_NULLPTR)
    {
        qDebug() << "monitoring data table size = " <<
                 m_task->monitoringDataSize();
    }

    emit finished();
}

void
GtTaskRunner::transferMonitoringProperties()
{
    // cast sender to process component
    GtProcessComponent* component = qobject_cast<GtProcessComponent*>(sender());

    // check process component
    if (component == Q_NULLPTR)
    {
        // not a process component
        return;
    }

    // check whether component is registered in mapping structure
    if (!m_componentMap.contains(component))
    {
        qWarning() << "component not found in mapping structure!";
        return;
    }

    // get original process component from mapping structure
    GtProcessComponent* orig = m_componentMap.value(component);

    // transfer all monitoring properties to original
    foreach (GtAbstractProperty* prop, component->monitoringProperties())
    {
        // find property in original
        GtAbstractProperty* origProp = orig->findProperty(prop->ident());

        // check original property
        if (origProp == Q_NULLPTR)
        {
            qWarning() << "original property not found!";
            return;
        }

        //    qDebug() << "   |-> setting property to " <<
        //                prop->valueToVariant().toDouble();

        // set original property to new value
        origProp->setValueFromVariant(prop->valueToVariant());
    }
}
