/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 06.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QFont>
#include <QVariant>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListView>
#include <QCheckBox>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QTreeWidget>
#include <QPointer>
#include <QMenu>

#include "gt_abstractloadinghelper.h"
#include "gt_project.h"
#include "gt_projectprovider.h"
#include "gt_projectexecutionguard.h"
#include "gt_objectfactory.h"
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
#include "gt_processexecutor.h"
#include "gt_confirmdeleteprojectdialog.h"
#include "gt_saveprojectmessagebox.h"
#include "gt_switchprojectmessagebox.h"
#include "gt_regularexpression.h"
#include "gt_inputdialog.h"
#include "gt_footprint.h"
#include "gt_versionnumber.h"
#include "gt_icons.h"
#include "gt_projectsettingsdialog.h"
#include "gt_objectmemento.h"
#include "gt_algorithms.h"
#include "gt_colors.h"
#include "gt_projectupgradedialog.h"
#include "gt_dialog.h"
#include "gt_generatebackupdialog.h"
#include "gt_projectrestorebackupdialog.h"
#include "gt_taskgroup.h"
#include "gt_xmlexpr.h"

#include "gt_projectui.h"

GtProjectUI::GtProjectUI()
{
    addSingleAction(tr("Upgrade Project Data..."),
                    &GtProjectUI::upgradeProjectData)
            .setIcon(gt::gui::icon::upload())
            .setVerificationMethod(&GtProjectUI::canUpgradeProjectData)
            .setVisibilityMethod(&GtProjectUI::canUpgradeProjectData);

    addSingleAction(tr("Set as active Project"),
                    &GtProjectUI::setCurrentProject)
            .setIcon(gt::gui::icon::objectEmpty())
            .setVerificationMethod(&GtProjectUI::canSetCurrentProject)
            .setVisibilityMethod(&GtProjectUI::canSetCurrentProject);

    addSingleAction(tr("Open Project"),
                    &GtProjectUI::openProject)
            .setIcon(gt::gui::icon::projectOpen())
            .setVerificationMethod(&GtProjectUI::canOpenProject)
            .setShortCut(gtApp->getShortCutSequence("openProject"));

    addSingleAction(tr("Close Project"),
                    &GtProjectUI::closeProject)
            .setIcon(gt::gui::icon::projectClose())
            .setVerificationMethod(&GtProjectUI::canCloseProject)
            .setShortCut(gtApp->getShortCutSequence("closeProject"));

    addSingleAction(tr("Save Project"),
                    &GtProjectUI::saveProject)
            .setIcon(gt::gui::icon::save())
            .setVerificationMethod(&GtProjectUI::canSaveProject)
            .setShortCut(gtApp->getShortCutSequence("save"));

    addSingleAction(tr("Save Project As..."),
                    &GtProjectUI::saveProjectAs)
            .setIcon(gt::gui::icon::save())
            .setVerificationMethod(&GtProjectUI::canSaveProjectAs);

    addSeparator();

    if (gtApp->devMode())
    {
        addSingleAction(tr("Enable Version Control"),
                        &GtProjectUI::enableVersionControl)
                .setIcon(gt::gui::icon::check())
                .setVerificationMethod(&GtProjectUI::canEnableVersionControl)
                .setVisibilityMethod(&GtProjectUI::canEnableVersionControl);

        addSeparator();

    }
 
    addSingleAction(tr("Open Project Settings..."),
                    &GtProjectUI::openProjectSettings)
            .setIcon(gt::gui::icon::config())
            .setVerificationMethod(&GtProjectUI::projectIsOpen);

    addSingleAction(tr("Show Project Footprint"),
                    &GtProjectUI::showFootprint)
            .setIcon(gt::gui::icon::info2())
            .setShortCut(gtApp->getShortCutSequence("ShowFootprint"));

    addSingleAction(tr("Edit Comment"),
                    &GtProjectUI::editComment)
            .setIcon(gt::gui::icon::comment())
            .setVerificationMethod(&GtProjectUI::canEditComment);

    addSeparator();

    addSingleAction(tr("Show in Explorer"),
                    &GtProjectUI::showInExplorer)
            .setIcon(gt::gui::icon::folderOpen());


    addSingleAction(tr("Rename Project"),
                    &GtProjectUI::renameProject)
            .setIcon(gt::gui::icon::rename())
            .setVerificationMethod(&GtProjectUI::canRenameProject)
            .setShortCut(gtApp->getShortCutSequence("rename"));

    addSeparator();

    addSingleAction(tr("Export Meta Data"),
                    &GtProjectUI::exportMetaData)
            .setIcon(gt::gui::icon::export_())
            .setVerificationMethod(&GtProjectUI::canExportMetaData)
            .setVisibilityMethod(&GtProjectUI::canExportMetaData);

    addSingleAction(tr("Delete from Session"),
                    &GtProjectUI::deleteProject)
            .setIcon(gt::gui::icon::projectRemove())
            .setVerificationMethod(&GtProjectUI::canDeleteProject)
            .setShortCut(gtApp->getShortCutSequence(QStringLiteral("delete")));

    addSingleAction(tr("Generate Backup"),
                    &GtProjectUI::backupProject)
            .setVerificationMethod(&GtProjectUI::projectIsOpen)
            .setVisibilityMethod(&GtProjectUI::projectIsOpen)
            .setIcon(gt::gui::icon::data());

    addSingleAction(tr("Restore Backup"),
                    &GtProjectUI::restoreBackup)
            .setIcon(gt::gui::icon::dataArrowUp())
            .setVerificationMethod(&GtProjectUI::canRestoreBackup);
}

