/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "gt_objectmementodiff.h"
#include "gt_taskrunner.h"
#include "gt_finishedprocessloadinghelper.h"

GtFinishedProcessLoadingHelper::GtFinishedProcessLoadingHelper(
        QList<GtObjectMemento>* data,
        QPointer<GtObject> source,
        const QString& /* taskName */) :
    m_data(data),
    m_sumDiff(std::make_unique<GtObjectMementoDiff>()),
    m_source(source)
{

}

GtFinishedProcessLoadingHelper::~GtFinishedProcessLoadingHelper() = default;

void
GtFinishedProcessLoadingHelper::run()
{
    *m_sumDiff = GtTaskRunner::dataDiffFor(*m_data);
}

GtObjectMementoDiff*
GtFinishedProcessLoadingHelper::sumDiff() const
{
    return m_sumDiff.get();
}
