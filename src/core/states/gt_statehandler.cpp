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
    if (!project)
    {
        return nullptr;
    }

    GtStateContainer* retval =
            findDirectChild<GtStateContainer*>(project->objectName());

    if (!retval)
    {
        retval = new GtStateContainer(this, project);
        retval->setObjectName(project->objectName());
    }

    return retval;
}

GtStateHandler*
GtStateHandler::instance()
{
    static GtStateHandler* retval = nullptr;
    if (!retval)
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
    if (!project)
    {
        return initializeState(groupId, id, path, initValue, guardian,
                               forceInitialization);
    }

    if (!guardian)
    {
        return nullptr;
    }

    if (groupId.isEmpty())
    {
        return nullptr;
    }

    if (id.isEmpty())
    {
        return nullptr;
    }

    if (path.isEmpty())
    {
        return nullptr;
    }

    GtStateContainer* container = findContainer(project);

    if (!container)
    {
        return nullptr;
    }

    return container->initializeState(groupId, id, path, initValue, guardian,
                                      forceInitialization);
}
