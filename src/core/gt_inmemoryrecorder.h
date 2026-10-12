/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 12.10.2026
 */

#pragma once

#include "gt_core_exports.h"

#include "gt_recording.h"

#include <QDateTime>
#include <QMutex>
#include <QSet>
#include <QString>
#include <QUuid>
#include <QVector>

#include <cstddef>

/**
 * @brief Detached copy of a finished GtRecording's data.
 *
 * GtRecording is move-only, so a backend cannot store the recording object
 * itself. A snapshot is a plain value holding the same detached data as the
 * recording had at the time the backend received it.
 */
struct GT_CORE_EXPORT GtRecordingSnapshot
{
    QUuid id;
    QUuid parentId;
    QUuid componentUuid;
    QString componentClassName;
    QString componentName;
    QDateTime startTime;
    QDateTime finishTime;
    GtProcessComponent::STATE outcome{GtProcessComponent::NONE};
    QSet<QUuid> accessedObjects;
};

/**
 * @brief In-memory recording backend.
 *
 * Collects one snapshot per delivered recording, in delivery order. Backends
 * are invoked synchronously from the delivering thread - worker threads for
 * component recordings, the main thread for execution-level recordings - so
 * the collection is mutex-protected.
 *
 * V1 limitation: the collection is not bounded; call clear() to release the
 * stored snapshots.
 */
class GT_CORE_EXPORT GtInMemoryRecorder : public GtAbstractRecorder
{
public:
    GtInMemoryRecorder() = default;
    ~GtInMemoryRecorder() override = default;

    /**
     * @brief Appends a snapshot of the delivered recording.
     * @param recording Finished recording.
     */
    void record(const GtRecording& recording) override;

    /**
     * @brief Returns the number of collected snapshots.
     */
    std::size_t count() const;

    /**
     * @brief Returns a copy of all collected snapshots (in delivery order).
     */
    QVector<GtRecordingSnapshot> recordings() const;

    /**
     * @brief Removes all collected snapshots.
     */
    void clear();

private:
    mutable QMutex m_mutex;
    QVector<GtRecordingSnapshot> m_recordings;
};
