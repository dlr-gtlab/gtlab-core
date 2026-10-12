/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2017 German Aerospace Center (DLR)
 * Source File: gt_recording.cpp
 *
 *  Created on: 28.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_recording.h"

#include "gt_coreapplication.h"

#include <atomic>

namespace
{
// second thread-local slot: the active per-execution recording
thread_local GtRecording* currentRecordingSlot{nullptr};

// application-wide recording backend (release on store / acquire on load)
std::atomic<GtAbstractRecorder*> g_accessRecorder{nullptr};
}

GtRecording::GtRecording()
{
    m_id = QUuid::createUuid();
}

GtRecording::~GtRecording() = default;

QUuid
GtRecording::id() const
{
    return m_id;
}

QUuid
GtRecording::parentId() const
{
    return m_parentId;
}

void
GtRecording::setParentId(const QUuid& id)
{
    m_parentId = id;
}

QUuid
GtRecording::componentUuid() const
{
    return m_componentUuid;
}

QString
GtRecording::componentClassName() const
{
    return m_componentClassName;
}

QString
GtRecording::componentName() const
{
    return m_componentName;
}

void
GtRecording::setComponentInfo(const QUuid& uuid, const QString& className, const QString& name)
{
    m_componentUuid = uuid;
    m_componentClassName = className;
    m_componentName = name;
}

QDateTime
GtRecording::startTime() const
{
    return m_startTime;
}

QDateTime
GtRecording::finishTime() const
{
    return m_finishTime;
}

GtProcessComponent::STATE
GtRecording::outcome() const
{
    return m_outcome;
}

QSet<QUuid>
GtRecording::accessedObjects() const
{
    return m_accessedObjects;
}

void
GtRecording::addAccessedObjects(const QSet<QUuid>& objects)
{
    m_accessedObjects.unite(objects);
}

bool
GtRecording::isFinished() const
{
    return !m_finishTime.isNull();
}

void
GtRecording::start()
{
    m_startTime = QDateTime::currentDateTimeUtc();
}

void
GtRecording::finish(GtProcessComponent::STATE state)
{
    if (isFinished())
    {
        return;
    }

    m_finishTime = QDateTime::currentDateTimeUtc();
    m_outcome = state;

    if (m_tracker)
    {
        m_accessedObjects.unite(m_tracker->accessedObjects());
        m_tracker.reset();
    }
}

const std::unique_ptr<GtAccessTracker>&
GtRecording::tracker() const
{
    return m_tracker;
}

void
GtRecording::setTracker(std::unique_ptr<GtAccessTracker> tracker)
{
    m_tracker = std::move(tracker);
}

GtRecordingScope::GtRecordingScope(GtRecording& recording) :
    m_recording(recording)
{
    // always activate the tracker slot: use the recording's tracker when it
    // has one, otherwise explicitly disable tracking (null) so a passive
    // recording never leaves a previous tracker active
    m_trackerScope = std::make_unique<GtAccessTrackerScope>(m_recording.tracker().get());

    m_previousRecording = currentRecordingSlot;
    currentRecordingSlot = &m_recording;
    m_active = true;
}

GtRecordingScope::~GtRecordingScope()
{
    if (m_suspended)
    {
        resume();
    }

    if (m_active)
    {
        currentRecordingSlot = m_previousRecording;
        m_active = false;
    }

    m_trackerScope.reset();
}

GtRecording*
GtRecordingScope::currentRecording()
{
    return currentRecordingSlot;
}

void
GtRecordingScope::suspend()
{
    if (!m_active)
    {
        return;
    }

    currentRecordingSlot = m_previousRecording;
    m_trackerScope.reset();
    m_active = false;
    m_suspended = true;
}

void
GtRecordingScope::resume()
{
    if (!m_suspended || m_active)
    {
        return;
    }

    m_trackerScope = std::make_unique<GtAccessTrackerScope>(m_recording.tracker().get());

    m_previousRecording = currentRecordingSlot;
    currentRecordingSlot = &m_recording;
    m_active = true;
    m_suspended = false;
}

namespace gt
{
void
setAccessRecorder(GtAbstractRecorder* recorder)
{
    g_accessRecorder.store(recorder, std::memory_order_release);
}

bool
isAccessRecordingEnabled()
{
    if (g_accessRecorder.load(std::memory_order_acquire) != nullptr)
    {
        return true;
    }

    // backends registered with the application's recording manager
    if (GtCoreApplication* application = GtCoreApplication::instance())
    {
        return application->recordingManager()->isEnabled();
    }

    return false;
}

void
deliverRecording(const GtRecording& recording)
{
    // Resolve the delivery target: an explicitly registered single backend
    // (the path also used when no application is running) takes precedence;
    // otherwise all backends registered with the application's recording
    // manager receive the recording
    GtAbstractRecorder* recorder = g_accessRecorder.load(std::memory_order_acquire);
    GtRecordingManager* manager = nullptr;

    if (recorder == nullptr)
    {
        if (GtCoreApplication* application = GtCoreApplication::instance())
        {
            manager = application->recordingManager();
        }
    }

    if (recorder == nullptr && manager == nullptr)
    {
        return;
    }

    // Suspend tracking for the duration of the backend call (a null
    // scope installs nullptr and restores the previous tracker on
    // destruction), so property reads inside the backend do not pollute
    // the recording of the currently executing context
    GtAccessTrackerScope suspendTracking(nullptr);

    if (manager != nullptr)
    {
        // the manager dispatches synchronously and logs/swallows backend
        // failures itself
        manager->deliver(recording);
        return;
    }

    // a recorder failure must not change the component's execution result
    try
    {
        recorder->record(recording);
    }
    catch (...)
    {
    }
}

GtAbstractRecorder*
currentAccessRecorder()
{
    return g_accessRecorder.load(std::memory_order_acquire);
}
}
