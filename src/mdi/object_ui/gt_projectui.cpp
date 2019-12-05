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
#include "gt_regexp.h"
#include "gt_inputdialog.h"

#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"
#include "gt_objectfactory.h"

#include "gt_projectui.h"

GtProjectUI::GtProjectUI()
{
    addSingleAction(tr("Set as active Project"),
                    QStringLiteral("emptyIcon_16.png"),
                    QStringLiteral("setCurrentProject"),
                    QStringLiteral("canSetCurrentProject"),
                    QStringLiteral("canSetCurrentProject"));

    addSingleAction(tr("Open Project"),
                    QStringLiteral("openProjectIcon_16.png"),
                    QStringLiteral("openProject"),
                    QStringLiteral("canOpenProject"));

    addSingleAction(tr("Close Project"),
                    QStringLiteral("closeProjectIcon_16.png"),
                    QStringLiteral("closeProject"),
                    QStringLiteral("canCloseProject"));

    addSingleAction(tr("Save Project"),
                    QStringLiteral("saveProjectIcon_16.png"),
                    QStringLiteral("saveProject"),
                    QStringLiteral("canSaveProject"));

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
                        QStringLiteral("devModeOnly/own_switchOffIcon.png"),
                        QStringLiteral("enableVersionControl"),
                        QStringLiteral("canEnableVersionControl"),
                        QStringLiteral("canEnableVersionControl"));

        addSeparator();


        // init version control, quick and dirty for now
        //        GtProjectDiffPatch * differ = new GtProjectDiffPatch;
        //        GtdVCSDBInterface * vcInterface = new GtdVCSDBInterface;
        //        vc = new GtdVersionControlCore(vcInterface, differ);

    }

    addSingleAction(tr("Show in Explorer"),
                    QStringLiteral("folder_16.png"),
                    QStringLiteral("showInExplorer"));

    addSingleAction(tr("Rename Project"),
                    QStringLiteral("inputIcon_16.png"),
                    QStringLiteral("renameProject"),
                    QStringLiteral("canRenameProject"));

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
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
    {
        return QIcon();
    }

    if (project->isOpen())
    {
        return gtApp->icon("componentsIcon_16.png");
    }
    else
    {
        return gtApp->icon("closedProjectIcon_16.png");
    }

    return QIcon();
}

void
GtProjectUI::doubleClicked(GtObject* obj)
{
    openProject(obj);
}

QVariant
GtProjectUI::specificData(GtObject* obj, int role, int column) const
{
    if (obj == Q_NULLPTR)
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
        }
    }
    else if (column == 1)
    {
        if (gtApp->devMode())
        {
            switch (role)
            {
                case Qt::DecorationRole:
                {
                    GtProject* project = qobject_cast<GtProject*>(obj);

                    if (project != Q_NULLPTR)
                    {
                        if (project->isOpen())
                        {
                            return gtApp->icon(
                                        "devModeOnly/own_switchOffIcon.png");
                        }
                    }

                    break;
                }
            }
        }
    }

    return QVariant();
}

void
GtProjectUI::openProject(GtObject* obj)
{
    if (gtProcessExecutor->taskCurrentlyRunning())
    {
        QMessageBox mb;
        mb.setIcon(QMessageBox::Information);
        mb.setWindowTitle(tr("Open Project"));
        mb.setWindowIcon(gtApp->icon("openProjectIcon_16.png"));
        mb.setText(tr("Cannot open new project while a task is running."));
        mb.setStandardButtons(QMessageBox::Ok);
        mb.setDefaultButton(QMessageBox::Ok);
        mb.exec();
        return;
    }

    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
    {
        return;
    }

    if (project->isOpen())
    {
        setCurrentProject(project);
        return;
    }

    if (project != gtApp->currentProject())
    {
        if (gtApp->hasProjectChanges())
        {
            QString text = tr("Found changes in current project.\n"
                              "Do you want to save all your changes "
                              "before opening new project?");

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
                    break;
                }

                default:
                    break;
            }
        }
        else if (gtApp->currentProject() != Q_NULLPTR)
        {
            QMessageBox mb;
            mb.setIcon(QMessageBox::Question);
            mb.setWindowTitle("Confirm Switch Project");
            mb.setWindowIcon(gtApp->icon("jumpToIcon.png"));
            mb.setText(QString("Switch project?"));
            mb.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
            mb.setDefaultButton(QMessageBox::Cancel);
            int ret = mb.exec();

            switch (ret)
            {
                case QMessageBox::Yes:
                {
                    break;
                }

                case QMessageBox::Cancel:
                {
                    return;
                    break;
                }

                default:
                    break;
            }
        }

        gtDataModel->closeProject(gtApp->currentProject());
    }

    gtDataModel->openProject(project);
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

    if (project == Q_NULLPTR)
    {
        return false;
    }

    return !project->isOpen();
}

void
GtProjectUI::closeProject(GtObject* obj)
{
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
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
                break;
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

    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
    {
        return false;
    }

    return project->isOpen();
}

