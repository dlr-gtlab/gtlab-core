/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Author: Matthias Schuff (SG-VTM)
 */

#include "gt_deepcopytask.h"
#include "gt_objectuuidmap.h"
#include "gt_taskgroup.h"

namespace gt {
namespace core {
namespace processmanagement {



GtTask* deepCopyTask(GtTask* taskOrig, bool strict)
{
    GtObjectUUIDMap uuidMap;
    auto copiedTask = qobject_cast<GtTask*>(taskOrig->copy(uuidMap));

    auto highestParent = highestParentTask(taskOrig);
    if(!highestParent)
    {
        gtError() << "Could not find highest parent task";
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

    // not root task, take with internal property connections
    if(highestParent != taskOrig)
    {
        bool ok1=false;
        auto allCons1 = analyzeTaskPropertyConnectionsRelationship(taskOrig, &ok1);
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

        for(auto* c: allCons1.internal)
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


void printPropConAnalysis(gt::core::processmanagement::TaskPropertyConnectionsRelationshipReturn& x)
{
    qDebug() << "internal:" << x.internal;
    qDebug() << "external:" << x.external;
    qDebug() << "foreign: " << x.foreign;
    qDebug() << "stale:   " << x.stale;
    qDebug() << "obsolete:" << x.obsolete;
}

bool transferPropertyConnectionsViaMapping(GtTask* copy, GtObjectUUIDMap* mappingUuidOldToNew)
{
    QList<GtPropertyConnection*> allCons = copy->findChildren<GtPropertyConnection*>();

    bool transferAllOk = true;

    for(auto* con: allCons)
    {
        auto origSrcUuid = con->sourceUuid();
        auto origTargetUuid = con->targetUuid();

        bool ok2 = true;

        if (mappingUuidOldToNew->containsOriginalUuid(origSrcUuid))
        {
            QString newUuid = mappingUuidOldToNew->copiedUuid(origSrcUuid);
            con->setSourceUuid( newUuid );
        }
        else
        {
            gtInfo() << "Could not update property connection: source!";
            con->setSourceUuid("");
            ok2 = false;
        }

        if (mappingUuidOldToNew->containsOriginalUuid(origTargetUuid))
        {
            QString newUuid = mappingUuidOldToNew->copiedUuid(origTargetUuid);
            con->setTargetUuid( newUuid );
        }
        else
        {
            gtInfo() << "Could not update property connection: target!";
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


GtTask*
highestParentTask(GtProcessComponent *processComponent)
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
        return highestParentTask(parent);
    }

    return nullptr;
}



TaskPropertyConnectionsRelationshipReturn
analyzeTaskPropertyConnectionsRelationship(GtTask *task, bool* ok)
{
    if(ok)
    {
        *ok = false;
    }

    TaskPropertyConnectionsRelationshipReturn retVal;

    if (!task)
    {
        return retVal;
    }

    GtTask* highestParent = highestParentTask(task);

    if (!highestParent)
    {
        return retVal;
    }

    QList<GtPropertyConnection*> allPropCons = highestParent->findChildren<GtPropertyConnection*>();

    QList<GtObject*> internalProcComps = task->findChildren<GtObject*>();
    internalProcComps.append(task);

    foreach (GtPropertyConnection* propCon, allPropCons)
    {
        GtObject* sourceObj = highestParent->getObjectByUuid(propCon->sourceUuid());
        GtObject* targetObj = highestParent->getObjectByUuid(propCon->targetUuid());

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


GtPropertyConnection*
makePropertyConnection(GtProcessComponent* sourceComponent, const QString &sourceProperty, GtProcessComponent* targetComponent, const QString &targetProperty, bool *ok)
{
    if(ok)
    {
        *ok = false;
    }

    if(!sourceComponent || !targetComponent)
    {
        gtFatal() << "Source or target not set!"
                  << "\nSource:" << sourceComponent
                  << "\nTarget:" << targetComponent;
        return nullptr;
    }


    if(!sourceComponent->findProperty(sourceProperty) || !targetComponent->findProperty(targetProperty))
    {
        QStringList msg;
        if(!sourceComponent->findProperty(sourceProperty))
        {
            msg << "Source property not found!";
        }
        if(!targetComponent->findProperty(targetProperty))
        {
            msg << "Target property not found!";
        }

        gtFatal() << msg.join("\n")
                  << QStringLiteral("\n")+"sourceComponent:" << sourceComponent
                  << QStringLiteral("\n")+"sourceProperty:"  << sourceProperty
                  << QStringLiteral("\n")+"targetComponent:" << targetComponent
                  << QStringLiteral("\n")+"targetProperty:"  << targetProperty;
        return nullptr;
    }

    auto highestParent = highestParentTask(sourceComponent);

    if(!highestParent)
    {
        gtFatal() << "Highest parent not found";
        return nullptr;
    }

    GtPropertyConnection* c = new GtPropertyConnection;
    c->setSourceUuid(sourceComponent->uuid());
    c->setSourceProp(sourceComponent->findProperty(sourceProperty)->ident());
    c->setTargetUuid(targetComponent->uuid());
    c->setTargetProp(targetComponent->findProperty(targetProperty)->ident());
    highestParent->appendChild(c);
    c->makeConnection();

    if (!c->isConnected())
    {
        gtFatal() << "Property connection created, but connection failed";
        delete c;
        return nullptr;
    }

    if(ok)
    {
        *ok = true;
    }

    return c;
}


// void makePropertyConnection(GtProcessComponent *parent, GtProcessComponent *src, const QString &propSrc, GtProcessComponent *dst, const QString &propDst)
// {
//     if(!src->findProperty(propSrc)) {
//         gtFatal() << "Parent:" << parent;
//         gtFatal() << "src:" << src;
//         gtFatal() << "dst:" << dst;
//         gtFatal() << "source property not found:" << propSrc;
//         return;
//     }
//     if(!dst->findProperty(propDst)) {
//         gtFatal() << "Parent:" << parent;
//         gtFatal() << "src:" << src;
//         gtFatal() << "dst:" << dst;
//         gtFatal() << "target property not found:" << propDst;
//         return;
//     }

//     GtPropertyConnection* c = new GtPropertyConnection;
//     c->setSourceUuid(src->uuid());
//     c->setSourceProp(src->findProperty(propSrc)->ident());
//     c->setTargetUuid(dst->uuid());
//     c->setTargetProp(dst->findProperty(propDst)->ident());
//     parent->appendChild(c);
//     c->makeConnection();
// }


// GtTask* deepCopyTask(GtTask* taskOrig)
// {
//     GtTask* taskNew = nullptr;







//     return taskNew;
// }


}
}
} // namespace gt
