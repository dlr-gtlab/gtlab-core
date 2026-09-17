/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_headlessprojectruntime.h"

#include "gt_coreapplication.h"
#include "gt_coredatamodel.h"
#include "gt_executioncontext.h"
#include "gt_executableoperation.h"
#include "gt_object.h"
#include "gt_project.h"
#include "gt_projectexecutionguard.h"
#include "provider/gt_projectprovider.h"

#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QMetaObject>
#include <QThread>
#include <QUuid>

#include <algorithm>
#include <exception>
#include <utility>
#include <vector>

namespace
{
bool isRuntimeOwnerThread()
{
    return gtApp && gtApp->thread() == QThread::currentThread();
}

GtHeadlessRuntimeResult failure(GtHeadlessRuntimeResult::Code code, QString message)
{
    return {code, std::move(message)};
}
}

struct GtHeadlessOperationHandle::State
{
    mutable QMutex mutex;
    QWaitCondition changed;
    GtHeadlessOperationStatus snapshot;
    GtCancellationToken cancellation;
    QSharedPointer<GtExecutionEventStream> eventStream;
    QSharedPointer<GtObject> executionResult;
    std::unique_ptr<GtExecutableOperation> operation;
    std::unique_ptr<GtObject> data;
    bool requiresProject{false};

    GtHeadlessOperationStatus status() const
    {
        QMutexLocker lock(&mutex);
        return snapshot;
    }

    void setState(GtHeadlessOperationStatus::State state,
                  GtHeadlessOperationStatus::Result result =
                      GtHeadlessOperationStatus::Result::None,
                  QString error = {})
    {
        QMutexLocker lock(&mutex);
        snapshot.state = state;
        snapshot.result = result;
        snapshot.error = std::move(error);
        changed.wakeAll();
    }

    void completeSuccessfully(std::unique_ptr<GtObject> result)
    {
        QMutexLocker lock(&mutex);
        if (cancellation.isCancellationRequested())
        {
            snapshot.state = GtHeadlessOperationStatus::State::Cancelled;
            snapshot.result = GtHeadlessOperationStatus::Result::Cancelled;
            snapshot.error = QStringLiteral("Operation cancellation was requested");
        }
        else
        {
            executionResult.reset(result.release());
            snapshot.state = GtHeadlessOperationStatus::State::Finished;
            snapshot.result = GtHeadlessOperationStatus::Result::Succeeded;
            snapshot.error.clear();
        }
        changed.wakeAll();
    }
};

struct GtHeadlessProjectRuntime::Private
{
    State state{State::Created};
    QPointer<GtProject> project;
    std::vector<QSharedPointer<GtHeadlessOperationHandle::State>> operations;
};

bool GtHeadlessOperationStatus::isDone() const
{
    return state == State::Finished || state == State::Failed ||
           state == State::Cancelled || state == State::Shutdown;
}

bool GtHeadlessOperationCancellationResult::succeeded() const
{
    return code == Code::Accepted;
}

GtHeadlessOperationHandle::GtHeadlessOperationHandle() = default;
GtHeadlessOperationHandle::~GtHeadlessOperationHandle() = default;
GtHeadlessOperationHandle::GtHeadlessOperationHandle(
    QSharedPointer<State> const& state) : m_state(state)
{
}

QString GtHeadlessOperationHandle::id() const
{
    return m_state ? m_state->status().id : QString();
}

bool GtHeadlessOperationHandle::isValid() const
{
    return !id().isEmpty();
}

GtHeadlessOperationStatus GtHeadlessOperationHandle::status() const
{
    return m_state ? m_state->status() : GtHeadlessOperationStatus{};
}

GtHeadlessOperationCancellationResult GtHeadlessOperationHandle::cancel() const
{
    if (!m_state)
    {
        return {GtHeadlessOperationCancellationResult::Code::InvalidHandle,
                QStringLiteral("Operation handle is invalid")};
    }

    QMutexLocker lock(&m_state->mutex);
    if (m_state->snapshot.isDone())
    {
        const auto code = m_state->snapshot.state ==
                                  GtHeadlessOperationStatus::State::Shutdown ?
                              GtHeadlessOperationCancellationResult::Code::RuntimeShutdown :
                              GtHeadlessOperationCancellationResult::Code::AlreadyCompleted;
        return {code, QStringLiteral("Operation has already completed")};
    }
    m_state->cancellation.requestCancellation();
    return {GtHeadlessOperationCancellationResult::Code::Accepted, {}};
}

