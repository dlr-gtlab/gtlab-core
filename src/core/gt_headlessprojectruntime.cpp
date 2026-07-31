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
#include <QScopeGuard>
#include <QTimer>
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

bool isRuntimeOwnerThread();

namespace
{
constexpr int shutdownTimeoutMs = 5000;

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
};

TaskSpec parseTaskSpec(const QString& reference)
{
    const int slash = reference.indexOf('/');
    if (slash < 0)
    {
        return {{}, reference.trimmed()};
    }

    return {reference.left(slash).trimmed(), reference.mid(slash + 1).trimmed()};
}
}

struct GtHeadlessTaskHandle::State
{
    QString id;
    QPointer<GtTask> task;
    QPointer<GtCoreProcessExecutor> executor;
    bool runtimeClosed{false};
    mutable bool hasTerminalStatus{false};
    mutable GtHeadlessTaskStatus terminalStatus;
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
    if (!isRuntimeOwnerThread())
    {
        result.state = GtHeadlessTaskStatus::State::Invalid;
        result.error = QStringLiteral("Task handles must be used from the GTlab owner thread");
        return result;
    }

    if (m_state->hasTerminalStatus)
    {
        return m_state->terminalStatus;
    }

    if (m_state->runtimeClosed)
    {
        result.state = GtHeadlessTaskStatus::State::Shutdown;
        result.error = QStringLiteral("Runtime was shut down before task completion");
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

    if (result.isDone())
    {
        m_state->terminalStatus = result;
        m_state->hasTerminalStatus = true;
    }
    return result;
}

bool GtHeadlessTaskHandle::cancel() const
{
    if (!m_state || !isRuntimeOwnerThread() || m_state->runtimeClosed ||
        m_state->hasTerminalStatus || !m_state->task || !m_state->executor)
    {
        return false;
    }

    return m_state->executor->terminateTask(m_state->task);
}

GtHeadlessTaskStatus GtHeadlessTaskHandle::wait(int timeoutMs) const
{
    if (!m_state || !isRuntimeOwnerThread())
    {
        return status();
    }

    auto current = status();
    const auto executorFinished = [&]() {
        return !m_state->executor ||
               (!m_state->executor->taskCurrentlyRunning() &&
                !m_state->executor->taskQueued(m_state->task));
    };
    if (current.isDone() && executorFinished())
    {
        return current;
    }

    if (!m_state->executor)
    {
        return current;
    }

    QEventLoop loop;
    QTimer timeout;
    timeout.setSingleShot(true);
    QObject::connect(m_state->executor, &GtCoreProcessExecutor::queueChanged,
                     &loop, &QEventLoop::quit);
    QObject::connect(m_state->executor, &GtCoreProcessExecutor::allTasksCompleted,
                     &loop, &QEventLoop::quit);
    if (timeoutMs >= 0)
    {
        timeout.setInterval(timeoutMs);
        QObject::connect(&timeout, &QTimer::timeout, &loop, &QEventLoop::quit);
        timeout.start();
    }

    while (!current.isDone() || !executorFinished())
    {
        loop.exec(QEventLoop::AllEvents);
        current = status();
        if (timeoutMs >= 0 && timeout.remainingTime() <= 0)
        {
            break;
        }
    }
    return current;
}

struct GtHeadlessProjectRuntime::Private
{
    State state{State::Created};
    QPointer<GtProject> project;
    QVector<QSharedPointer<GtHeadlessTaskHandle::State>> tasks;
    QPointer<GtCoreProcessExecutor> configuredExecutor;
    GtCoreProcessExecutor::Flags previousExecutorFlags{};
    bool executorFlagsOverridden{false};
    QMetaObject::Connection executorCompletionConnection;
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
    if (!isRuntimeOwnerThread())
    {
        if (gtApp && gtApp->thread() && gtApp->thread()->isRunning() &&
            !QCoreApplication::closingDown())
        {
            QMetaObject::invokeMethod(gtApp, [this]() { shutdown(); },
                                       Qt::BlockingQueuedConnection);
        }
        return;
    }

