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
 */
class GT_CORE_EXPORT GtExecutionContext
{
public:
    /**
     * @brief Creates an execution context.
     * @param project Project associated with the execution, not owned.
     * @param executionDataRoot Root directory for execution data.
     * @param source Source identifier or path for the execution.
     * @param projectPath Project path. If empty, it is taken from @p project.
     * @param jobId Optional job or execution identifier.
     */
    explicit GtExecutionContext(
        GtProject* project = nullptr,
        QString executionDataRoot = {},
        QString source = {},
        QString projectPath = {},
        QString jobId = {});

    /// Returns the borrowed project associated with this execution.
    GtProject* project() const noexcept;

    /// Returns the root directory for execution data.
    QString const& executionDataRoot() const noexcept;

    /// Returns the execution source identifier or path.
    QString const& source() const noexcept;

    /// Returns the project path associated with this execution.
    QString const& projectPath() const noexcept;

    /// Returns the optional job or execution identifier.
    QString const& jobId() const noexcept;

    /// Returns whether a project is associated with this context.
    bool isValid() const noexcept;

    /**
     * @brief Returns the context installed on the current thread.
     * @return Borrowed current context, or nullptr if no scope is active.
     */
    static GtExecutionContext const* current() noexcept;

    /// Alias for current(), for callers that prefer the explicit name.
    static GtExecutionContext const* currentContext() noexcept;

private:
    GtProject* m_project;
    QString m_executionDataRoot;
    QString m_source;
    QString m_projectPath;
    QString m_jobId;
};

/**
 * @brief Installs an execution context for the lifetime of a scope.
 *
 * Scopes are thread-local, nestable, non-copyable, and restore the previous
 * context in their destructor. The referenced context must outlive the scope.
 */
class GT_CORE_EXPORT GtExecutionContextScope
{
public:
    explicit GtExecutionContextScope(GtExecutionContext const& context);
    GtExecutionContextScope(GtExecutionContext&&) = delete;
    ~GtExecutionContextScope();

    GtExecutionContextScope(GtExecutionContextScope const&) = delete;
    GtExecutionContextScope& operator=(GtExecutionContextScope const&) = delete;

private:
    GtExecutionContext const* m_previous;
};

#endif // GTEXECUTIONCONTEXT_H
