/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Author: Matthias Schuff (SG-VTM)
 */
#ifndef GT_MOVEPROPERTYCONNECTIONSTONEWROOT_H
#define GT_MOVEPROPERTYCONNECTIONSTONEWROOT_H

#include "gt_core_exports.h"

#include <QList>
#include <optional>

class GtObject;
class GtProcessComponent;
class GtTask;
class GtPropertyConnection;

namespace gt
{
namespace utils
{
namespace process
{

struct PropertyConnectionMovePlan
{
    QList<GtPropertyConnection*> toMove;
    QList<GtPropertyConnection*> toDelete;
};

/**
 * @brief Prepares a plan which GtPropertyConnection to move and which to delete when moving a GtProcessComponent to a new root task
 * @param pComp the GtProcessComponent
 * @param oldRoot old root task
 * @param newRoot new root task
 * @return
 */
GT_CORE_EXPORT std::optional<PropertyConnectionMovePlan>
prepareMovePropertyConnectionToNewRoot(GtProcessComponent& pComp,
                                       GtTask& oldRoot, GtTask& newRoot);

/**
 * @brief Print warnings when connections are lost due to reparenting under a new root task
 * @param pComp
 * @param verbose
 */
GT_CORE_EXPORT void printWarningForLostPropertyConnectionsWhenMovingToNewRoot(
    GtProcessComponent& pComp, bool verbose = false);

} // namespace process
} // namespace utils
} // namespace gt

#endif // GT_MOVEPROPERTYCONNECTIONSTONEWROOT_H
