/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_recordingsession.h"

#include "gt_logging.h"
#include "gt_object.h"
#include "gt_recordingobserver.h"

#include <QReadWriteLock>
#include <QSet>

#include <atomic>
#include <exception>

namespace
{

/// Session active on the current thread
thread_local GtRecordingSession* currentSession = nullptr;

/// Clears the active session of the current thread for its lifetime, so that
/// observer callbacks never create observations.
class SuppressionScope
{
public:
    SuppressionScope() : m_previous(currentSession)
    {
        currentSession = nullptr;
    }

    ~SuppressionScope() { currentSession = m_previous; }

    SuppressionScope(const SuppressionScope&) = delete;
    SuppressionScope& operator=(const SuppressionScope&) = delete;

private:
    GtRecordingSession* m_previous;
};

/// Calls an observer callback with observations suppressed. Returns false if
/// the callback threw.
template <typename Callback>
bool
callObserver(const char* callbackName, Callback&& callback) noexcept
{
    SuppressionScope suppression;

    try
    {
        callback();
        return true;
    }
    catch (const std::exception& e)
    {
        gtWarning() << QStringLiteral("Recording observer failed in %1: %2")
                           .arg(callbackName, e.what());
    }
    catch (...)
    {
        gtWarning() << QStringLiteral("Recording observer failed in %1")
                           .arg(callbackName);
    }
    return false;
}

} // namespace

struct GtRecordingSession::Impl
{
    Impl(const GtObject& component,
         GtRecordingObserver& observer,
         QList<const GtObject*> roots,
         GtRecordingSession::Events eventMask) :
        id(QUuid::createUuid()),
        component(component),
        observer(observer),
        roots(std::move(roots)),
        eventMask(eventMask)
    {}

    QUuid id;
    QUuid parentId;

    const GtObject& component;
    GtRecordingObserver& observer;
    QList<const GtObject*> roots;
    GtRecordingSession::Events eventMask;

    /// Session that was active when run() was called
    GtRecordingSession* parent{nullptr};

    /// Observed roots and their descendants
    mutable QReadWriteLock membershipLock;
    QSet<const GtObject*> observed;

    /// Set once an event callback failed; no further events are forwarded
    std::atomic<bool> faulted{false};

    bool used{false};
    bool startFailed{false};

    void buildMembership()
    {
        QWriteLocker locker(&membershipLock);
        for (const GtObject* root : qAsConst(roots))
        {
            if (!root) continue;

            observed.insert(root);
            for (const GtObject* child :
                 root->QObject::findChildren<GtObject*>())
            {
                observed.insert(child);
            }
        }
    }

    template <typename Callback>
    void dispatch(GtRecordingSession::Event event, Callback&& callback)
    {
        if (!eventMask.testFlag(event) || faulted.load()) return;

        if (!callObserver("event callback", callback))
        {
            if (!faulted.exchange(true))
            {
                gtWarning() << QStringLiteral(
                    "Recording %1: no further events are forwarded")
                                   .arg(id.toString());
            }
        }
    }
};

GtRecordingSession::GtRecordingSession(const GtObject& component,
                                       GtRecordingObserver& observer,
                                       const QList<const GtObject*>& roots,
                                       Events eventMask) :
    pimpl(std::make_unique<Impl>(component, observer, roots, eventMask))
{}

GtRecordingSession::GtRecordingSession(const GtObject& component,
                                       GtRecordingObserver& observer,
                                       const QList<QPointer<GtObject>>& roots,
                                       Events eventMask) :
    GtRecordingSession(component, observer,
                       QList<const GtObject*>{roots.begin(), roots.end()},
                       eventMask)
{}

GtRecordingSession::~GtRecordingSession() = default;

QUuid
GtRecordingSession::id() const
{
    return pimpl->id;
}

QUuid
GtRecordingSession::parentId() const
{
    return pimpl->parentId;
}

const GtObject&
GtRecordingSession::component() const
{
    return pimpl->component;
}

GtRecordingSession::Events
GtRecordingSession::eventMask() const
{
    return pimpl->eventMask;
}

bool
GtRecordingSession::isObserved(const GtObject& object) const
{
    QReadLocker locker(&pimpl->membershipLock);
    return pimpl->observed.contains(&object);
}

void
GtRecordingSession::notifyPropertyRead(const GtObject& owner,
                                       const GtAbstractProperty& property)
{
    if (!isObserved(owner)) return;

    pimpl->dispatch(PropertyRead, [&]() {
        pimpl->observer.propertyRead(owner, property);
    });
}

void
GtRecordingSession::notifyPropertyWritten(const GtObject& owner,
                                          const GtAbstractProperty& property)
{
    if (!isObserved(owner)) return;

    pimpl->dispatch(PropertyWritten, [&]() {
        pimpl->observer.propertyWritten(owner, property);
    });
}

void
GtRecordingSession::notifyObjectAttached(const GtObject& parent,
                                         const GtObject& child)
{
    if (!isObserved(parent)) return;

    pimpl->dispatch(ObjectAttached, [&]() {
        pimpl->observer.objectAttached(parent, child);
    });
}

void
GtRecordingSession::notifyObjectDetached(const GtObject& parent,
                                         const GtObject& child)
{
    if (!isObserved(parent)) return;

    pimpl->dispatch(ObjectDetached, [&]() {
        pimpl->observer.objectDetached(parent, child);
    });
}

GtRecordingSession*
GtRecordingSession::current() noexcept
{
    return currentSession;
}

bool
GtRecordingSession::start()
{
    if (pimpl->used)
    {
        gtWarning() << QStringLiteral("Recording %1 was already run; running "
                                      "the work without recording")
                           .arg(pimpl->id.toString());
        return false;
    }
    pimpl->used = true;

    pimpl->parent = currentSession;
    pimpl->parentId = pimpl->parent ? pimpl->parent->id() : QUuid();

    pimpl->buildMembership();

    if (!callObserver("recordingStarted", [this]() {
            pimpl->observer.recordingStarted(*this);
        }))
    {
        // keep the session active so that the work's events do not leak
        // into the parent session, but forward nothing
        pimpl->startFailed = true;
        pimpl->faulted = true;
    }

    currentSession = this;
    return true;
}

void
GtRecordingSession::finish(bool success) noexcept
{
    currentSession = pimpl->parent;

    if (pimpl->startFailed) return;

    callObserver("recordingFinished", [this, success]() {
        pimpl->observer.recordingFinished(*this, success);
    });
}
