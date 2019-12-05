/* GTlab - Gas Turbine laboratory
 * Source File: gt_statehandler.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>

#include "gt_statecontainer.h"
#include "gt_state.h"
#include "gt_project.h"

#include "gt_statehandler.h"

GtStateHandler::GtStateHandler(QObject* parent)
{
    setParent(parent);
    setObjectName(QStringLiteral("GTlab States"));

    m_globalStates = new GtStateContainer(this);
    m_globalStates->setObjectName(QStringLiteral("-- Global --"));
    m_globalStates->setDefault(true);
}

GtStateContainer*
GtStateHandler::findContainer(GtProject* project)
{
    if (project == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    GtStateContainer* retval =
            findDirectChild<GtStateContainer*>(project->objectName());

    if (retval == Q_NULLPTR)
    {
        retval = new GtStateContainer(this, project);
        retval->setObjectName(project->objectName());
    }

    return retval;
}

GtStateHandler*
GtStateHandler::instance()
{
    static GtStateHandler* retval = 0;
    if (retval == 0)
    {
        retval = new GtStateHandler(qApp);
    }
    return retval;
}

GtState*
GtStateHandler::initializeState(const QString& groupId, const QString& id,
                                const QString& path, const QVariant& initValue,
                                GtObject* guardian, bool forceInitialization)
{
    return m_globalStates->initializeState(groupId, id, path, initValue,
                                           guardian, forceInitialization);
}

GtState*
GtStateHandler::initializeState(GtProject* project, const QString& groupId,
                                const QString& id, const QString& path,
                                const QVariant& initValue, GtObject* guardian,
                                bool forceInitialization)
{
    if (project == Q_NULLPTR)
    {
        return initializeState(groupId, id, path, initValue, guardian,
                               forceInitialization);
    }

    if (guardian == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    if (groupId.isEmpty())
    {
        return Q_NULLPTR;
    }

    if (id.isEmpty())
    {
        return Q_NULLPTR;
    }

    if (path.isEmpty())
    {
        return Q_NULLPTR;
    }

    GtStateContainer* container = findContainer(project);

    if (container == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    return container->initializeState(groupId, id, path, initValue, guardian,
                                      forceInitialization);
}
