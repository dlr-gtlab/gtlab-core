/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QFont>
#include <QVariant>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListView>
#include <QCheckBox>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QTreeWidget>

#include "gt_project.h"
#include "gt_projectprovider.h"
#include "gt_datamodel.h"
#include "gt_checkablestringlistmodel.h"
#include "gt_application.h"
#include "gt_processdata.h"
#include "gt_task.h"
#include "gt_filedialog.h"
#include "gt_package.h"
#include "gt_logging.h"
#include "gt_projectspecwidget.h"
#include "gt_label.h"
#include "gt_abstractloadinghelper.h"
#include "gt_processexecutor.h"
#include "gt_confirmdeleteprojectdialog.h"
#include "gt_saveprojectmessagebox.h"
#include "gt_switchprojectmessagebox.h"
#include "gt_regexp.h"
#include "gt_inputdialog.h"
#include "gt_footprint.h"
#include "gt_versionnumber.h"
#include "gt_icons.h"
#include "gt_projectsettingsdialog.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"
#include "gt_objectfactory.h"
#include "gt_algorithms.h"
#include "gt_colors.h"
#include "gt_projectupgradedialog.h"

#include "gt_projectui.h"

GtProjectUI::GtProjectUI()
{
    addSingleAction(tr("Upgrade Project Data..."),
                    QStringLiteral("upgradeProjectDataIcon.png"),
                    QStringLiteral("upgradeProjectData"),
                    QStringLiteral("canUpgradeProjectData"),
                    QStringLiteral("canUpgradeProjectData"));

    addSingleAction(tr("Set as active Project"),
                    QStringLiteral("emptyIcon_16.png"),
                    QStringLiteral("setCurrentProject"),
                    QStringLiteral("canSetCurrentProject"),
                    QStringLiteral("canSetCurrentProject"));

    addSingleAction(tr("Open Project"),
                    QStringLiteral("openProjectIcon_16.png"),
                    QStringLiteral("openProject"),
                    QStringLiteral("canOpenProject"),
                    gtApp->getShortCutSequence("openProject"));

    addSingleAction(tr("Close Project"),
                    QStringLiteral("closeProjectIcon_16.png"),
                    QStringLiteral("closeProject"),
                    QStringLiteral("canCloseProject"),
                    gtApp->getShortCutSequence("closeProject"));

    addSingleAction(tr("Save Project"),
                    QStringLiteral("saveProjectIcon_16.png"),
                    QStringLiteral("saveProject"),
                    QStringLiteral("canSaveProject"),
                    gtApp->getShortCutSequence("save"));

    addSingleAction(tr("Save Project As..."),
                    QStringLiteral("saveProjectIcon_16.png"),
                    QStringLiteral("saveProjectAs"),
                    QStringLiteral("canSaveProjectAs"));

    addSeparator();

    if (gtApp->devMode())
    {
        addSingleAction(tr("Duplicate Project..."),
                        QStringLiteral("duplicateIcon_16.png"),
                        QStringLiteral("duplicateProject"),
                        QStringLiteral("canDuplicateProject"));

        addSeparator();
    }

    if (gtApp->devMode())
    {
        //        addAction(tr("Test Commit"),
        //                  QStringLiteral("jumpToIcon.png"),
        //                  QStringLiteral("testCommit"),
        //                  QStringLiteral("canTestCommit"));


        //          ATTENTION: former Icon "arrowRightIconBlue"
        //                     is not in resources anymore!

        //        addAction(tr("Test Checkout"),
        //                  QStringLiteral("arrowRightIconBlue.png"),
        //                  QStringLiteral("testCheckout"),
        //                  QStringLiteral("canTestCheckout"));

        addSingleAction(tr("Enable Version Control"),
                        QStringLiteral("checkSmallIcon_16.png"),
                        QStringLiteral("enableVersionControl"),
                        QStringLiteral("canEnableVersionControl"),
                        QStringLiteral("canEnableVersionControl"));

        addSeparator();


        // init version control, quick and dirty for now
        //        GtProjectDiffPatch * differ = new GtProjectDiffPatch;
        //        GtdVCSDBInterface * vcInterface = new GtdVCSDBInterface;
        //        vc = new GtdVersionControlCore(vcInterface, differ);
    }

    addSingleAction(tr("Open Project Settings..."),
                    QStringLiteral("processIcon_16.png"),
                    QStringLiteral("openProjectSettings"),
                    QStringLiteral("canSaveProject"));

    addSingleAction(tr("Show Project Footprint"),
                    QStringLiteral("infoBlueIcon_16.png"),
                    QStringLiteral("showFootprint"),
                    gtApp->getShortCutSequence("ShowFootprint"));

    addSingleAction(tr("Edit Comment"),
                    QStringLiteral("commentIcon.png"),
                    QStringLiteral("editComment"),
                    QStringLiteral("canEditComment"));

    addSeparator();

    addSingleAction(tr("Show in Explorer"),
                    QStringLiteral("folder_16.png"),
                    QStringLiteral("showInExplorer"));

    addSingleAction(tr("Rename Project"),
                    QStringLiteral("inputIcon_16.png"),
                    QStringLiteral("renameProject"),
                    QStringLiteral("canRenameProject"),
                    gtApp->getShortCutSequence("rename"));

    addSeparator();

    addSingleAction(tr("Choose Project Modules"),
                    QStringLiteral("pluginIcon_16.png"),
                    QStringLiteral("chooseProjectModule"),
                    QStringLiteral("canChooseProjectModule"));

    addSingleAction(tr("Export Meta Data"),
                    QStringLiteral("exportIcon_16.png"),
                    QStringLiteral("exportMetaData"),
                    QStringLiteral("canExportMetaData"),
                    QStringLiteral("canExportMetaData"));

    addSingleAction(tr("Delete from Session"),
                    QStringLiteral("closeIcon_16.png"),
                    QStringLiteral("deleteProject"),
                    QStringLiteral("canDeleteProject"));
}

