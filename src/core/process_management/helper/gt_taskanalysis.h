/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Author: Matthias Schuff (SG-VTM)
 */

#ifndef GT_TASKANALYSIS_H
#define GT_TASKANALYSIS_H

#include "gt_core_exports.h"

#include <QList>

class GtPropertyConnection;
class GtTask;
class GtProcessComponent;


namespace gt {
namespace utils {
namespace process {

/**
 * @brief Helper datatype for returning the relation of property connections to a process component
 * @param internal Referring only to process components within this task (including itself).
 * @param external Connections within this task or its children connected to an outside process component.
 * @param foreign Connections that are not connected to this task or its children. (For a subtask this can include stale connections
 * of the root task, but they are irrelevant for the investigated sub task. Perform check on root task to find all stale connections.)
 * @param stale Connections that have a connection inside the task but the partner is not found (in the root task).
 * @param obsolete Connections do not connect to any process component (even in the root task).
 */
struct GT_CORE_EXPORT PropertyConnectionsRelationshipReturn
{
    QList<GtPropertyConnection*> internal;
    QList<GtPropertyConnection*> external;
    QList<GtPropertyConnection*> foreign;
    QList<GtPropertyConnection*> stale;
    QList<GtPropertyConnection*> obsolete;
};

/**
 * @brief highestParentTask
 * @param processComponent
 * @return
 */
GT_CORE_EXPORT GtTask* findRootTask(GtProcessComponent* processComponent);

/**
 * @brief Check the property connections of a GtProcessComponent and return the analysis in a struct (see return datatype for more).
 * @param task
 * @return
 */
GT_CORE_EXPORT const PropertyConnectionsRelationshipReturn analyzePropertyConnectionsRelationship(GtProcessComponent* pComp, bool* ok = nullptr);


/**
 * @brief Show a warning when property connections would be lost due to copying
 * @param task The GtTask to be analyzed
 * @param allCons optionally handover a previous analysis
 * @param verbose Output the connections analysis at the end
 */
GT_CORE_EXPORT void printWarningForLostPropertyConnections(GtTask* task, bool verbose=false);
GT_CORE_EXPORT void printWarningForLostPropertyConnections(GtTask* task, PropertyConnectionsRelationshipReturn* allCons, bool verbose=false);


} // namespace process
} // namespace utils
} // namespace gt


#endif // GT_TASKANALYSIS_H