void
GtProjectUI::saveProject(GtObject* obj)
{
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
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
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
    {
        return false;
    }

    return project->isOpen();
}

void
GtProjectUI::saveProjectAs(GtObject* obj)
{
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
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
    dialog.setWindowIcon(gtApp->icon(QStringLiteral("saveProjectIcon_16.png")));

    Qt::WindowFlags flags = dialog.windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    dialog.setWindowFlags(flags);

    QVBoxLayout* layout = new QVBoxLayout;

    GtProjectSpecWidget* pspecs = new GtProjectSpecWidget;
    layout->addWidget(pspecs);

    QDir projectDir(project->path());
    projectDir.cdUp();

    pspecs->setProjectInformation(id, projectDir.absolutePath());

    layout->addStretch(1);

    QHBoxLayout* btnLay = new QHBoxLayout;

    QPushButton* acceptBtn = new QPushButton(tr("Save"));
    acceptBtn->setIcon(gtApp->icon(QStringLiteral("saveProjectIcon_16.png")));

    connect(acceptBtn, SIGNAL(clicked(bool)), &dialog, SLOT(accept()));

    QPushButton* cancleBtn = new QPushButton(tr("Cancel"));
    cancleBtn->setIcon(gtApp->icon(QStringLiteral("closeIcon_16.png")));

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

        if (newProject == Q_NULLPTR)
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


    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
    {
        return false;
    }

    return project->isOpen();
}

void
GtProjectUI::duplicateProject(GtObject* obj)
{
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
    {
        return;
    }

    if (!project->isOpen())
    {
        return;
    }

    bool ok;

    QString id = gtDataModel->uniqueObjectName(project->objectName(),
                 gtApp->session());

    QString text = QInputDialog::getText(Q_NULLPTR,
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

        if (newProject == Q_NULLPTR)
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


    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
    {
        return false;
    }

    return project->isOpen();
}

void
GtProjectUI::deleteProject(GtObject* obj)
{
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
    {
        return;
    }

    if (project->isOpen())
    {
        return;
    }

    GtConfirmDeleteProjectDialog confirmationDialog(NULL,
            project->objectName());

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
                               "from hard-disk successflly");
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
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
    {
        return false;
    }

    return !project->isOpen();
}

void
GtProjectUI::setCurrentProject(GtObject* obj)
{
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
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

        if (lastProject != Q_NULLPTR)
        {
            gtDataModel->updateObject(lastProject);
        }
    }
}

bool
GtProjectUI::canSetCurrentProject(GtObject* obj)
{
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
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
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
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
    dialog.setWindowIcon(gtApp->icon(QStringLiteral("pluginIcon_16.png")));

    Qt::WindowFlags flags = dialog.windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    dialog.setWindowFlags(flags);


    QVBoxLayout* layout = new QVBoxLayout;

    QLabel* label = new QLabel(tr("Select project specific modules"));
    layout->addWidget(label);

    GtCheckableStringListModel* model = new GtCheckableStringListModel(
        gtApp->moduleDatamodelInterfaceIds(), &dialog);

    model->selectItems(project->moduleIds());

    QListView* view = new QListView;
    view->setModel(model);
    view->setFrameStyle(QListView::NoFrame);
    layout->addWidget(view);

    layout->addStretch(1);



    QPushButton* okButton = new QPushButton(tr("Ok"));
    okButton->setIcon(gtApp->icon(QStringLiteral("checkIcon_16.png")));

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
            mb.setWindowIcon(
                gtApp->icon(QStringLiteral("saveProjectIcon_16.png")));
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


    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
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

    //    if (project == Q_NULLPTR)
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
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
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

    //    if (project == Q_NULLPTR)
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
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
    {
        return;
    }
}

bool
GtProjectUI::canEnableVersionControl(GtObject* obj)
{
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
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
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
    {
        return;
    }

    GtProcessData* pd =  project->processData();

    if (pd == Q_NULLPTR)
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
    dialog.setWindowIcon(gtApp->icon(QStringLiteral("exportIcon_16.png")));

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
                                    gtApp->moduleVersion(str));

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

                if (clonedPkg == Q_NULLPTR)
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
                clonedPkg = Q_NULLPTR;
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

        GtTask* task = pd->findDirectChild<GtTask*>(selItems.first());

        if (task == Q_NULLPTR)
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
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
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
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
    {
        return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(project->path()));
}

void
GtProjectUI::renameProject(GtObject* obj)
{
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
    {
        return;
    }

    if (gtApp->session() == Q_NULLPTR)
    {
        return;
    }

    GtInputDialog dialog;

//    dialog.setInputMode(QInputDialog::IntInput);
    dialog.setWindowTitle(tr("Rename Project"));
    dialog.setWindowIcon(gtApp->icon(QStringLiteral("inputIcon_16.png")));
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

    bool ok;

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
GtProjectUI::canRenameProject(GtObject* obj)
{
    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project == Q_NULLPTR)
    {
        return false;
    }

    if (project->isOpen())
    {
        return false;
    }

    return true;
}