QIcon
GtProjectUI::icon(GtObject* obj) const
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return QIcon();
    }

    if (project->isOpen())
    {
        return GtGUI::Icon::components16();
    }

    if (project->upgradesAvailable())
    {
        return GtGUI::Icon::inconsistentProject16();
    }

    return GtGUI::Icon::closedProject16();

}

void
GtProjectUI::doubleClicked(GtObject* obj)
{
    openProject(obj);
}

QVariant
GtProjectUI::specificData(GtObject* obj, int role, int column) const
{
    if (!obj)
    {
        return QVariant();
    }

    if (column == 0)
    {
        switch (role)
        {
            case Qt::FontRole:
            {
                if (obj == gtApp->currentProject())
                {
                    QFont font;
                    font.setBold(true);
                    return font;
                }

                break;
            }
            case Qt::ToolTipRole:
            {
                auto project = qobject_cast<GtProject*>(obj);

                if (project)
                {
                    if (project->upgradesAvailable())
                    {
                        return tr("Some of the project data is outdated and "
                              "requires an upgrade before the project can "
                              "be opened.");
                    }
                }

                break;
            }
        }
    }
    else if (column == 1)
    {
        switch (role)
        {
        case Qt::DecorationRole:
        {
            auto project = qobject_cast<GtProject*>(obj);

            if (project)
            {
                if (!project->comment().isEmpty())
                {
                    return GtGUI::Icon::comment();
                }
            }

            break;
        }
        case Qt::ToolTipRole:
        {
            auto project = qobject_cast<GtProject*>(obj);

            if (project)
            {
                if (!project->comment().isEmpty())
                {
                    return project->comment();
                }
            }

            break;
        }
        }
    }

    return QVariant();
}

bool GtProjectUI::saveAndCloseCurrentProject()
{
    if (gtApp->hasProjectChanges())
    {

        QString text = tr("Found changes in current project.\n"
                          "Do you want to save all your changes "
                          "before opening new project?");

        GtSaveProjectMessageBox mb(text);
        int ret = mb.exec();

        if (ret == QMessageBox::Yes)
        {
            gtDataModel->saveProject(gtApp->currentProject());
        }
        else if (ret == QMessageBox::Cancel)
        {
            return false;
        }
    }

    gtDataModel->closeProject(gtApp->currentProject());
    return true;
}


void GtProjectUI::switchToProject(GtProject& toProject)
{
    if (&toProject == gtApp->currentProject())
    {
        return;
    }


    if (gtApp->currentProject() && !gtApp->hasProjectChanges())
    {
        GtSwitchProjectMessageBox mb;
        if (mb.exec() == QMessageBox::Cancel)
        {
            return;
        }
    }

    if (!saveAndCloseCurrentProject())
    {
        return;
    }

    gtDataModel->openProject(&toProject);
}

