/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_projectruntime.h"

#include "gt_coreapplication.h"
#include "gt_objectgroup.h"
#include "gt_coredatamodel.h"
#include "gt_executioncontext.h"
#include "gt_executableoperation.h"
#include "gt_object.h"
#include "gt_project.h"
#include "gt_package.h"
#include "gt_projectexecutionguard.h"
#include "provider/gt_projectprovider.h"

#include <QDir>
#include <QFileInfo>
#include <QPointer>
#include <QThread>

#include <exception>
#include <utility>

namespace
{
class MementoExecutionProject final : public GtProject
{
public:
    explicit MementoExecutionProject(QString const& path) : GtProject(path) {}
};

GtProjectRuntimeResult failure(GtProjectRuntimeResult::Code code, QString message)
{
    return {code, std::move(message)};
}

GtProjectRuntimeExecutionResult executionFailure(
    GtProjectRuntimeExecutionResult::Code code, QString message)
{
    return {code, std::move(message), {}};
}
}

struct GtProjectRuntime::Private
{
    State state{State::Created};
    QPointer<GtProject> project;
    std::unique_ptr<GtProject> mementoProject;
};

bool GtProjectRuntimeResult::succeeded() const
{
    return code == Code::Success;
}

bool GtProjectRuntimeExecutionResult::succeeded() const
{
    return code == Code::Success;
}

GtProjectRuntime::GtProjectRuntime(QObject* parent) :
    QObject(parent), m_private(new Private)
{
}

GtProjectRuntime::~GtProjectRuntime()
{
    Q_ASSERT_X(thread() == QThread::currentThread(), "GtProjectRuntime::~GtProjectRuntime",
               "Destroy the runtime on its execution thread");
    if (m_private->project) closeProject();
}

