/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTEXECUTIONCONTEXT_H
#define GTEXECUTIONCONTEXT_H

#include "gt_core_exports.h"

#include <QString>

class GtProject;

/**
 * @brief Describes the project and paths associated with one execution.
 *
 * The project pointer is borrowed and is never owned by this object. The
 * pointed-to project must outlive every context and scope that refers to it.
 * A context is usable when it contains a project or a project path. Contexts
 * are intended for synchronous execution boundaries: the project is borrowed,
 * the scope is thread-local, and neither is propagated to child threads.
 */
class GT_CORE_EXPORT GtExecutionContext
{
public:
    /**
     * @brief Creates an execution context.
     * @param project Project associated with the execution, not owned.
     * @param projectPath Project path. If empty, it is taken from @p project.
     */
    explicit GtExecutionContext(
        GtProject* project = nullptr,
        QString projectPath = {});

    /// Returns the borrowed project associated with this execution.
    GtProject* project() const noexcept;

    /// Returns the project path associated with this execution.
    QString const& projectPath() const noexcept;

    /// Returns whether a project or project path is available.
    bool isValid() const noexcept;

    /**
     * @brief Returns the context installed on the current thread.
     * @return Borrowed current context, or nullptr if no scope is active.
     */
    static GtExecutionContext const* current() noexcept;

private:
    GtProject* m_project;
    QString m_projectPath;
};

/**
 * @brief Installs an execution context for the lifetime of a scope.
 *
 * Scopes are thread-local, nestable, non-copyable, and restore the previous
 * context in their destructor. The referenced context must outlive the scope.
 * Neither the scope nor the context extends the lifetime of the borrowed
 * project. Do not retain the context or project pointer for asynchronous work.
 */
class GT_CORE_EXPORT GtExecutionContextScope
{
public:
    explicit GtExecutionContextScope(GtExecutionContext const& context);
    GtExecutionContextScope(GtExecutionContext&&) = delete;
    ~GtExecutionContextScope();

    GtExecutionContextScope(GtExecutionContextScope const&) = delete;
    GtExecutionContextScope& operator=(GtExecutionContextScope const&) = delete;
    GtExecutionContextScope& operator=(GtExecutionContextScope&&) = delete;

private:
    GtExecutionContext const* m_previous;
};

#endif // GTEXECUTIONCONTEXT_H
