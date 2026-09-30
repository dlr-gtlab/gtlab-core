/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTEXECUTIONRESULT_H
#define GTEXECUTIONRESULT_H

#include "gt_core_exports.h"
#include "gt_operationexecutionresult.h"

#include <variant>

#include <QString>

class GtExecutionEnvironment;

/**
 * @brief Represents either an operation outcome or an error raised by the
 *        execution environment.
 *
 * An operation outcome is present exactly when error() returns Error::None.
 * Environment failures have no operation outcome. Use operationResult() to
 * inspect the outcome without confusing a domain failure with a boundary
 * failure.
 */
class GT_CORE_EXPORT GtExecutionResult
{
public:
    /**
     * @brief Describes a failure in the execution environment boundary.
     */
    enum class Error
    {
        /** No environment error; an operation outcome is present. */
        None,
        /** The operation requires a project, but none was supplied. */
        ProjectRequired,
        /**
         * An object required by the invocation has the wrong thread affinity.
         */
        WrongThread,
        /** An exception escaped the execution boundary. */
        UnhandledException
    };

    GtExecutionResult() = delete;

    /**
     * @brief Returns the boundary error, or Error::None for an operation
     *        outcome.
     */
    Error error() const noexcept;

    /**
     * @brief Returns the boundary failure message, or an empty string for an
     *        operation outcome.
     */
    QString const& message() const noexcept;

    /**
     * @brief Returns the operation outcome when the boundary succeeded.
     * @return Borrowed operation outcome, or nullptr for a boundary failure.
     */
    GtOperationExecutionResult const* operationResult() const noexcept;

private:
    struct Failure
    {
        Error error{Error::UnhandledException};
        QString message;
    };

    explicit GtExecutionResult(GtOperationExecutionResult result);
    explicit GtExecutionResult(Failure failure);

    static GtExecutionResult fromError(Error error, QString message);

    std::variant<GtOperationExecutionResult, Failure> m_value;

    friend class GtExecutionEnvironment;
};

#endif // GTEXECUTIONRESULT_H
