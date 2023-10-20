/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_statecontainer.cpp
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
    if (m_project)
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

    GtStateGroup* group = findGroup(groupId);

    if (!group)
    {
        return nullptr;
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
        return nullptr;
    }

    GtStateGroup* retval = findDirectChild<GtStateGroup*>(id);

    if (!retval)
    {
        retval = new GtStateGroup(this);
        retval->setObjectName(id);
    }

    return retval;
}
