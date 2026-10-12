/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 12.10.2026
 */

#include "gt_recordingmanager.h"

#include <QDebug>
#include <QMutexLocker>

GtRecordingManager::GtRecordingManager()
{
}

GtRecordingManager::~GtRecordingManager()
{
    // The backends are destroyed with the manager; no backend is called back
}

void
GtRecordingManager::addRecorder(std::unique_ptr<GtAbstractRecorder> recorder)
{
    if (recorder == nullptr)
    {
        return;
    }

    QMutexLocker locker(&m_mutex);
    m_recorders.push_back(std::move(recorder));
}

std::size_t
GtRecordingManager::recorderCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_recorders.size();
}

bool
GtRecordingManager::isEnabled() const
{
    QMutexLocker locker(&m_mutex);
    return !m_recorders.empty();
}

void
GtRecordingManager::deliver(const GtRecording& recording)
{
    // Snapshot the backends so a backend that re-enters the manager from
    // record() cannot deadlock on the mutex
    std::vector<GtAbstractRecorder*> targets;
    {
        QMutexLocker locker(&m_mutex);
        targets.reserve(m_recorders.size());
        for (const auto& recorder : m_recorders)
        {
            targets.push_back(recorder.get());
        }
    }

    // a backend failure must not change the component's execution result:
    // log it and continue with the remaining backends
    for (GtAbstractRecorder* target : targets)
    {
        try
        {
            target->record(recording);
        }
        catch (...)
        {
            qWarning() << "GtRecordingManager: a recording backend failed";
        }
    }
}
