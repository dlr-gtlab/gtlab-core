/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_executioncontext.h"

#include <utility>

#include "gt_project.h"

namespace {

thread_local GtExecutionContext const* currentExecutionContext = nullptr;

} // namespace

GtExecutionContext::GtExecutionContext(GtProject const* project,
                                       QString executionDataRoot,
                                       QString source,
                                       QString projectPath,
                                       QString jobId) :
    m_project(project),
    m_executionDataRoot(std::move(executionDataRoot)),
    m_source(std::move(source)),
    m_projectPath(std::move(projectPath)),
    m_jobId(std::move(jobId))
{
    if (m_projectPath.isEmpty() && m_project)
    {
        m_projectPath = m_project->path();
    }
}

GtProject const*
GtExecutionContext::project() const noexcept
{
    return m_project;
}

QString const&
GtExecutionContext::executionDataRoot() const noexcept
{
    return m_executionDataRoot;
}

QString const&
GtExecutionContext::source() const noexcept
{
    return m_source;
}

QString const&
GtExecutionContext::projectPath() const noexcept
{
    return m_projectPath;
}

QString const&
GtExecutionContext::jobId() const noexcept
{
    return m_jobId;
}

bool
GtExecutionContext::isValid() const noexcept
{
    return m_project != nullptr;
}

GtExecutionContext const*
GtExecutionContext::current() noexcept
{
    return currentExecutionContext;
}

GtExecutionContext const*
GtExecutionContext::currentContext() noexcept
{
    return current();
}

GtExecutionContextScope::GtExecutionContextScope(
    GtExecutionContext const& context) :
    m_previous(currentExecutionContext)
{
    currentExecutionContext = &context;
}

GtExecutionContextScope::~GtExecutionContextScope()
{
    currentExecutionContext = m_previous;
}