void
GtProjectUI::openProject(GtObject* obj)
{
    if (gtProcessExecutor->taskCurrentlyRunning())
    {
        QMessageBox mb;
        mb.setIcon(QMessageBox::Information);
        mb.setWindowTitle(tr("Open Project"));
        mb.setWindowIcon(GtGUI::Icon::openProject16());
        mb.setText(tr("Cannot open new project while a task is running."));
        mb.setStandardButtons(QMessageBox::Ok);
        mb.setDefaultButton(QMessageBox::Ok);
        mb.exec();
        return;
    }

    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    if (project->isOpen())
    {
        setCurrentProject(project);
        return;
    }

    switchToProject(*project);
    //    if (gtApp->devMode())
    //    {
    //    // open version control data
    //    if (!vc->dbInterface()->open(project->path(), project->uuid()))
    //    {
    //        qDebug() << "Cannot open project database";
    //        return;
    //    }

    //    if (!vc->dbInterface()->isInitialized())
    //    {
    //        vc->dbInterface()->createInitialLayout();
    //        vc->dbInterface()->addBranch("trunk", -1);
    //    }
    //    }
}

bool
GtProjectUI::canOpenProject(GtObject* obj)
{
    if (gtProcessExecutor->taskCurrentlyRunning())
    {
        return false;
    }

    GtProject* project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    if (project->upgradesAvailable())
    {
        return false;
    }

    return !project->isOpen();
}

void
GtProjectUI::closeProject(GtObject* obj)
{
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    if (!project->isOpen())
    {
        return;
    }

    if (gtApp->hasProjectChanges())
    {
        QString text = tr("Found unsaved project.\nDo you want to ") +
                       tr("save all your changes before closing project?");

        GtSaveProjectMessageBox mb(text);
        int ret = mb.exec();

        switch (ret)
        {
            case QMessageBox::Yes:
            {
                gtDataModel->saveProject(gtApp->currentProject());
                break;
            }

            case QMessageBox::No:
            {
                break;
            }

            case QMessageBox::Cancel:
            {
                return;
            }

            default:
                break;
        }
    }

    gtDataModel->closeProject(gtApp->currentProject());

}

bool
GtProjectUI::canCloseProject(GtObject* obj)
{
    if (gtProcessExecutor->taskCurrentlyRunning())
    {
        return false;
    }

    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    return project->isOpen();
}

void
GtProjectUI::saveProject(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    if (!project->isOpen())
    {
        return;
    }

    gtDataModel->saveProject(project);
}

bool
GtProjectUI::canSaveProject(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    return project->isOpen();
}

void
GtProjectUI::saveProjectAs(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    if (!project->isOpen())
    {
        return;
    }

    QString id = gtDataModel->uniqueObjectName(project->objectName(),
                 gtApp->session());

    QDialog dialog;

    dialog.setWindowTitle(tr("Save Project As..."));
    dialog.setWindowIcon(GtGUI::Icon::saveProject16());

    Qt::WindowFlags flags = dialog.windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    dialog.setWindowFlags(flags);

    auto layout = new QVBoxLayout;

    auto pspecs = new GtProjectSpecWidget;
    layout->addWidget(pspecs);

    QDir projectDir(project->path());
    projectDir.cdUp();

    pspecs->setProjectInformation(id, projectDir.absolutePath());

    layout->addStretch(1);

    auto btnLay = new QHBoxLayout;

    auto acceptBtn = new QPushButton(tr("Save"));
    acceptBtn->setIcon(GtGUI::Icon::saveProject16());

    connect(acceptBtn, SIGNAL(clicked(bool)), &dialog, SLOT(accept()));

    auto cancleBtn = new QPushButton(tr("Cancel"));
    cancleBtn->setIcon(GtGUI::Icon::delete16());

    connect(cancleBtn, SIGNAL(clicked(bool)), &dialog, SLOT(reject()));
    connect(pspecs, SIGNAL(validated(bool)), acceptBtn, SLOT(setEnabled(bool)));

    btnLay->addStretch(1);
    btnLay->addWidget(acceptBtn);
    btnLay->addWidget(cancleBtn);

    layout->addLayout(btnLay);

    dialog.setLayout(layout);

    dialog.resize(400, 170);

    if (dialog.exec())
    {
        QDir pathNew(pspecs->pathLine()->text());

        qDebug() << "pathNew = " << pathNew.absolutePath();

        if (pathNew.exists())
        {
            gtError() << tr("Could not create new project directory!") <<
                      QStringLiteral(" ") <<
                      tr("Path already exists!");
            return;
        }

        if (!pathNew.mkpath(pathNew.absolutePath()))
        {
            gtError() << tr("Could not create new project directory!");
            return;
        }

        GtProjectProvider provider(project);

        GtProject* newProject =
            provider.duplicateProject(pspecs->nameLine()->text(),
                                      pathNew.absolutePath());

        if (!newProject)
        {
            gtError() << tr("Could not duplicate project!");
            return;
        }

        gtDataModel->newProject(newProject);
        gtDataModel->saveProject(newProject);
        gtDataModel->closeProject(newProject);
        gtDataModel->closeProject(project);
        gtDataModel->openProject(newProject);
        gtApp->setCurrentProject(newProject);
    }

    //    gtDataModel->saveProject(project);
}

