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

#include <QMutex>

#include <cstddef>
#include <memory>
#include <vector>

/**
 * @brief Owns the application's recording backends and dispatches to them.
 *
 * The manager is owned by the GtCoreApplication and acts as the
 * application-wide recording endpoint: finished GtRecordings are delivered
 * synchronously to every registered backend. All registered backends are
 * active (V1 has no per-task selection).
 *
 * The backends are owned by the manager (std::unique_ptr) and destroyed with
 * it; the destructor never calls back into a backend. A backend failure
 * (exception) is logged and swallowed - a backend failure must never change
 * the execution result of the recorded component - and the remaining
 * backends still receive the recording.
 *
 * Thread-safe: backends may be registered while recordings are delivered
 * (QMutex-protected).
 */
class GT_CORE_EXPORT GtRecordingManager
{
public:
    /**
     * @brief Constructor.
     */
    GtRecordingManager();

    /**
     * @brief Destructor. Destroys the registered backends without
     * calling back into them.
     */
    ~GtRecordingManager();

    GtRecordingManager(const GtRecordingManager& other) = delete;
    GtRecordingManager& operator=(const GtRecordingManager& other) = delete;

    /**
     * @brief Registers a recording backend (the manager takes ownership).
     * @param recorder Backend to register (nullptr is ignored).
     */
    void addRecorder(std::unique_ptr<GtAbstractRecorder> recorder);

    /**
     * @brief Returns the number of registered backends.
     */
    std::size_t recorderCount() const;

    /**
     * @brief Returns whether at least one backend is registered.
     */
    bool isEnabled() const;

    /**
     * @brief Synchronously delivers a finished recording to all registered
     * backends. A backend failure is logged and swallowed; the remaining
     * backends still receive the recording.
     * @param recording Finished recording.
     */
    void deliver(const GtRecording& recording);

private:
    /// Registered backends
    std::vector<std::unique_ptr<GtAbstractRecorder>> m_recorders;

    /// Protects the backend list
    mutable QMutex m_mutex;
};
