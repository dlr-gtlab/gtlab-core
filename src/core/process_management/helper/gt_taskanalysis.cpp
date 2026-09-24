/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Author: Matthias Schuff (SG-VTM)
 */

#include "gt_taskanalysis.h"

#include "gt_processcomponent.h"
#include "gt_taskgroup.h"
#include "gt_task.h"

#include "gt_propertyconnection.h"

namespace gt {
namespace utils {
namespace process {

GtTask*
findRootTask(GtProcessComponent *processComponent)
{
    if (!processComponent)
    {
        return nullptr;
    }

    GtTask* task = qobject_cast<GtTask*>(processComponent);

    if (!processComponent->parent())
    {
        if(task)
        {
            return task;
        }
        else
        {
            return nullptr;
        }
    }

    if (qobject_cast<GtTaskGroup*>(processComponent->parent()))
    {
        if(task)
        {
            return task;
        }
        else
        {
            return nullptr;
        }
    }

    GtTask* parent = qobject_cast<GtTask*>(processComponent->parent());

    if (parent)
    {
        return findRootTask(parent);
    }

    return nullptr;
}


const PropertyConnectionsRelationshipReturn
analyzePropertyConnectionsRelationship(GtProcessComponent* pComp, bool* ok)
{
    if(ok)
    {
        *ok = false;
    }

    PropertyConnectionsRelationshipReturn retVal;

    if (!pComp)
    {
        return retVal;
    }

    GtTask* rootTask = findRootTask(pComp);

    if (!rootTask)
    {
        return retVal;
    }

    QList<GtPropertyConnection*> allPropCons = rootTask->findChildren<GtPropertyConnection*>();

    QList<GtProcessComponent*> internalProcComps = pComp->findChildren<GtProcessComponent*>();
    internalProcComps.append(pComp);

    foreach (GtPropertyConnection* propCon, allPropCons)
    {
        GtProcessComponent* sourceObj = qobject_cast<GtProcessComponent*>(rootTask->getObjectByUuid(propCon->sourceUuid()));
        GtProcessComponent* targetObj = qobject_cast<GtProcessComponent*>(rootTask->getObjectByUuid(propCon->targetUuid()));

        // lost connection to any object
        if (!sourceObj && !targetObj)
        {
            retVal.obsolete.append(propCon);
        }
        // if one connection side is lost and other connects internally
        else if( (!sourceObj &&  targetObj && internalProcComps.contains(targetObj))
                 || ( sourceObj && !targetObj && internalProcComps.contains(sourceObj)) )
        {
            retVal.stale.append(propCon);
        }
        // both internal
        else if ( sourceObj && targetObj && internalProcComps.contains(sourceObj) && internalProcComps.contains(targetObj) )
        {
            retVal.internal.append(propCon);
        }
        // one side is outside
        else if ( sourceObj && targetObj && (internalProcComps.contains(sourceObj) || internalProcComps.contains(targetObj)) )
        {
            retVal.external.append(propCon);
        }
        // otherwise its "foreign"
        else
        {
            retVal.foreign.append(propCon);
        }
    }

    if(ok)
    {
        *ok = true;
    }
    return retVal;
}


QString propertyConnectionsPrint(QList<GtPropertyConnection*>& cons)
{
    QStringList retval;
    for(auto* c: qAsConst(cons))
    {
        retval.append("   |-> Connection: " + c->objectName() + "Connected:" + QString(c->isConnected())
                  + "\n      |-> " + c->sourceUuid() + "("+c->sourceProp()+")"
                  + " -> "
                  + c->targetUuid() + "("+c->targetProp()+")");
    }

    return retval.join("\n");
}

void
printPropConAnalysis(PropertyConnectionsRelationshipReturn* x)
{
    gtInfo() << "Property connections analysis:"
          << "\n|-> internal:\n" + propertyConnectionsPrint(x->internal)
          << "\n|-> external:\n" + propertyConnectionsPrint(x->external)
          << "\n|-> foreign:\n" + propertyConnectionsPrint(x->foreign)
          << "\n|-> stale:\n" + propertyConnectionsPrint(x->stale)
          << "\n|-> obsolete:\n" + propertyConnectionsPrint(x->obsolete);
}

void
printWarningForLostPropertyConnections(GtTask *task, bool verbose)
{
    bool ok1=false;
    auto allCons = analyzePropertyConnectionsRelationship(task, &ok1);
    if(!ok1)
    {
        gtError() << "Analyzing the task property connections of task failed.";
        return;
    }

    printWarningForLostPropertyConnections(task, &allCons, verbose);
}

void
printWarningForLostPropertyConnections(GtTask *task, PropertyConnectionsRelationshipReturn* allCons, bool verbose)
{
    if(!allCons)
    {
        return;
    }

    if(allCons->external.size()>0)
    {
        gtWarning() << "Task" << task->objectName() << "has property connection(s) to outside process component, lost during copying";
    }

    if(allCons->stale.size()>0)
    {
        gtWarning() << "Task" << task->objectName() << "has stale property connection(s), source OR target missing";
    }

    if(allCons->obsolete.size()>0)
    {
        gtWarning() << "Task" << task->objectName() << "has obsolete property connection(s), source AND target missing";
    }

    if (verbose)
    {
        printPropConAnalysis(allCons);
    }

}




} // namespace process
} // namespace utils
} // namespace gt