bool
GtProjectUI::canSaveProjectAs(GtObject* obj)
{
    if (gtProcessExecutor->taskCurrentlyRunning())
    {
        return false;
    }


    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    return project->isOpen();
}

void
GtProjectUI::duplicateProject(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    if (!project->isOpen())
    {
        return;
    }

    bool ok = true;

    QString id = gtDataModel->uniqueObjectName(project->objectName(),
                 gtApp->session());

    QString text = QInputDialog::getText(nullptr,
                                         tr("New Project Name"),
                                         tr("Project name:"),
                                         QLineEdit::Normal,
                                         id, &ok);

    if (ok && !text.isEmpty())
    {
        QDir pathOrig(project->path());

        qDebug() << "pathOrig = " << pathOrig.absolutePath();

        if (!pathOrig.cdUp())
        {
            gtError() << tr("Could not create new project directory!");
            return;
        }

        QDir pathNew(pathOrig.absoluteFilePath(text));

        qDebug() << "pathNew = " << pathNew.absolutePath();

        if (pathNew.exists())
        {
            gtError() << tr("Could not create new project directory!") <<
                      QStringLiteral(" ") <<
                      tr("Path already exists!");
            return;
        }

        if (!pathOrig.mkpath(pathNew.absolutePath()))
        {
            gtError() << tr("Could not create new project directory!");
            return;
        }

        GtProjectProvider provider(project);

        GtProject* newProject =
            provider.duplicateProject(text, pathNew.absolutePath());

        if (!newProject)
        {
            gtError() << tr("Could not duplicate project!");
            return;
        }

        gtDataModel->newProject(newProject);
        gtDataModel->saveProject(newProject);
        gtDataModel->closeProject(newProject);
        gtDataModel->closeProject(project);
        gtDataModel->openProject(newProject);
        gtApp->setCurrentProject(newProject);
    }
}

bool
GtProjectUI::canDuplicateProject(GtObject* obj)
{
    if (gtProcessExecutor->taskCurrentlyRunning())
    {
        return false;
    }


    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    return project->isOpen();
}

void
GtProjectUI::deleteProject(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    if (project->isOpen())
    {
        return;
    }

    GtConfirmDeleteProjectDialog confirmationDialog(
                nullptr, project->objectName());

    if (confirmationDialog.exec())
    {
        if (confirmationDialog.result() == QDialog::Accepted
                && confirmationDialog.deleteFromHardDisk())
        {
            //gtDebug() << "Delete from HardDisk";
            QString projectPath = project->path();
            gtDataModel->deleteProject(project);

            QDir dir(projectPath);

            if (dir.removeRecursively())
            {
                gtInfo() << tr("Project has been deleted "
                               "from hard-disk successfully");
            }
            else
            {
                gtWarning() << tr("Project could not been deleted "
                                  "from hard-disk!");
            }
        }
        else if (confirmationDialog.result() == QDialog::Accepted)
        {
            //gtDebug() << "Do not delete from HardDisk";
            gtDataModel->deleteProject(project);
        }
    }
}

bool
GtProjectUI::canDeleteProject(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    return !project->isOpen();
}

void
GtProjectUI::setCurrentProject(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    if (!project->isOpen())
    {
        return;
    }

    GtProject* lastProject = gtApp->currentProject();

    if (lastProject != project)
    {
        gtApp->setCurrentProject(project);

        // update object entries
        gtDataModel->updateObject(project);

        if (lastProject)
        {
            gtDataModel->updateObject(lastProject);
        }
    }
}

bool
GtProjectUI::canSetCurrentProject(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    if (!project->isOpen())
    {
        return false;
    }

    if (gtApp->currentProject() == project)
    {
        return false;
    }

    return true;
}

