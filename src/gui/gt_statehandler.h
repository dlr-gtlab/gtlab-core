/* GTlab - Gas Turbine laboratory
 * Source File: gt_statehandler.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSTATEHANDLER_H
#define GTSTATEHANDLER_H

#include "gt_gui_exports.h"

#include "gt_object.h"

class GtState;
class GtStateContainer;
class GtProject;

#define gtStateHandler (GtStateHandler::instance())

/**
 * @brief The GtStateHandler class
 */
class GT_GUI_EXPORT GtStateHandler : public GtObject
{
    Q_OBJECT

public:
    /**
     * @brief Returns singleton of state handler.
     * @return State handler instance.
     */
    static GtStateHandler* instance();

    /**
     * @brief Initializes a new global state.
     * @param Group identification string.
     * @param State identification string.
     * @param State specific path.
     * @param Initial value.
     * @param State guardian.
     * @param Force initialization indicator.
     * @return New state.
     */
    GtState* initializeState(const QString& groupId, const QString& id,
                             const QString& path, const QVariant& initValue,
                             GtObject* guardian,
                             bool forceInitialization = false);

    /**
     * @brief Initializes a new project specific state.
     * @param Project.
     * @param Group identification string.
     * @param State identification string.
     * @param State specific path.
     * @param Initial value.
     * @param State guardian.
     * @param Force initialization indicator.
     * @return New state.
     */
    GtState* initializeState(GtProject* project, const QString& groupId,
                             const QString& id, const QString& path,
                             const QVariant& initValue, GtObject* guardian,
                             bool forceInitialization = false);

private:
    /// Global states.
    GtStateContainer* m_globalStates;

    /**
     * @brief Constructor.
     * @param Parent object.
     */
    GtStateHandler(QObject* parent = NULL);

    /**
     * @brief findGroup
     * @param id
     * @return
     */
    GtStateContainer* findContainer(GtProject* project);

};

#endif // GTSTATEHANDLER_H
