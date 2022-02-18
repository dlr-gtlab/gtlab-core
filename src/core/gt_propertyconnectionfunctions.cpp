/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyconnectionfunctions.cpp
 * copyright 2009-2022 by DLR
 *
 *  Created on: 24.01.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gt_propertyconnectionfunctions.h"

#include "gt_propertyconnection.h"
#include "gt_task.h"
#include "gt_calculator.h"
#include "gt_relativeobjectlinkproperty.h"
#include "gt_processdata.h"

GtPropertyConnection*
GtPropertyConnectionFunctions::findConnectionCopy(
        const GtPropertyConnection* origCon,
        const QList<GtPropertyConnection*>& newCons)
{
    if (origCon == nullptr)
    {
        return nullptr;
    }

    if (newCons.isEmpty())
    {
        return nullptr;
    }

    foreach (GtPropertyConnection* newCon, newCons)
    {
        bool check = true;

        if (newCon->sourceUuid() != origCon->sourceUuid())
        {
            check = false;
        }
        else if (newCon->targetUuid() != origCon->targetUuid())
        {
            check = false;
        }
        else if (newCon->sourceProp() != origCon->sourceProp())
        {
            check = false;
        }
        else if (newCon->targetProp() != origCon->targetProp())
        {
            check = false;
        }

        if (check)
        {
            return newCon;
        }
    }

    return nullptr;
}

bool
GtPropertyConnectionFunctions::updateConnectionProperties(
        GtPropertyConnection* origCon,
        GtPropertyConnection* copyCon,
        GtTask* orig, GtTask* copy)
{
    if (origCon == nullptr || copyCon == nullptr)
    {
        return false;
    }

    if (orig == nullptr || copy == nullptr)
    {
        return false;
    }

    GtObject* origSourceObj = orig->getObjectByUuid(origCon->sourceUuid());
    GtObject* origTargetObj = orig->getObjectByUuid(origCon->targetUuid());

    if (origSourceObj == nullptr || origTargetObj == nullptr)
    {
        gtError() << "Could not find original objects...";
        return false;
    }

    GtObject* newSourceObj = findEquivalentObject(copy, origSourceObj);
    GtObject* newTargetObj = findEquivalentObject(copy, origTargetObj);

    if (newSourceObj == nullptr || newTargetObj == nullptr)
    {
        gtError() << "Could not find new objects...";
        return false;
    }

    copyCon->setSourceUuid(newSourceObj->uuid());
    copyCon->setTargetUuid(newTargetObj->uuid());

    gtDebug() << "Connection updated successfully!";

    return true;
}

bool
GtPropertyConnectionFunctions::updateRelativeObjectLinks(
        GtObject* orig, GtObject* copy)
{
    if (orig == nullptr || copy == nullptr)
    {
        return false;
    }

    QList<GtObject*> origObjects = orig->findChildren<GtObject*>();
    origObjects.append(orig);

    GtObject* copyClone = copy->clone();

    if (copyClone == nullptr)
    {
        return false;
    }

    if (copyClone->uuid() != copy->uuid())
    {
        return false;
    }

    foreach (GtObject* obj, origObjects)
    {
        QList<GtRelativeObjectLinkProperty*> relLinks =
            obj->propertiesByType<GtRelativeObjectLinkProperty*>();

        foreach (GtRelativeObjectLinkProperty* relLink, relLinks)
        {
            if (!mapRelativeObjectLink(orig, copy, copyClone, obj, relLink))
            {
                gtDebug() << "Failed to update relative object link!";
                continue;
            }
            gtDebug() << "Successfully Updated Relative Object Link!";
        }
    }

    delete copyClone;

    return true;
}

