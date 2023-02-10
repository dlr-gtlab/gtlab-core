/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2012 by DLR
 *
 *  Created on: 16.09.2022
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include <QDir>
#include <QFile>

#include "gt_logging.h"
#include "gt_datamodel.h"
#include "gt_application.h"
#include "gt_mdilauncher.h"
#include "gt_projectprovider.h"
#include "gt_projectwizard.h"
#include "gt_project.h"
#include "gt_projectui.h"
#include "gt_markdowneditor.h"

#include "gt_toolbarhandler.h"

GtToolbarHandler::GtToolbarHandler()
{

}

void
GtToolbarHandler::buttonClicked(const QString &btnId)
{
    if (btnId == "btn_save_project")
    {
        emit saveProjectButtonClicked();
    }
    else if (btnId == "btnNewProject")
    {
        emit newProjectButtonClicked();
    }
    else if (btnId == "btnOpenProject")
    {
        emit openProjectButtonClicked();
    }
    else if (btnId == "btn_undo")
    {
        emit undoButtonClicked();
    }
    else if (btnId == "btn_redo")
    {
        emit redoButtonClicked();
    }
    else if (btnId == "btn_info")
    {
        // check project readme and show content
        GtProject* currentProject = gtApp->currentProject();

        if (currentProject)
        {
            QDir projectDir(currentProject->path());
            QFile readmeFile(projectDir.absoluteFilePath("README.md"));
            if (readmeFile.exists() && readmeFile.open(QFile::ReadOnly))
            {
                QByteArray data = readmeFile.readAll();
                QString str(data);

                GtMarkdownEditor* editor = new GtMarkdownEditor(str);
                editor->setWindowTitle(currentProject->objectName());
                editor->show();
            }


        }
    }
    else if (btnId == "btnHome")
    {
        gtMdiLauncher->open("GtStartupPage");
    }
    else if (btnId == "btnPrint")
    {
       gtMdiLauncher->printCurrentWindow();
    }
}

void
GtToolbarHandler::onObjectSelected(GtObject* obj)
{
    if (obj != m_selectedObj)
    {
        m_selectedObj = obj;
    }
}

bool
GtToolbarHandler::projectHasInfo()
{
    // check project readme and show content
    GtProject* currentProject = gtApp->currentProject();

    if (!currentProject)
    {
        return false;
    }

    QDir projectDir(currentProject->path());
    QFile readmeFile(projectDir.absoluteFilePath("README.md"));

    return readmeFile.exists();
}