QIcon
GtProjectUI::icon(GtObject* obj) const
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project) return {};

    if (project->isOpen())
    {
        return gt::gui::icon::projectOpen();
    }

    if (project->upgradesAvailable())
    {
        return gt::gui::colorize(gt::gui::icon::objectInvalid(),
                                 gt::gui::color::warningText);
    }

    return gt::gui::colorize(gt::gui::icon::projectClosed(),
                             gt::gui::color::debugText);
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

                if (!project) break;

                if (project->upgradesAvailable())
                {
                    return tr("Some of the project data is outdated and "
                              "requires an upgrade before the project can "
                              "be opened.");
                }
                break;
            }
            case Qt::ForegroundRole:
            {
                auto project = qobject_cast<GtProject*>(obj);

                if (!project) break;

                if (project->upgradesAvailable())
                {
                    return gt::gui::color::warningText();
                }

                if (!project->isOpen())
                {
                    return gt::gui::color::debugText();
                }
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

            if (project && !project->comment().isEmpty())
            {
                if (project->isOpen())
                {
                    return gt::gui::icon::comment();
                }
                return gt::gui::colorize(gt::gui::icon::comment(),
                                         gt::gui::color::debugText());
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

bool
GtProjectUI::saveAndCloseCurrentProject()
{
    auto* project = gtApp->currentProject();
    if (!project) return true;

    if (gtApp->hasProjectChanges())
    {

        QString text = tr("Found changes in current project.\n"
                          "Do you want to save all your changes "
                          "before opening new project?");

        GtSaveProjectMessageBox mb(text);
        int ret = mb.exec();

        if (ret == QMessageBox::Yes)
        {
            if (!gtDataModel->saveProject(project)) return false;
        }
        else if (ret == QMessageBox::Cancel)
        {
            return false;
        }
    }

    return gtDataModel->closeProject(project);
}


void
GtProjectUI::switchToProject(GtProject& toProject)
{
    if (&toProject == gtApp->currentProject())
    {
        return;
    }


    if (gtApp->currentProject() && !gtApp->hasProjectChanges())
    {
        GtSwitchProjectMessageBox mb(toProject.objectName(), nullptr);
        if (mb.exec() == QMessageBox::Cancel)
        {
            return;
        }
    }

    if (gtApp->hasProjectChanges())
    {
        QString text = tr("Found changes in current project.\n"
                          "Do you want to save all your changes "
                          "before opening new project?");

        GtSaveProjectMessageBox mb(text);
        const int ret = mb.exec();

        if (ret == QMessageBox::Yes)
        {
            if (!gtDataModel->saveProject(gtApp->currentProject()))
            {
                return;
            }
        }
        else if (ret == QMessageBox::Cancel)
        {
            return;
        }
    }

    if (!gtDataModel->switchProject(&toProject))
    {
        gtError() << tr("Could not switch project!");
    }
}

void
GtProjectUI::openProject(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    if (gt::currentProcessExecutor().taskCurrentlyRunning() && !project->isOpen())
    {
        QMessageBox mb;
        mb.setIcon(QMessageBox::Information);
        mb.setWindowTitle(tr("Open Project"));
        mb.setWindowIcon(gt::gui::icon::projectOpen());
        mb.setText(tr("Cannot open new project while a task is running."));
        mb.setStandardButtons(QMessageBox::Ok);
        mb.setDefaultButton(QMessageBox::Ok);
        mb.exec();
        return;
    }

    if (project->isOpen())
    {
        setCurrentProject(project);
        return;
    }

    if (canUpgradeProjectData(obj))
    {
        // Note upgradeProjectData changes the currentProject
        upgradeProjectData(obj);
        project = gtApp->currentProject();
    }

    switchToProject(*project);
}

bool
GtProjectUI::canOpenProject(GtObject* obj)
{
    if (gt::currentProcessExecutor().taskCurrentlyRunning())
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
                if (!gtDataModel->saveProject(project)) return;
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
    if (gt::currentProcessExecutor().taskCurrentlyRunning())
    {
        return false;
    }

    return projectIsOpen(obj);
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
    return projectIsOpen(obj);
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

    QString id = gt::makeUniqueName(project->objectName(), gtApp->session());

    GtDialog dialog;

    dialog.setWindowTitle(tr("Save Project As..."));
    dialog.setWindowIcon(gt::gui::icon::save());

    auto layout = new QVBoxLayout;

    auto pspecs = new GtProjectSpecWidget;
    layout->addWidget(pspecs);

    QDir projectDir(project->path());
    projectDir.cdUp();

    pspecs->setProjectInformation(id, projectDir.absolutePath());

    layout->addStretch(1);

    auto btnLay = new QHBoxLayout;

    auto acceptBtn = new QPushButton(tr("Save"));
    acceptBtn->setIcon(gt::gui::icon::save());

    connect(acceptBtn, SIGNAL(clicked(bool)), &dialog, SLOT(accept()));

    auto cancleBtn = new QPushButton(tr("Cancel"));
    cancleBtn->setIcon(gt::gui::icon::cancel());

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

        gtDebug().verbose() << "pathNew = " << pathNew.absolutePath();

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

        if (!gtDataModel->newProject(newProject, false))
        {
            delete newProject;
            gtError() << tr("Could not add duplicated project to session!");
            return;
        }

        switchToProject(*newProject);
    }

    //    gtDataModel->saveProject(project);
}

bool
GtProjectUI::canSaveProjectAs(GtObject* obj)
{
    if (gt::currentProcessExecutor().taskCurrentlyRunning())
    {
        return false;
    }

    return projectIsOpen(obj);
}

void
GtProjectUI::deleteProject(GtObject* obj)
{
    if (!canDeleteProject(obj)) return;

    auto project = static_cast<GtProject*>(obj);

    GtConfirmDeleteProjectDialog confirmationDialog(
                nullptr, project->objectName());

    if (confirmationDialog.exec())
    {
        if (confirmationDialog.result() == GtDialog::Accepted
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
        else if (confirmationDialog.result() == GtDialog::Accepted)
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

namespace
{
QStringList
availablePackageModules(const GtProject& project)
{
    QStringList ids;
    const auto loadedModules = gtApp->moduleDatamodelInterfaceIds();
    for (const auto& id : loadedModules)
    {
        if (project.packageSlotError(id).isEmpty()) ids.append(id);
    }
    ids.sort(Qt::CaseInsensitive);
    return ids;
}

QIcon
defaultPackageIcon(const QString& moduleId)
{
    const auto className = gtApp->modulePackageId(moduleId);
    if (const auto* ui = gtApp->defaultObjectUI(className))
    {
        // A module's ObjectUI can use the package's default state to choose its icon.
        const std::unique_ptr<GtObject> package(
            gtObjectFactory->newObject(className));
        const auto icon =
            ui->data(package.get(), Qt::DecorationRole).value<QIcon>();
        if (!icon.isNull()) return icon;
    }
    return gt::gui::icon::plugin();
}

} // namespace

bool
GtProjectUI::canEditPackages(GtObject* obj)
{
    auto* project = qobject_cast<GtProject*>(obj);
    return project && project == gtApp->currentProject() && project->isOpen() &&
           !GtProjectExecutionGuard::isLocked(project) &&
           !gt::currentProcessExecutor().taskCurrentlyRunning();
}

QMenu*
GtProjectUI::createAddPackageMenu(QMenu& parent, GtProject& project)
{
    if (!project.isOpen()) return nullptr;

    auto* submenu = new QMenu(tr("Add Package"), &parent);
    submenu->setIcon(gt::gui::icon::plugin());
    const auto modules = availablePackageModules(project);
    submenu->setEnabled(canEditPackages(&project) && !modules.isEmpty());

    const QPointer<GtProject> target(&project);
    for (const auto& module : modules)
    {
        QString label = module;
        auto* action =
            submenu->addAction(defaultPackageIcon(module),
                               label.replace('&', QStringLiteral("&&")));
        action->setIconVisibleInMenu(true);
        connect(action, &QAction::triggered, submenu, [target, module] {
            if (target) addPackage(*target, module);
        });
    }
    return submenu;
}

void
GtProjectUI::addPackage(GtProject& project, const QString& moduleId)
{
    if (!canEditPackages(&project)) return;

    const auto title = tr("Add Package");
    const QPointer<GtProject> target(&project);
    bool restore = false;
    if (QFile::exists(project.moduleDataPath(moduleId)))
    {
        QMessageBox choice(
            QMessageBox::Question, title,
            tr("This project already has a saved package for '%1'.")
                .arg(moduleId),
            QMessageBox::Cancel);
        auto* restoreButton = choice.addButton(tr("Restore Existing Package"),
                                               QMessageBox::AcceptRole);
        auto* emptyButton = choice.addButton(tr("Create Empty Package"),
                                             QMessageBox::ActionRole);
        choice.setDefaultButton(restoreButton);
        choice.exec();
        restore = choice.clickedButton() == restoreButton;
        if (!restore && choice.clickedButton() != emptyButton) return;
    }

    if (!canEditPackages(target.data())) return;

    QString error;
    auto package = restore ? target->restorePackage(moduleId, error)
                           : target->createPackage(moduleId, error);
    if (restore && !package && error.isEmpty())
    {
        if (QMessageBox::warning(nullptr, title,
                                 tr("The saved package has no module version. "
                                    "Restore it anyway?"),
                                 QMessageBox::Yes | QMessageBox::Cancel,
                                 QMessageBox::Cancel) != QMessageBox::Yes ||
            !canEditPackages(target.data()))
            return;
        package = target->restorePackage(moduleId, error, true);
    }

    if (package &&
        gtDataModel->appendChild(std::move(package), target.data()).isValid())
        return;

    QMessageBox::warning(
        nullptr, title,
        error.isEmpty() ? tr("Could not insert the package into the project.")
                        : error);
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

    GtTaskGroup* pd =  project->processData()->taskGroup();

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

    GtDialog dialog;
    dialog.setWindowTitle(tr("Export Meta Data"));
    dialog.setWindowIcon(gt::gui::icon::export_());

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
            document.createElement(gt::xml::S_GTLAB_TAG);

        rootElement.setAttribute(QStringLiteral("version"),
                                 gtApp->version().toString());

        project->saveModuleMetaData(rootElement, document);

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
            gtWarning() << project->objectName() << QStringLiteral(": ")
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
    if (!canRenameProject(obj)) return;

    auto project = qobject_cast<GtProject*>(obj);

    assert(project);
    // cppcheck-suppress assertWithSideEffect
    assert(gtApp->session());

    GtInputDialog dialog;

//    dialog.setInputMode(QInputDialog::IntInput);
    dialog.setWindowTitle(tr("Rename Project"));
    dialog.setWindowIcon(gt::gui::icon::input());
    dialog.setLabelText(tr("Note: The associated project path "
                           "on the disk is not changed."
                           "\n\nNew project name:"));
    dialog.setInitialTextValue(project->objectName());

    QValidator* validator = new QRegularExpressionValidator(
                                gt::rex::onlyLettersAndNumbersAndSpace(),
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

    if (!gt::rex::exactMatch(gt::rex::onlyLettersAndNumbersAndSpace(), text))
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
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    if (project->isOpen())
    {
        return false;
    }

    if (!gtApp->session())
    {
        return false;
    }

    return true;
}

void
GtProjectUI::showFootprint(GtObject* obj)
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
        mb.setWindowIcon(gt::gui::icon::error());
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

    GtDialog dialog;
    dialog.setWindowIcon(gt::gui::icon::info2());
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

    QColor warnC = gt::gui::color::footprintWarning();
    QColor errorC = gt::gui::color::footprintError();
    if (footprint.isNewerRelease())
    {
        versionItem->setBackground(1, errorC);
        versionItem->setBackground(2, errorC);
    }
    else if (footprint.isOlderRelease())
    {
        versionItem->setBackground(1, warnC);
        versionItem->setBackground(2, warnC);
    }

    tWid->addTopLevelItem(versionItem);

    QMap<QString, GtVersionNumber> unknownModules = footprint.unknownModules();

    if (!unknownModules.isEmpty())
    {
        auto unknownRoot =
                new QTreeWidgetItem(QStringList() << "Unknown Modules");

        gt::for_each_key(unknownModules, [&](const QString& e)
        {
            auto unknownModule =
                    new QTreeWidgetItem(QStringList() << e <<
                                        unknownModules.value(e).toString());

            unknownModule->setBackground(1, errorC);
            unknownModule->setBackground(2, errorC);

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

        gt::for_each_key(incompatibleModules, [&](const QString& e)
        {
            auto incompatibleModule =
                    new QTreeWidgetItem(QStringList() << e <<
                                   incompatibleModules.value(e).toString() <<
                                   gtApp->moduleVersion(e).toString());

            incompatibleModule->setBackground(1, errorC);
            incompatibleModule->setBackground(2, errorC);

            incompatibleRoot->addChild(incompatibleModule);
        });

        tWid->addTopLevelItem(incompatibleRoot);
    }

    QMap<QString, GtVersionNumber> updatedModules = footprint.updatedModules();

    if (!updatedModules.isEmpty())
    {
        auto updatedRoot =
                new QTreeWidgetItem(QStringList() << "Updated Modules");

        gt::for_each_key(updatedModules, [&](const QString& e)
        {
            auto updatedModule =
                    new QTreeWidgetItem(QStringList() << e <<
                                        updatedModules.value(e).toString() <<
                                        gtApp->moduleVersion(e).toString());

            updatedModule->setBackground(1, warnC);
            updatedModule->setBackground(2, warnC);

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

bool
GtProjectUI::projectIsOpen(GtObject *obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    return project->isOpen();
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
        bool overwrite = dialog.overwriteExistingDataAllowed();
        QString projectPath = overwrite ? project->path()
                                        : dialog.newProjectPath();

        project->upgradeProject(projectPath);

        if (overwrite)
        {
            gtDataModel->openProject(project);
            gtApp->setCurrentProject(project);
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
    return projectIsOpen(obj);
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
                     &dialog, [&]() {
        if (!canSaveProject(project))
        {
            gtError() << tr("Project is not ready for saving!");
            return;
        }
        saveProject(project);
    });
    QObject::connect(&dialog, &GtProjectSettingsDialog::projectCloseRequested,
                     &dialog, [&]() {
        if (!canCloseProject(project))
        {
            gtError() << tr("Project is not ready for closing!");
            return;
        }
        closeProject(project);
    });

    dialog.exec();
}

void
GtProjectUI::backupProject(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    /// check if project has changes. These should be
    if (gtApp->hasProjectChanges())
    {
        QString text = tr("Found unsaved project.\nDo you want to ") +
                       tr("save all your changes before backup project?");

        GtSaveProjectMessageBox mb(text);
        int ret = mb.exec();

        switch (ret)
        {
            case QMessageBox::Yes:
            {
                if (!gtDataModel->saveProject(project)) return;
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

    /// Dialog to inform about backup
    /// Write reason and store in markdown string
    GtGenerateBackUpDialog backUpDialog;
    if (backUpDialog.exec() && backUpDialog.result() == GtDialog::Accepted)
    {
        gtInfo() << tr("Generate backup");
        gtApp->loadingProcedure(gt::makeLoadingHelper([&]() {
            project->createBackup(backUpDialog.message());
        }).get());
    }
}

QList<QDir>
GtProjectUI::validBackupDirectories(GtProject* project)
{
    if (!project)
    {
        return{};
    }

    QDir backUpMainDir (gt::project::backupDirPath(*project));

    if (!backUpMainDir.exists())
    {
        return {};
    }

    QFileInfoList contentList = backUpMainDir.entryInfoList(
                QDir::Dirs | QDir::NoDotAndDotDot);

    if (contentList.isEmpty())
    {
        return {};
    }

    QList<QDir> validBackupDirs;

    for (const QFileInfo& i : qAsConst(contentList))
    {
        /// simple check shuld never fail as filter for directories is active
        if (!i.isDir())
        {
            continue;
        }

        QDir currentBackUp (i.absoluteFilePath());

        if (!currentBackUp.exists())
        {
            continue;
        }

        QFile projectFile (currentBackUp.absoluteFilePath(
                               GtProject::mainFilename()));

        if (!projectFile.exists())
        {
            continue;
        }

        validBackupDirs.append(currentBackUp);
    }

    return validBackupDirs;
}

bool
GtProjectUI::canRestoreBackup(GtObject* obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return false;
    }

    return !validBackupDirectories(project).isEmpty();
}

void
GtProjectUI::restoreBackup(GtObject *obj)
{
    auto project = qobject_cast<GtProject*>(obj);

    if (!project)
    {
        return;
    }

    QList<QDir> validDirs = validBackupDirectories(project);

    if (validDirs.isEmpty())
    {
        return;
    }

    /// Dialog to select backup to restore from
    GtProjectRestoreBackupDialog restoreDialog(validDirs);
    if (restoreDialog.exec() && restoreDialog.result() == GtDialog::Accepted)
    {
        const QString selectedFolder = restoreDialog.selectedFolderName();
        gtInfo() << tr("Restore backup from folder '%1'")
                        .arg(selectedFolder);

        QMessageBox mb;
        mb.setIcon(QMessageBox::Question);
        mb.setWindowTitle(tr("Confirm Restore"));
        mb.setWindowIcon(gt::gui::icon::check());

        const bool initialOpenProject = project->isOpen();

        if (initialOpenProject)
        {
            mb.setText(tr("Are you sure to restore the project? \n "
                      "Your current project will be overwritten which "
                      "cannot be undone. \n"
                      "The project will be closed automatically to "
                          "start restore."));
        }
        else
        {
            mb.setText(tr("Are you sure to restore the project? \n "
                      "Your current project will be overwritten which "
                          "cannot be undone."));
        }


        mb.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        mb.setDefaultButton(QMessageBox::Cancel);

        if (mb.exec() != QMessageBox::Yes)
        {
            return;
        }

        if (initialOpenProject && !gtDataModel->closeProject(project)) return;

        bool success = project->restoreBackupFiles(selectedFolder) ==
                       GtProject::RestoreStatus::Success;

        if (success && initialOpenProject)
        {
            success = gtDataModel->openProject(project);
        }

        // notify the user on the success
        if (success)
        {
            QMessageBox::information(nullptr, tr("Backup restored"),
                tr("The project has been successfully restored from backup."));
        }
        else
        {
            QMessageBox::critical(nullptr, tr("Backup restore failed"),
                tr("The project backup could not be restored."));
        }
    }
}
