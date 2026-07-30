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

GtExecutionContext::GtExecutionContext(GtProject* project,
                                       QString projectPath) :
    m_project(project),
    m_projectPath(std::move(projectPath))
{
    if (m_projectPath.isEmpty() && m_project)
    {
        m_projectPath = m_project->path();
    }
}

GtProject*
GtExecutionContext::project() const noexcept
{
    return m_project;
}

QString const&
GtExecutionContext::projectPath() const noexcept
{
    return m_projectPath;
}

bool
GtExecutionContext::isValid() const noexcept
{
    return m_project != nullptr || !m_projectPath.isEmpty();
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