GtHeadlessOperationStatus GtHeadlessOperationHandle::wait(int timeoutMs) const
{
    if (!m_state)
    {
        return {};
    }

    QMutexLocker lock(&m_state->mutex);
    if (isRuntimeOwnerThread() && !m_state->snapshot.isDone())
    {
        auto status = m_state->snapshot;
        status.waitRejected = true;
        status.error = QStringLiteral(
            "Waiting on the GTlab owner thread could block operation execution");
        return status;
    }

    QElapsedTimer timer;
    timer.start();
    while (!m_state->snapshot.isDone())
    {
        if (timeoutMs == 0)
        {
            break;
        }
        if (timeoutMs < 0)
        {
            m_state->changed.wait(&m_state->mutex);
        }
        else
        {
            const auto remaining = timeoutMs - static_cast<int>(timer.elapsed());
            if (remaining <= 0 || !m_state->changed.wait(&m_state->mutex,
                                                          static_cast<unsigned long>(remaining)))
            {
                break;
            }
        }
    }
    return m_state->snapshot;
}

QSharedPointer<GtExecutionEventStream> GtHeadlessOperationHandle::events() const
{
    return m_state ? m_state->eventStream : QSharedPointer<GtExecutionEventStream>{};
}

QSharedPointer<GtObject const> GtHeadlessOperationHandle::result() const
{
    if (!m_state)
    {
        return {};
    }
    QMutexLocker lock(&m_state->mutex);
    return m_state->executionResult;
}

bool GtHeadlessRuntimeResult::succeeded() const
{
    return code == Code::Success;
}

GtHeadlessProjectRuntime::GtHeadlessProjectRuntime(QObject* parent) :
    QObject(parent), m_private(new Private)
{
}

GtHeadlessProjectRuntime::~GtHeadlessProjectRuntime()
{
    if (isRuntimeOwnerThread())
    {
        shutdown();
        return;
    }
    if (gtApp && gtApp->thread() && gtApp->thread()->isRunning() &&
        !QCoreApplication::closingDown())
    {
        QMetaObject::invokeMethod(gtApp, [this] { shutdown(); },
                                  Qt::BlockingQueuedConnection);
        return;
    }
    Q_ASSERT_X(!m_private->project, "GtHeadlessProjectRuntime::~GtHeadlessProjectRuntime",
               "Runtime must be destroyed on the GTlab owner thread while the application is running");
}

void GtHeadlessProjectRuntime::shutdown()
{
    for (const auto& operation : m_private->operations)
    {
        if (!operation->status().isDone())
        {
            GtHeadlessOperationHandle handle(operation);
            handle.cancel();
            // execute() runs synchronously on this thread. A queued operation
            // has not started and can be safely made terminal here.
            if (operation->status().state == GtHeadlessOperationStatus::State::Queued)
            {
                operation->setState(GtHeadlessOperationStatus::State::Shutdown,
                                    GtHeadlessOperationStatus::Result::RuntimeShutdown,
                                    QStringLiteral("Runtime shut down before operation started"));
                operation->operation.reset();
                operation->data.reset();
            }
        }
    }
    if (m_private->project)
    {
        closeProject();
    }
}

