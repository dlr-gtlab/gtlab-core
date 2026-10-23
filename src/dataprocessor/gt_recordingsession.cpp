/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_recordingsession.h"

#include "gt_abstractproperty.h"
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

/// Nesting depth of property reads on the current thread
thread_local int propertyReadDepth = 0;

/// Clears the active session of the current thread for its lifetime, so that
/// observer callbacks never create observations.
class SuppressionScope
{
public:
    SuppressionScope() :
        m_previous(currentSession)
    {
        currentSession = nullptr;
    }

    ~SuppressionScope()
    {
        currentSession = m_previous;
    }

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
    Impl(const GtObject& component, GtRecordingObserver& observer,
         QList<const GtObject*> roots, GtRecordingSession::Events eventMask) :
        id(QUuid::createUuid()),
        component(component),
        observer(observer),
        roots(std::move(roots)),
        eventMask(eventMask)
    { }

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
    QSet<const GtObject*> rootSet;
    QSet<const GtObject*> observed;

    /// Set once an event callback failed; no further events are forwarded
    std::atomic<bool> faulted{false};

    bool used{false};
    bool startFailed{false};

    /// Whether run() is currently executing the work
    std::atomic<bool> running{false};

    void buildMembership()
    {
        QWriteLocker locker(&membershipLock);
        for (const GtObject* root : qAsConst(roots))
        {
            if (!root) continue;

            rootSet.insert(root);
            observed.insert(root);
            for (const GtObject* child :
                 root->QObject::findChildren<GtObject*>())
            { // cppcheck-suppress useStlAlgorithm
                observed.insert(child);
            }
        }
    }

    /// Sets the membership of a subtree; roots always stay observed.
    /// Requires the write lock.
    void setSubtreeObserved(const GtObject& object, bool parentObserved)
    {
        const bool subtreeObserved =
            parentObserved || rootSet.contains(&object);

        if (subtreeObserved)
            observed.insert(&object);
        else
            observed.remove(&object);

        for (const QObject* child : object.children())
        {
            if (auto* c = qobject_cast<const GtObject*>(child))
            {
                setSubtreeObserved(*c, subtreeObserved);
            }
        }
    }

    void updateOnAttach(const GtObject& parentObject, const GtObject& child)
    {
        QWriteLocker locker(&membershipLock);
        setSubtreeObserved(child, observed.contains(&parentObject));
    }

    void updateOnDetach(const GtObject& child)
    {
        QWriteLocker locker(&membershipLock);
        setSubtreeObserved(child, false);
    }

    void forget(const GtObject& object)
    {
        QWriteLocker locker(&membershipLock);
        observed.remove(&object);
        rootSet.remove(&object);
    }

