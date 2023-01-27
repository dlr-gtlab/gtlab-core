/* GTlab - Gas Turbine laboratory
 * Source File: gt_loadprojecthelper.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 14.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDomDocument>
#include <QThread>

#include "gt_project.h"

#include "gt_loadprojecthelper.h"

GtLoadProjectHelper::GtLoadProjectHelper(GtProject* proj) :
    m_project(proj)
{
}

void
GtLoadProjectHelper::run()
{
    if (!m_project)
    {
        return;
    }

    // module data
    GtObjectList moduleData = m_project->readModuleData();
    m_objects.append(moduleData);

    // process data
    m_objects.append(m_project->readProcessData());

    // label data
    m_objects.append(m_project->readLabelData(moduleData));

    foreach (GtObject* obj, m_objects)
    {
        gt::moveToThread(*obj, m_project->thread());
    }
}

const QList<GtObject*>&
GtLoadProjectHelper::loadedObjects()
{
    return m_objects;
}

GtProject*
GtLoadProjectHelper::project()
{
    return m_project;
}
