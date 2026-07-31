/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_headlessprojectruntime.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFileInfo>
#include <QDir>
#include <QPointer>
#include <QThread>
#include <QUuid>

#include <algorithm>
#include <memory>
#include <utility>

#include "gt_coreapplication.h"
#include "gt_coredatamodel.h"
#include "gt_coreprocessexecutor.h"
#include "gt_processexecutormanager.h"
#include "gt_project.h"
#include "gt_projectexecutionguard.h"
#include "gt_processdata.h"
#include "gt_task.h"
#include "gt_taskgroup.h"
#include "provider/gt_projectprovider.h"

namespace
{
GtHeadlessTaskStatus::State taskState(GtProcessComponent::STATE state)
{
    using ProcessState = GtProcessComponent::STATE;
    using TaskState = GtHeadlessTaskStatus::State;

    switch (state)
    {
    case ProcessState::QUEUED: return TaskState::Queued;
    case ProcessState::RUNNING:
    case ProcessState::CONNECTING:
    case ProcessState::TERMINATION_REQUESTED: return TaskState::Running;
    case ProcessState::FINISHED:
    case ProcessState::WARN_FINISHED: return TaskState::Finished;
    case ProcessState::TERMINATED: return TaskState::Cancelled;
    case ProcessState::FAILED:
    case ProcessState::SKIPPED: return TaskState::Failed;
    case ProcessState::NONE: return TaskState::Invalid;
    }

    return TaskState::Invalid;
}

struct TaskSpec
{
    QString group;
    QString task;
    bool explicitGroup{false};
};

TaskSpec parseTaskSpec(const QString& reference)
{
    const int slash = reference.indexOf('/');
    if (slash < 0)
    {
        return {{}, reference.trimmed(), false};
    }

    return {reference.left(slash).trimmed(), reference.mid(slash + 1).trimmed(), true};
}
}

struct GtHeadlessTaskHandle::State
{
    QString id;
    QPointer<GtTask> task;
    QPointer<GtCoreProcessExecutor> executor;
    bool runtimeClosed{false};
};

bool GtHeadlessTaskStatus::isDone() const
{
    return state == State::Finished || state == State::Failed ||
           state == State::Cancelled || state == State::Shutdown;
}

GtHeadlessTaskHandle::GtHeadlessTaskHandle() = default;
GtHeadlessTaskHandle::~GtHeadlessTaskHandle() = default;

GtHeadlessTaskHandle::GtHeadlessTaskHandle(const QSharedPointer<State>& state) :
    m_state(state)
{
}

QString GtHeadlessTaskHandle::id() const
{
    return m_state ? m_state->id : QString();
}

bool GtHeadlessTaskHandle::isValid() const
{
    return !id().isEmpty();
}

GtHeadlessTaskStatus GtHeadlessTaskHandle::status() const
{
    GtHeadlessTaskStatus result;
    if (!m_state)
    {
        return result;
    }

    result.id = m_state->id;
    if (m_state->runtimeClosed)
    {
        result.state = GtHeadlessTaskStatus::State::Shutdown;
        return result;
    }

    if (!m_state->task)
    {
        result.state = GtHeadlessTaskStatus::State::Invalid;
        result.error = QStringLiteral("Task object is no longer available");
        return result;
    }

    result.processState = m_state->task->currentState();
    result.state = taskState(result.processState);
    if (result.state == GtHeadlessTaskStatus::State::Failed)
    {
        result.error = QStringLiteral("Task execution failed (%1)")
                           .arg(static_cast<int>(result.processState));
    }
    else if (result.state == GtHeadlessTaskStatus::State::Cancelled)
    {
        result.error = QStringLiteral("Task execution was cancelled");
    }
    return result;
}

bool GtHeadlessTaskHandle::cancel() const
{
    if (!m_state || m_state->runtimeClosed || !m_state->task || !m_state->executor)
    {
        return false;
    }

    return m_state->executor->terminateTask(m_state->task);
}