GtHeadlessRuntimeResult GtHeadlessProjectRuntime::initialize()
{
    if (m_private->state == State::Initialized)
    {
        return {};
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
    if (!isRuntimeOwnerThread())
    {
        return failure(GtHeadlessRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime must be initialized on the GTlab owner thread"));
    }
    gtApp->init();
    if (!gtApp->session()) gtApp->initSession();
    if (!gtApp->session())
    {
        return failure(GtHeadlessRuntimeResult::Code::CoreUnavailable,
                       QStringLiteral("GTlab Core session could not be initialized"));
    }
    m_private->state = State::Initialized;
    return {};
}

GtHeadlessRuntimeResult GtHeadlessProjectRuntime::openProject(QString const& projectPath)
{
    if (!isRuntimeOwnerThread())
        return failure(GtHeadlessRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime must be used from the GTlab owner thread"));
    if (m_private->project || (gtDataModel && gtDataModel->currentProject()))
        return failure(GtHeadlessRuntimeResult::Code::ProjectAlreadyLoaded,
                       QStringLiteral("A project is already loaded"));
    if (m_private->state != State::Initialized)
        return failure(GtHeadlessRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime is not initialized"));

    const QFileInfo info(projectPath);
    const QString filePath = info.isDir() ?
        QDir(info.absoluteFilePath()).filePath(GtProject::mainFilename()) : projectPath;
    if (!QFileInfo::exists(filePath))
        return failure(GtHeadlessRuntimeResult::Code::InvalidProject,
                       QStringLiteral("Project file does not exist: %1").arg(filePath));
    GtProjectProvider provider(filePath);
    std::unique_ptr<GtProject> project(provider.project());
    if (!project || !project->isValid())
        return failure(GtHeadlessRuntimeResult::Code::InvalidProject,
                       QStringLiteral("Project is invalid: %1").arg(filePath));
    auto* ptr = project.get();
    if (!gtDataModel->newProject(ptr, true))
        return failure(GtHeadlessRuntimeResult::Code::InvalidProject,
                       QStringLiteral("Project could not be opened: %1").arg(filePath));
    project.release();
    if (!ptr->isOpen())
    {
        gtDataModel->deleteProject(ptr);
        return failure(GtHeadlessRuntimeResult::Code::InvalidProject,
                       QStringLiteral("Project could not be loaded: %1").arg(filePath));
    }
    m_private->project = ptr;
    m_private->state = State::ProjectLoaded;
    return {};
}

namespace
{
template<typename Operations>
bool hasRunningProjectOperation(Operations const& operations)
{
    for (const auto& operation : operations)
    {
        if (operation->requiresProject && !operation->status().isDone()) return true;
    }
    return false;
}
}

GtHeadlessRuntimeResult GtHeadlessProjectRuntime::saveProject()
{
    if (!isRuntimeOwnerThread())
        return failure(GtHeadlessRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime must be used from the GTlab owner thread"));
    if (m_private->state != State::ProjectLoaded || !m_private->project)
        return failure(GtHeadlessRuntimeResult::Code::InvalidState,
                       QStringLiteral("No project is loaded"));
    if (hasRunningProjectOperation(m_private->operations) ||
        GtProjectExecutionGuard::isLocked(m_private->project))
        return failure(GtHeadlessRuntimeResult::Code::ProjectBusy,
                       QStringLiteral("Cannot save while project execution is active"));
    return gtDataModel->saveProject(m_private->project) ? GtHeadlessRuntimeResult{} :
        failure(GtHeadlessRuntimeResult::Code::SaveFailed,
                QStringLiteral("Project could not be saved"));
}

GtHeadlessRuntimeResult GtHeadlessProjectRuntime::closeProject()
{
    if (!isRuntimeOwnerThread())
        return failure(GtHeadlessRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime must be used from the GTlab owner thread"));
    if ((m_private->state != State::ProjectLoaded && m_private->state != State::CloseFailed) ||
        !m_private->project)
        return failure(GtHeadlessRuntimeResult::Code::InvalidState,
                       QStringLiteral("No project is loaded"));
    if (hasRunningProjectOperation(m_private->operations) ||
        GtProjectExecutionGuard::isLocked(m_private->project))
        return failure(GtHeadlessRuntimeResult::Code::ProjectBusy,
                       QStringLiteral("Cannot close while project execution is active"));

    auto* project = m_private->project.data();
    if (m_private->state != State::CloseFailed && !gtDataModel->closeProject(project))
        return failure(GtHeadlessRuntimeResult::Code::CloseFailed,
                       QStringLiteral("Project could not be closed"));
    if (!gtDataModel->deleteProject(project))
    {
        m_private->state = State::CloseFailed;
        return failure(GtHeadlessRuntimeResult::Code::CloseFailed,
                       QStringLiteral("Project could not be removed"));
    }
    m_private->project.clear();
    m_private->state = State::Closed;
    return {};
}

GtHeadlessProjectRuntime::State GtHeadlessProjectRuntime::state() const
{
    return m_private->state;
}

QString GtHeadlessProjectRuntime::projectPath() const
{
    return m_private->project ? m_private->project->path() : QString();
}

GtHeadlessOperationHandle GtHeadlessProjectRuntime::submitOperation(
    std::unique_ptr<GtExecutableOperation> operation,
    std::unique_ptr<GtObject> data,
    GtHeadlessRuntimeResult* result)
{
    const auto reject = [&](GtHeadlessRuntimeResult value) {
        if (result) *result = std::move(value);
        return GtHeadlessOperationHandle{};
    };
    if (!isRuntimeOwnerThread())
        return reject(failure(GtHeadlessRuntimeResult::Code::InvalidState,
                              QStringLiteral("Operation submission requires the GTlab owner thread")));
    if (!operation)
        return reject(failure(GtHeadlessRuntimeResult::Code::OperationRejected,
                              QStringLiteral("Operation is null")));
    if (m_private->state != State::Initialized && m_private->state != State::ProjectLoaded)
        return reject(failure(GtHeadlessRuntimeResult::Code::InvalidState,
                              QStringLiteral("Runtime is not available for operation submission")));
    if (operation->requiresProject() && !m_private->project)
        return reject(failure(GtHeadlessRuntimeResult::Code::OperationRejected,
                              QStringLiteral("Operation requires a loaded project")));

    auto state = QSharedPointer<GtHeadlessOperationHandle::State>::create();
    state->snapshot.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    state->snapshot.state = GtHeadlessOperationStatus::State::Queued;
    state->requiresProject = operation->requiresProject();
    state->operation = std::move(operation);
    state->data = std::move(data);
    GtExecutionId executionId;
    state->eventStream.reset(new GtExecutionEventStream(executionId));
    // The event stream provides the authoritative execution identity.
    state->snapshot.id = state->eventStream->executionId().toString();
    m_private->operations.push_back(state);

    const bool queued = QMetaObject::invokeMethod(this, [this, state] {
        const auto releaseStateFromRuntime = [this, &state] {
            auto& operations = m_private->operations;
            operations.erase(std::remove(operations.begin(), operations.end(), state),
                             operations.end());
        };
        if (state->cancellation.isCancellationRequested())
        {
            state->setState(GtHeadlessOperationStatus::State::Cancelled,
                            GtHeadlessOperationStatus::Result::Cancelled,
                            QStringLiteral("Operation cancelled before execution"));
            state->operation.reset();
            state->data.reset();
            releaseStateFromRuntime();
            return;
        }
        if (state->requiresProject && !m_private->project)
        {
            state->setState(GtHeadlessOperationStatus::State::Failed,
                            GtHeadlessOperationStatus::Result::ExecutionFailed,
                            QStringLiteral("Required project is no longer available"));
            state->operation.reset();
            state->data.reset();
            releaseStateFromRuntime();
            return;
        }
        state->setState(GtHeadlessOperationStatus::State::Running);
        GtProjectExecutionGuard guard;
        if (state->requiresProject &&
            guard.tryAcquire(m_private->project) != GtProjectExecutionGuard::Result::Acquired)
        {
            state->setState(GtHeadlessOperationStatus::State::Failed,
                            GtHeadlessOperationStatus::Result::ExecutionFailed,
                            QStringLiteral("Project is busy"));
            state->operation.reset();
            state->data.reset();
            releaseStateFromRuntime();
            return;
        }
        auto operation = std::move(state->operation);
        auto data = std::move(state->data);
        GtOperationExecutionContext operationContext(
            data.get(), *state->eventStream, state->cancellation);
        try
        {
            std::unique_ptr<GtObject> value;
            if (state->requiresProject)
            {
                GtExecutionContext projectContext(m_private->project);
                GtExecutionContextScope scope(projectContext);
                value = operation->execute(operationContext);
            }
            else
            {
                value = operation->execute(operationContext);
            }
            state->completeSuccessfully(std::move(value));
        }
        catch (std::exception const& e)
        {
            state->setState(GtHeadlessOperationStatus::State::Failed,
                            GtHeadlessOperationStatus::Result::ExecutionFailed,
                            QString::fromUtf8(e.what()));
        }
        catch (...)
        {
            state->setState(GtHeadlessOperationStatus::State::Failed,
                            GtHeadlessOperationStatus::Result::ExecutionFailed,
                            QStringLiteral("Operation failed with an unknown exception"));
        }
        releaseStateFromRuntime();
    }, Qt::QueuedConnection);
    if (!queued)
    {
        m_private->operations.pop_back();
        return reject(failure(GtHeadlessRuntimeResult::Code::OperationRejected,
                              QStringLiteral("Operation could not be scheduled")));
    }
    if (result) *result = {};
    return GtHeadlessOperationHandle(state);
}
