/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Author: Matthias Schuff (SG-VTM)
 */

#include "gt_pasteprocesselement.h"

#include "gt_processcomponent.h"
#include "gt_task.h"
#include "gt_propertyconnection.h"
#include "gt_coredatamodel.h"
#include "gt_taskanalysis.h"

namespace gt
{
namespace utils
{
namespace process
{


bool
pasteProcessComponentHelper(GtObject* targetParent, GtProcessComponent* child,
                            bool useGtDataModel = false,
                            QModelIndex* modelIndex = nullptr)
{   
    if (useGtDataModel)
    {
        auto newIndex = gtDataModel->appendChild(child, targetParent);

        if (!newIndex.isValid())
        {
            gtError() << "Pasting process component failed!";
            return false;
        }

        if (modelIndex)
        {
            *modelIndex = newIndex;
        }
    }
    else
    {
        targetParent->appendChild(child);
    }

    auto parentTask = qobject_cast<GtTask*>(targetParent);

    if (parentTask)
    {
        auto propCons = child->findChildren<GtPropertyConnection*>();
        auto roottask = findRootTask(parentTask);
        if (!roottask)
        {
            gtError()
                << "Could not find root task to transfer property connections";
            return false;
        }

        for (auto c : qAsConst(propCons))
        {
            if (useGtDataModel)
            {
                gtDataModel->appendChild(c, roottask);
            }
            else
            {
                roottask->appendChild(c);
            }
        }
    }

    return true;
}

bool
pasteProcessComponent(GtObject* targetParent, GtProcessComponent* child)
{
    return pasteProcessComponentHelper(targetParent, child, false, nullptr);
}

QModelIndex
pasteProcessComponentToDatamodel(GtObject* targetParent,
                                 GtProcessComponent* child, bool* success)
{
    QModelIndex idx;
    bool helperRetval =
        pasteProcessComponentHelper(targetParent, child, true, &idx);

    if (success)
    {
        *success = helperRetval;
    }

    if (!helperRetval)
    {
        return {};
    }

    return idx;
}


} // namespace process
} // namespace utils
} // namespace gt
