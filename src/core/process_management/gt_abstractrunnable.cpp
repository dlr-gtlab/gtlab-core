/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
}

const QList<GtObject*>&
GtAbstractRunnable::linkedObjects() const
{
    return m_linkedObjects;
}

void
GtAbstractRunnable::setLinkedObjects(const QList<GtObject*>& objects)
{
    m_linkedObjects = objects;
}

void
GtAbstractRunnable::addLinkedObject(GtObject* object)
{
    m_linkedObjects.append(object);
}

void
GtAbstractRunnable::setInputData(const QList<GtObjectMemento>& data)
{
    m_inputData = data;
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

bool
GtAbstractRunnable::appendProcessComponent(GtProcessComponent* comp)
{
    if (!comp)
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

const QList<GtProcessComponent*>&
GtAbstractRunnable::queue() const
{
    return m_queue;
}

bool
GtAbstractRunnable::successful()
{
    return m_successfulRun;
}

void
GtAbstractRunnable::setDOEContext(const DOEContext& context)
{
    m_doeContext = context;
}

GtAbstractRunnable::DOEContext
GtAbstractRunnable::doeContext() const
{
    return m_doeContext;
}

const QList<GtObjectMemento>&
GtAbstractRunnable::inputData() const
{
    return m_inputData;
}

const QList<GtObjectMemento>&
GtAbstractRunnable::outputData() const
{
    return m_outputData;
}
