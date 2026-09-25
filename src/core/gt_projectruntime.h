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

/**
 * @brief Result of a project lifecycle operation.
 *
 * The code identifies success or the failure category. The message contains
 * additional diagnostic information when an operation fails.
 */
struct GT_CORE_EXPORT GtProjectRuntimeResult
{
    /**
     * @brief Failure categories for project lifecycle operations.
     */
    enum class Code
    {
        Success,               ///< The requested operation succeeded.
        InvalidState,          ///< The runtime is in a state that rejects the request.
        CoreUnavailable,       ///< Required execution-local Core services are missing.
        InvalidProject,        ///< The project data is invalid or could not be loaded.
        ProjectAlreadyLoaded,  ///< A project is already active in the runtime or Core.
        ProjectBusy,           ///< Active execution prevents saving or closing.
        SaveFailed,            ///< The project could not be saved.
        CloseFailed            ///< The project could not be closed or removed.
    };

    Code code{Code::Success}; ///< Status of the lifecycle request.
    QString message;          ///< Diagnostic message, particularly on failure.

    /**
     * @brief Checks whether the lifecycle operation succeeded.
     * @return True if the result code is Success; otherwise false.
     */
    bool succeeded() const;

    /**
     * @brief Converts the result to its success state.
     * @return True if the result code is Success; otherwise false.
     */
    explicit operator bool() const
    {
        return succeeded();
    }
};

/**
 * @brief Result of one synchronous operation execution.
 *
 * On success, result owns the detached value returned by the operation. On
 * failure, code and message describe the execution failure and result is null.
 */
struct GT_CORE_EXPORT GtProjectRuntimeExecutionResult
{
    /**
     * @brief Failure categories for operation execution.
     */
    enum class Code
    {
        Success,           ///< The operation completed successfully.
        InvalidState,      ///< The runtime is not ready to execute an operation.
        InvalidOperation,  ///< The operation is null or otherwise invalid.
        ProjectRequired,   ///< The operation needs a project, but none is loaded.
        ProjectBusy,       ///< The loaded project is guarded by another execution.
        ExecutionFailed    ///< The operation reported or threw an execution error.
    };

    Code code{Code::Success}; ///< Status of the execution request.
    QString message;          ///< Diagnostic message, particularly on failure.
    std::unique_ptr<GtObject>
        result; ///< Detached result on success; otherwise null.

    /**
     * @brief Checks whether the operation executed successfully.
     * @return True if the result code is Success; otherwise false.
     */
    bool succeeded() const;

    /**
     * @brief Converts the result to its success state.
     * @return True if the result code is Success; otherwise false.
     */
    explicit operator bool() const
    {
        return succeeded();
    }
};

/**
 * @brief Manages one execution project and runs operations on it.
 *
 * Use this class after the caller has chosen where an operation will run and
 * provided GTlab Core services on that thread. The runtime can open one project
 * at that location, call an operation's execute() method synchronously, and
 * return its detached result. It can also run operations that do not need a
 * project.
 *
 * The runtime does not choose or create a thread or process, schedule work, or
 * move calls between threads. The caller must invoke it on the Qt thread that
 * owns its Core services. GtOperationExecutor prepares operation data, chooses
 * and schedules an execution backend, reports status, transports the result,
 * and calls applyResult(). The backend creates and calls this runtime at the
 * selected execution location.
 */
class GT_CORE_EXPORT GtProjectRuntime : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Lifecycle state of the runtime and its execution-local project.
     */
    enum class State
    {
        Created,        ///< Core services have not been initialized.
        Initialized,    ///< Core services are ready; no project is loaded.
        ProjectLoaded,  ///< A project is loaded and available for execution.
        CloseFailed,    ///< Closing failed; cleanup may be retried.
        Closed          ///< The project was closed; the runtime cannot reopen it.
    };
    Q_ENUM(State)

    /**
     * @brief Creates a project runtime.
     * @param parent Optional QObject parent that owns this runtime.
     */
    explicit GtProjectRuntime(QObject* parent = nullptr);

    /**
     * @brief Closes the loaded project, if any, and destroys the runtime.
     */
    ~GtProjectRuntime() override;

    /**
     * @brief Opens execution-local project data restored from a project Memento.
     *
     * The Memento must use the format returned by
     * GtProject::toProjectDataMemento(). The runtime takes ownership of the
     * object group and keeps the in-memory project alive until closeProject().
     * @param projectData Project-data object group transferred to the runtime.
     * @param workingDirectory Existing directory used as the project's path.
     * @return Success when the in-memory project is opened; otherwise the
     *         failure code and diagnostic message.
     */
    GtProjectRuntimeResult openProjectFromMemento(
        std::unique_ptr<GtObjectGroup> projectData,
        QString const& workingDirectory);

    /**
     * @brief Initializes Core services at the host-selected execution location.
     * @return Success when Core services are ready; otherwise the failure code
     *         and diagnostic message.
     */
    GtProjectRuntimeResult initialize();

    /**
     * @brief Opens one project in the current execution-local Core session.
     * @param path Project directory or path to its main project file.
     * @return Success when the project is loaded; otherwise the failure code
     *         and diagnostic message.
     */
    GtProjectRuntimeResult openProject(QString const& path);

    /**
     * @brief Saves the loaded project unless execution currently guards it.
     * @return Success when the project is saved; otherwise the failure code
     *         and diagnostic message.
     */
    GtProjectRuntimeResult saveProject();

    /**
     * @brief Closes and removes the loaded project unless execution guards it.
     * @return Success when the project is closed and removed; otherwise the
     *         failure code and diagnostic message.
     */
    GtProjectRuntimeResult closeProject();

    /**
     * @brief Returns the current runtime lifecycle state.
     * @return Current lifecycle state.
     */
    State state() const;

    /**
     * @brief Returns the loaded project's path.
     * @return Project path, or an empty string if no project is loaded.
     */
    QString projectPath() const;

    /**
     * @brief Executes one operation synchronously on the calling thread.
     *
     * Takes ownership of the operation and detached input data. The caller
     * supplies an invocation-local event stream and cooperative cancellation
     * token. Execution exceptions become structured failures. This method
     * returns only after execute() finishes and transfers ownership of its
     * detached result to the caller. If the operation requires a project, a
     * project must already be loaded in this runtime.
     * @param operation Operation to execute; ownership is transferred to the
     *                  runtime for the duration of the call.
     * @param data Optional detached input data transferred to the operation.
     * @param events Event stream that receives events published during this
     *               invocation; it must outlive this call.
     * @param cancellation Cooperative cancellation token observed by the
     *                      operation; cancellation does not interrupt it
     *                      automatically.
     * @return Execution status and, on success, ownership of the detached
     *         operation result.
     */
    GtProjectRuntimeExecutionResult executeOperation(
        std::unique_ptr<GtExecutableOperation> operation,
        std::unique_ptr<GtObject> data, GtExecutionEventStream& events,
        GtCancellationToken cancellation = {});

private:
    Q_DISABLE_COPY(GtProjectRuntime)
    struct Private;
    QScopedPointer<Private> m_private;
};

#endif // GTPROJECTRUNTIME_H
