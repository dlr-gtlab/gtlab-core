/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 26.1.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTPROCESSEXECUTORMANAGER_H
#define GTPROCESSEXECUTORMANAGER_H

#include "gt_core_exports.h"

#include <QObject>

#include <memory>

class GtCoreProcessExecutor;

/**
 * @brief Helper class for adding and selecting a Process Executor
 */
class GT_CORE_EXPORT GtProcessExecutorManager : public QObject
{
    Q_OBJECT

    /// private ctor
    GtProcessExecutorManager();

public:

    /**
     * @brief instance
     */
    static GtProcessExecutorManager& instance();

    /**
     * @brief Getter for the current selected process executor
     * @return executor
     */
    GtCoreProcessExecutor* currentExecutor();
    GtCoreProcessExecutor const* currentExecutor() const;

    GtCoreProcessExecutor* getExecutor(std::string const& id);
    GtCoreProcessExecutor const* getExecutor(std::string const& id) const;

    /**
     * @brief operator bool
     */
    explicit operator bool() const { return currentExecutor(); }

    /**
     * @brief Sets the current executor using its id. The Executor must be
     * present previously. If the current executor is not ready (e.g a task
     * is running) the function returns false and the executor is not switched
     * @param id Id of the executor to set as active
     * @return success
     */
    bool setCurrentExecutor(std::string const& id);

    /**
     * @brief Registers the executor specified. If no executor is
     * currently active, the selected executor will be updated as well.
     * An Executor can only be set once
     * @param id Id of the executor. Must be unique.
     * @param exec Executor to add
     * @return success
     */
    bool registerExecutor(std::string id,
                          std::unique_ptr<GtCoreProcessExecutor>&& exec);

    /**
     * @brief Removes the executor by its id. Executor must not be active.
     * @param id Id of the executor to remove
     * @return success
     */
    bool removeExecutor(std::string const& id);

    /**
     * @brief Returns whether the executor is already present
     * @param id Id of the executor
     * @return Is executor registered
     */
    bool hasExecutor(std::string const& id);

    /**
     * @brief Clears all registered executors
     */
    void clearAllExecutors();

signals:

    /**
     * @brief Emitted once the process executor has changed.
     * @param New executor
     */
    void executorChanged(GtCoreProcessExecutor*);
};

namespace gt
{

/**
 * @brief Returns gtlab's execution manager
 *
 * Beware: this is a global object and thus has shared state
 */
inline GtProcessExecutorManager& processExecutorManager()
{
    return GtProcessExecutorManager::instance();
}

/**
 * Registers an executor type at the executor manager
 */
template <typename Executor>
inline bool registerExecutorType()
{
    return processExecutorManager()
           .registerExecutor(Executor::S_ID, std::make_unique<Executor>());
}
/**
 * Finds an executor by type in the executor manager
 */
template <typename Executor>
inline Executor* findExecutorByType()
{
    return qobject_cast<Executor*>(
                processExecutorManager().getExecutor(Executor::S_ID));
}

/**
 * @brief Returns the current process executor as reference.
 * Current process executor must not be null!
 * @return Current Process Executor
 */
inline GtCoreProcessExecutor& currentProcessExecutor()
{
    auto p = processExecutorManager().currentExecutor();
    assert(p);
    return *p;
}

namespace deprecated
{

[[deprecated("Use gt::currentProcessExecutor() instead")]]
inline GtCoreProcessExecutor* processExecutor()
{
    return &currentProcessExecutor();
}

} // namespace deprecated

} // namespace gt

#endif // GTPROCESSEXECUTORMANAGER_H
