/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 26.1.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_processexecutormanager.h"
#include "gt_coreprocessexecutor.h"
#include "gt_logging.h"

struct ExecutorEntry
{
    /// pointer to executor
    std::unique_ptr<GtCoreProcessExecutor> ptr;
    /// id of executor
    std::string id;
};

std::vector<ExecutorEntry> s_executors{};
std::string s_currentExecutorId;

namespace
{

// helper method to find the executor
auto
findExecutor(std::string const& id)
{
    return std::find_if(s_executors.begin(), s_executors.end(),
                        [&](ExecutorEntry const& e){
        return e.id == id;
    });
}

bool
isExecutorReady(GtCoreProcessExecutor * e)
{
    return e && !e->taskCurrentlyRunning() && e->queue().isEmpty();
}

}

GtProcessExecutorManager::GtProcessExecutorManager()
{
    setObjectName("ProcessExecutorSwitcher");
}

GtProcessExecutorManager&
GtProcessExecutorManager::instance()
{
    static GtProcessExecutorManager self;
    return self;
}

GtCoreProcessExecutor*
GtProcessExecutorManager::currentExecutor()
{
    return getExecutor(s_currentExecutorId);
}

GtCoreProcessExecutor const*
GtProcessExecutorManager::currentExecutor() const
{
    return getExecutor(s_currentExecutorId);
}

GtCoreProcessExecutor*
GtProcessExecutorManager::getExecutor(const std::string& id)
{
    auto iter = findExecutor(id);
    return iter != s_executors.end() ? iter->ptr.get() : nullptr;
}

const GtCoreProcessExecutor*
GtProcessExecutorManager::getExecutor(const std::string& id) const
{
    return const_cast<GtProcessExecutorManager*>(this)->getExecutor(id);
}

bool
GtProcessExecutorManager::setCurrentExecutor(std::string const& id)
{
    auto iter = findExecutor(id);
    if (iter == s_executors.end())
    {
        gtFatalId(GT_EXEC_ID)
                << tr("Failed to set current Executor! %1 was not found")
                   .arg(id.c_str());
        return false;
    }

    // nothing to do here
    if (s_currentExecutorId == id)
    {
        return true;
    }

    // check if switiching is possible
    auto* oldExec = getExecutor(s_currentExecutorId);
    if (oldExec && !isExecutorReady(oldExec))
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Cannot switch the current Executor while a task is "
                      "still running!");
        return false;
    }

    gtDebugId(GT_EXEC_ID) << tr("Now using Executor '%1'").arg(id.c_str());

    // switch executor
    s_currentExecutorId = id;
    emit executorChanged(getExecutor(s_currentExecutorId));
    return true;
}

bool
GtProcessExecutorManager::registerExecutor(std::string id,
                                           std::unique_ptr<GtCoreProcessExecutor>&& exec)
{
    if (id.empty())
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Failed to register Executor with an invalid ID!");
        return false;
    }
    if (hasExecutor(id))
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Failed to register the Executor! %1 has "
                      "already been registered!").arg(id.c_str());
        return false;
    }

    qDebug().noquote() << tr("#### Registered executor '%1'").arg(id.c_str());

    s_executors.push_back(ExecutorEntry{std::move(exec), std::move(id)});

    return getExecutor(s_currentExecutorId) || setCurrentExecutor(s_executors.back().id);
}

bool
GtProcessExecutorManager::removeExecutor(std::string const& id)
{
    auto iter = findExecutor(id);
    if (iter == s_executors.end())
    {
        return false;
    }

    if (!isExecutorReady(iter->ptr.get()))
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Cannot remove the executor while its not ready!");
        return false;
    }
    if (iter->id == s_currentExecutorId)
    {
        gtErrorId(GT_EXEC_ID)
                << tr("Cannot remove the active executor!");
        return false;
    }

    s_executors.erase(iter);

    return true;
}

bool
GtProcessExecutorManager::hasExecutor(std::string const& id)
{
    return findExecutor(id) != s_executors.end();
}

void GtProcessExecutorManager::clearAllExecutors()
{
    s_executors.clear();
    s_currentExecutorId.clear();
}
