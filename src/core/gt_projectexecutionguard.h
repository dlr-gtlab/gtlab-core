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

    Result tryAcquire(GtProject* project);
    void release();

    bool isHeld() const;
    QString key() const;

    static QString projectKey(GtProject const* project);
    static bool isBusy(GtProject const* project);

private:
    QString m_key;
};

#endif // GTPROJECTEXECUTIONGUARD_H
