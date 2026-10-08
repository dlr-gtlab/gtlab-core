/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processexecutorlist.cpp
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>
#include <QStringList>

#include "gt_processexecutorlist.h"
#include "gt_abstractprocessexecutor.h"
#include "gt_processexecinterface.h"

GtProcessExecutorList::GtProcessExecutorList(QObject* parent) :
    QObject(parent)
{

}

bool
GtProcessExecutorList::addExecutor(GtProcessExecInterface* executorInterface)
{
    if (!executorInterface)
    {
        return false;
    }

    QString id = executorInterface->executorId();

    if (executorExists(id))
    {
        return false;
    }

    m_calcExecs.insert(id, executorInterface);

    return true;
}

GtAbstractProcessExecutor*
GtProcessExecutorList::generateExecutor(const QString& id)
{
    if (!executorExists(id))
    {
        return nullptr;
    }

    GtProcessExecInterface* cei = m_calcExecs.value(id);

    if (!cei)
    {
        return nullptr;
    }

    QMetaObject meta = cei->executor();

    QObject* obj = meta.newInstance();

    if (!obj)
    {
        return nullptr;
    }

    GtAbstractProcessExecutor* retval =
        qobject_cast<GtAbstractProcessExecutor*>(obj);

    if (!retval)
    {
        // free memory
        delete obj;
    }

    return retval;
}

QList<GtAbstractProperty*>
GtProcessExecutorList::generateSettings(const QString& id)
{
    if (!executorExists(id))
    {
        return QList<GtAbstractProperty*>();
    }

    GtProcessExecInterface* cei = m_calcExecs.value(id);

    if (!cei)
    {
        return QList<GtAbstractProperty*>();
    }

    return cei->executorSettings();
}

GtProcessExecutorList*
GtProcessExecutorList::instance()
{
    static GtProcessExecutorList* retval = nullptr;
    if (!retval)
    {
        retval = new GtProcessExecutorList(qApp);
    }
    return retval;
}

QStringList
GtProcessExecutorList::executorIds()
{
    return m_calcExecs.keys();
}

bool
GtProcessExecutorList::executorExists(const QString& id)
{
    return m_calcExecs.contains(id);
}

GtAbstractProcessExecutor*
GtProcessExecutorList::executor(const QString& id)
{
    QMutexLocker locker{&m_mutex};

    return generateExecutor(id);
}

QList<GtAbstractProperty*>
GtProcessExecutorList::settings(const QString& id)
{
    QMutexLocker locker{&m_mutex};

    return generateSettings(id);
}
