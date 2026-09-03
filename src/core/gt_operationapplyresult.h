/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTOPERATIONAPPLYRESULT_H
#define GTOPERATIONAPPLYRESULT_H

#include "gt_core_exports.h"

#include <QString>

/**
 * @brief Result of applying a detached operation result on the originating side.
 */
class GT_CORE_EXPORT GtOperationApplyResult
{
public:
    static GtOperationApplyResult success();
    static GtOperationApplyResult failure(QString errorMessage);

    bool isSuccess() const noexcept;
    QString const& errorMessage() const noexcept;

private:
    explicit GtOperationApplyResult(bool success, QString errorMessage = {});

    bool m_success;
    QString m_errorMessage;
};

#endif // GTOPERATIONAPPLYRESULT_H
