/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTOPERATIONEXECUTIONRESULT_H
#define GTOPERATIONEXECUTIONRESULT_H

#include "gt_core_exports.h"
#include "gt_object.h"

#include <memory>

#include <QString>

/**
 * @brief Describes the outcome returned by one executable operation.
 *
 * This value contains operation-level status and an optional result payload.
 * It does not describe failures in the execution environment. The result
 * payload is owned by this value and may be present for every status.
 */
struct GT_CORE_EXPORT GtOperationExecutionResult
{
    /**
     * @brief Describes how the operation ended.
     */
    enum class Status
    {
        /** The operation completed successfully. */
        Success,
        /** The operation reported a domain failure. */
        Failed,
        /** The operation reported cooperative cancellation. */
        Cancelled
    };

    /** @brief How the operation ended. */
    Status status{Status::Success};
    /** @brief Optional operation-defined machine-readable code. */
    QString code;
    /** @brief Optional human-readable message. */
    QString message;
    /** @brief Optional owned result payload, which may accompany every status. */
    std::unique_ptr<GtObject> result;
};

#endif // GTOPERATIONEXECUTIONRESULT_H
