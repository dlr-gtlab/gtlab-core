/* GTlab - Gas Turbine laboratory
 * Source File: gt_statecontainer.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_project.h"
#include "gt_statehandler.h"
#include "gt_stategroup.h"
#include "gt_state.h"

#include "gt_statecontainer.h"

GtStateContainer::GtStateContainer(GtObject* parent, GtProject* project) :
    GtObject(parent),
    m_project(project)
{
    if (m_project != Q_NULLPTR)
    {
        setObjectName(m_project->objectName());
        connect(project, SIGNAL(destroyed(QObject*)), SLOT(deleteLater()));
    }
    else
    {
        setObjectName(QStringLiteral("Global States"));
    }
}

GtProject*
GtStateContainer::project()
{
    return m_project;
}

GtState*
GtStateContainer::initializeState(const QString& groupId, const QString& id,
                                  const QString& path,
                                  const QVariant& initValue,
                                  GtObject* guardian, bool forceInitialization)
{
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

    GtStateGroup* group = findGroup(groupId);

    if (group == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    GtState* state = group->findState(id, path, initValue, guardian);

    if (forceInitialization)
    {
        state->setValue(initValue, false);
    }

    return state;
}

GtStateGroup*
GtStateContainer::findGroup(const QString& id)
{
    if (id.isEmpty())
    {
        return Q_NULLPTR;
    }

    GtStateGroup* retval = findDirectChild<GtStateGroup*>(id);

    if (retval == Q_NULLPTR)
    {
        retval = new GtStateGroup(this);
        retval->setObjectName(id);
    }

    return retval;
}
