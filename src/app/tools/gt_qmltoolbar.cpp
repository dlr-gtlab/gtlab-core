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
#include <QQmlContext>
#include <QAction>

#include "gt_logging.h"
#include "gt_application.h"
#include "gt_mdilauncher.h"
#include "gt_project.h"
#include "gt_markdowneditor.h"

#include "gt_qmltoolbar.h"

#include "gt_mainwin.h"
#include "gt_icons.h"

GtQmlToolbar::GtQmlToolbar(GtMainWin* parent)
    : QQuickWidget(parent)
    , m_customActions(new GtQmlObjectListModel(this))
{
    setObjectName("MainWindowToolbar");

    rootContext()->setContextProperty("mainwin",
                                      parent);
    rootContext()->setContextProperty("gtapp",
                                                  gtApp);
    rootContext()->setContextProperty("undostack",
                                                  gtApp->undoStack());
    rootContext()->setContextProperty("toolbar", this);

    rootContext()->setContextProperty("customActions", m_customActions);

    setSource(QUrl("qrc:/qml/toolbar.qml"));
}

void
GtQmlToolbar::buttonClicked(const QString &btnId)
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
}

void
GtQmlToolbar::onObjectSelected(GtObject* obj)
{
    if (obj != m_selectedObj)
    {
        m_selectedObj = obj;
    }
}

bool
GtQmlToolbar::projectHasInfo()
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

GtQmlAction*
GtQmlToolbar::addCustomButton(const QString &text, const QUrl &iconUrl)
{
    auto action = new GtQmlAction(text, iconUrl, this);
    m_customActions->addObject(action);
    return action;
}
