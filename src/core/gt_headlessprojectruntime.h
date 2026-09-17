/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTHEADLESSPROJECTRUNTIME_H
#define GTHEADLESSPROJECTRUNTIME_H

#include "gt_core_exports.h"
#include "operations/gt_executioneventstream.h"

#include <QObject>
#include <QMutex>
#include <QPointer>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QString>
#include <QWaitCondition>

#include <memory>

class GtExecutableOperation;
class GtObject;
class GtProject;

struct GT_CORE_EXPORT GtHeadlessOperationStatus
{
    enum class State { Invalid, Queued, Running, Finished, Failed, Cancelled, Shutdown };
    enum class Result { None, Succeeded, Cancelled, ExecutionFailed, RuntimeShutdown };

    QString id;
    State state{State::Invalid};
    Result result{Result::None};
    QString error;
    bool waitRejected{false};

    /** Returns true for every terminal state. */
    bool isDone() const;
};

struct GT_CORE_EXPORT GtHeadlessOperationCancellationResult
{
    enum class Code { Accepted, AlreadyCompleted, RuntimeShutdown, InvalidHandle };
    Code code{Code::InvalidHandle};
    QString message;
    bool succeeded() const;
    explicit operator bool() const { return succeeded(); }
};

class GT_CORE_EXPORT GtHeadlessOperationHandle
{
public:
    GtHeadlessOperationHandle();
    ~GtHeadlessOperationHandle();
    GtHeadlessOperationHandle(const GtHeadlessOperationHandle&) = default;
    GtHeadlessOperationHandle& operator=(const GtHeadlessOperationHandle&) = default;
    GtHeadlessOperationHandle(GtHeadlessOperationHandle&&) noexcept = default;
    GtHeadlessOperationHandle& operator=(GtHeadlessOperationHandle&&) noexcept = default;

    /** Returns the stable execution ID, or an empty string for an invalid handle. */
    QString id() const;
    /** Returns whether this handle refers to a submitted operation. */
    bool isValid() const;
    /** Returns a synchronized value snapshot and is safe from any thread. */
    GtHeadlessOperationStatus status() const;
    /** Requests cooperative cancellation; safe from any thread. */
    GtHeadlessOperationCancellationResult cancel() const;
    /** Waits off the owner thread; owner-thread waiting while active is rejected. */
    GtHeadlessOperationStatus wait(int timeoutMs = -1) const;
    /** Returns shared access to the invocation event stream. */
    QSharedPointer<GtExecutionEventStream> events() const;
    /**
     * Returns the non-consuming detached result, or null before success.
     * Inspect and release the GTlab object on the GTlab owner thread.
     */
    QSharedPointer<GtObject const> result() const;

private:
    struct State;
    explicit GtHeadlessOperationHandle(QSharedPointer<State> const& state);
    QSharedPointer<State> m_state;
    friend class GtHeadlessProjectRuntime;
};

struct GT_CORE_EXPORT GtHeadlessRuntimeResult
{
    enum class Code
    {
        Success, InvalidState, CoreUnavailable, InvalidProject,
        ProjectAlreadyLoaded, ProjectBusy, OperationRejected,
        SaveFailed, CloseFailed
    };
    Code code{Code::Success};
    QString message;
    bool succeeded() const;
    explicit operator bool() const { return succeeded(); }
};

/**
 * @brief Single-project application and operation-execution boundary.
 *
 * Lifecycle methods and submission run on the GTlab owner thread. Operation
 * execute() is queued on that thread; handle status and cancellation are
 * independently synchronized. The runtime owns one loaded project and never
 * applies an operation result to the originating project.
 */
class GT_CORE_EXPORT GtHeadlessProjectRuntime : public QObject
{
    Q_OBJECT
public:
    enum class State { Created, Initialized, ProjectLoaded, CloseFailed, Closed };
    Q_ENUM(State)

    explicit GtHeadlessProjectRuntime(QObject* parent = nullptr);
    ~GtHeadlessProjectRuntime() override;

    /** Initializes the required Core services on the owner thread. */
    GtHeadlessRuntimeResult initialize();
    /** Opens exactly one project into the current Core session. */
    GtHeadlessRuntimeResult openProject(QString const& projectPath);
    /** Saves the project, rejecting active project-bound execution. */
    GtHeadlessRuntimeResult saveProject();
    /** Closes and removes the project, rejecting active project-bound execution. */
    GtHeadlessRuntimeResult closeProject();
    /** Returns the current runtime lifecycle state. */
    State state() const;
    /** Returns the explicitly loaded project's path, or an empty string. */
    QString projectPath() const;

    /**
     * @brief Takes ownership and queues one execution-local operation.
     *
     * Submission must run on the GTlab owner thread and returns before
     * execute() begins. The optional data must be detached from the originating
     * project. A project-required operation is rejected when no project is
     * loaded. The result output describes submission, not operation completion.
     */
    GtHeadlessOperationHandle submitOperation(
        std::unique_ptr<GtExecutableOperation> operation,
        std::unique_ptr<GtObject> data = {},
        GtHeadlessRuntimeResult* result = nullptr);

private:
    Q_DISABLE_COPY(GtHeadlessProjectRuntime)
    void shutdown();
    struct Private;
    QScopedPointer<Private> m_private;
};

#endif // GTHEADLESSPROJECTRUNTIME_H
