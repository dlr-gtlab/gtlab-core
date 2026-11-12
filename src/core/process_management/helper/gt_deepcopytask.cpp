/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Author: Matthias Schuff (SG-VTM)
 */

#include "gt_deepcopytask.h"
#include "gt_coredatamodel.h"
#include "gt_objectuuidmap.h"
#include "gt_relativeobjectlinkproperty.h"
#include "gt_taskgroup.h"
#include "gt_taskanalysis.h"


namespace gt
{
namespace utils
{
namespace process
{


std::unique_ptr<GtTask>
deepCopyTask(GtTask* taskOrig)
{
    GtObjectUUIDMap uuidMap;

    if (!taskOrig)
    {
        gtError() << "Task to copy is null";
        return {};
    }

    //TODO: copy will fire a gtError: "Property connection could not be established!   Source object not found! "
    //During restoring from memento, GtTask::onObjectDataMerged() will trigger makeConnection() of each property connection
    //This error needs to be suppressed, PR #1654 makes a suggestion for that
    gtError() << "TODO: The possible following error(s) 'Property connection "
                 "could not be established!   Source object not found!' are "
                 "misleading and need suppression.";
    std::unique_ptr<GtObject> objectCopy{taskOrig->copy(uuidMap)};
    gtError() << "<<< END TODO misleading error(s)";

    if (!qobject_cast<GtTask*>(objectCopy.get()))
    {
        return {};
    }

    std::unique_ptr<GtTask> copiedTask{
        static_cast<GtTask*>(objectCopy.release())};

    const auto* rootTask = findRootTask(taskOrig);
    if (!rootTask)
    {
        gtError() << "Could not find root task";
        return {};
    }


    if (!transferPropertyConnectionsViaMapping(*taskOrig, *copiedTask.get(),
                                               uuidMap))
    {
        gtError() << "Could not transfer the property connections";
        return {};
    }

    if (!transferRelativeObjectLinkPropertyViaMapping(*copiedTask.get(),
                                                      uuidMap))
    {
        gtError() << "Could not transfer the relative object link properties";
        return {};
    }

    return copiedTask;
}


bool
transferPropertyConnectionsViaMapping(GtTask& taskOrig, GtTask& taskCopy,
                                      const GtObjectUUIDMap& uuidMap)
{
    // delete all connections, will be added in next step from analysis
    auto connections = taskCopy.findChildren<GtPropertyConnection*>();
    qDeleteAll(connections);

    // get the internal connections to be transferred
    auto analyzedConsOptional = analyzePropertyConnectionsRelationship(taskOrig);
    if (!analyzedConsOptional.has_value())
    {
        gtError() << "Analyzing the task property connections of original task "
                     "failed.";
        return false;
    }

    auto analyzedCons = analyzedConsOptional.value();

    // copy only the internal connections
    foreach (auto c, analyzedCons.internal)
    {
        auto newCon = c->copy();
        taskCopy.appendChild(newCon);
    }

    // transfer the UUID mapping of connections
    QList<GtPropertyConnection*> allCons =
        taskCopy.findChildren<GtPropertyConnection*>();
    bool transferAllOk = true;
    for (auto con : qAsConst(allCons))
    {
        auto origSrcUuid = con->sourceUuid();
        auto origTargetUuid = con->targetUuid();

        // Remap each endpoint independently; missing mappings yield empty UUIDs.
        con->setSourceUuid(uuidMap.copiedUuid(origSrcUuid));
        con->setTargetUuid(uuidMap.copiedUuid(origTargetUuid));

        if (uuidMap.containsOriginalUuid(origSrcUuid) &&
            uuidMap.containsOriginalUuid(origTargetUuid))
        {
            con->makeConnection();
        }
        else
        {
            gtError() << "Failed to update connection: " + con->objectName() +
                             "\n      |-> " + origSrcUuid + "(" +
                             con->sourceProp() + ")" + " -> " + origTargetUuid +
                             "(" + con->targetProp() + ")";
            if (!uuidMap.containsOriginalUuid(origSrcUuid))
            {
                gtError() << "|-> Source UUID not in original task to copy";
            }
            if (!uuidMap.containsOriginalUuid(origTargetUuid))
            {
                gtError() << "|-> Target UUID not in original task to copy";
            }

            transferAllOk = false;
        }
    }

    return transferAllOk;
}

bool
transferRelativeObjectLinkPropertyViaMapping(GtObject& objCopy,
                                             const GtObjectUUIDMap& uuidMap)
{
    bool retval = true;

    for (auto prop : qAsConst(objCopy.properties()))
    {
        auto propRelLink = qobject_cast<GtRelativeObjectLinkProperty*>(prop);

        if (propRelLink && propRelLink->getVal() != "")
        {
            auto originalUuid = propRelLink->getVal();

            if (!uuidMap.containsOriginalUuid(originalUuid))
            {
                gtError()
                    << &objCopy << " with uuid" << objCopy.uuid()
                    << ": Could not transfer " << propRelLink
                    << ", original linked UUID" << originalUuid
                    << "not found in original process component structure";
                retval = false;
                continue;
            }

            auto copiedUuid = uuidMap.copiedUuid(originalUuid);

            propRelLink->setVal(copiedUuid);
        }
    }

    for (auto child : objCopy.findDirectChildren<GtObject*>())
    {
        if (!transferRelativeObjectLinkPropertyViaMapping(*child, uuidMap))
        {
            retval = false;
        }
    }

    return retval;
}

bool
cleanupPropertyConnections(GtTask& task)
{
    if (&task != findRootTask(&task))
    {
        gtWarning() << "Performing cleanup is only allowed on root tasks";
        return false;
    }

    auto allCons = analyzePropertyConnectionsRelationship(task);
    if (!allCons.has_value())
    {
        gtError() << "Analyzing the task property connections of original task "
                     "failed.";
        return false;
    }

    QList<GtPropertyConnection*> toRemove;

    toRemove << allCons.value().obsolete;
    toRemove << allCons.value().stale;

    qDeleteAll(toRemove);

    return true;
}


} // namespace process
} // namespace utils
} // namespace gt