GtObject*
GtPropertyConnectionFunctions::findEquivalentObject(GtObject* parent,
                                                    GtObject* origObj)
{
    if (parent == nullptr || origObj == nullptr)
    {
        return nullptr;
    }

    QString name = origObj->objectName();
    QString className = origObj->metaObject()->className();

    QList<GtObject*> objects = parent->findChildren<GtObject*>();
    objects.append(parent);

    foreach (GtObject* obj, objects)
    {
        QString nameTmp = obj->objectName();
        QString classNameTmp = obj->metaObject()->className();

        if (nameTmp == name && classNameTmp == className)
        {
            QList<GtAbstractProperty*> props = origObj->properties();

            bool cont = false;

            foreach (GtAbstractProperty* absProp, props)
            {
                GtAbstractProperty* newProp =
                    obj->findProperty(absProp->ident());

                if (newProp == nullptr)
                {
                    continue;
                }

                if (absProp->valueToVariant() != newProp->valueToVariant())
                {
                    cont = true;
                    break;
                }
            }

            if (cont)
            {
                continue;
            }

            GtObject* origPar = qobject_cast<GtObject*>(origObj->parent());
            GtObject* parent = qobject_cast<GtObject*>(obj->parent());

            if (origPar == nullptr && parent == nullptr)
            {
                return obj;
            }

            if (origPar->metaObject()->className() !=
                    parent->metaObject()->className())
            {
                gtDebug() << "Parent class names are different!";
                continue;
            }

            int children =  origPar->childCount<GtObject*>();
            int childrenTmp = parent->childCount<GtObject*>();

            if (children != childrenTmp)
            {
                gtDebug() << "Number of direct children are different!";
                continue;
            }

            children = origPar->findChildren<GtObject*>().size();
            childrenTmp = parent->findChildren<GtObject*>().size();

            if (children != childrenTmp)
            {
                gtDebug() << "Number of children different!";
                continue;
            }

            return obj;
        }
    }

    return nullptr;
}

GtTask*
GtPropertyConnectionFunctions::highestParentTask(GtTask* childTask)
{
    if (childTask == nullptr)
    {
        return nullptr;
    }

    if (childTask->parent() == nullptr)
    {
        return childTask;
    }

    if (qobject_cast<GtProcessData*>(childTask->parent()) != nullptr)
    {
        return childTask;
    }
    else
    {
        GtTask* parent = qobject_cast<GtTask*>(childTask->parent());

        if (parent != nullptr)
        {
            return highestParentTask(parent);
        }
    }

    return nullptr;
}

GtTask*
GtPropertyConnectionFunctions::highestParentTask(const GtCalculator* childCalc)
{
    if (childCalc == nullptr)
    {
        return nullptr;
    }

    if (childCalc->parent() == nullptr)
    {
        return nullptr;
    }

    GtTask* task = qobject_cast<GtTask*>(childCalc->parent());

    if (task != nullptr)
    {
        return highestParentTask(task);
    }

    return nullptr;
}

GtTask*
GtPropertyConnectionFunctions::highestParentTask(
        GtProcessComponent* childComp)
{
    if (childComp == nullptr)
    {
        return nullptr;
    }

    if (GtTask* task = qobject_cast<GtTask*>(childComp))
    {
        return highestParentTask(task);
    }

    if (GtCalculator* calc = qobject_cast<GtCalculator*>(childComp))
    {
        return highestParentTask(calc);
    }

    return nullptr;
}

QList<GtPropertyConnection*>
GtPropertyConnectionFunctions::internalPropertyConnections(GtTask *task)
{
    QList<GtPropertyConnection*> retVal = {};

    if (task == nullptr)
    {
        return retVal;
    }

    GtTask* highestParent = highestParentTask(task);

    if (highestParent == nullptr)
    {
        return retVal;
    }

    QList<GtPropertyConnection*> allPropCons =
        highestParent->findChildren<GtPropertyConnection*>();

    foreach (GtPropertyConnection* propCon, allPropCons)
    {
        GtObject* sourceObj =
            highestParent->getObjectByUuid(propCon->sourceUuid());
        GtObject* targetObj =
            highestParent->getObjectByUuid(propCon->targetUuid());

        if (sourceObj == nullptr || targetObj == nullptr)
        {
            continue;
        }

        QList<GtObject*> allChildren = task->findChildren<GtObject*>();

        if (allChildren.contains(sourceObj) && allChildren.contains(targetObj))
        {
            retVal.append(propCon);
        }
        else if (sourceObj == qobject_cast<GtObject*>(task) &&
                 allChildren.contains(targetObj))
        {
            retVal.append(propCon);
        }
        else if (targetObj == qobject_cast<GtObject*>(task) &&
                 allChildren.contains(sourceObj))
        {
            retVal.append(propCon);
        }
    }

    return retVal;
}

