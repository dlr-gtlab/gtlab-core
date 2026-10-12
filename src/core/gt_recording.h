/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2017 German Aerospace Center (DLR)
 * Source File: gt_recording.h
 *
 *  Created on: 28.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#pragma once

#include "gt_core_exports.h"

#include "gt_processcomponent.h"
#include "gt_accesstracking.h"

#include <QDateTime>
#include <QSet>
#include <QString>
#include <QUuid>

#include <memory>

/**
 * @brief A single per-execution recording of a process component.
 *
 * A recording captures the identity of the executed component, the start and
 * finish timestamps (UTC), the final process state and the set of object
 * identifiers that were accessed while the component executed. It is move-only
 * and holds no live object references: once finished it is a plain value that
 * can be handed to a GtAbstractRecorder.
 *
 * A recording instance is not thread-safe: create, activate (via
 * GtRecordingScope), finalize and deliver it from a single context (one
 * thread). The thread-local activation slots keep other threads isolated but
 * do not make the object thread-safe.
 */
class GT_CORE_EXPORT GtRecording
{
public:
    /**
     * @brief Constructor. Generates a fresh unique id.
     */
    GtRecording();

    ~GtRecording();

    GtRecording(const GtRecording& other) = delete;
    GtRecording& operator=(const GtRecording& other) = delete;

    GtRecording(GtRecording&& other) noexcept = default;
    GtRecording& operator=(GtRecording&& other) noexcept = default;

    /**
     * @brief Returns the unique id of this recording.
     */
    QUuid id() const;

    /**
     * @brief Returns the id of the parent recording (empty for top-level).
     */
    QUuid parentId() const;

    /**
     * @brief Sets the id of the parent recording.
     * @param id Parent recording id.
     */
    void setParentId(const QUuid& id);

    /**
     * @brief Returns the uuid of the recorded component.
     */
    QUuid componentUuid() const;

    /**
     * @brief Returns the class name of the recorded component.
     */
    QString componentClassName() const;

    /**
     * @brief Returns the object name of the recorded component.
     */
    QString componentName() const;

    /**
     * @brief Sets the identity of the recorded component.
     * @param uuid Component uuid.
     * @param className Component class name.
     * @param name Component object name.
     */
    void setComponentInfo(const QUuid& uuid, const QString& className, const QString& name);

    /**
     * @brief Returns the start time (UTC).
     */
    QDateTime startTime() const;

    /**
     * @brief Returns the finish time (UTC), or an invalid time while running.
     */
    QDateTime finishTime() const;

    /**
     * @brief Returns the final process state.
     */
    GtProcessComponent::STATE outcome() const;

    /**
     * @brief Returns the set of object ids that were accessed during execution.
     */
    QSet<QUuid> accessedObjects() const;

    /**
     * @brief Returns whether the recording has been finished.
     */
    bool isFinished() const;

    /**
     * @brief Marks the recording as started and stores the start time (UTC).
     */
    void start();

    /**
     * @brief Finishes the recording (idempotent).
     * Stores the finish time (UTC) and the final state, transfers the accessed
     * objects from the tracker and releases the tracker.
     * @param state Final process state.
     */
    void finish(GtProcessComponent::STATE state);

    /**
     * @brief Returns the (optional) access tracker owned by this recording.
     */
    const std::unique_ptr<GtAccessTracker>& tracker() const;

    /**
     * @brief Sets the access tracker owned by this recording.
     * @param tracker Tracker to adopt.
     */
    void setTracker(std::unique_ptr<GtAccessTracker> tracker);

private:
    /// Unique id of this recording
    QUuid m_id;

    /// Id of the parent recording (empty for top-level)
    QUuid m_parentId;

    /// Uuid of the recorded component
    QUuid m_componentUuid;

    /// Class name of the recorded component
    QString m_componentClassName;

    /// Object name of the recorded component
    QString m_componentName;

    /// Start time (UTC)
    QDateTime m_startTime;

    /// Finish time (UTC)
    QDateTime m_finishTime;