void
GtProjectUI::chooseProjectModule(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    if (!project->isOpen())
    {
        return;
    }

    QDialog dialog;

    dialog.setWindowTitle(tr("Project Modules") +
                          QStringLiteral(" - ") + project->objectName());
    dialog.setWindowIcon(GtGUI::Icon::plugin16());

    Qt::WindowFlags flags = dialog.windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    dialog.setWindowFlags(flags);


    auto layout = new QVBoxLayout;

    auto label = new QLabel(tr("Select project specific modules"));
    layout->addWidget(label);

    auto model = new GtCheckableStringListModel(
        gtApp->moduleDatamodelInterfaceIds(), &dialog);

    model->selectItems(project->moduleIds());

    auto view = new QListView;
    view->setModel(model);
    view->setFrameStyle(QListView::NoFrame);
    layout->addWidget(view);

    layout->addStretch(1);



    auto okButton = new QPushButton(tr("Ok"));
    okButton->setIcon(GtGUI::Icon::check16());

    connect(okButton, SIGNAL(clicked(bool)), &dialog, SLOT(accept()));
    connect(model, SIGNAL(noItemSelected(bool)), okButton,
            SLOT(setDisabled(bool)));

    layout->addWidget(okButton);

    dialog.setLayout(layout);

    if (dialog.exec())
    {
        QStringList tmpAdd = model->selectedStringList();
        QStringList tmpRemove;

        foreach (const QString& str, project->moduleIds())
        {
            if (!tmpAdd.removeOne(str))
            {
                tmpRemove << str;
            }
        }

        qDebug() << "added modules = " << tmpAdd;
        qDebug() << "removed modules = " << tmpRemove;

        if (!tmpAdd.isEmpty() || !tmpRemove.isEmpty())
        {
            QMessageBox mb;
            mb.setIcon(QMessageBox::Question);
            mb.setWindowTitle(tr("Save Project"));
            mb.setWindowIcon(GtGUI::Icon::saveProject16());
            mb.setText(tr("Found unsaved changes.\nDo you want to save ")
                       + tr("your changes?"));
            mb.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);

            mb.setDefaultButton(QMessageBox::Cancel);
            int ret = mb.exec();

            switch (ret)
            {
                case QMessageBox::Yes:
                {
                    qDebug() << "changing project...";

                    gtDataModel->saveProject(project);
                    gtDataModel->closeProject(project);

                    QStringList modIds = project->moduleIds();
                    modIds.append(tmpAdd);

                    foreach (const QString& str, tmpRemove)
                    {
                        modIds.removeOne(str);
                    }

                    GtProjectProvider provider(project);

                    provider.setProjectModules(modIds);

                    if (provider.apply())
                    {
                        gtDataModel->openProject(project);
                    }
                    else
                    {
                        gtError() << tr("could not apply changes!");
                    }

                    break;
                }

                default:
                    break;
            }
        }
    }
}

bool
GtProjectUI::canChooseProjectModule(GtObject* obj)
{
    if (gtProcessExecutor->taskCurrentlyRunning())
    {
        return false;
    }


    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    if (!project->isOpen())
    {
        return false;
    }

    return true;
}

void
GtProjectUI::testCommit(GtObject* /*obj*/)
{
    //    GtProject* project = qobject_cast<GtProject*>(obj);

    //    if (!project)
    //    {
    //        return;
    //    }

    //    GtObjectMemento memento = project->toProjectDataMemento();

    //    // commit functionality
    //    static int commitId = 1;
    //    QString commitMsg = QString("My commit %1").arg(commitId);
    //    vc->commit("trunk", commitMsg, "Martin Siggel", memento.toString(), commitId);

    //    QString resultStr;
    //    vc->checkout(commitId, resultStr);

    //    QFile file(QString("commit%1.xml").arg(commitId));


    //    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    //    {

    //        QTextStream stream(&file);
    //        stream << resultStr;
    //    }

}

bool
GtProjectUI::canTestCommit(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    if (!project->isOpen())
    {
        return false;
    }

    return true;
}

void
GtProjectUI::testCheckout(GtObject* /*obj*/)
{
    //    GtProject* project = qobject_cast<GtProject*>(obj);

    //    if (!project)
    //    {
    //        return;
    //    }

    //    int rev = QInputDialog::getInt(0, "Revision", "Revision Number:", 1);

    //    QString resultString;
    //    vc->checkout(rev, resultString);

    //    GtObjectMemento memento;
    //    memento.setContent(resultString.toUtf8(), false);
    //    project->fromProjectDataMemento(memento);

}

bool
GtProjectUI::canTestCheckout(GtObject* obj)
{
    return canTestCommit(obj);
}

void
GtProjectUI::enableVersionControl(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }
}

bool
GtProjectUI::canEnableVersionControl(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    if (!project->isOpen())
    {
        return false;
    }

    return false;
}