QList<GtPropertyConnection*>
GtPropertyConnectionFunctions::internalPropertyConnections(
        GtProcessComponent* pComp)
{
    QList<GtPropertyConnection*> retVal;

    if (qobject_cast<GtCalculator*>(pComp))
    {
        return retVal;
    }
    else if (GtTask* task = qobject_cast<GtTask*>(pComp))
    {
        retVal = internalPropertyConnections(task);
    }

    return retVal;
}

QList<GtPropertyConnection*>
GtPropertyConnectionFunctions::lostPropertyConnections(GtTask* task)
{
    QList<GtPropertyConnection*> retVal;

    if (task == nullptr)
    {
        return retVal;
    }

    GtTask* highestParent = highestParentTask(task);

    if (highestParent == nullptr)
    {
        return retVal;
    }

    QList<GtPropertyConnection*> allPropCons =
        highestParent->findChildren<GtPropertyConnection*>();

    foreach (GtPropertyConnection* propCon, allPropCons)
    {
        GtObject* sourceObj =
            highestParent->getObjectByUuid(propCon->sourceUuid());
        GtObject* targetObj =
            highestParent->getObjectByUuid(propCon->targetUuid());

        if (sourceObj == nullptr || targetObj == nullptr)
        {
            continue;
        }

        QList<GtObject*> allChildren = task->findChildren<GtObject*>();

        if (allChildren.contains(sourceObj) &&
                !allChildren.contains(targetObj))
        {
            if (qobject_cast<GtObject*>(task) != targetObj)
            {
                retVal.append(propCon);
            }
        }
        else if (!allChildren.contains(sourceObj) &&
                 allChildren.contains(targetObj))
        {
            if (qobject_cast<GtObject*>(task) != sourceObj)
            {
                retVal.append(propCon);
            }
        }
    }

    return retVal;
}

QList<GtPropertyConnection*>
GtPropertyConnectionFunctions::lostPropertyConnections(GtCalculator* calc)
{
    return relatedPropertyConnections(calc);
}

QList<GtPropertyConnection*>
GtPropertyConnectionFunctions::lostPropertyConnections(
        GtProcessComponent* pComp)
{
    if (GtTask* task = qobject_cast<GtTask*>(pComp))
    {
        return lostPropertyConnections(task);
    }
    else if (GtCalculator* calc = qobject_cast<GtCalculator*>(pComp))
    {
        return lostPropertyConnections(calc);
    }

    return QList<GtPropertyConnection*>();
}

QList<GtPropertyConnection*>
GtPropertyConnectionFunctions::relatedPropertyConnections(GtTask* task)
{
    QList<GtPropertyConnection*> retVal;

    foreach (GtPropertyConnection* propCon,
             internalPropertyConnections(task))
    {
        retVal.append(propCon);
    }

    foreach (GtPropertyConnection* propCon,
             lostPropertyConnections(task))
    {
        retVal.append(propCon);
    }

    return retVal;
}

QList<GtPropertyConnection*>
GtPropertyConnectionFunctions::relatedPropertyConnections(GtCalculator* calc)
{
    QList<GtPropertyConnection*> retVal;

    if (calc == nullptr)
    {
        return retVal;
    }

    GtTask* highestParent = highestParentTask(calc);

    if (highestParent == nullptr)
    {
        return retVal;
    }

    QList<GtPropertyConnection*> allPropCons =
        highestParent->findChildren<GtPropertyConnection*>();

    foreach (GtPropertyConnection* propCon, allPropCons)
    {
        GtObject* sourceObj =
            highestParent->getObjectByUuid(propCon->sourceUuid());

        GtObject* targetObj =
            highestParent->getObjectByUuid(propCon->targetUuid());

        if (sourceObj == nullptr || targetObj == nullptr)
        {
            continue;
        }

        if (calc == sourceObj || calc == targetObj)
        {
            retVal.append(propCon);
        }
    }

    return retVal;
}

