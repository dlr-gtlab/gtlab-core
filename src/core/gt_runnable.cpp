/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_runnable.cpp
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDir>
#include <QDebug>

#include <optional>

#include "gt_runnable.h"
#include "gt_recording.h"
#include "gt_objectmemento.h"
#include "gt_objectfactory.h"
#include "gt_coreapplication.h"
#include "gt_task.h"
#include "gt_project.h"
#include "gt_logging.h"

GtRunnable::GtRunnable(QString projectPath) :
    GtRunnable(std::move(projectPath), GtExecutionContext())
{
}

GtRunnable::GtRunnable(QString projectPath,
                       GtExecutionContext executionContext) :
    m_projectPath{std::move(projectPath)},
    m_executionContext{std::move(executionContext)}
{
    setObjectName("GtRunnable");
}

void
GtRunnable::run()
{
    std::optional<GtExecutionContextScope> contextScope;
    if (m_executionContext.isValid())
    {
        contextScope.emplace(m_executionContext);
    }

    // Activate the execution-context recording on the worker thread:
    // property accesses outside the per-execution scopes opened by
    // GtProcessComponent::execute() are attributed to the context, and
    // those per-execution recordings link to it as their parent
    std::optional<GtRecordingScope> recordingScope;
    if (m_contextRecording != nullptr)
    {
        recordingScope.emplace(*m_contextRecording);
    }

    bool success = true;

    readObjects();

    // set all states to QUEUED
    for (GtProcessComponent* calc : qAsConst(m_queue))
    {
        calc->setStateRecursively(GtProcessComponent::QUEUED);
    }

    qDebug() << "";
    qDebug() << "--- calculator execution ---";

    for (GtProcessComponent* pc : qAsConst(m_queue))
    {
        // success
        if (pc->execute()) continue;

        // failure
        gtError() << "Calculator execution failed!";
        success = false;

        auto const tasks = pc->findChildren<GtTask*>();

        for (GtTask* task : tasks)
        {
            if (task->currentState() ==
                GtProcessComponent::TERMINATION_REQUESTED)
            {
                task->setState(GtProcessComponent::TERMINATED);
            }

            if (task->currentState() == GtProcessComponent::TERMINATED &&
                pc->currentState() != GtProcessComponent::TERMINATED)
            {
                pc->setState(GtProcessComponent::TERMINATED);
            }
        }
        break;
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
    return GtCoreApplication::applicationTempDir();
}

QString
GtRunnable::projectPath()
{
    if (auto const* context = GtExecutionContext::current())
    {
        return context->projectPath();
    }

    if (!m_projectPath.isEmpty())
    {
        return m_projectPath;
    }

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
GtRunnable::setContextRecording(GtRecording* recording)
{
    m_contextRecording = recording;
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
    }
}

void
GtRunnable::writeObjects()
{
    for (GtObject* obj : qAsConst(m_linkedObjects))
    {
        m_outputData << GtObjectMemento(obj);
    }
}
