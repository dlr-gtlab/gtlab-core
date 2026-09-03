/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_operationapplyresult.h"

#include <utility>

GtOperationApplyResult::GtOperationApplyResult(bool success,
                                               QString errorMessage) :
    m_success(success),
    m_errorMessage(std::move(errorMessage))
{
}

GtOperationApplyResult
GtOperationApplyResult::success()
{
    return GtOperationApplyResult(true);
}

GtOperationApplyResult
GtOperationApplyResult::failure(QString errorMessage)
{
    return GtOperationApplyResult(false, std::move(errorMessage));
}

bool
GtOperationApplyResult::isSuccess() const noexcept
{
    return m_success;
}

QString const&
GtOperationApplyResult::errorMessage() const noexcept
{
    return m_errorMessage;
}