void
GtProjectUI::exportMetaData(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    GtProcessData* pd =  project->processData();

    if (!pd)
    {
        return;
    }

    QList<GtTask*> tasks = pd->findDirectChildren<GtTask*>();

    if (tasks.isEmpty())
    {
        gtWarning() << tr("no tasks found!");
        return;
    }

    QStringList taskNames;

    foreach (GtTask* task, tasks)
    {
        taskNames << task->objectName();
    }

    QDialog dialog;
    dialog.setWindowTitle(tr("Export Meta Data"));
    dialog.setWindowIcon(GtGUI::Icon::export16());

    Qt::WindowFlags flags = dialog.windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    dialog.setWindowFlags(flags);


    GtCheckableStringListModel model(taskNames);
    model.setSelectionMode(GtCheckableStringListModel::SingleSelection);
    model.selectItem(taskNames.first());

    QCheckBox expResultsCheckbox(tr("Remove Results"));

    expResultsCheckbox.setChecked(true);

    QListView view;
    QVBoxLayout lay;
    QPushButton expButton(tr("Export"));

    lay.addWidget(&view);
    lay.addWidget(&expResultsCheckbox);
    lay.addWidget(&expButton);

    view.setFrameStyle(QListView::NoFrame);

    view.setModel(&model);

    dialog.setLayout(&lay);

    connect(&expButton, SIGNAL(clicked(bool)), &dialog, SLOT(accept()));

    if (dialog.exec())
    {
        QStringList selItems = model.selectedStringList();

        if (selItems.isEmpty())
        {
            return;
        }

        QString filename = GtFileDialog::getSaveFileName(&dialog,
                           tr("Choose File"),
                           QString(), tr("XML files (*.xml)"),
                           QStringLiteral("meta.xml"));

        if (filename.isEmpty())
        {
            return;
        }

        gtInfo() << tr("exporting meta data to ") << filename;

        // export meta data
        QDomDocument document;
        QDomProcessingInstruction header =
            document.createProcessingInstruction(
                QStringLiteral("xml"),
                QStringLiteral("version=\"1.0\" encoding=\"utf-8\""));

        document.appendChild(header);

        QDomElement rootElement =
            document.createElement(QStringLiteral("GTLAB"));

        QString verStr = QString::number(gtApp->majorRelease()) +
                         QStringLiteral(".") +
                         QString::number(gtApp->minorRelease()) +
                         QStringLiteral(".") +
                         QString::number(gtApp->patchLevel());

        rootElement.setAttribute(QStringLiteral("version"), verStr);

        // write module meta data
        QDomElement modsElement =
            document.createElement(QStringLiteral("MODULES"));

        foreach (const QString& str, project->moduleIds())
        {
            QDomElement modElement =
                document.createElement(QStringLiteral("MODULE"));

            modElement.setAttribute(QStringLiteral("name"), str);
            modElement.setAttribute(QStringLiteral("version"),
                                    gtApp->moduleVersion(str).toString());

            modsElement.appendChild(modElement);
        }

        rootElement.appendChild(modsElement);

        // write data memento
        QDomElement dataElement =
            document.createElement(QStringLiteral("DATA"));

        bool clearResults = expResultsCheckbox.isChecked();

        foreach (GtPackage* pkg, project->findDirectChildren<GtPackage*>())
        {
            if (clearResults)
            {
                GtObject* clonedPkg = pkg->clone();

                if (!clonedPkg)
                {
                    gtError() << tr("Could not clone package!");
                    return;
                }

                QList<GtLabel*> lbls = clonedPkg->findChildren<GtLabel*>();

                foreach (GtLabel* l, lbls)
                {
                    lbls.removeOne(l);
                    delete l;
                }

                GtObjectMemento mem = clonedPkg->toMemento();
                dataElement.appendChild(mem.documentElement());

                delete clonedPkg;
                clonedPkg = nullptr;
            }
            else
            {
                GtObjectMemento mem = pkg->toMemento();
                dataElement.appendChild(mem.documentElement());
            }
        }

        rootElement.appendChild(dataElement);

        // write process meta data
        QDomElement processElement =
            document.createElement(QStringLiteral("PROCESS"));

        auto task = pd->findDirectChild<GtTask*>(selItems.first());

        if (!task)
        {
            return;
        }

        GtObjectMemento mem = task->toMemento();
        processElement.appendChild(mem.documentElement());

        rootElement.appendChild(processElement);

        document.appendChild(rootElement);

        QFile file(filename);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            gtWarning() << objectName() << QStringLiteral(": ")
                        << tr("Failed to save project data!");
            return;
        }

        QTextStream stream(&file);
        stream << document.toString(5);

        file.close();
    }

    gtInfo() << tr("meta data exported!");
}

bool
GtProjectUI::canExportMetaData(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    if (!project->isOpen())
    {
        return false;
    }

    return gtApp->devMode();
}

void
GtProjectUI::showInExplorer(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(project->path()));
}

