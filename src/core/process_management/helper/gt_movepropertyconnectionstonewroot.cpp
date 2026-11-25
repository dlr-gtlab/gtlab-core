/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Author: Matthias Schuff (SG-VTM)
 */
#include "gt_processcomponent.h"
#include "gt_movepropertyconnectionstonewroot.h"
#include "gt_taskanalysis.h"
#include "gt_logging.h"
#include "gt_propertyconnection.h"

namespace gt
{
namespace utils
{
namespace process
{

std::optional<PropertyConnectionMovePlan>
prepareMovePropertyConnectionToNewRoot(GtProcessComponent& pComp,
                                       GtTask& oldRoot, GtTask& newRoot)
{
    if (&oldRoot == &newRoot)
    {
        gtWarning() << "Root task does not change";
        return {};
    }

    auto allConsRet =
        gt::utils::process::analyzePropertyConnectionsRelationship(pComp);
    if (!allConsRet.has_value())
    {
        gtError() << "Could not analyze property connections of "
                  << pComp.uuid();
        return {};
    }

    PropertyConnectionMovePlan plan;

    auto allCons = allConsRet.value();

    foreach (auto c, allCons.external)
    {
        plan.toDelete.append(c);
    }

    foreach (auto c, allCons.internal)
    {
        plan.toMove.append(c);
    }

    return plan;
}

void
printWarningForLostPropertyConnectionsWhenMovingToNewRoot(
    GtProcessComponent& pComp, bool verbose)
{
    auto allCons =
        gt::utils::process::analyzePropertyConnectionsRelationship(pComp);
    if (allCons.has_value() && allCons.value().external.size() > 0)
    {
        gtWarning() << pComp.objectName()
                    << "has property connection(s) lost during moving";
        if (verbose)
        {
            foreach (auto c, allCons.value().external)
            {
                gtWarning()
                    << "   |-> Connection lost: " + c->objectName() +
                           "Connected:" +
                           (c->isConnected() ? QStringLiteral("true")
                                             : QStringLiteral("false")) +
                           "\n      |-> " + c->sourceUuid() + "(" +
                           c->sourceProp() + ")" + " -> " + c->targetUuid() +
                           "(" + c->targetProp() + ")";
            }
        }
    }
}

} // namespace process
} // namespace utils
} // namespace gt
