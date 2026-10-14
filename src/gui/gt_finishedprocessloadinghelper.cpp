/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include <utility>

#include "gt_objectmementodiff.h"
#include "gt_finishedprocessloadinghelper.h"

GtFinishedProcessLoadingHelper::GtFinishedProcessLoadingHelper(
        GtObjectMementoDiff dataDiff) :
    m_sumDiff(std::make_unique<GtObjectMementoDiff>(std::move(dataDiff)))
{

}

GtFinishedProcessLoadingHelper::~GtFinishedProcessLoadingHelper() = default;

void
GtFinishedProcessLoadingHelper::run()
{
}

GtObjectMementoDiff*
GtFinishedProcessLoadingHelper::sumDiff() const
{
    return m_sumDiff.get();
}
