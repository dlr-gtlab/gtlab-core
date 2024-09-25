/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 */


#include "gt_maintoolbar.h"

#include <gt_application.h>
#include <gt_project.h>
#include <gt_markdowneditor.h>
#include <gt_mdilauncher.h>

GtMainToolbar::GtMainToolbar(GtMainWin *parent)
    : GtQmlToolbar(parent)
{
    auto tbSystemContext = new GtQmlToolbarGroup("System", this);

    homeAction
        = new GtQmlAction(tr("Home"), QUrl("/icons/home.svg"), this);

    newProjectAction
        = new GtQmlAction(tr("New Project"), QUrl("/icons/projectAdd.svg"),
                          this);
    openProjectAction
        = new GtQmlAction(tr("Open Project"), QUrl("/icons/folderOpen.svg"),
                          this);

    tbSystemContext->append(homeAction);
    tbSystemContext->append(newProjectAction);
    tbSystemContext->append(openProjectAction);

    addToolbarGroup(tbSystemContext);

    // --------------------------------------------------------------------- //

    projectContext = new GtQmlToolbarGroup("Project", this);
    projectContext->setVisible(false);

    saveProjectAction
        = new GtQmlAction(tr("Save Project"), QUrl("/icons/save.svg"), this);

    undoAction = new GtQmlAction(tr("Undo"), QUrl("/icons/undo.svg"), this);
    undoAction->setVisible(false);

    redoAction = new GtQmlAction(tr("Redo"), QUrl("/icons/redo.svg"), this);
    redoAction->setVisible(false);

    infoAction
        = new GtQmlAction(tr("Project Info"), QUrl("/icons/info2.svg"), this);


    projectContext->append(saveProjectAction);
    projectContext->append(undoAction);
    projectContext->append(redoAction);
    projectContext->append(infoAction);

    addToolbarGroup(projectContext);

    // --------------------------------------------------------------------- //

    editorContext = new GtQmlToolbarGroup("Edit", this);

    printAction = new GtQmlAction(tr("Print"), QUrl("/icons/print.svg"), this);
    printAction->setVisible(false);

    editorContext->append(printAction);

    addToolbarGroup(editorContext);


    setDarkmode(gtApp->inDarkMode());
    makeConnections(parent);

    resize(QSize(1500, 50));
}

void
GtMainToolbar::onProjectChanged(bool isOpen)
{
    projectContext->setVisible(isOpen);
    editorContext->setVisible(isOpen);
    infoAction->setVisible(projectHasInfo());
}

void
GtMainToolbar::showProjectInfo()
{
    // check project readme and show content
    GtProject* currentProject = gtApp->currentProject();

    if (!currentProject) return;

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

bool
GtMainToolbar::projectHasInfo()
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

void
GtMainToolbar::makeConnections(GtMainWin* mainwin)
{

    connect(undoAction, SIGNAL(triggered()),
            gtApp->undoStack(), SLOT(undo()));

    connect(gtApp->undoStack(), &QUndoStack::canUndoChanged, undoAction,
            [this](bool canUndo) {
                undoAction->setVisible(undoAction->visible() || canUndo);
                undoAction->setEnabled(canUndo);
            });

    connect(gtApp->undoStack(), &QUndoStack::undoTextChanged, undoAction,
            [this](const QString& txt) {
                undoAction->setTooltip(txt);
            });

    connect(redoAction, SIGNAL(triggered()),
            gtApp->undoStack(), SLOT(redo()));

    connect(infoAction, &GtQmlAction::triggered,
            this, &GtMainToolbar::showProjectInfo);

    connect(gtApp->undoStack(), &QUndoStack::canRedoChanged, redoAction,
            [this](bool canRedo) {
                redoAction->setVisible(redoAction->visible() || canRedo);
                redoAction->setEnabled(canRedo);
            });

    connect(gtApp->undoStack(), &QUndoStack::redoTextChanged, redoAction,
            [this](const QString& txt) {
                redoAction->setTooltip(txt);
            });

    connect(gtApp, &GtApplication::themeChanged,
            this, &GtMainToolbar::setDarkmode);

    if (mainwin)
    {
        connect(homeAction, &GtQmlAction::triggered, this, []() {
            gtMdiLauncher->open("GtStartupPage");
        });

        connect(newProjectAction, SIGNAL(triggered()), mainwin,
                SLOT(showProjectWizard()));

        connect(openProjectAction, SIGNAL(triggered()),
                mainwin, SLOT(importProject()));

        connect(saveProjectAction, SIGNAL(triggered()),
                SLOT(saveCurrentProject()));

        connect(mainwin, &GtMainWin::projectOpened,
                this, &GtMainToolbar::onProjectChanged);

        connect(mainwin, &GtMainWin::currentMdiItemPrintable,
                this->printAction, &GtQmlAction::setVisible);

        connect(printAction, SIGNAL(triggered()),
                mainwin, SLOT(printCurrentMdiItem()));
    }
}
