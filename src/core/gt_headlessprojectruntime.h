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

    QString id;
    State state{State::Invalid};
    GtProcessComponent::STATE processState{GtProcessComponent::NONE};
    int progress{-1};
    QString error;

    bool isDone() const;
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

    QString id() const;
    bool isValid() const;
    GtHeadlessTaskStatus status() const;

    /**
     * @brief Request cancellation of the associated task.
     * @return Whether a cancellation request was accepted.
     */
    bool cancel() const;

    /**
     * @brief Wait for completion while processing the Qt event loop.
     * @param timeoutMs Negative values wait without a timeout.
     * @return Final status, or the current status after a timeout.
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
    QString name;
    QString group;
    QString taskId;
    QString uuid;
    QString path;
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
        SaveFailed,
        CloseFailed
    };

    Code code{Code::Success};
    QString message;

    bool succeeded() const;
    explicit operator bool() const { return succeeded(); }
};

/**
 * @brief Headless facade for one explicitly loaded GTlab project.
 *
 * The facade uses the existing Core datamodel and process executor. It does
 * not own QCoreApplication. The project added by openProject() is removed
 * from the current session again by closeProject().
 */
class GT_CORE_EXPORT GtHeadlessProjectRuntime : public QObject
{
    Q_OBJECT

public:
    enum class State
    {
        Created,
        Initialized,
        ProjectLoaded,
        Closed
    };
    Q_ENUM(State)

    explicit GtHeadlessProjectRuntime(QObject* parent = nullptr);
    ~GtHeadlessProjectRuntime() override;

    GtHeadlessRuntimeResult initialize();
    GtHeadlessRuntimeResult openProject(const QString& projectPath);
    GtHeadlessRuntimeResult saveProject();
    GtHeadlessRuntimeResult closeProject();

    State state() const;
    QString projectPath() const;
    QVector<GtHeadlessTaskDescriptor> listTasks() const;
    GtHeadlessTaskHandle submitTask(const QString& taskReference,
                                    GtHeadlessRuntimeResult* result = nullptr);

private:
    Q_DISABLE_COPY(GtHeadlessProjectRuntime)

    struct Private;
    QScopedPointer<Private> m_private;
};

#endif // GTHEADLESSPROJECTRUNTIME_H
