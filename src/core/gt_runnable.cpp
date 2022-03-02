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
#include "gt_externalizedobject.h"
#include "gt_h5filemanager.h"

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

    // create a new temp hdf5 file for this process
    int h5FileId = gtH5FileManager->createNewTempFile();

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

        // commit the temp hdf5 datasets to the main datamodel
        gtH5FileManager->commitObjectsInTempFile(h5FileId, m_linkedObjects);

        writeObjects();
    }
    else
    {
        qDebug() << "--- calculator execution failed ---";
        qDebug() << "";

        // remove the temp hdf5 file
        gtH5FileManager->removeTempFile(h5FileId);
    }

    m_successfulRun = success;

    emit runnableFinished();
}

bool
GtRunnable::appendCalculator(GtProcessComponent* calc)
{
    if (!calc)
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

        if (task)
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
    if (!gtApp->currentProject())
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
    for (GtObjectMemento& memento : m_inputData)
    {
        GtObject* obj = memento.restore(gtObjectFactory);

        if (obj)
        {
            m_linkedObjects.append(obj);
        }

        for (auto* externObj : obj->findChildren<GtExternalizedObject*>())
        {
            externObj->resetRefCount();
        }
    }
}

void
GtRunnable::writeObjects()
{
    for (GtObject* obj : m_linkedObjects)
    {
        m_outputData << GtObjectMemento(obj);
    }
}
