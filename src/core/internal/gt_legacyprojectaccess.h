/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTLEGACYPROJECTACCESS_H
#define GTLEGACYPROJECTACCESS_H

#include <QMutex>
#include <QMutexLocker>
#include <QSet>
#include <QString>

class GtProcessComponent;

namespace gt::detail
{

    /** Thread-safe, non-exported deduplication state for legacy access warnings. */
    class LegacyProjectAccessRegistry
    {
    public:
        bool markIfNew(QString const& key)
        {
            QMutexLocker locker(&m_mutex);
            if (m_reported.contains(key))
            {
                return false;
            }

            m_reported.insert(key);
            return true;
        }

    private:
        QMutex m_mutex;
        QSet<QString> m_reported;
    };

} // namespace gt::detail

/**
 * @brief Marks the process component that is executed on the current thread.
 *
 * The scope is an internal developer diagnostic. It stores a borrowed pointer,
 * never extends the lifetime of the component, and is not propagated to child
 * threads. Scopes may be nested; the destructor restores the previously
 * executing component.
 *
 * The scope is deliberately not part of the execution context contract. It
 * neither changes project resolution nor execution order and must not be used
 * to derive execution data in calculators.
 */
class GtProcessComponentExecutionScope
{
public:
    /**
     * @brief Constructor.
     * @param component Component being executed, not owned. The component must
     * outlive the scope.
     */
    explicit GtProcessComponentExecutionScope(GtProcessComponent* component);

    ~GtProcessComponentExecutionScope();

    GtProcessComponentExecutionScope(GtProcessComponentExecutionScope const&) =
        delete;
    GtProcessComponentExecutionScope(GtProcessComponentExecutionScope&&) =
        delete;
    GtProcessComponentExecutionScope& operator=(
        GtProcessComponentExecutionScope const&) = delete;
    GtProcessComponentExecutionScope& operator=(
        GtProcessComponentExecutionScope&&) = delete;

    /**
     * @brief Returns the component executed on the current thread.
     * @return Borrowed component pointer, or nullptr if no component is being
     * executed.
     */
    static GtProcessComponent* current() noexcept;

private:
    GtProcessComponent* m_previous;
};

/**
 * @brief Developer diagnostics for legacy project access during execution.
 *
 * During a process component execution, @c gtApp->currentProject() resolves
 * the execution project through the active @c GtExecutionContext. That
 * compatibility path keeps existing calculators source compatible, but it is
 * not the recommended API for new project scoped code.
 *
 * This class reports such access once per process-component class while
 * GTlab Developer Mode is active. It is a migration aid only: it never
 * changes the resolved project, never fails an execution, and does not imply
 * that @c currentProject() is deprecated for GUI code that intentionally
 * targets the project selected in the desktop application.
 *
 * The environment variable @c GTLAB_LEGACY_PROJECT_ACCESS_WARNING overrides
 * the Developer Mode condition, e.g. for headless runs where the GUI flag is
 * never enabled. Its value is read once on the first legacy access check.
 * Values @c 1, @c true, @c on and @c yes enable the diagnostic independently
 * of Developer Mode, values @c 0, @c false, @c off and @c no disable it, and
 * any other value (including an empty one) keeps the default. The override
 * affects the diagnostic only, never the project resolution.
 */
class GtLegacyProjectAccess
{
public:
    GtLegacyProjectAccess() = delete;
    ~GtLegacyProjectAccess() = delete;

    /**
     * @brief Reports project access of the currently executed process component.
     *
     * Called by the canonical project resolution while an execution context is
     * active. Does nothing if no process component is executed on the current
     * thread, if warnings are disabled, or if the component class has already
     * been reported. Warnings are enabled by GTlab Developer Mode unless the
     * @c GTLAB_LEGACY_PROJECT_ACCESS_WARNING overrides it. Registry
     * lookups are thread safe.
     */
    static void reportLegacyAccess();

private:
    /// Returns whether legacy access warnings are currently enabled, i.e.
    /// whether the environment override or Developer Mode allow it
    static bool warningsEnabled();

    /// Returns the deduplication key of a component
    static QString warningKey(GtProcessComponent const& component);

    /// Returns the actionable warning text of a component
    static QString warningMessage(GtProcessComponent const& component);
};

#endif // GTLEGACYPROJECTACCESS_H
