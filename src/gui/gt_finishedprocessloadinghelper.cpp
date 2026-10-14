/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "gt_objectmementodiff.h"
#include "gt_taskrunner.h"
#include "gt_finishedprocessloadinghelper.h"

#include <utility>

GtFinishedProcessLoadingHelper::GtFinishedProcessLoadingHelper(
    QList<GtTaskMementoPair> data, QPointer<GtObject> source,
    const QString& /* taskName */) :
    m_data(std::move(data)), m_sumDiff(std::make_unique<GtObjectMementoDiff>()),
    m_source(source)
{

}

GtFinishedProcessLoadingHelper::~GtFinishedProcessLoadingHelper() = default;

void
GtFinishedProcessLoadingHelper::run()
{
    const auto dataDiff = GtTaskRunner::dataDiffFor(m_data);
    if (!dataDiff)
    {
        m_valid = false;
        return;
    }

    *m_sumDiff = *dataDiff;
    m_valid = true;
}

GtObjectMementoDiff*
GtFinishedProcessLoadingHelper::sumDiff() const
{
    return m_sumDiff.get();
}

bool
GtFinishedProcessLoadingHelper::isValid() const
{
    return m_valid;
}
