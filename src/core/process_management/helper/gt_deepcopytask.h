/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Author: Matthias Schuff (SG-VTM)
 */

#ifndef GT_DEEPCOPYTASK_H
#define GT_DEEPCOPYTASK_H

#include "gt_core_exports.h"
#include "gt_task.h"


namespace gt {
namespace core {
namespace processmanagement {



/**
 * @brief Perform a deep copy of a task, assigning new UUIDs to the copied task's process components
 * Property connections are transferred using the mapping of UUIDs from original to the copied objects
 * If the task is a root task, all property connections are transferred.
 * If the task is NOT a root task, the internal property connections (connection between this task and/or its
 * sub-components) are transferred. Connections to other process components outside of this task will trigger a
 * lost connection warning.
 * @param taskOrig
 * @param strict If true, the operation fails when property connections could not be transferred
 * @return
 */
GT_CORE_EXPORT GtTask* deepCopyTask(GtTask* taskOrig, bool strict=false);




/**
 * @brief Helper datatype for returning the relation of property connections to a task
 * @param internal Referring only to process components within this task (including itself).
 * @param external Connections within this task or its children connected to an outside process component.
 * @param foreign Connections that are not connected to this task or its children. (For a subtask this can include stale connections
 * of the root task, but they are irrelevant for the investigated sub task. Perform check on root task to find all stale connections.)
 * @param stale Connections that have a connection inside the task but the partner is not found (in the root task).
 * @param obsolete Connections do not connect to any process component (even in the root task).
 */
struct GT_CORE_EXPORT TaskPropertyConnectionsRelationshipReturn
{
    QList<GtPropertyConnection*> internal;
    QList<GtPropertyConnection*> external;
    QList<GtPropertyConnection*> foreign;
    QList<GtPropertyConnection*> stale;
    QList<GtPropertyConnection*> obsolete;
};


GT_CORE_EXPORT GtTask* highestParentTask(GtProcessComponent* processComponent);

/**
 * @brief Check the property connections for this task and return the analysis in a struct (see return datatype for more).
 * @param task
 * @return
 */
GT_CORE_EXPORT TaskPropertyConnectionsRelationshipReturn analyzeTaskPropertyConnectionsRelationship(GtTask *task, bool* ok = nullptr);


GT_CORE_EXPORT bool transferPropertyConnectionsViaMapping(GtTask* copy, GtObjectUUIDMap* mappingUuidOldToNew);



GT_CORE_EXPORT GtPropertyConnection* makePropertyConnection(GtProcessComponent* sourceComponent, const QString &sourceProperty, GtProcessComponent* targetComponent, const QString &targetProperty, bool *ok=nullptr);




}
}
} // namespace gt


#endif // GT_DEEPCOPYTASK_H
