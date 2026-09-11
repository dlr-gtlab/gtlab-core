/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTOPERATIONAPPLYSTATUS_H
#define GTOPERATIONAPPLYSTATUS_H

#include "gt_core_exports.h"

#include <QString>

/**
 * @brief Status of applying a detached execution result on the originating side.
 */
class GT_CORE_EXPORT GtOperationApplyStatus
{
public:
    /**
     * @brief Creates a successful apply status.
     * @return A status indicating that the result was applied.
     */
    static GtOperationApplyStatus success();

    /**
     * @brief Creates a failed apply status.
     * @param errorMessage Description of the failure.
     * @return A status indicating that the result was not applied.
     */
    static GtOperationApplyStatus failure(QString errorMessage);

    /**
     * @brief Returns whether the result was applied successfully.
     * @return True if the result was applied.
     */
    bool succeeded() const noexcept;

    /**
     * @brief Returns the failure description.
     * @return An empty string if the apply step succeeded.
     */
    QString const& errorMessage() const noexcept;

private:
    explicit GtOperationApplyStatus(bool succeeded, QString errorMessage = {});

    bool m_succeeded;
    QString m_errorMessage;
};

#endif // GTOPERATIONAPPLYSTATUS_H
