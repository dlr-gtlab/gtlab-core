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
     * Guards are process-local, thread-safe, and held for the lifetime of the
     * guard object. Acquiring a new project first releases a previously held
     * key.
     */
    enum class Result
    {
        Acquired,
        Busy,
        InvalidProject
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

    /// Releases the currently held project key, if any.
    void release();

    bool isHeld() const;
    QString key() const;

    /**
     * @brief Returns the project identity used by the process-local guard.
     *
     * Existing project paths are canonicalized. Pathless projects use their
     * stable object UUID and finally their in-process object address.
     */
    static QString projectKey(GtProject const* project);
    static bool isBusy(GtProject const* project);

private:
    QString m_key;
};

#endif // GTPROJECTEXECUTIONGUARD_H
