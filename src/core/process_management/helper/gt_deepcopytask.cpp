/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Author: Matthias Schuff (SG-VTM)
 */

#include "gt_deepcopytask.h"
#include "gt_objectuuidmap.h"
#include "gt_taskgroup.h"
#include "gt_taskanalysis.h"


namespace gt {
namespace utils {
namespace process {


GtTask*
deepCopyTask(GtTask* taskOrig, bool strict)
{
    GtObjectUUIDMap uuidMap;


    //TODO: copy will fire a gtError: "Property connection could not be established!   Source object not found! "
    //During restoring from memento, GtTask::onObjectDataMerged() will trigger makeConnection() of each property connection
    //This error needs to be suppressed
    gtError() << "TODO: The possible following error(s) 'Property connection could not be established!   Source object not found!' are misleading and need suppression.";
    auto copiedTask = qobject_cast<GtTask*>(taskOrig->copy(uuidMap));
    gtError() << "<<< END TODO misleading error(s)";



    auto rootTask = findRootTask(taskOrig);
    if(!rootTask)
    {
        gtError() << "Could not find root task";
        if(strict)
        {
            delete copiedTask;
            return nullptr;
        }
        else
        {
            return copiedTask;
        }
    }

    bool ok1=false;
    auto allCons = analyzePropertyConnectionsRelationship(taskOrig, &ok1);
    if(!ok1)
    {
        gtError() << "Analyzing the task property connections of original task failed.";

        if(strict)
        {
            delete copiedTask;
            return nullptr;
        }
        else
        {
            return copiedTask;
        }
    }

    printWarningForLostPropertyConnections(taskOrig, &allCons, true);

    // not root task, take with the internal property connections
    if(rootTask != taskOrig)
    {
        for(auto* c: allCons.internal)
        {
            auto newCon = c->copy();
            copiedTask->appendChild(newCon);
        }
    }

    if(!transferPropertyConnectionsViaMapping(copiedTask, &uuidMap))
    {
        gtError() << "Could not transfer the property connections";

        if(strict)
        {
            delete copiedTask;
            return nullptr;
        }
        else
        {
            return copiedTask;
        }
    }

    return copiedTask;
}

bool
transferPropertyConnectionsViaMapping(GtTask* taskCopy, GtObjectUUIDMap* uuidMap)
{
    QList<GtPropertyConnection*> allCons = taskCopy->findChildren<GtPropertyConnection*>();

    bool transferAllOk = true;

    for(auto con: qAsConst(allCons))
    {
        gtError() << "Connection: " + con->objectName()
                      + "\n      |-> " + con->sourceUuid() + "("+con->sourceProp()+")"
                      + " -> "
                      + con->targetUuid() + "("+con->targetProp()+")";

        auto origSrcUuid = con->sourceUuid();
        auto origTargetUuid = con->targetUuid();

        bool ok2 = true;

        if (uuidMap->containsOriginalUuid(origSrcUuid))
        {
            QString newUuid = uuidMap->copiedUuid(origSrcUuid);
            con->setSourceUuid( newUuid );
        }
        else
        {
            gtInfo() << "Could not update property connection source!";
            con->setSourceUuid("");
            ok2 = false;
        }

        if (uuidMap->containsOriginalUuid(origTargetUuid))
        {
            QString newUuid = uuidMap->copiedUuid(origTargetUuid);
            con->setTargetUuid( newUuid );
        }
        else
        {
            gtInfo() << "Could not update property connection target!";
            con->setTargetUuid("");
            ok2 = false;
        }

        if(ok2)
        {
            con->makeConnection();
        }
        else
        {
            transferAllOk = false;
        }

    }

    return transferAllOk;
}

bool
cleanupPropertyConnections(GtTask *task)
{
    if(task!=findRootTask(task))
    {
        gtWarning() << "Performing cleanup is only allowed on root tasks";
        return false;
    }

    bool ok=false;
    auto allCons = analyzePropertyConnectionsRelationship(task, &ok);
    if(!ok)
    {
        gtError() << "Analyzing the task property connections of original task failed.";
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