    template <typename Callback>
    void dispatch(GtRecordingSession::Event event, Callback&& callback)
    {
        if (!eventMask.testFlag(event) || faulted.load()) return;

        if (!callObserver("event callback", callback))
        {
            if (!faulted.exchange(true))
            {
                gtWarning()
                    << QStringLiteral(
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
{ }

GtRecordingSession::GtRecordingSession(const GtObject& component,
                                       GtRecordingObserver& observer,
                                       const QList<QPointer<GtObject>>& roots,
                                       Events eventMask) :
    GtRecordingSession(component, observer,
                       QList<const GtObject*>{roots.begin(), roots.end()},
                       eventMask)
{ }

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

    pimpl->dispatch(PropertyRead,
                    [&]() { pimpl->observer.propertyRead(owner, property); });
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
GtRecordingSession::notifyObjectValueRead(const GtObject& object)
{
    if (!isObserved(object)) return;

    pimpl->dispatch(ObjectValueRead,
                    [&]() { pimpl->observer.objectValueRead(object); });
}

void
GtRecordingSession::notifyObjectAttached(const GtObject& parent,
                                         const GtObject& child)
{
    // suspended parent sessions keep their observed tree up to date
    for (GtRecordingSession* s = this; s; s = s->pimpl->parent)
    {
        s->pimpl->updateOnAttach(parent, child);
    }

    if (!isObserved(parent)) return;

    pimpl->dispatch(ObjectAttached,
                    [&]() { pimpl->observer.objectAttached(parent, child); });
}

void
GtRecordingSession::notifyObjectDetached(const GtObject& parent,
                                         const GtObject& child)
{
    if (isObserved(parent))
    {
        pimpl->dispatch(ObjectDetached, [&]() {
            pimpl->observer.objectDetached(parent, child);
        });
    }

    for (GtRecordingSession* s = this; s; s = s->pimpl->parent)
    {
        s->pimpl->updateOnDetach(child);
    }
}

void
GtRecordingSession::forget(const GtObject& object)
{
    for (GtRecordingSession* s = this; s; s = s->pimpl->parent)
    {
        s->pimpl->forget(object);
    }
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
    ++gt::detail::runningRecordings;

    pimpl->parent = currentSession;
    pimpl->parentId = pimpl->parent ? pimpl->parent->id() : QUuid();

    pimpl->buildMembership();

    if (!callObserver("recordingStarted",
                      [this]() { pimpl->observer.recordingStarted(*this); }))
    {
        // keep the session active so that the work's events do not leak
        // into the parent session, but forward nothing
        pimpl->startFailed = true;
        pimpl->faulted = true;
    }

    pimpl->running = true;
    currentSession = this;
    return true;
}

void
GtRecordingSession::finish(bool success) noexcept
{
    pimpl->running = false;
    currentSession = pimpl->parent;
    --gt::detail::runningRecordings;

    if (pimpl->startFailed) return;

    callObserver("recordingFinished", [this, success]() {
        pimpl->observer.recordingFinished(*this, success);
    });
}

std::atomic<int> gt::detail::runningRecordings{0};

void
gt::detail::enterPropertyRead(const GtAbstractProperty& property) noexcept
{
    // only the outermost read is reported
    if (propertyReadDepth++ > 0) return;

    GtRecordingSession* session = currentSession;
    if (!session) return;

    if (const GtObject* owner = property.ownerObject())
    {
        session->notifyPropertyRead(*owner, property);
    }
}

void
gt::detail::leavePropertyRead() noexcept
{
    --propertyReadDepth;
}

void
gt::detail::enterObjectValueRead(const GtObject& object) noexcept
{
    // only the outermost read is reported, property reads included
    if (propertyReadDepth++ > 0) return;

    if (GtRecordingSession* session = currentSession)
    {
        session->notifyObjectValueRead(object);
    }
}

void
gt::detail::recordPropertyWritten(const GtAbstractProperty& property) noexcept
{
    if (!isAnyRecordingRunning()) return;

    GtRecordingSession* session = currentSession;
    if (!session) return;

    if (const GtObject* owner = property.ownerObject())
    {
        session->notifyPropertyWritten(*owner, property);
    }
}

gt::detail::SuppressRecordingScope::SuppressRecordingScope() noexcept :
    m_active(isAnyRecordingRunning())
{
    if (!m_active) return;

    m_previous = currentSession;
    currentSession = nullptr;
}

gt::detail::SuppressRecordingScope::~SuppressRecordingScope()
{
    if (m_active) currentSession = m_previous;
}

void
gt::detail::recordObjectAttached(const GtObject& parent,
                                 const GtObject& child) noexcept
{
    if (!isAnyRecordingRunning()) return;

    if (GtRecordingSession* session = currentSession)
    {
        session->notifyObjectAttached(parent, child);
    }
}

void
gt::detail::recordObjectDetached(const GtObject& parent,
                                 const GtObject& child) noexcept
{
    if (!isAnyRecordingRunning()) return;

    if (GtRecordingSession* session = currentSession)
    {
        session->notifyObjectDetached(parent, child);
    }
}

void
gt::detail::recordObjectDestroyed(const GtObject* parent,
                                  const GtObject& object) noexcept
{
    if (!isAnyRecordingRunning()) return;

    GtRecordingSession* session = currentSession;
    if (!session) return;

    // children of a deleted object see no GtObject parent but still a QObject
    // parent: only the top of a deleted subtree is reported
    const bool topLevel = parent || !object.QObject::parent();

    // an observed object is deleted within the chain that observes it: report
    // to the suspended parent sessions too, they hold the object as well
    for (GtRecordingSession* s = session; topLevel && s; s = s->pimpl->parent)
    {
        if (parent && s->isObserved(*parent))
        {
            s->pimpl->dispatch(GtRecordingSession::ObjectDetached, [&]() {
                s->pimpl->observer.objectDetached(*parent, object);
            });
        }
        else if (!parent && s->isObserved(object))
        {
            s->pimpl->dispatch(GtRecordingSession::ObjectDestroyed, [&]() {
                s->pimpl->observer.objectDestroyed(object);
            });
        }

        s->pimpl->updateOnDetach(object);
    }

    // the address may be reused by a new object
    session->forget(object);
}

gt::detail::GtRecordingScope::GtRecordingScope(
    GtRecordingSession& session) noexcept :
    m_active(session.pimpl->running.load())
{
    if (!m_active) return;

    m_previous = currentSession;
    currentSession = &session;
}

gt::detail::GtRecordingScope::~GtRecordingScope()
{
    if (m_active) currentSession = m_previous;
}
