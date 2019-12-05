/* GTlab - Gas Turbine laboratory
 * Source File: gt_runnable.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDir>
#include <QDebug>

#include "gt_runnable.h"
#include "gt_objectmemento.h"
#include "gt_objectfactory.h"
#include "gt_coreapplication.h"
#include "gt_task.h"
#include "gt_project.h"
#include "gt_logging.h"

GtRunnable::GtRunnable()
{

}

GtRunnable::~GtRunnable()
{
}

void
GtRunnable::run()
{
    qDebug() << "GtRunnable::run()";
    bool success = true;

    readObjects();

    // set all states to QUEUED
    foreach (GtProcessComponent* calc, m_queue)
    {
        calc->setStateRecursively(GtProcessComponent::QUEUED);
    }

    qDebug() << "";
    qDebug() << "--- calculator execution ---";

    foreach (GtProcessComponent* calc, m_queue)
    {
        if (!calc->exec())
        {
            gtError() << "Calculator execution failed!";
            success = false;

            QList<GtTask*> tasks = calc->findChildren<GtTask*>();

            foreach (GtTask* task, tasks)
            {
                if (task->currentState() ==
                        GtProcessComponent::TERMINATION_REQUESTED)
                {
                    task->setState(GtProcessComponent::TERMINATED);
                }

                if (task->currentState() == GtProcessComponent::TERMINATED &&
                        calc->currentState() != GtProcessComponent::TERMINATED)
                {
                    calc->setState(GtProcessComponent::TERMINATED);
                }
            }

            break;
        }
    }

    if (success)
    {
        qDebug() << "--- calculator execution finished ---";
        qDebug() << "";

        writeObjects();
    }
    else
    {
        qDebug() << "--- calculator execution failed ---";
        qDebug() << "";
    }

    m_successfulRun = success;

    emit runnableFinished();
}

bool
GtRunnable::appendCalculator(GtProcessComponent* calc)
{
    if (calc == Q_NULLPTR)
    {
        return false;
    }

    if (m_queue.contains(calc))
    {
        qDebug() << "CalculatorRun already contained in runnable queue";
        return false;
    }

    calc->setParent(this);
    m_queue.append(calc);

    return true;
}

void
GtRunnable::setExecutionPath(const QString& path)
{
    m_executionPath = path;
}

void
GtRunnable::requestInterruption()
{
    QList<GtTask*> tasks;

    foreach (GtProcessComponent* calc, m_queue)
    {
        GtTask* task = qobject_cast<GtTask*>(calc);

        if (task != Q_NULLPTR)
        {
            tasks << task;
        }

        tasks.append(calc->findChildren<GtTask*>());
    }

    foreach (GtTask* task, tasks)
    {
        task->requestInterruption();
    }
}

QDir
GtRunnable::tempDir()
{
    if (m_executionPath.isEmpty())
    {
        return GtCoreApplication::applicationTempDir();
    }

    QDir dir(m_executionPath + QDir::separator() + QStringLiteral("temp"));

    if (!dir.exists())
    {
        dir = QDir(m_executionPath);

        if (!dir.mkpath(dir.absolutePath() + QDir::separator()
                        + QStringLiteral("temp")))
        {
            gtWarning() << tr("Could not create temporary directory!");
            return QDir();
        }

        if (!dir.cd(QStringLiteral("temp")))
        {
            gtWarning() << tr("Could not create temporary directory!");
            return QDir();
        }
    }

    return GtCoreApplication::createTempDir(dir.absolutePath());
}

QString
GtRunnable::projectPath()
{
    if (gtApp->currentProject() == Q_NULLPTR)
    {
        return QString();
    }

    return gtApp->currentProject()->path();
}

bool
GtRunnable::clearTempDir(const QString& path)
{
    return GtCoreApplication::removeTempDir(path);
}

void
GtRunnable::readObjects()
{
    foreach (GtObjectMemento memento, m_inputData)
    {
        GtObject* obj = memento.restore(gtObjectFactory);

        if (obj != Q_NULLPTR)
        {
            m_linkedObjects.append(obj);
        }
    }
}

void
GtRunnable::writeObjects()
{
    foreach (GtObject* obj, m_linkedObjects)
    {
        m_outputData << GtObjectMemento(obj);
    }
}
