/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 12.10.2026
 */
#include "gt_inmemoryrecorder.h"

#include <QMutexLocker>

void
GtInMemoryRecorder::record(const GtRecording& recording)
{
    QMutexLocker locker(&m_mutex);

    GtRecordingSnapshot snapshot;
    snapshot.id                 = recording.id();
    snapshot.parentId           = recording.parentId();
    snapshot.componentUuid      = recording.componentUuid();
    snapshot.componentClassName = recording.componentClassName();
    snapshot.componentName      = recording.componentName();
    snapshot.startTime          = recording.startTime();
    snapshot.finishTime         = recording.finishTime();
    snapshot.outcome            = recording.outcome();
    snapshot.accessedObjects    = recording.accessedObjects();

    m_recordings.append(snapshot);
}

std::size_t
GtInMemoryRecorder::count() const
{
    QMutexLocker locker(&m_mutex);
    return m_recordings.size();
}

QVector<GtRecordingSnapshot>
GtInMemoryRecorder::recordings() const
{
    QMutexLocker locker(&m_mutex);
    return m_recordings;
}

void
GtInMemoryRecorder::clear()
{
    QMutexLocker locker(&m_mutex);
    m_recordings.clear();
}
