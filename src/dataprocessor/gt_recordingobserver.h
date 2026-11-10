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

    /**
     * @brief Called once before the recorded work starts.
     *
     * Like recordingFinished(), it is called regardless of the session's
     * event mask. If it throws, the work runs without recording.
     */
    virtual void recordingStarted(const GtRecordingSession& session);

    /**
     * @brief Called once after the recorded work finished.
     *
     * Not called if recordingStarted() threw.
     *
     * @param success Whether the recorded execution succeeded.
     */
    virtual void recordingFinished(const GtRecordingSession& session,
                                   bool success);

    /// A property of an observed object was read.
    virtual void propertyRead(const GtObject& owner,
                              const GtAbstractProperty& property);

    /**
     * @brief A value of an observed object was read through an accessor
     * of the object itself (see gt::ObjectValueReadScope), not through a
     * registered property.
     */
    virtual void objectValueRead(const GtObject& object);

    /// A property of an observed object reported a change.
    virtual void propertyWritten(const GtObject& owner,
                                 const GtAbstractProperty& property);

    /**
     * @brief A value of an observed object was changed through an accessor
     * of the object itself (see gt::notifyObjectValueWritten()), not through
     * a registered property. Counterpart of objectValueRead().
     */
    virtual void objectValueWritten(const GtObject& object);

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
     * The child is still alive. Deleting a child is reported by
     * objectDestroyed() instead.
     */
    virtual void objectDetached(const GtObject& parent, const GtObject& child);

    /**
     * @brief An observed object was deleted.
     *
     * Sent for every deleted observed object, roots included. The object's
     * derived class is already destroyed; only its identity and UUID are
     * valid. Descendants of the object are not reported separately.
     *
     * Unlike the other events, suspended parent sessions receive it too if
     * they observe the object, so an observer never keeps a pointer to a
     * deleted object.
     *
     * @param parent Parent the object had, which may be unobserved (e.g. for
     *        an observed root), or nullptr if it had none.
     * @param object Deleted object.
     */
    virtual void objectDestroyed(const GtObject* parent,
                                 const GtObject& object);
};

#endif // GTRECORDINGOBSERVER_H
