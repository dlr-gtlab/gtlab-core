/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 27.01.2016
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include "gt_processcomponent.h"

#include "gt_abstractrunnable.h"

GtAbstractRunnable::GtAbstractRunnable() : m_successfulRun(false)
{
}

GtAbstractRunnable::~GtAbstractRunnable()
{
    qDeleteAll(m_linkedObjects);
    m_linkedObjects.clear();
}

QList<GtObject*>
GtAbstractRunnable::linkedObjects()
{
    return m_linkedObjects;
}

void
GtAbstractRunnable::appendSourceData(const GtObjectMemento& memento)
{
    m_inputData << memento;

    //    const QString uuid = memento.uuid();

    //    if (!m_outputData.contains(uuid))
    //    {
    //        m_outputData.insert(uuid, memento);
    //    }
}

QList<GtObjectMemento>&
GtAbstractRunnable::outputData()
{
    return m_outputData;
}

bool
GtAbstractRunnable::appendProcessComponent(GtProcessComponent* comp)
{
    if (comp == Q_NULLPTR)
    {
        return false;
    }

    if (m_queue.contains(comp))
    {
        qDebug() << tr("Process component already contained in runnable queue");
        return false;
    }

    comp->setParent(this);
    m_queue.append(comp);

    return true;
}

QList<GtProcessComponent*>
GtAbstractRunnable::queue()
{
    return m_queue;
}

bool
GtAbstractRunnable::successful()
{
    return m_successfulRun;
}
