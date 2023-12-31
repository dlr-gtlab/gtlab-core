/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_calculatorexecutorlist.cpp
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>
#include <QStringList>

#include "gt_calculatorexecutorlist.h"
#include "gt_abstractcalculatorexecutor.h"
#include "gt_calculatorexecinterface.h"

GtCalculatorExecutorList::GtCalculatorExecutorList(QObject* parent) :
    QObject(parent)
{

}

bool
GtCalculatorExecutorList::addExecutor(GtCalculatorExecInterface* executor)
{
    if (!executor)
    {
        return false;
    }

    QString id = executor->executorId();

    if (executorExists(id))
    {
        return false;
    }

    m_calcExecs.insert(id, executor);

    return true;
}

GtAbstractCalculatorExecutor*
GtCalculatorExecutorList::generateExecutor(const QString& id)
{
    if (!executorExists(id))
    {
        return nullptr;
    }

    GtCalculatorExecInterface* cei = m_calcExecs.value(id);

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

    GtAbstractCalculatorExecutor* retval =
            qobject_cast<GtAbstractCalculatorExecutor*>(obj);

    if (!retval)
    {
        // free memory
        delete obj;
    }

    return retval;
}

QList<GtAbstractProperty*>
GtCalculatorExecutorList::generateSettings(const QString& id)
{
    if (!executorExists(id))
    {
        return QList<GtAbstractProperty*>();
    }

    GtCalculatorExecInterface* cei = m_calcExecs.value(id);

    if (!cei)
    {
        return QList<GtAbstractProperty*>();
    }

    return cei->executorSettings();
}

GtCalculatorExecutorList*
GtCalculatorExecutorList::instance()
{
    static GtCalculatorExecutorList* retval = nullptr;
    if (!retval)
    {
        retval = new GtCalculatorExecutorList(qApp);
    }
    return retval;
}

QStringList
GtCalculatorExecutorList::executorIds()
{
    return m_calcExecs.keys();
}

bool
GtCalculatorExecutorList::executorExists(const QString& id)
{
    return m_calcExecs.contains(id);
}

GtAbstractCalculatorExecutor*
GtCalculatorExecutorList::executor(const QString& id)
{
    QMutexLocker locker{&m_mutex};

    return generateExecutor(id);
}

QList<GtAbstractProperty*>
GtCalculatorExecutorList::settings(const QString& id)
{
    QMutexLocker locker{&m_mutex};

    return generateSettings(id);
}
