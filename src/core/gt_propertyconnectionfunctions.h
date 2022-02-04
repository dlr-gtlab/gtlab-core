/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyconnectionfunctions.h
 * copyright 2009-2022 by DLR
 *
 *  Created on: 24.01.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#ifndef GTPROPERTYCONNECTIONFUNCTIONS_H
#define GTPROPERTYCONNECTIONFUNCTIONS_H

#include <QList>
#include "gt_core_exports.h"

class GtPropertyConnection;
class GtObject;
class GtTask;
class GtCalculator;
class GtProcessComponent;
class GtRelativeObjectLinkProperty;

/**
 *
 */
namespace GtPropertyConnectionFunctions
{

    /**
     * @brief searches for a copy of certain property connection within a
     * list of all copies
     * @param origCon - original connection
     * @param newCons - copied connections
     * @return
     */
    GT_CORE_EXPORT GtPropertyConnection* findConnectionCopy(
            const GtPropertyConnection* origCon,
            const QList<GtPropertyConnection *> newCons);

    /**
     * @brief updates a certain property connection to new values
     * regarding target and source uuids/properties in case of a copy process
     * @param origCon - original connection
     * @param copyCon - copied conneciton
     * @param orig - original task
     * @param copy - copied task
     * @return
     */
    GT_CORE_EXPORT bool updateConnectionProperties(
        GtPropertyConnection* origCon,
        GtPropertyConnection* copyCon,
        GtTask* orig, GtTask* copy);

    /**
     * @brief Updates all relative object links in a copied object based on
     * the original object
     * @param orig - original object
     * @param copy - copied object
     * @return
     */
    GT_CORE_EXPORT bool updateRelativeObjectLinks(GtObject* orig,
                                                  GtObject* copy);

    /**
     * @brief searches for an equivalent object in a new task
     * (after a copy process) in order to updated property connections
     * @param parent - copied task
     * @param origObj - otiginal object from original task
     * @return
     */
    GT_CORE_EXPORT GtObject* findEquivalentObject(GtObject* parent,
                                                  GtObject* origObj);

    /**
     * @brief returns the highest parent object of type GtTask of the
     * considered task
     * @param childTask
     * @return
     */
    GT_CORE_EXPORT GtTask* highestParentTask(GtTask *childTask);

    /**
     * @brief returns the highest parent object of type GtTask of the
     * considered calculator
     * @param childCalc
     * @return
     */
    GT_CORE_EXPORT GtTask* highestParentTask(const GtCalculator* childCalc);

    /**
     * @brief returns the highest parent object of type GtTask of the
     * considered process component
     * @param childComp - process component
     * @return
     */
    GT_CORE_EXPORT GtTask* highestParentTask(
            GtProcessComponent* childComp);

    /**
     * @brief Returns all property connections which will be kept in case
     * that the considered task will be cut/moved/copied
     * @param task
     * @return
     */
    GT_CORE_EXPORT QList<GtPropertyConnection*> internalPropertyConnections(
            GtTask* task);

    /**
     * @brief Returns all property connections which will be kept in case
     * that the considered process component will be cut/moved/copied
     * @param pComp - process component
     * @return
     */
    GT_CORE_EXPORT QList<GtPropertyConnection*> internalPropertyConnections(
            GtProcessComponent* pComp);

    /**
     * @brief Returns all property connections which will be lost in case
     * that the considered calculator will be cut/moved/copied
     * @param task
     * @return
     */
    GT_CORE_EXPORT QList<GtPropertyConnection*> lostPropertyConnections(
            GtTask* task);

    /**
     * @brief Returns all property connections which will be lost in case
     * that the considered calculator will be cut/moved/copied
     * @param calc - calculator
     * @return
     */
    GT_CORE_EXPORT QList<GtPropertyConnection*> lostPropertyConnections(
            GtCalculator* calc);

    /**
     * @brief Returns all property connections which will be lost in case
     * that the considered processcomponent
     * @param pComp - process component
     * @return
     */
    GT_CORE_EXPORT QList<GtPropertyConnection*> lostPropertyConnections(
            GtProcessComponent* pComp);

    /**
     * @brief Returns all property connections related to the considered task
     * @param task
     * @return
     */
    GT_CORE_EXPORT QList<GtPropertyConnection*> relatedPropertyConnections(
            GtTask* task);

    /**
     * @brief Returns all property connections related to the considered
     * calculator
     * @param calc - calculator
     * @return
     */
    GT_CORE_EXPORT QList<GtPropertyConnection*> relatedPropertyConnections(
            GtCalculator* calc);

    /**
     * @brief Returns all property connections related to the considered
     * process component
     * @param comp - calculator
     * @return
     */
    GT_CORE_EXPORT QList<GtPropertyConnection*> relatedPropertyConnections(
            GtProcessComponent* comp);

    /**
     * @brief Standardized output messages for lost property connections in
     * case of cut/copy/move of process components
     * @param lostCons - lost connections
     * @param highestParent
     */
    GT_CORE_EXPORT void  setOffLostConnectionWarnings(
            QList<GtPropertyConnection*> lostCons,
            GtTask* highestParent);

    /**
     * @brief Updates relative object link properties in a copied object based
     * on the information contained in the original object
     * @param orig - original parent object
     * @param copy - copied parent object
     * @param copyClone - clone of copied parent object
     * @param obj - parent object of considered property
     * @param relLink  - considered property
     * @return
     */
    bool mapRelativeObjectLink(GtObject* orig, GtObject* copy,
                               GtObject* copyClone, GtObject* obj,
                               GtRelativeObjectLinkProperty* relLink);
}

#endif // GTPROPERTYCONNECTIONFUNCTIONS_H
