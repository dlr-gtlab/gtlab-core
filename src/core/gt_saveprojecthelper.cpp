/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_saveprojecthelper.cpp
 *
 *  Created on: 06.08.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_saveprojecthelper.h"

#include "gt_project.h"

GtSaveProjectHelper::GtSaveProjectHelper(GtProject* proj) :
    m_project(proj), m_success(false)
{
}

void
GtSaveProjectHelper::run()
{
    if (!m_project)
    {
        return;
    }

    if (!m_project->isOpen())
    {
        return;
    }

    if (!m_project->saveProjectOverallData())
    {
        return;
    }

    if (!m_project->saveModuleData())
    {
        return;
    }

    // saving was successfull
    m_success = true;

    m_project->acceptChangesRecursively();
}

GtProject*
GtSaveProjectHelper::project()
{
    return m_project;
}

bool
GtSaveProjectHelper::success()
{
    return m_success;
}