void
GtProjectUI::renameProject(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    if (!gtApp->session())
    {
        return;
    }

    GtInputDialog dialog;

//    dialog.setInputMode(QInputDialog::IntInput);
    dialog.setWindowTitle(tr("Rename Project"));
    dialog.setWindowIcon(GtGUI::Icon::input16());
    dialog.setLabelText(tr("Note: The associated project path "
                           "on the hard disk is not changed."
                           "\n\nNew project name:"));
    dialog.setInitialTextValue(project->objectName());

    QValidator* validator = new QRegExpValidator(
                                GtRegExp::onlyLettersAndNumbersAndSpace(),
                                &dialog);
    dialog.setTextValidator(validator);

    QStringList blackList = gtApp->session()->projectIds();
    blackList.removeOne(project->objectName());
    dialog.setTextBlackList(blackList);

    if (!dialog.exec())
    {
        return;
    }

    QString text = dialog.textValue();

    gtDebug() << "new project name = " << text;

    bool ok = true;

    if (text.isEmpty())
    {
        ok = false;
    }

    if (!GtRegExp::onlyLettersAndNumbersAndSpace().exactMatch(text))
    {
        ok = false;
    }

    if (!ok)
    {
        QMessageBox msg(QMessageBox::Critical, tr("Rename Project"),
                        tr("Could not rename project.\n"
                           "Please enter a valid project name!"),
                        QMessageBox::Ok);

        msg.exec();

        return;
    }

    if (project->objectName() == text)
    {
        return;
    }

    if (!project->renameProject(text))
    {
        QMessageBox msg(QMessageBox::Critical, tr("Rename Project"),
                        tr("Could not rename project.\n"
                           "Project file error!"),
                        QMessageBox::Ok);

        msg.exec();
    }
}

bool
GtProjectUI::canRenameProject(GtObject* obj) const
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    if (project->isOpen())
    {
        return false;
    }

    return true;
}

void
GtProjectUI::showFootprint(GtObject* obj) const
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    // read footprint of saved project file
    GtFootprint footprint(project->readFootprint());

    if (!footprint.isValid())
    {
        gtWarning() << "no footprint found in project!";

        QMessageBox mb;
        mb.setIcon(QMessageBox::Information);
        mb.setWindowTitle(tr("Footprint Not Found"));
        mb.setWindowIcon(GtGUI::Icon::components16());
        mb.setText(tr("No footprint was found in the project file.\nIt seems "
                      "that the project was created with a GTlab version "
                      "< 1.6.\n\nSaving the project with a GTlab version >= "
                      "1.6 allows the recording of a footprint."));
        mb.setStandardButtons(QMessageBox::Ok);
        mb.setDefaultButton(QMessageBox::Ok);
        mb.exec();
        return;
    }

    GtFootprint frameworkFootpring;

    const QString projectHash =
            QString::fromStdString(footprint.generateHash().toStdString());

    const QString frameworkHash =
            QString::fromStdString(
                frameworkFootpring.generateHash().toStdString());

    const QString projectVersion = footprint.frameworkVersion().toString();
    const QString frameworkVersion =
            frameworkFootpring.frameworkVersion().toString();

    QDialog dialog;
    dialog.setWindowIcon(GtGUI::Icon::components16());
    dialog.setWindowTitle(tr("Footprint Information"));

    auto mLay = new QVBoxLayout;

    auto tWid = new QTreeWidget;
    tWid->setColumnCount(3);
    tWid->setHeaderLabels(QStringList() << "" << "Project" << "Framework");

    if (gtApp->devMode())
    {
        auto hashItem = new QTreeWidgetItem(QStringList() << "Hash" <<
                                            projectHash << frameworkHash);
        tWid->addTopLevelItem(hashItem);
    }

    auto versionItem = new QTreeWidgetItem(QStringList() << "Version" <<
                                           projectVersion << frameworkVersion);

    QColor warnC = GtGUI::Color::footprintWarning();
    QColor errorC = GtGUI::Color::footprintError();
    if (footprint.isNewerRelease())
    {
        versionItem->setBackgroundColor(1, errorC);
        versionItem->setBackgroundColor(2, errorC);
    }
    else if (footprint.isOlderRelease())
    {
        versionItem->setBackgroundColor(1, warnC);
        versionItem->setBackgroundColor(2, warnC);
    }

    tWid->addTopLevelItem(versionItem);

    QMap<QString, GtVersionNumber> unknownModules = footprint.unknownModules();

    if (!unknownModules.isEmpty())
    {
        auto unknownRoot =
                new QTreeWidgetItem(QStringList() << "Unknown Modules");

        for_each_key(unknownModules, [&](const QString& e)
        {
            auto unknownModule =
                    new QTreeWidgetItem(QStringList() << e <<
                                        unknownModules.value(e).toString());

            unknownModule->setBackgroundColor(1, errorC);
            unknownModule->setBackgroundColor(2, errorC);

            unknownRoot->addChild(unknownModule);
        });

        tWid->addTopLevelItem(unknownRoot);
    }

    QMap<QString, GtVersionNumber> incompatibleModules =
            footprint.incompatibleModules();

    if (!incompatibleModules.isEmpty())
    {
        auto incompatibleRoot =
                new QTreeWidgetItem(QStringList() << "Incompatible Modules");

        for_each_key(incompatibleModules, [&](const QString& e)
        {
            auto incompatibleModule =
                    new QTreeWidgetItem(QStringList() << e <<
                                   incompatibleModules.value(e).toString() <<
                                   gtApp->moduleVersion(e).toString());

            incompatibleModule->setBackgroundColor(1, errorC);
            incompatibleModule->setBackgroundColor(2, errorC);

            incompatibleRoot->addChild(incompatibleModule);
        });

        tWid->addTopLevelItem(incompatibleRoot);
    }

    QMap<QString, GtVersionNumber> updatedModules = footprint.updatedModules();

    if (!updatedModules.isEmpty())
    {
        auto updatedRoot =
                new QTreeWidgetItem(QStringList() << "Updated Modules");

        for_each_key(updatedModules, [&](const QString& e)
        {
            auto updatedModule =
                    new QTreeWidgetItem(QStringList() << e <<
                                        updatedModules.value(e).toString() <<
                                        gtApp->moduleVersion(e).toString());

            updatedModule->setBackgroundColor(1, warnC);
            updatedModule->setBackgroundColor(2, warnC);

            updatedRoot->addChild(updatedModule);
        });

        tWid->addTopLevelItem(updatedRoot);
    }

    mLay->addWidget(tWid);

    auto btnLay = new QHBoxLayout;

    auto spacer = new QSpacerItem(10, 20, QSizePolicy::Expanding,
                                  QSizePolicy::Minimum);

    btnLay->addSpacerItem(spacer);

    auto okBtn = new QPushButton(tr("Ok"));

    connect(okBtn, SIGNAL(clicked()), &dialog, SLOT(accept()));

    btnLay->addWidget(okBtn);

    mLay->addLayout(btnLay);

    dialog.setLayout(mLay);

    dialog.resize(500, 400);
    tWid->setColumnWidth(0, 200);
    tWid->expandAll();

    dialog.exec();
}

