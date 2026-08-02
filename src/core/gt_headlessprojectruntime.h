/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTHEADLESSPROJECTRUNTIME_H
#define GTHEADLESSPROJECTRUNTIME_H

#include "gt_core_exports.h"
#include "process_management/gt_processcomponent.h"

#include <QObject>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QString>
#include <QVector>

class GtCoreProcessExecutor;
class GtProject;

/**
 * @brief Value-based status returned by a headless task execution.
 */
struct GT_CORE_EXPORT GtHeadlessTaskStatus
{
    enum class State
    {
        Invalid,
        Queued,
        Running,
        Finished,
        Failed,
        Cancelled,
        Shutdown
    };

    enum class Result
    {
        None,
        Succeeded,
        Cancelled,
        TaskUnavailable,
        ExecutionFailed,
        RuntimeShutdown
    };

    /// Stable opaque identifier assigned at submission time.
    QString id;
    /// Current lifecycle state. Finished includes executor post-processing.
    State state{State::Invalid};
    /// Underlying Core process state, when the task object is available.
    GtProcessComponent::STATE processState{GtProcessComponent::NONE};
    /// Current task progress in percent, or -1 when unavailable.
    int progress{-1};
    /// Human-readable diagnostic for a failed, cancelled or shutdown task.
    QString error;
    /// Structured terminal outcome; None is used for non-terminal snapshots.
    Result result{Result::None};

    /// Returns true for Finished, Failed, Cancelled and Shutdown states.
    bool isDone() const;
};

/**
 * @brief Outcome of a headless task cancellation request.
 */
struct GT_CORE_EXPORT GtHeadlessTaskCancellationResult
{
    enum class Code
    {
        Accepted,
        AlreadyCompleted,
        RuntimeShutdown,
        TaskUnavailable,
        WrongThread,
        ExecutorUnavailable,
        ExecutorRejected
    };

    /// Exact outcome of the cancellation request.
    Code code{Code::TaskUnavailable};
    /// Human-readable explanation for a rejected request.
    QString message;

    /// Returns true only when the executor accepted cancellation.
    bool succeeded() const;
    explicit operator bool() const { return succeeded(); }
};

/**
 * @brief Copyable handle for a task submitted to a headless runtime.
 *
 * The handle contains no public Qt or GTlab object pointer. All methods must
 * be called from the thread owning the GTlab application and event loop.
 */
class GT_CORE_EXPORT GtHeadlessTaskHandle
{
public:
    GtHeadlessTaskHandle();
    ~GtHeadlessTaskHandle();

    GtHeadlessTaskHandle(const GtHeadlessTaskHandle&) = default;
    GtHeadlessTaskHandle& operator=(const GtHeadlessTaskHandle&) = default;
    GtHeadlessTaskHandle(GtHeadlessTaskHandle&&) noexcept = default;
    GtHeadlessTaskHandle& operator=(GtHeadlessTaskHandle&&) noexcept = default;

    /// Returns the stable opaque task identifier.
    QString id() const;
    /// Returns whether this handle refers to a submitted task.
    bool isValid() const;
    /// Returns the latest value snapshot. Must be called on the owner thread.
    GtHeadlessTaskStatus status() const;

    /**
     * @brief Request cancellation of the associated task.
     * @return Structured result describing whether cancellation was accepted.
     */
    GtHeadlessTaskCancellationResult cancel() const;

    /**
     * @brief Wait for completion while processing the Qt event loop.
     * @param timeoutMs Negative values wait without a timeout.
     * @return Final status, or the latest non-terminal status after a timeout.
     * Finished is returned only after complete executor processing, including
     * post-processing and result merging.
     */
    GtHeadlessTaskStatus wait(int timeoutMs = -1) const;

private:
    struct State;
    explicit GtHeadlessTaskHandle(const QSharedPointer<State>& state);

    QSharedPointer<State> m_state;

    friend class GtHeadlessProjectRuntime;
};

/**
 * @brief Lightweight task metadata exposed by a headless runtime.
 */
struct GT_CORE_EXPORT GtHeadlessTaskDescriptor
{
    /// Display name of the task.
    QString name;
    /// Task-group identifier.
    QString group;
    /// Task identifier used for name-based lookup.
    QString taskId;
    /// Stable task UUID used for group-independent lookup.
    QString uuid;
    /// Project path containing the task.
    QString path;
    /// Current underlying Core process state.
    GtProcessComponent::STATE state{GtProcessComponent::NONE};
};

/**
 * @brief Result code for runtime operations.
 */
struct GT_CORE_EXPORT GtHeadlessRuntimeResult
{
    enum class Code
    {
        Success,
        InvalidState,
        CoreUnavailable,
        InvalidProject,
        ProjectAlreadyLoaded,
        ProjectBusy,
        TaskNotFound,
        ExecutionRejected,
        ExecutionFailed,
        SaveFailed,
        CloseFailed
    };

    /// Exact outcome of the runtime operation.
    Code code{Code::Success};
    /// Human-readable explanation for a failed operation.
    QString message;

    /// Returns true only for Code::Success.
    bool succeeded() const;
    explicit operator bool() const { return succeeded(); }
};

/**
 * @brief Headless facade for one explicitly loaded GTlab project.
 *
 * The facade uses the existing Core datamodel and process executor. It does
 * not own QCoreApplication. The project added by openProject() is removed
 * from the current session again by closeProject(). The runtime must be
 * destroyed on the GTlab owner thread while the application is running;
 * destruction from another thread is marshalled only while that thread is
 * available.
 */
class GT_CORE_EXPORT GtHeadlessProjectRuntime : public QObject
{
    Q_OBJECT

public:
    /// Runtime lifecycle. CloseFailed can be retried; Closed is terminal.
    enum class State
    {
        Created,
        Initialized,
        ProjectLoaded,
        CloseFailed,
        Closed
    };
    Q_ENUM(State)

    explicit GtHeadlessProjectRuntime(QObject* parent = nullptr);
    /// Destroys the runtime and performs bounded owner-thread shutdown.
    ~GtHeadlessProjectRuntime() override;

    /// Initializes Core services. Must be called from the owner thread.
    GtHeadlessRuntimeResult initialize();
    /// Opens exactly one project. A second project is rejected.
    GtHeadlessRuntimeResult openProject(const QString& projectPath);
    /// Saves the loaded project, rejecting concurrent task execution.
    GtHeadlessRuntimeResult saveProject();
    /// Closes and removes the loaded project. CloseFailed remains retryable;
    /// Closed is terminal.
    GtHeadlessRuntimeResult closeProject();

    /// Returns the current lifecycle state.
    State state() const;
    /// Returns the path of the explicitly loaded project, if any.
    QString projectPath() const;
    /// Lists tasks without changing the selected task-group state.
    QVector<GtHeadlessTaskDescriptor> listTasks() const;
    /// Submits a task reference and optionally returns a structured error.
    GtHeadlessTaskHandle submitTask(const QString& taskReference,
                                    GtHeadlessRuntimeResult* result = nullptr);

private:
    Q_DISABLE_COPY(GtHeadlessProjectRuntime)

    void shutdown();
    void cleanupCompletedTasks();
    void restoreExecutorFlags();

    struct Private;
    QScopedPointer<Private> m_private;
};

#endif // GTHEADLESSPROJECTRUNTIME_H
