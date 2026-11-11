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

    if(!taskOrig)
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

    auto* task = qobject_cast<GtTask*>(objectCopy.get());
    if (!task)
    {
        return {};
    }

    objectCopy.release();
    std::unique_ptr<GtTask> copiedTask{task};

    if (!copiedTask)
    {
        return {};
    }

    auto rootTask = findRootTask(taskOrig);
    if (!rootTask)
    {
        gtError() << "Could not find root task";
        return {};
    }


    if (!transferPropertyConnectionsViaMapping(taskOrig, copiedTask.get(), uuidMap))
    {
        gtError() << "Could not transfer the property connections";
        return {};
    }

    if (!transferRelativeObjectLinkPropertyViaMapping(copiedTask.get(), uuidMap))
    {
        gtError() << "Could not transfer the relative object link properties";
        return {};
    }

    return copiedTask;
}




bool
transferPropertyConnectionsViaMapping(GtTask* taskOrig, GtTask* taskCopy, const GtObjectUUIDMap& uuidMap)
{
    // delete all connections, will be added in next step from analysis
    auto connections = taskCopy->findChildren<GtPropertyConnection*>();
    qDeleteAll(connections);

    // get the internal connections to be transferred
    bool ok1 = false;
    auto analyzedCons = analyzePropertyConnectionsRelationship(taskOrig, &ok1);
    if (!ok1)
    {
        gtError() << "Analyzing the task property connections of original task "
                     "failed.";
        return false;
    }

    // copy only the internal connections
    foreach (auto c, analyzedCons.internal)
    {
        auto newCon = c->copy();
        taskCopy->appendChild(newCon);
    }

    // transfer the UUID mapping of connections
    QList<GtPropertyConnection*> allCons = taskCopy->findChildren<GtPropertyConnection*>();
    bool transferAllOk = true;
    for (auto con : qAsConst(allCons))
    {
        auto origSrcUuid = con->sourceUuid();
        auto origTargetUuid = con->targetUuid();

        if (uuidMap.containsOriginalUuid(origSrcUuid) && uuidMap.containsOriginalUuid(origTargetUuid) )
        {
            QString newUuidSrc = uuidMap.copiedUuid(origSrcUuid);
            con->setSourceUuid(newUuidSrc);

            QString newUuidDst = uuidMap.copiedUuid(origTargetUuid);
            con->setTargetUuid(newUuidDst);

            con->makeConnection();
        }
        else
        {
            gtError() << "Failed to update connection: " + con->objectName() + "\n      |-> " +
                         con->sourceUuid() + "(" + con->sourceProp() + ")" +
                         " -> " + con->targetUuid() + "(" + con->targetProp() +
                         ")";
            if(!uuidMap.containsOriginalUuid(origSrcUuid))
            {
                gtError() << "|-> Source UUID not in original task to copy";
                con->setSourceUuid("");
            }
            else if(!uuidMap.containsOriginalUuid(origTargetUuid))
            {
                gtError() << "|-> Target UUID not in original task to copy";
                con->setTargetUuid("");
            }

            transferAllOk = false;
        }

    }

    return transferAllOk;
}

bool
transferRelativeObjectLinkPropertyViaMapping(GtObject* objCopy, const GtObjectUUIDMap& uuidMap)
{
    bool retval = true;


    for (auto prop : qAsConst(objCopy->properties()))
    {
        auto propRelLink = qobject_cast<GtRelativeObjectLinkProperty*>(prop);

        if (propRelLink && propRelLink->getVal() != "")
        {
            auto originalUuid = propRelLink->getVal();

            if (!uuidMap.containsOriginalUuid(originalUuid))
            {
                gtError()
                    << objCopy << " with uuid" << objCopy->uuid()
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


    for (auto child : objCopy->findDirectChildren<GtObject*>())
    {
        if (!transferRelativeObjectLinkPropertyViaMapping(child, uuidMap))
        {
            retval = false;
        }
    }


    return retval;
}

bool
cleanupPropertyConnections(GtTask* task)
{
    if (task != findRootTask(task))
    {
        gtWarning() << "Performing cleanup is only allowed on root tasks";
        return false;
    }

    bool ok = false;
    auto allCons = analyzePropertyConnectionsRelationship(task, &ok);
    if (!ok)
    {
        gtError() << "Analyzing the task property connections of original task "
                     "failed.";
        return false;
    }

    QList<GtPropertyConnection*> toRemove;

    toRemove << allCons.obsolete;
    toRemove << allCons.stale;

    qDeleteAll(toRemove);

    return true;
}



} // namespace process
} // namespace utils
} // namespace gt
