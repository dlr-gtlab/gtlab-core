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

#include <gt_icons.h>

GtMainToolbar::GtMainToolbar(GtMainWin *parent)
    : GtQmlToolbar(parent)
{
    using namespace gt::gui;

    auto tbSystemContext = new GtQmlToolbarGroup(tr("System"), this);

    homeAction
        = new GtQmlAction(tr("Home"), icon::url(icon::home), this);

    newProjectAction
        = new GtQmlAction(tr("New Project"), icon::url(icon::projectAdd),
                          this);
    openProjectAction
        = new GtQmlAction(tr("Open Project"), icon::url(icon::folderOpen),
                          this);

    tbSystemContext->append(homeAction);
    tbSystemContext->append(newProjectAction);
    tbSystemContext->append(openProjectAction);

    addToolbarGroup(tbSystemContext);

    // --------------------------------------------------------------------- //

    projectContext = new GtQmlToolbarGroup(tr("Project"), this);
    projectContext->setVisible(false);

    saveProjectAction
        = new GtQmlAction(tr("Save Project"), icon::url(icon::save), this);

    undoAction = new GtQmlAction(tr("Undo"), icon::url(icon::undo), this);
    undoAction->setVisible(false);

    redoAction = new GtQmlAction(tr("Redo"), icon::url(icon::redo), this);
    redoAction->setVisible(false);

    infoAction
        = new GtQmlAction(tr("Project Info"), icon::url(icon::info2), this);


    projectContext->append(saveProjectAction);
    projectContext->append(undoAction);
    projectContext->append(redoAction);
    projectContext->append(infoAction);

    addToolbarGroup(projectContext);

    // --------------------------------------------------------------------- //

    editorContext = new GtQmlToolbarGroup(tr("Edit"), this);

    printAction = new GtQmlAction(tr("Print"), icon::url(icon::print), this);
    printAction->setVisible(false);

    editorContext->append(printAction);

    addToolbarGroup(editorContext);


    setDarkMode(gtApp->inDarkMode());
    makeConnections(parent);

    resize(QSize(1500, 50));
}

void
GtMainToolbar::setEditorActions(std::vector<GtQmlAction *> actions)
{
    actions.push_back(printAction);
    editorContext->setListData(std::move(actions));
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
                undoAction->setVisible(undoAction->isVisible() || canUndo);
                undoAction->setEnabled(canUndo);
            });

    connect(gtApp->undoStack(), &QUndoStack::undoTextChanged, undoAction,
            [this](const QString& txt) {
                undoAction->setToolTip(txt);
            });

    connect(redoAction, SIGNAL(triggered()),
            gtApp->undoStack(), SLOT(redo()));

    connect(infoAction, &GtQmlAction::triggered,
            this, &GtMainToolbar::showProjectInfo);

    connect(gtApp->undoStack(), &QUndoStack::canRedoChanged, redoAction,
            [this](bool canRedo) {
                redoAction->setVisible(redoAction->isVisible() || canRedo);
                redoAction->setEnabled(canRedo);
            });

    connect(gtApp->undoStack(), &QUndoStack::redoTextChanged, redoAction,
            [this](const QString& txt) {
                redoAction->setToolTip(txt);
            });

    connect(gtApp, &GtApplication::themeChanged,
            this, &GtMainToolbar::setDarkMode);

    if (mainwin)
    {
        connect(homeAction, &GtQmlAction::triggered, this, []() {
            gtMdiLauncher->open("GtStartupPage");
        });

        connect(newProjectAction, SIGNAL(triggered()), mainwin,
                SLOT(showProjectWizard()));

        connect(openProjectAction, SIGNAL(triggered()),
                mainwin, SLOT(importProject()));

        connect(saveProjectAction, SIGNAL(triggered()), mainwin,
                SLOT(saveCurrentProject()));

        connect(mainwin, &GtMainWin::projectOpened,
                this, &GtMainToolbar::onProjectChanged);

        connect(mainwin, &GtMainWin::currentMdiItemPrintable,
                this->printAction, &GtQmlAction::setVisible);

        connect(printAction, SIGNAL(triggered()),
                mainwin, SLOT(printCurrentMdiItem()));
    }
}
