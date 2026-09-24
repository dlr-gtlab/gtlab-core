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

class GtTask;
class GtObjectUUIDMap;

namespace gt {
namespace utils {
namespace process {


struct PropertyConnectionsRelationshipReturn;

/**
 * @brief Perform a deep copy of a task, assigning new UUIDs to the copied task's process components
 * Property connections are transferred using the mapping of UUIDs from original to the copied objects
 * If the task is a root task, all property connections are transferred.
 * If the task is NOT a root task, the internal property connections (connection between this task and/or its
 * sub-components) are transferred. Connections to other process components outside of this task will trigger a
 * lost connection warning.
 * @param taskOrig
 * @param strict If true, the operation fails when property connections could not be transferred (does not count for stale or obsolete properties)
 * @return
 */
GT_CORE_EXPORT GtTask* deepCopyTask(GtTask* taskOrig, bool strict=false);




/**
 * @brief In a copied task, the property connections are st to the new UUIDs of source/target with the mapping received
 * through GtObject::copy(GtObjectUUIDMap&)
 * @param copy
 * @param uuidMap
 * @return
 */
GT_CORE_EXPORT bool transferPropertyConnectionsViaMapping(GtTask* taskCopy, GtObjectUUIDMap* uuidMap);


/**
 * @brief Remove stale or obsolete property connections from a task
 * As the property connections are only stored in the root task, it can only be applied to root tasks.
 * If tasks is no root task, a warning will be issued and the function returns false.
 * @param task
 * @return Whether cleanup was successful
 */
GT_CORE_EXPORT bool cleanupPropertyConnections(GtTask* task);



} // namespace process
} // namespace utils
} // namespace gt


#endif // GT_DEEPCOPYTASK_H
