/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTRECORDINGOBSERVER_H
#define GTRECORDINGOBSERVER_H

#include "gt_datamodel_exports.h"

class GtObject;
class GtAbstractProperty;
class GtRecordingSession;

/**
 * @brief Receives the events of a recording session.
 *
 * Implemented by modules (e.g. Provenance). Events are delivered directly and
 * synchronously in the thread that caused them, so implementations must be
 * thread-safe if the recorded code uses worker threads.
 *
 * While any callback runs, no recording is active on the calling thread:
 * callbacks may read properties without creating observations. Exceptions
 * thrown by callbacks are caught by the session and never reach the recorded
 * execution.
 */
class GT_DATAMODEL_EXPORT GtRecordingObserver
{
public:
    virtual ~GtRecordingObserver();

    /// Called once before the recorded work starts.
    virtual void recordingStarted(const GtRecordingSession& session);

    /// A property of an observed object was read.
    virtual void propertyRead(const GtObject& owner,
                              const GtAbstractProperty& property);

    /// A property of an observed object reported a change.
    virtual void propertyWritten(const GtObject& owner,
                                 const GtAbstractProperty& property);

    /**
     * @brief A child was attached to an observed parent.
     *
     * The child may still be under construction (no valid UUID, derived
     * class not constructed, properties not registered yet). Only store its
     * identity and resolve it later, e.g. in recordingFinished().
     */
    virtual void objectAttached(const GtObject& parent, const GtObject& child);

    /**
     * @brief A child was detached from an observed parent.
     *
     * Also sent when the child is deleted. In that case its derived class is
     * already destroyed; only its identity and UUID are valid.
     */
    virtual void objectDetached(const GtObject& parent, const GtObject& child);

    /**
     * @brief Called once after the recorded work finished.
     * @param success Whether the recorded execution succeeded.
     */
    virtual void recordingFinished(const GtRecordingSession& session,
                                   bool success);
};

#endif // GTRECORDINGOBSERVER_H