GtHeadlessTaskStatus GtHeadlessTaskHandle::wait(int timeoutMs) const
{
    QElapsedTimer timer;
    timer.start();

    while (true)
    {
        const auto current = status();
        if (current.isDone())
        {
            return current;
        }

        if (timeoutMs >= 0 && timer.elapsed() >= timeoutMs)
        {
            return current;
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
    }
}

struct GtHeadlessProjectRuntime::Private
{
    State state{State::Created};
    QPointer<GtProject> project;
    QVector<QSharedPointer<GtHeadlessTaskHandle::State>> tasks;
};

bool isRuntimeOwnerThread()
{
    return gtApp && gtApp->thread() == QThread::currentThread();
}

GtHeadlessRuntimeResult success()
{
    return {};
}

GtHeadlessRuntimeResult failure(GtHeadlessRuntimeResult::Code code, QString message)
{
    return {code, std::move(message)};
}

bool GtHeadlessRuntimeResult::succeeded() const
{
    return code == Code::Success;
}

GtHeadlessProjectRuntime::GtHeadlessProjectRuntime(QObject* parent) :
    QObject(parent),
    m_private(new Private)
{
}

GtHeadlessProjectRuntime::~GtHeadlessProjectRuntime()
{
    for (const auto& task : std::as_const(m_private->tasks))
    {
        GtHeadlessTaskHandle handle(task);
        if (!handle.status().isDone())
        {
            handle.cancel();
            handle.wait();
        }
    }

    for (const auto& task : std::as_const(m_private->tasks))
    {
        task->runtimeClosed = true;
    }

    if (m_private->project && m_private->project->isOpen())
    {
        closeProject();
    }

    if (auto* executor = gt::processExecutorManager().currentExecutor())
    {
        executor->setCoreExecutorFlags({});
    }
}

GtHeadlessRuntimeResult GtHeadlessProjectRuntime::initialize()
{
    if (m_private->state == State::Initialized)
    {
        return success();
    }

    if (m_private->state != State::Created)
    {
        return failure(GtHeadlessRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime cannot be initialized in its current state"));
    }

    if (!gtApp || !gtDataModel)
    {
        return failure(GtHeadlessRuntimeResult::Code::CoreUnavailable,
                       QStringLiteral("Required GTlab Core services are unavailable"));
    }

    if (gtApp->thread() != QThread::currentThread())
    {
        return failure(GtHeadlessRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime must be initialized on the GTlab owner thread"));
    }

    gtApp->init();
    if (!gtApp->session())
    {
        gtApp->initSession();
    }

    if (!gtApp->session() || !gt::processExecutorManager().currentExecutor())
    {
        return failure(GtHeadlessRuntimeResult::Code::CoreUnavailable,
                       QStringLiteral("GTlab Core session or executor could not be initialized"));
    }

    auto* executor = gt::processExecutorManager().currentExecutor();
    connect(executor,
            &GtCoreProcessExecutor::allTasksCompleted,
            this,
            [executor]() { executor->setCoreExecutorFlags({}); },
            Qt::UniqueConnection);

    m_private->state = State::Initialized;
    return success();
}

GtHeadlessRuntimeResult GtHeadlessProjectRuntime::openProject(const QString& projectPath)
{
    if (!isRuntimeOwnerThread())
    {
        return failure(GtHeadlessRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime must be used from the GTlab owner thread"));
    }

    if (m_private->state != State::Initialized)
    {
        return failure(GtHeadlessRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime is not initialized"));
    }

    if (m_private->project)
    {
        return failure(GtHeadlessRuntimeResult::Code::ProjectAlreadyLoaded,
                       QStringLiteral("A project is already loaded"));
    }

    if (gtDataModel->currentProject())
    {
        return failure(GtHeadlessRuntimeResult::Code::ProjectAlreadyLoaded,
                       QStringLiteral("The Core datamodel already has an open project"));
    }

    QFileInfo projectInfo(projectPath);
    const QString filePath = projectInfo.isDir() ?
                                 QDir(projectInfo.absoluteFilePath())
                                     .filePath(GtProject::mainFilename()) :
                                 projectPath;
    if (!QFileInfo::exists(filePath))
    {
        return failure(GtHeadlessRuntimeResult::Code::InvalidProject,
                       QStringLiteral("Project file does not exist: %1").arg(filePath));
    }

    GtProjectProvider provider(filePath);
    std::unique_ptr<GtProject> project(provider.project());
    if (!project || !project->isValid())
    {
        return failure(GtHeadlessRuntimeResult::Code::InvalidProject,
                       QStringLiteral("Project is invalid: %1").arg(filePath));
    }

    GtProject* projectPtr = project.get();
    if (!gtDataModel->newProject(projectPtr, true))
    {
        return failure(GtHeadlessRuntimeResult::Code::InvalidProject,
                       QStringLiteral("Project could not be opened: %1").arg(filePath));
    }

    project.release();
    if (!projectPtr->isOpen())
    {
        gtDataModel->deleteProject(projectPtr);
        return failure(GtHeadlessRuntimeResult::Code::InvalidProject,
                       QStringLiteral("Project could not be loaded: %1").arg(filePath));
    }

    m_private->project = projectPtr;
    m_private->state = State::ProjectLoaded;
    return success();
}

GtHeadlessRuntimeResult GtHeadlessProjectRuntime::saveProject()
{
    if (!isRuntimeOwnerThread())
    {
        return failure(GtHeadlessRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime must be used from the GTlab owner thread"));
    }

    if (m_private->state != State::ProjectLoaded || !m_private->project)
    {
        return failure(GtHeadlessRuntimeResult::Code::InvalidState,
                       QStringLiteral("No project is loaded"));
    }

    for (const auto& task : std::as_const(m_private->tasks))
    {
        if (!task->runtimeClosed && !task->task.isNull() &&
            !GtHeadlessTaskHandle(task).status().isDone())
        {
            return failure(GtHeadlessRuntimeResult::Code::ProjectBusy,
                           QStringLiteral("Cannot save while a task is running"));
        }
    }

    GtProjectExecutionGuard guard;
    const auto guardResult = guard.tryAcquire(m_private->project);
    if (guardResult == GtProjectExecutionGuard::Result::Busy)
    {
        return failure(GtHeadlessRuntimeResult::Code::ProjectBusy,
                       QStringLiteral("Project execution is still active"));
    }
    if (guardResult == GtProjectExecutionGuard::Result::InvalidProject)
    {
        return failure(GtHeadlessRuntimeResult::Code::SaveFailed,
                       QStringLiteral("Project could not be guarded for saving"));
    }

    return gtDataModel->saveProject(m_private->project) ?
               success() :
               failure(GtHeadlessRuntimeResult::Code::SaveFailed,
                       QStringLiteral("Project could not be saved"));
}

GtHeadlessRuntimeResult GtHeadlessProjectRuntime::closeProject()
{
    if (!isRuntimeOwnerThread())
    {
        return failure(GtHeadlessRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime must be used from the GTlab owner thread"));
    }

    if (m_private->state != State::ProjectLoaded || !m_private->project)
    {
        return failure(GtHeadlessRuntimeResult::Code::InvalidState,
                       QStringLiteral("No project is loaded"));
    }

    for (const auto& task : std::as_const(m_private->tasks))
    {
        if (!task->runtimeClosed && !task->task.isNull() &&
            !GtHeadlessTaskHandle(task).status().isDone())
        {
            return failure(GtHeadlessRuntimeResult::Code::ProjectBusy,
                           QStringLiteral("Cannot close while a task is running"));
        }
    }

    GtProjectExecutionGuard guard;
    const auto guardResult = guard.tryAcquire(m_private->project);
    if (guardResult == GtProjectExecutionGuard::Result::Busy)
    {
        return failure(GtHeadlessRuntimeResult::Code::ProjectBusy,
                       QStringLiteral("Project execution is still active"));
    }
    if (guardResult == GtProjectExecutionGuard::Result::InvalidProject)
    {
        return failure(GtHeadlessRuntimeResult::Code::CloseFailed,
                       QStringLiteral("Project could not be guarded for closing"));
    }

    for (const auto& task : std::as_const(m_private->tasks))
    {
        task->runtimeClosed = true;
    }

    GtProject* project = m_private->project;
    if (!gtDataModel->closeProject(project) || !gtDataModel->deleteProject(project))
    {
        return failure(GtHeadlessRuntimeResult::Code::CloseFailed,
                       QStringLiteral("Project could not be closed"));
    }

    m_private->project.clear();
    m_private->state = State::Closed;
    return success();
}

GtHeadlessProjectRuntime::State GtHeadlessProjectRuntime::state() const
{
    return m_private->state;
}

QString GtHeadlessProjectRuntime::projectPath() const
{
    return m_private->project ? m_private->project->path() : QString();
}

QVector<GtHeadlessTaskDescriptor> GtHeadlessProjectRuntime::listTasks() const
{
    QVector<GtHeadlessTaskDescriptor> result;
    if (!isRuntimeOwnerThread() ||
        m_private->state != State::ProjectLoaded || !m_private->project)
    {
        return result;
    }

    auto* processData = m_private->project->processData();
    if (!processData)
    {
        return result;
    }

    const auto previousGroup = processData->taskGroup() ?
                                   processData->taskGroup()->objectName() : QString();
    const auto collect = [&result, processData, this]()
    {
        const QString group = processData->taskGroup() ?
                                  processData->taskGroup()->objectName() : QString();
        for (auto* task : m_private->project->findChildren<GtTask*>())
        {
            if (!task)
            {
                continue;
            }

            const auto* taskGroup = task->findParent<GtTaskGroup*>();
            const QString taskGroupId = taskGroup ? taskGroup->objectName() : group;
            const QString name = taskGroupId.isEmpty() ? task->objectName() :
                                                          taskGroupId + '/' + task->objectName();
            const auto duplicate = std::any_of(result.cbegin(), result.cend(),
                                               [&](const auto& item) { return item.uuid == task->uuid(); });
            if (!duplicate)
            {
                result.append({name, taskGroupId, task->objectName(), task->uuid(),
                               task->objectPath(), task->currentState()});
            }
        }
    };

    processData->switchCurrentTaskGroup(GtTaskGroup::defaultUserGroupId(),
                                        GtTaskGroup::USER, m_private->project->path());
    collect();
    for (const auto& group : processData->userGroupIds())
    {
        if (processData->switchCurrentTaskGroup(group, GtTaskGroup::USER,
                                                m_private->project->path()))
        {
            collect();
        }
    }
    for (const auto& group : processData->customGroupIds())
    {
        if (processData->switchCurrentTaskGroup(group, GtTaskGroup::CUSTOM,
                                                m_private->project->path()))
        {
            collect();
        }
    }
    if (!previousGroup.isEmpty())
    {
        processData->switchCurrentTaskGroup(previousGroup, GtTaskGroup::USER,
                                            m_private->project->path());
    }

    std::sort(result.begin(), result.end(), [](const auto& left, const auto& right) {
        return left.path == right.path ? left.name < right.name : left.path < right.path;
    });
    return result;
}

GtHeadlessTaskHandle GtHeadlessProjectRuntime::submitTask(
    const QString& taskReference,
    GtHeadlessRuntimeResult* result)
{
    const auto setResult = [&](GtHeadlessRuntimeResult value) {
        if (result)
        {
            *result = std::move(value);
        }
    };

    if (!isRuntimeOwnerThread())
    {
        setResult(failure(GtHeadlessRuntimeResult::Code::InvalidState,
                          QStringLiteral("Runtime must be used from the GTlab owner thread")));
        return {};
    }

    if (m_private->state != State::ProjectLoaded || !m_private->project)
    {
        setResult(failure(GtHeadlessRuntimeResult::Code::InvalidState,
                          QStringLiteral("No project is loaded")));
        return {};
    }

    auto* executor = gt::processExecutorManager().currentExecutor();
    if (!executor || executor->taskCurrentlyRunning() || !executor->queue().isEmpty())
    {
        setResult(failure(GtHeadlessRuntimeResult::Code::ProjectBusy,
                          QStringLiteral("The process executor is busy")));
        return {};
    }

    const auto spec = parseTaskSpec(taskReference);
    if (spec.task.isEmpty())
    {
        setResult(failure(GtHeadlessRuntimeResult::Code::TaskNotFound,
                          QStringLiteral("Task reference is empty")));
        return {};
    }

    auto* processData = m_private->project->processData();
    if (!processData)
    {
        setResult(failure(GtHeadlessRuntimeResult::Code::ExecutionRejected,
                          QStringLiteral("Project has no process data")));
        return {};
    }

    if (spec.explicitGroup &&
        !processData->switchCurrentTaskGroup(spec.group, GtTaskGroup::CUSTOM,
                                             m_private->project->path()) &&
        !processData->switchCurrentTaskGroup(spec.group, GtTaskGroup::USER,
                                             m_private->project->path()))
    {
        setResult(failure(GtHeadlessRuntimeResult::Code::TaskNotFound,
                          QStringLiteral("Task group not found: %1").arg(spec.group)));
        return {};
    }
    else if (!spec.explicitGroup)
    {
        processData->switchCurrentTaskGroup(GtTaskGroup::defaultUserGroupId(),
                                            GtTaskGroup::USER, m_private->project->path());
    }

    auto* task = m_private->project->findProcess(spec.task);
    if (!task)
    {
        setResult(failure(GtHeadlessRuntimeResult::Code::TaskNotFound,
                          QStringLiteral("Task not found: %1").arg(taskReference)));
        return {};
    }

    if (!executor->setSource(m_private->project))
    {
        setResult(failure(GtHeadlessRuntimeResult::Code::ExecutionRejected,
                          QStringLiteral("The process executor rejected the project source")));
        return {};
    }

    executor->setCoreExecutorFlags(
        GtCoreProcessExecutor::Flags{gt::NonBlockingExecution});
    const auto runResult = executor->runTaskWithResult(task);
    if (runResult != GtCoreProcessExecutor::RunTaskResult::Started &&
        runResult != GtCoreProcessExecutor::RunTaskResult::Queued)
    {
        executor->setCoreExecutorFlags({});
        const auto code = runResult == GtCoreProcessExecutor::RunTaskResult::Busy ?
                              GtHeadlessRuntimeResult::Code::ProjectBusy :
                              GtHeadlessRuntimeResult::Code::ExecutionRejected;
        setResult(failure(code,
                          QStringLiteral("Task could not be queued: %1").arg(taskReference)));
        return {};
    }

    auto state = QSharedPointer<GtHeadlessTaskHandle::State>::create();
    state->id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    state->task = task;
    state->executor = executor;
    m_private->tasks.append(state);
    setResult(success());
    return GtHeadlessTaskHandle(state);
}
