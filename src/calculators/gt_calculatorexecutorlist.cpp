/* GTlab - Gas Turbine laboratory
 * Source File: gt_calculatorexecutorlist.cpp
 * copyright 2009-2016 by DLR
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
    if (executor == Q_NULLPTR)
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
        return Q_NULLPTR;
    }

    GtCalculatorExecInterface* cei = m_calcExecs.value(id);

    if (cei == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    QMetaObject meta = cei->executor();

    QObject* obj = meta.newInstance();

    if (obj == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    GtAbstractCalculatorExecutor* retval =
            qobject_cast<GtAbstractCalculatorExecutor*>(obj);

    if (retval == Q_NULLPTR)
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

    if (cei == Q_NULLPTR)
    {
        return QList<GtAbstractProperty*>();
    }

    return cei->executorSettings();
}

GtCalculatorExecutorList*
GtCalculatorExecutorList::instance()
{
    static GtCalculatorExecutorList* retval = 0;
    if (retval == 0)
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
    m_mutex.lock();

    GtAbstractCalculatorExecutor* retval = generateExecutor(id);

    m_mutex.unlock();

    return retval;
}

QList<GtAbstractProperty*>
GtCalculatorExecutorList::settings(const QString& id)
{
    m_mutex.lock();

    QList<GtAbstractProperty*> retval = generateSettings(id);

    m_mutex.unlock();

    return retval;
}
