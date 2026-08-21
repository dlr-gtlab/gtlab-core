/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTPROJECTEXECUTIONGUARD_H
#define GTPROJECTEXECUTIONGUARD_H

#include "gt_core_exports.h"

#include <QString>

class GtProject;

class GT_CORE_EXPORT GtProjectExecutionGuard
{
public:
    /**
     * @brief Result of trying to acquire a project execution guard.
     *
     * The process-local guard registry is thread-safe. A guard instance must
     * be owned and used by one thread and is held for its lifetime. Acquiring
     * a new project first releases a previously held key.
     */
    enum class Result
    {
        InvalidProject = 0,
        Acquired,
        Busy,
    };

    GtProjectExecutionGuard() = default;
    ~GtProjectExecutionGuard();

    GtProjectExecutionGuard(GtProjectExecutionGuard const&) = delete;
    GtProjectExecutionGuard& operator=(GtProjectExecutionGuard const&) = delete;

    /**
     * @brief Tries to acquire exclusive access for @p project.
     * @return Acquired on success, Busy if another guard holds the project,
     *         or InvalidProject if no stable project identity is available.
     */
    Result tryAcquire(GtProject* project);

    /**
     * @brief Releases the currently held project key, if any.
     */
    void release();

    /**
     * Returns, whether this guard aquired a project or not
     */
    bool isLocked() const;

    /**
     * @brief The indentity of the project this guard protects.
     *
     * The key is used to identify a certain project and discriminate
     * it from others.
     *
     * In the current implementation, it is just the project path,
     * but it could also be its uuid.
     *
     * @return Value of the key
     */
    QString projectKey() const;

    /**
     * @brief Returns the project identity used by the process-local guard.
     *
     * Existing project paths are canonicalized. Pathless projects use their
     * stable object UUID and finally their in-process object address.
     */
    static QString projectKey(GtProject const* project);
    static bool isLocked(GtProject const* project);

private:
    QString m_key;
};

#endif // GTPROJECTEXECUTIONGUARD_H