void
GtProjectUI::upgradeProjectData(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    GtProjectUpgradeDialog dialog(project);

    if (dialog.exec())
    {
        if (dialog.overwriteExistingDataAllowed())
        {
            gtDebug() << "backup and overwriting project data...";
            project->createBackup();
            project->upgradeProjectData();
            gtDataModel->openProject(project);
            gtApp->setCurrentProject(project);
        }
        else
        {
            gtDebug() << "upgrading data as new project...";
            gtDebug() << "  |-> " << dialog.newProjectName();
            gtDebug() << "  |-> " << dialog.newProjectPath();

            GtProjectProvider provider(project);

            GtProject* newProject =
                provider.duplicateProject(dialog.newProjectName(),
                                          dialog.newProjectPath());

            newProject->upgradeProjectData();
            gtDataModel->newProject(newProject);
            gtApp->setCurrentProject(newProject);
        }


    }
}

bool
GtProjectUI::canUpgradeProjectData(GtObject *obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    return project->upgradesAvailable();
}

void
GtProjectUI::editComment(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    if (!project->isOpen())
    {
        return;
    }

    bool ok;

    QWidget parent;
    QString text = QInputDialog::getMultiLineText(&parent,
                                                  "Edit Project Comment",
                                                  "Comment:",
                                                  project->comment(),
                                                  &ok);

    if (ok && (text != project->comment()))
    {
        project->setComment(text);
    }
}

bool
GtProjectUI::canEditComment(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    return project->isOpen();
}

void
GtProjectUI::openProjectSettings(GtObject* obj)
{
    auto* project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    GtProjectSettingsDialog dialog{project};

    // connect signals
    QObject::connect(&dialog, &GtProjectSettingsDialog::projectSaveRequested,
                     [&]() {
        if (!canSaveProject(project))
        {
            gtError() << "project not ready for saving!";
            return;
        }
        saveProject(project);
    });
    QObject::connect(&dialog, &GtProjectSettingsDialog::projectCloseRequested,
                     [&]() {
        if (!canCloseProject(project))
        {
            gtError() << "project not ready for closing!";
            return;
        }
        closeProject(project);
    });

    dialog.exec();
}