GtProjectRuntimeResult GtProjectRuntime::initialize()
{
    if (thread() != QThread::currentThread())
        return failure(GtProjectRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime must be used on its execution thread"));
    if (m_private->state == State::Initialized) return {};
    if (m_private->state != State::Created)
        return failure(GtProjectRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime cannot be initialized in its current state"));
    if (!gtApp || !gtDataModel || gtApp->thread() != QThread::currentThread() ||
        gtDataModel->thread() != QThread::currentThread())
        return failure(GtProjectRuntimeResult::Code::CoreUnavailable,
                       QStringLiteral("Execution-local GTlab Core services are unavailable"));
    gtApp->init();
    if (!gtApp->session()) gtApp->initSession();
    if (!gtApp->session())
        return failure(GtProjectRuntimeResult::Code::CoreUnavailable,
                       QStringLiteral("GTlab Core session could not be initialized"));
    m_private->state = State::Initialized;
    return {};
}

GtProjectRuntimeResult GtProjectRuntime::openProject(QString const& projectPath)
{
    if (thread() != QThread::currentThread())
        return failure(GtProjectRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime must be used on its execution thread"));
    if (m_private->project || (gtDataModel && gtDataModel->currentProject()))
        return failure(GtProjectRuntimeResult::Code::ProjectAlreadyLoaded,
                       QStringLiteral("A project is already loaded"));
    if (m_private->state != State::Initialized)
        return failure(GtProjectRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime is not initialized"));

    const QFileInfo info(projectPath);
    const QString filePath = info.isDir() ?
        QDir(info.absoluteFilePath()).filePath(GtProject::mainFilename()) : projectPath;
    if (!QFileInfo::exists(filePath))
        return failure(GtProjectRuntimeResult::Code::InvalidProject,
                       QStringLiteral("Project file does not exist: %1").arg(filePath));
    GtProjectProvider provider(filePath);
    std::unique_ptr<GtProject> project(provider.project());
    if (!project || !project->isValid())
        return failure(GtProjectRuntimeResult::Code::InvalidProject,
                       QStringLiteral("Project is invalid: %1").arg(filePath));
    auto* ptr = project.get();
    if (!gtDataModel->newProject(ptr, true))
        return failure(GtProjectRuntimeResult::Code::InvalidProject,
                       QStringLiteral("Project could not be opened: %1").arg(filePath));
    project.release();
    if (!ptr->isOpen())
    {
        gtDataModel->deleteProject(ptr);
        return failure(GtProjectRuntimeResult::Code::InvalidProject,
                       QStringLiteral("Project could not be loaded: %1").arg(filePath));
    }
    m_private->project = ptr;
    m_private->state = State::ProjectLoaded;
    return {};
}

GtProjectRuntimeResult GtProjectRuntime::openProjectFromMemento(
    std::unique_ptr<GtObjectGroup> projectData, QString const& workingDirectory)
{
    if (thread() != QThread::currentThread())
        return failure(GtProjectRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime must be used on its execution thread"));
    if (m_private->state != State::Initialized)
        return failure(GtProjectRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime is not initialized"));
    if (!gtDataModel || m_private->project || gtDataModel->currentProject())
        return failure(GtProjectRuntimeResult::Code::ProjectAlreadyLoaded,
                       QStringLiteral("A project is already loaded or Core is unavailable"));
    if (!projectData || !QFileInfo(workingDirectory).isDir())
        return failure(GtProjectRuntimeResult::Code::InvalidProject,
                       QStringLiteral("Project data or working directory is invalid"));

    const auto objects = projectData->findDirectChildren<GtObject*>();
    for (GtObject* object : objects)
    {
        if (!qobject_cast<GtPackage*>(object))
            return failure(GtProjectRuntimeResult::Code::InvalidProject,
                           QStringLiteral("Project Memento contains a non-package root object"));
    }

    auto project = std::make_unique<MementoExecutionProject>(workingDirectory);
    project->setObjectName(projectData->objectName().isEmpty()
                               ? QStringLiteral("Execution Project")
                               : projectData->objectName());
    project->setUuid(projectData->uuid());
    for (GtObject* object : objects)
    {
        object->disconnectFromParent();
        if (!project->appendChild(object))
        {
            delete object;
            return failure(GtProjectRuntimeResult::Code::InvalidProject,
                           QStringLiteral("Project data could not be attached"));
        }
    }

    // Project-data Mementos have no on-disk project metadata. Keep this
    // execution-local project under runtime ownership, outside the session.
    project->m_valid = true;
    project->markOpen();
    m_private->project = project.get();
    m_private->mementoProject = std::move(project);
    m_private->state = State::ProjectLoaded;
    return {};
}

GtProjectRuntimeResult GtProjectRuntime::saveProject()
{
    if (thread() != QThread::currentThread())
        return failure(GtProjectRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime must be used on its execution thread"));
    if (m_private->state != State::ProjectLoaded || !m_private->project)
        return failure(GtProjectRuntimeResult::Code::InvalidState,
                       QStringLiteral("No project is loaded"));
    if (GtProjectExecutionGuard::isLocked(m_private->project))
        return failure(GtProjectRuntimeResult::Code::ProjectBusy,
                       QStringLiteral("Cannot save while project execution is active"));
    return gtDataModel->saveProject(m_private->project) ? GtProjectRuntimeResult{} :
        failure(GtProjectRuntimeResult::Code::SaveFailed,
                QStringLiteral("Project could not be saved"));
}

GtProjectRuntimeResult GtProjectRuntime::closeProject()
{
    if (thread() != QThread::currentThread())
        return failure(GtProjectRuntimeResult::Code::InvalidState,
                       QStringLiteral("Runtime must be used on its execution thread"));
    if ((m_private->state != State::ProjectLoaded && m_private->state != State::CloseFailed) ||
        !m_private->project)
        return failure(GtProjectRuntimeResult::Code::InvalidState,
                       QStringLiteral("No project is loaded"));
    if (GtProjectExecutionGuard::isLocked(m_private->project))
        return failure(GtProjectRuntimeResult::Code::ProjectBusy,
                       QStringLiteral("Cannot close while project execution is active"));

    auto* project = m_private->project.data();
    if (m_private->mementoProject)
    {
        project->markClosed();
        m_private->project.clear();
        m_private->mementoProject.reset();
        m_private->state = State::Closed;
        return {};
    }
    if (m_private->state != State::CloseFailed && !gtDataModel->closeProject(project))
        return failure(GtProjectRuntimeResult::Code::CloseFailed,
                       QStringLiteral("Project could not be closed"));
    if (!gtDataModel->deleteProject(project))
    {
        m_private->state = State::CloseFailed;
        return failure(GtProjectRuntimeResult::Code::CloseFailed,
                       QStringLiteral("Project could not be removed"));
    }
    m_private->project.clear();
    m_private->state = State::Closed;
    return {};
}

GtProjectRuntime::State GtProjectRuntime::state() const
{
    return m_private->state;
}

QString GtProjectRuntime::projectPath() const
{
    return m_private->project ? m_private->project->path() : QString();
}

GtProjectRuntimeExecutionResult GtProjectRuntime::executeOperation(
    std::unique_ptr<GtExecutableOperation> operation,
    std::unique_ptr<GtObject> data,
    GtExecutionEventStream& events,
    GtCancellationToken cancellation)
{
    using Code = GtProjectRuntimeExecutionResult::Code;
    if (thread() != QThread::currentThread())
        return executionFailure(Code::InvalidState,
                                QStringLiteral("Runtime must be used on its execution thread"));
    if (m_private->state != State::Initialized && m_private->state != State::ProjectLoaded)
        return executionFailure(Code::InvalidState,
                                QStringLiteral("Runtime is not available for execution"));
    if (!operation)
        return executionFailure(Code::InvalidOperation, QStringLiteral("Operation is null"));
    const bool requiresProject = operation->requiresProject();
    if (requiresProject && !m_private->project)
        return executionFailure(Code::ProjectRequired,
                                QStringLiteral("Operation requires a loaded project"));

    GtProjectExecutionGuard guard;
    if (requiresProject &&
        guard.tryAcquire(m_private->project) != GtProjectExecutionGuard::Result::Acquired)
        return executionFailure(Code::ProjectBusy, QStringLiteral("Project is busy"));

    GtOperationExecutionContext context(data.get(), events, cancellation);
    try
    {
        std::unique_ptr<GtObject> value;
        if (requiresProject)
        {
            GtExecutionContext projectContext(m_private->project);
            GtExecutionContextScope scope(projectContext);
            value = operation->execute(context);
        }
        else
        {
            value = operation->execute(context);
        }
        return {Code::Success, {}, std::move(value)};
    }
    catch (std::exception const& e)
    {
        return executionFailure(Code::ExecutionFailed, QString::fromUtf8(e.what()));
    }
    catch (...)
    {
        return executionFailure(Code::ExecutionFailed,
                                QStringLiteral("Operation failed with an unknown exception"));
    }
}
