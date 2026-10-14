/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTRECORDINGSESSION_H
#define GTRECORDINGSESSION_H

#include "gt_datamodel_exports.h"

#include <QFlags>
#include <QList>
#include <QPointer>
#include <QUuid>

#include <functional>
#include <memory>
#include <type_traits>

class GtObject;
class GtAbstractProperty;
class GtRecordingObserver;

/**
 * @brief One opt-in recording of one execution.
 *
 * Created by an executor for a single invocation. The session borrows the
 * observer and the observed roots; it stores no events. While run() executes
 * the work, the session is active on the calling thread and forwards events
 * on the observed roots and their descendants to the observer.
 *
 * @code
 * ProvenanceObserver observer;
 * GtRecordingSession session{*calc, observer, calc->linkedObjects()};
 * return session.run([&] { return calc->run(); });
 * @endcode
 */
class GT_DATAMODEL_EXPORT GtRecordingSession
{
public:
    enum Event
    {
        PropertyRead    = 0x1,
        PropertyWritten = 0x2,
        ObjectAttached  = 0x4,
        ObjectDetached  = 0x8,
        AllEvents       = PropertyRead | PropertyWritten |
                          ObjectAttached | ObjectDetached
    };
    Q_DECLARE_FLAGS(Events, Event)

    /**
     * @brief Constructor.
     * @param component Recorded process component (borrowed).
     * @param observer Receiver of the events (borrowed).
     * @param roots Observed root objects; their descendants are observed too.
     *        Null entries are ignored.
     * @param eventMask Events forwarded to the observer.
     */
    GtRecordingSession(const GtObject& component,
                       GtRecordingObserver& observer,
                       const QList<const GtObject*>& roots,
                       Events eventMask = AllEvents);

    GtRecordingSession(const GtObject& component,
                       GtRecordingObserver& observer,
                       const QList<QPointer<GtObject>>& roots,
                       Events eventMask = AllEvents);

    ~GtRecordingSession();

    GtRecordingSession(const GtRecordingSession&) = delete;
    GtRecordingSession& operator=(const GtRecordingSession&) = delete;
    GtRecordingSession(GtRecordingSession&&) = delete;
    GtRecordingSession& operator=(GtRecordingSession&&) = delete;

    /// Unique ID of this recording.
    QUuid id() const;

    /// ID of the session that was active when run() was called, or null.
    QUuid parentId() const;

    /// Recorded process component.
    const GtObject& component() const;

    /// Events forwarded to the observer.
    Events eventMask() const;

    /**
     * @brief Runs the work while recording.
     *
     * Calls recordingStarted(), activates the session on the calling thread,
     * invokes the work, restores the previously active session and calls
     * recordingFinished(). A session is single-use; a second call runs the
     * work without recording.
     *
     * If the work throws, the recording finishes with success = false and
     * the exception is rethrown. If recordingStarted() throws, the work runs
     * without recording.
     *
     * @param work Callable returning a value convertible to bool.
     * @return Result of the work.
     */
    template <typename Work>
    bool run(Work&& work)
    {
        static_assert(std::is_convertible<std::invoke_result_t<Work&>,
                                          bool>::value,
                      "The recorded work must return a value convertible "
                      "to bool");

        if (!start()) return static_cast<bool>(std::invoke(work));

        bool success = false;
        try
        {
            success = static_cast<bool>(std::invoke(work));
        }
        catch (...)
        {
            finish(false);
            throw;
        }

        finish(success);
        return success;
    }

    /**
     * @brief Returns whether the object is an observed root or a descendant
     * of one. The observed tree is captured when run() starts.
     */
    bool isObserved(const GtObject& object) const;

    void notifyPropertyRead(const GtObject& owner,
                            const GtAbstractProperty& property);
    void notifyPropertyWritten(const GtObject& owner,
                               const GtAbstractProperty& property);
    void notifyObjectAttached(const GtObject& parent, const GtObject& child);
    void notifyObjectDetached(const GtObject& parent, const GtObject& child);

    /**
     * @brief Returns the session active on the calling thread, or nullptr.
     *
     * Returns nullptr while an observer callback runs.
     */
    static GtRecordingSession* current() noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

    /// Starts the recording. Returns false if the work must run unrecorded.
    bool start();

    /// Restores the previous session and finishes the recording.
    void finish(bool success) noexcept;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GtRecordingSession::Events)

#endif // GTRECORDINGSESSION_H
