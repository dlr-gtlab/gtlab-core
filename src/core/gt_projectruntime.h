/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTPROJECTRUNTIME_H
#define GTPROJECTRUNTIME_H

#include "gt_core_exports.h"
#include "gt_object.h"
#include "operations/gt_operationexecutioncontext.h"

#include <QObject>
#include <QScopedPointer>
#include <QString>

#include <memory>

class GtExecutableOperation;
class GtObjectGroup;
class GtExecutionEventStream;

/** Outcome of a project lifecycle request. */
struct GT_CORE_EXPORT GtProjectRuntimeResult
{
    enum class Code
    {
        Success, InvalidState, CoreUnavailable, InvalidProject,
        ProjectAlreadyLoaded, ProjectBusy, SaveFailed, CloseFailed
    };
    Code code{Code::Success};
    QString message;
    bool succeeded() const;
    explicit operator bool() const { return succeeded(); }
};

/** Outcome of one synchronous operation execution. */
struct GT_CORE_EXPORT GtProjectRuntimeExecutionResult
{
    enum class Code
    {
        Success, InvalidState, InvalidOperation, ProjectRequired,
        ProjectBusy, ExecutionFailed
    };
    Code code{Code::Success};
    QString message;
    std::unique_ptr<GtObject> result;
    bool succeeded() const;
    explicit operator bool() const { return succeeded(); }
};

/**
 * @brief Synchronous, placement-neutral execution boundary for one project.
 *
 * The host places this runtime and its Core services in a compatible execution
 * location. All methods must be called on the runtime's Qt thread; no method
 * schedules, marshals or spawns execution. Events and cancellation belong to
 * the caller and must outlive executeOperation(). The returned result is
 * detached and is never applied to an originating project here.
 */
class GT_CORE_EXPORT GtProjectRuntime : public QObject
{
    Q_OBJECT
public:
    enum class State { Created, Initialized, ProjectLoaded, CloseFailed, Closed };
    Q_ENUM(State)

    explicit GtProjectRuntime(QObject* parent = nullptr);
    ~GtProjectRuntime() override;

    /**
     * @brief Opens execution-local project data restored from a project Memento.
     *
     * The Memento must use the format returned by
     * GtProject::toProjectDataMemento(). The runtime takes ownership of the
     * object group and keeps the in-memory project alive until closeProject().
     */
    GtProjectRuntimeResult openProjectFromMemento(
        std::unique_ptr<GtObjectGroup> projectData, QString const& workingDirectory);
    /** Initializes Core services at the host-selected execution location. */
    GtProjectRuntimeResult initialize();
    /** Opens one project in the current execution-local Core session. */
    GtProjectRuntimeResult openProject(QString const& projectPath);
    /** Saves the loaded project unless it is guarded by execution. */
    GtProjectRuntimeResult saveProject();
    /** Closes and removes the loaded project unless it is guarded. */
    GtProjectRuntimeResult closeProject();
    /** Returns the runtime lifecycle state. */
    State state() const;
    /** Returns the loaded project's path, or an empty string. */
    QString projectPath() const;

    /**
     * @brief Executes one operation directly on the calling thread.
     *
     * Takes ownership of the operation and detached input data. The caller
     * supplies an invocation-local event stream and cooperative cancellation
     * token. Execution exceptions become structured failures. This method
     * returns only after execute() finishes and transfers ownership of its
     * detached result to the caller.
     */
    GtProjectRuntimeExecutionResult executeOperation(
        std::unique_ptr<GtExecutableOperation> operation,
        std::unique_ptr<GtObject> data,
        GtExecutionEventStream& events,
        GtCancellationToken cancellation = {});

private:
    Q_DISABLE_COPY(GtProjectRuntime)
    struct Private;
    QScopedPointer<Private> m_private;
};

#endif // GTPROJECTRUNTIME_H
#include "gt_object.h"
