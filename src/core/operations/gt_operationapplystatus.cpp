/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_operationapplystatus.h"

#include <utility>

GtOperationApplyStatus::GtOperationApplyStatus(bool succeeded,
                                               QString errorMessage) :
    m_succeeded(succeeded),
    m_errorMessage(std::move(errorMessage))
{
}

GtOperationApplyStatus
GtOperationApplyStatus::success()
{
    return GtOperationApplyStatus(true);
}

GtOperationApplyStatus
GtOperationApplyStatus::failure(QString errorMessage)
{
    return GtOperationApplyStatus(false, std::move(errorMessage));
}

bool
GtOperationApplyStatus::succeeded() const noexcept
{
    return m_succeeded;
}

QString const&
GtOperationApplyStatus::errorMessage() const noexcept
{
    return m_errorMessage;
}