    shutdown();
}

void GtHeadlessProjectRuntime::shutdown()
{
    for (const auto& task : std::as_const(m_private->tasks))
    {
        GtHeadlessTaskHandle handle(task);
        if (!handle.status().isDone())
        {
            handle.cancel();
            auto status = handle.wait(shutdownTimeoutMs);
            if (!status.isDone() && task->executor)
            {
                task->executor->terminateAllTasks();
                status = handle.wait(100);
            }
            if (!status.isDone())
            {
                task->terminalStatus = status;
                task->terminalStatus.state = GtHeadlessTaskStatus::State::Shutdown;
                task->terminalStatus.error =
                    QStringLiteral("Runtime shutdown timed out while cancelling task");
                task->hasTerminalStatus = true;
                task->runtimeClosed = true;
            }
        }
    }

    if (m_private->project)
    {
        closeProject();
    }

    restoreExecutorFlags();
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

    m_private->state = State::Initialized;
    return success();
}

void GtHeadlessProjectRuntime::restoreExecutorFlags()
{
    if (!m_private->executorFlagsOverridden)
    {
        return;
    }

    if (m_private->configuredExecutor)
    {
        m_private->configuredExecutor->setCoreExecutorFlags(
            m_private->previousExecutorFlags);
    }
    QObject::disconnect(m_private->executorCompletionConnection);
    m_private->executorCompletionConnection = {};
    m_private->configuredExecutor.clear();
    m_private->executorFlagsOverridden = false;
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

    if (GtProjectExecutionGuard::isBusy(m_private->project))
    {
        return failure(GtHeadlessRuntimeResult::Code::ProjectBusy,
                       QStringLiteral("Project execution is still active"));
    }

    if (gtDataModel->saveProject(m_private->project))
    {
        return success();
    }

    return GtProjectExecutionGuard::isBusy(m_private->project) ?
               failure(GtHeadlessRuntimeResult::Code::ProjectBusy,
                       QStringLiteral("Project execution started during save")) :
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

    if ((m_private->state != State::ProjectLoaded &&
         m_private->state != State::CloseFailed) || !m_private->project)
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

    GtProject* project = m_private->project;

    if (m_private->state == State::CloseFailed)
    {
        if (!gtDataModel->deleteProject(project))
        {
            return failure(GtHeadlessRuntimeResult::Code::CloseFailed,
                           QStringLiteral("Project could not be removed"));
        }

        for (const auto& task : std::as_const(m_private->tasks))
        {
            GtHeadlessTaskHandle(task).status();
            task->runtimeClosed = true;
        }
        m_private->project.clear();
        m_private->state = State::Closed;
        restoreExecutorFlags();
        return success();
    }

    if (GtProjectExecutionGuard::isBusy(project))
    {
        return failure(GtHeadlessRuntimeResult::Code::ProjectBusy,
                       QStringLiteral("Project execution is still active"));
    }

    if (!gtDataModel->closeProject(project))
    {
        return GtProjectExecutionGuard::isBusy(project) ?
                   failure(GtHeadlessRuntimeResult::Code::ProjectBusy,
                           QStringLiteral("Project execution started during close")) :
                   failure(GtHeadlessRuntimeResult::Code::CloseFailed,
                           QStringLiteral("Project could not be closed"));
    }

    if (!gtDataModel->deleteProject(project))
    {
        m_private->state = State::CloseFailed;
        restoreExecutorFlags();
        return failure(GtHeadlessRuntimeResult::Code::CloseFailed,
                       QStringLiteral("Project was closed but could not be removed"));
    }

    for (const auto& task : std::as_const(m_private->tasks))
    {
        GtHeadlessTaskHandle(task).status();
        task->runtimeClosed = true;
    }

    m_private->project.clear();
    m_private->state = State::Closed;
    restoreExecutorFlags();
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

    const auto* previousTaskGroup = processData->taskGroup();
    const auto previousGroup = previousTaskGroup ?
                                   previousTaskGroup->objectName() : QString();
    auto previousScope = GtTaskGroup::USER;
    if (previousTaskGroup && previousTaskGroup->parent() &&
        previousTaskGroup->parent()->objectName() ==
            GtTaskGroup::scopeId(GtTaskGroup::CUSTOM))
    {
        previousScope = GtTaskGroup::CUSTOM;
    }
    const auto restoreTaskGroup = qScopeGuard([processData,
                                               previousGroup,
                                               previousScope,
                                               projectPath = m_private->project->path()]() {
        if (!previousGroup.isEmpty())
        {
            processData->switchCurrentTaskGroup(previousGroup, previousScope,
                                                projectPath);
        }
    });
    Q_UNUSED(restoreTaskGroup);
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
        processData->switchCurrentTaskGroup(previousGroup, previousScope,
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
    const bool explicitGroup = taskReference.indexOf('/') >= 0;
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

    const auto* previousTaskGroup = processData->taskGroup();
    const auto previousGroup = previousTaskGroup ?
                                   previousTaskGroup->objectName() : QString();
    auto previousScope = GtTaskGroup::USER;
    if (previousTaskGroup && previousTaskGroup->parent() &&
        previousTaskGroup->parent()->objectName() ==
            GtTaskGroup::scopeId(GtTaskGroup::CUSTOM))
    {
        previousScope = GtTaskGroup::CUSTOM;
    }
    const auto restoreTaskGroup = qScopeGuard([processData,
                                               previousGroup,
                                               previousScope,
                                               projectPath = m_private->project->path()]() {
        if (!previousGroup.isEmpty())
        {
            processData->switchCurrentTaskGroup(previousGroup, previousScope,
                                                projectPath);
        }
    });
    Q_UNUSED(restoreTaskGroup);

    GtTask* task = nullptr;
    if (!explicitGroup)
    {
        const auto matchingTasks = m_private->project->findChildren<GtTask*>();
        const auto uuidMatch = std::find_if(
            matchingTasks.cbegin(), matchingTasks.cend(),
            [&](const auto* candidate) { return candidate->uuid() == spec.task; });
        if (uuidMatch != matchingTasks.cend())
        {
            task = *uuidMatch;
        }
    }

    if (!task && explicitGroup &&
        !processData->switchCurrentTaskGroup(spec.group, GtTaskGroup::CUSTOM,
                                             m_private->project->path()) &&
        !processData->switchCurrentTaskGroup(spec.group, GtTaskGroup::USER,
                                             m_private->project->path()))
    {
        setResult(failure(GtHeadlessRuntimeResult::Code::TaskNotFound,
                          QStringLiteral("Task group not found: %1").arg(spec.group)));
        return {};
    }
    else if (!task && !explicitGroup)
    {
        processData->switchCurrentTaskGroup(GtTaskGroup::defaultUserGroupId(),
                                            GtTaskGroup::USER, m_private->project->path());
    }

    if (!task)
    {
        task = m_private->project->findProcess(spec.task);
    }
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

    if (!m_private->executorFlagsOverridden)
    {
        m_private->configuredExecutor = executor;
        m_private->previousExecutorFlags = executor->coreExecutorFlags();
        m_private->executorFlagsOverridden = true;
        m_private->executorCompletionConnection = connect(
            executor,
            &GtCoreProcessExecutor::allTasksCompleted,
            this,
            [this]() { restoreExecutorFlags(); });
    }

    auto flags = executor->coreExecutorFlags();
    flags.setFlag(gt::NonBlockingExecution, true);
    executor->setCoreExecutorFlags(flags);
    const auto runResult = executor->runTaskWithResult(task);
    if (runResult != GtCoreProcessExecutor::RunTaskResult::Started &&
        runResult != GtCoreProcessExecutor::RunTaskResult::Queued)
    {
        restoreExecutorFlags();
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
