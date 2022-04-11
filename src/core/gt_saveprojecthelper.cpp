/* GTlab - Gas Turbine laboratory
 * Source File: gt_saveprojecthelper.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 06.08.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_saveprojecthelper.h"

#include "gt_project.h"
#include "gt_logging.h"
#include "gt_externalizationsettings.h"

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

    // force externalization or internalization
    if (gtExternalizationSettings->isExternalizationEnabled() &&
        gtExternalizationSettings->autoExternalizeOnSave())
    {
        // externalizes every child if its still fetched
        if (!m_project->externalizeAllChildren())
        {
            gtWarning() << "could not externalize all data!";
        }
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
