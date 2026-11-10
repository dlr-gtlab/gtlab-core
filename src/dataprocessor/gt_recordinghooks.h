/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTRECORDINGHOOKS_H
#define GTRECORDINGHOOKS_H

#include "gt_datamodel_exports.h"

#include <atomic>

class GtAbstractProperty;
class GtObject;
class GtRecordingSession;

/**
 * Hooks that report events of the data model to the active
 * GtRecordingSession. The gt::detail hooks are not intended for direct use;
 * GtObject subclasses with own value accessors use gt::ObjectValueReadScope
 * and gt::recordObjectValueWritten().
 */
namespace gt
{
namespace detail
{

/// Number of recording sessions currently running in the process
GT_DATAMODEL_EXPORT extern std::atomic<int> runningSessions;

/// Cheap check used before any thread-local lookup
inline bool
isAnyRecordingRunning() noexcept
{
    return runningSessions.load(std::memory_order_relaxed) > 0;
}

GT_DATAMODEL_EXPORT void enterPropertyRead(
    const GtAbstractProperty& property) noexcept;

/// Ends a property or object value read
GT_DATAMODEL_EXPORT void leaveRead() noexcept;

/// Reports a value read of the object; shares the nesting depth with
/// property reads. Leave with leaveRead().
GT_DATAMODEL_EXPORT void enterObjectValueRead(const GtObject& object) noexcept;

/// Reports a change reported by the property to the active session
GT_DATAMODEL_EXPORT void recordPropertyWritten(
    const GtAbstractProperty& property) noexcept;

/// Called by GtObject after a child was attached
GT_DATAMODEL_EXPORT void recordObjectAttached(const GtObject& parent,
                                              const GtObject& child) noexcept;

/// Called by GtObject before a child is detached
GT_DATAMODEL_EXPORT void recordObjectDetached(const GtObject& parent,
                                              const GtObject& child) noexcept;

/// Called by ~GtObject; parent is nullptr if the object has no parent
GT_DATAMODEL_EXPORT void recordObjectDestroyed(const GtObject* parent,
                                               const GtObject& object) noexcept;

/**
 * @brief Marks a property read for the lifetime of the scope.
 *
 * Only the outermost read on a thread is reported, so accessors that call
 * each other produce one event.
 */
class PropertyReadScope
{
public:
    explicit PropertyReadScope(const GtAbstractProperty& property) noexcept :
        m_entered(isAnyRecordingRunning())
    {
        if (m_entered) enterPropertyRead(property);
    }

    ~PropertyReadScope()
    {
        if (m_entered) leaveRead();
    }

    PropertyReadScope(const PropertyReadScope&) = delete;
    PropertyReadScope& operator=(const PropertyReadScope&) = delete;

private:
    bool m_entered;
};

/**
 * @brief Suspends recording on the current thread for the lifetime of the
 * scope. Used for internal bookkeeping reads that are not data accesses.
 */
class GT_DATAMODEL_EXPORT SuppressRecordingScope
{
public:
    SuppressRecordingScope() noexcept;
    ~SuppressRecordingScope();

    SuppressRecordingScope(const SuppressRecordingScope&) = delete;
    SuppressRecordingScope& operator=(const SuppressRecordingScope&) = delete;

private:
    GtRecordingSession* m_previous{nullptr};
    bool m_active{false};
};

} // namespace detail

/**
 * @brief Marks a value read of an object for the lifetime of the scope.
 *
 * For GtObject subclasses whose accessors return data that is not held in
 * registered properties. Reported to the observer as objectValueRead() if
 * the object is observed:
 *
 * @code
 * double MyObject::value() const
 * {
 *     gt::ObjectValueReadScope readScope(*this);
 *     return m_value;
 * }
 * @endcode
 *
 * Shares the nesting with property reads: only the outermost read on a
 * thread is reported, so property reads inside the accessor produce no
 * additional event.
 */
class ObjectValueReadScope
{
public:
    explicit ObjectValueReadScope(const GtObject& object) noexcept :
        m_entered(detail::isAnyRecordingRunning())
    {
        if (m_entered) detail::enterObjectValueRead(object);
    }

    ~ObjectValueReadScope()
    {
        if (m_entered) detail::leaveRead();
    }

    ObjectValueReadScope(const ObjectValueReadScope&) = delete;
    ObjectValueReadScope& operator=(const ObjectValueReadScope&) = delete;

private:
    bool m_entered;
};

/**
 * @brief Reports a changed value of an object.
 *
 * Write counterpart of gt::ObjectValueReadScope for GtObject subclasses whose
 * setters change data that is not held in registered properties. Call it
 * after the change; it is reported to the observer as objectValueWritten()
 * if the object is observed:
 *
 * @code
 * void MyObject::setValue(double value)
 * {
 *     m_value = value;
 *     gt::recordObjectValueWritten(*this);
 * }
 * @endcode
 *
 * Writes are reported even inside a read, e.g. when a getter fills a cache.
 */
GT_DATAMODEL_EXPORT void recordObjectValueWritten(
    const GtObject& object) noexcept;

} // namespace gt

#endif // GTRECORDINGHOOKS_H