QList<GtPropertyConnection*>
GtPropertyConnectionFunctions::relatedPropertyConnections(
        GtProcessComponent* comp)
{
    if (GtTask* task = qobject_cast<GtTask*>(comp))
    {
        return relatedPropertyConnections(task);
    }
    else if (GtCalculator* calc = qobject_cast<GtCalculator*>(comp))
    {
        return relatedPropertyConnections(calc);
    }

    return QList<GtPropertyConnection*>();
}

void
GtPropertyConnectionFunctions::setOffLostConnectionWarnings(
        QList<GtPropertyConnection*> const& lostCons, GtTask* highestParent)
{
    foreach (GtPropertyConnection* propCon, lostCons)
    {
        GtObject* source = highestParent->getObjectByUuid(
                               propCon->sourceUuid());

        GtObject* target = highestParent->getObjectByUuid(
                               propCon->targetUuid());

        gtWarning() << "WARNING: Losing property connection!";

        GtTask* sourceParent = nullptr;
        GtTask* targetParent = nullptr;

        if (qobject_cast<GtCalculator*>(source) != nullptr)
        {
            sourceParent = qobject_cast<GtTask*>(source->parentObject());
        }
        else
        {
            sourceParent = qobject_cast<GtTask*>(source);
        }

        if (qobject_cast<GtCalculator*>(target) != nullptr)
        {
            targetParent = qobject_cast<GtTask*>(target->parentObject());
        }
        else
        {
            targetParent = qobject_cast<GtTask*>(target);
        }

        if (source != nullptr && target != nullptr)
        {
            if (sourceParent != nullptr &&  targetParent != nullptr)
            {
                gtWarning() << "Source:" << source->objectName()
                            << "in Parent Task:" << sourceParent->objectName();
                gtWarning() << "Target:" << target->objectName()
                            << "in Parent Task:" << targetParent->objectName();
            }
        }
    }
}


bool
GtPropertyConnectionFunctions::mapRelativeObjectLink(
        GtObject* orig, GtObject* copy,
        GtObject* copyClone, GtObject* obj,
        GtRelativeObjectLinkProperty* relLink)
{
    if (orig == nullptr || copy == nullptr || copyClone == nullptr)
    {
        return false;
    }

    if (copy->uuid() != copyClone->uuid())
    {
        return false;
    }

    if (orig->uuid() == copy->uuid())
    {
        return false;
    }

    QString origTargetUUID = relLink->getVal();
    if (origTargetUUID.isEmpty())
    {
        return true;
    }

    GtObject* origTarget = orig->getObjectByUuid(origTargetUUID);

    if (origTarget == nullptr)
    {
        gtDebug() << "Could not identify target object...";
        return false;
    }

    GtObject* targetCopyClone =
            findEquivalentObject(copyClone, origTarget);

    if (targetCopyClone == nullptr)
    {
        gtDebug() << "Could not find equivalent target object...";
        return false;
    }

    GtObject* objCopyClone =  findEquivalentObject(copyClone, obj);

    if (objCopyClone == nullptr)
    {
        gtDebug() << "Could not find equivalent object...";
        return false;
    }

    GtObject* objCopy = copy->getObjectByUuid(objCopyClone->uuid());

    GtObject* targetCopy = copy->getObjectByUuid(
                targetCopyClone->uuid());

    if (objCopy == nullptr || targetCopy == nullptr)
    {
        return false;
    }

    GtAbstractProperty* prop =
            objCopy->findProperty(relLink->ident());

    if (prop == nullptr)
    {
        gtDebug() << "Could not find property";
        return false;
    }

    GtRelativeObjectLinkProperty*  newRelLink =
            qobject_cast<GtRelativeObjectLinkProperty*>(prop);

    if (newRelLink == nullptr)
    {
        gtDebug() << "Is no relative obj Link!";
        return false;
    }

    newRelLink->setVal(targetCopy->uuid());

    return true;
}