    /// Final process state
    GtProcessComponent::STATE m_outcome{GtProcessComponent::NONE};

    /// Object ids accessed during execution
    QSet<QUuid> m_accessedObjects;

    /// Optional access tracker (released on finish)
    std::unique_ptr<GtAccessTracker> m_tracker;
};

/**
 * @brief RAII scope that activates a GtRecording on the current thread.
 *
 * The scope borrows the recording (ownership stays with the recording, which
 * must outlive the scope). The scope always activates the tracker slot on the
 * current thread (reusing the GtAccessTrackerScope thread-local slot): it uses
 * the recording's tracker
 * when the recording owns one and explicitly disables tracking (null)
 * otherwise, so a passive recording never leaves a previous tracker active. The
 * recording itself is tracked in a second, recording-specific thread-local slot
 * so nested executions can link to their parent. Nested scopes restore the
 * previous thread state exactly on destruction. The destructor never finishes
 * the recording nor calls back into any backend.
 */
class GT_CORE_EXPORT GtRecordingScope
{
public:
    /**
     * @brief Constructor. Activates the recording (and its tracker, if any)
     * on the current thread.
     * @param recording Recording to activate.
     */
    explicit GtRecordingScope(GtRecording& recording);

    ~GtRecordingScope();

    GtRecordingScope(const GtRecordingScope& other) = delete;
    GtRecordingScope& operator=(const GtRecordingScope& other) = delete;

    /**
     * @brief Suspends the scope, restoring the previous thread state.
     */
    void suspend();

    /**
     * @brief Resumes a previously suspended scope.
     */
    void resume();

    /**
     * @brief Returns the recording currently active on this thread.
     * @return Active recording or nullptr.
     */
    static GtRecording* currentRecording();

private:
    /// Recording activated by this scope
    GtRecording& m_recording;

    /// Tracker scope (null when the recording is passive)
    std::unique_ptr<GtAccessTrackerScope> m_trackerScope;

    /// Previously active recording
    GtRecording* m_previousRecording{nullptr};

    /// Whether the scope is currently active
    bool m_active{false};

    /// Whether the scope is suspended
    bool m_suspended{false};
};

/**
 * @brief Base class for recording backends.
 *
 * Backends receive one finished GtRecording per executed process component.
 */
class GT_CORE_EXPORT GtAbstractRecorder
{
public:
    /**
     * @brief Constructor.
     */
    explicit GtAbstractRecorder() = default;

    /**
     * @brief Destructor.
     */
    virtual ~GtAbstractRecorder() = default;

    /**
     * @brief Receives a finished recording.
     * @param recording Finished recording.
     */
    virtual void record(const GtRecording& recording) = 0;
};

namespace gt
{
/**
 * @brief Registers an explicit single recording backend.
 *
 * While a backend is registered here, it is the exclusive delivery target of
 * gt::deliverRecording; when it is null, finished recordings are instead
 * dispatched to the backends registered with the application's
 * GtRecordingManager (GtCoreApplication::recordingManager).
 *
 * @param recorder Backend to register, or nullptr to fall back to the
 * application's recording manager.
 */
GT_CORE_EXPORT void setAccessRecorder(GtAbstractRecorder* recorder);

/**
 * @brief Returns whether access recording is currently enabled.
 *
 * Enabled when an explicit single backend is registered
 * (gt::setAccessRecorder) or when the application's recording manager owns
 * at least one backend.
 */
GT_CORE_EXPORT bool isAccessRecordingEnabled();

/**
 * @brief Delivers a finished recording.
 *
 * Delivers to the explicitly registered single backend when one is set,
 * otherwise to all backends registered with the application's
 * GtRecordingManager. Delivery runs with access tracking suspended and never
 * propagates backend failures.
 *
 * @param recording Finished recording.
 */
GT_CORE_EXPORT void deliverRecording(const GtRecording& recording);

/**
 * @brief Returns the explicitly registered single backend (may be nullptr).
 */
GT_CORE_EXPORT GtAbstractRecorder* currentAccessRecorder();
}
