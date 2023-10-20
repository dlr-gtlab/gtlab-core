/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 06.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECTUI_H
#define GTPROJECTUI_H

#include <QDir>

#include "gt_gui_exports.h"

#include "gt_objectui.h"

/**
 * @brief The GtProjectUI class
 */
class GT_GUI_EXPORT GtProjectUI : public GtObjectUI
{
    Q_OBJECT

public:
    /**
     * @brief GtProjectUI
     */
    Q_INVOKABLE GtProjectUI();

    /**
     * Virtual function to specify object specific icon.
     * @return Object specific icon
     */
    QIcon icon(GtObject* obj) const override;

    /**
     * @brief doubleClicked
     * @param obj
     */
    void doubleClicked(GtObject* obj) override;

    /**
     * @brief Asks, whether to save current project changes
     * and performs the save
     *
     * Returns false, if aborted, otherwise true
     */
    static bool saveAndCloseCurrentProject();

    /**
     * @brief Switches the project and performs user queries
     * how to do it
     */
    static void switchToProject(class GtProject& project);

    /**
     * @brief renameProject
     * @param obj
     */
    static void renameProject(GtObject* obj);

    /**
     * @brief canRenameProject
     * @param obj
     * @return
     */
    static bool canRenameProject(GtObject* obj);

    /**
     * @brief saveProjectAs
     * @param obj
     */
    static void saveProjectAs(GtObject* obj);

    /**
     * @brief closeProject
     * @param obj
     */
    static void closeProject(GtObject* obj);

    /**
     * @brief openProject
     * @param obj
     */
    static void openProject(GtObject* obj);

private:
    /**
     * @brief saveProject
     * @param obj
     */
    static void saveProject(GtObject* obj);

    /**
     * @brief Opens comment editor if project ist opened.
     * @param obj Selected project.
     */
    static void editComment(GtObject* obj);

    /**
     * @brief Returns true if project is opened so comment can be edited.
     * Otherwise false is returned.
     * @param obj Selected project
     * @return Whether project comment can be edited or not.
     */
    static bool canEditComment(GtObject* obj);

    /**
     * @brief showInExplorer
     * @param obj
     */
    static void showInExplorer(GtObject* obj);

    /**
     * @brief projectIsOpen
     * @param obj
     * @return true if the object is a GtProject and is open
     */
    static bool projectIsOpen(GtObject* obj);

    /**
     * @brief Shows footprint of selected project.
     * @param obj Selected project
     */
    static void showFootprint(GtObject* obj);

    /**
     * @brief canCloseProject
     * @param obj
     * @return
     */
    static bool canCloseProject(GtObject* obj);

    /**
     * @brief canSaveProject
     * @param obj
     * @return
     */
    static bool canSaveProject(GtObject* obj);

    /**
     * @brief openProjectSettings opens the project settings
     * @param obj Selected project
     */
    static void openProjectSettings(GtObject* obj);

    /**
     * @brief backupProject
     * @param obj
     * Store the content of the project in a backup directory
     * in a new folder named by the timestamp
     */
    static void backupProject(GtObject* obj);

    /**
     * @brief canRestoreBackup
     * @param obj
     * @return check the requirements if a restore
     * of project data is possible as if there is a backup folder
     * which is containg at least one project folder
     */
    static bool canRestoreBackup(GtObject* obj);

    static void restoreBackup(GtObject* obj);

    /**
     * @brief canOpenProject
     * @param obj
     * @return
     */
    static bool canOpenProject(GtObject* obj);

    /**
     * @brief Indicator function to specify whether upgrades of project data
     * are available.
     * @param obj Selected project.
     * @return True if upgrades are available.
     */
    static bool canUpgradeProjectData(GtObject* obj);

    /**
     * @brief exportMetaData
     * @param obj
     */
    static void exportMetaData(GtObject* obj);

    /**
     * @brief canExportMetaData
     * @param obj
     * @return
     */
    static bool canExportMetaData(GtObject* obj);

    /**
     * @brief chooseProjectModule
     * @param obj
     */
    static void chooseProjectModule(GtObject* obj);

    /**
     * @brief canChooseProjectModule
     * @param obj
     */
    static bool canChooseProjectModule(GtObject* obj);

    /**
     * @brief enableVersionControl
     * @param obj
     */
    static void enableVersionControl(GtObject* obj);

    /**
     * @brief canEnableVersionControl
     * @param obj
     * @return
     */
    static bool canEnableVersionControl(GtObject* obj);

    /**
     * @brief canSaveProjectAs
     * @param obj
     * @return
     */
    static bool canSaveProjectAs(GtObject* obj);

    /**
     * @brief Triggers upgrad of project data provided by modules.
     * @param obj Selected project.
     */
    static void upgradeProjectData(GtObject* obj);

    /**
     * @brief canSetCurrentProject
     * @param obj
     * @return
     */
    static bool canSetCurrentProject(GtObject* obj);

    /**
     * @brief Prompts the user whether to remove the project from session and
     * whether to delte the project permanently
     * @param obj Project object
     */
    static void deleteProject(GtObject* obj);

    /**
     * @brief canDeleteProject
     * @param obj
     * @return
     */
    static bool canDeleteProject(GtObject* obj);

    /**
     * @brief setCurrentProject
     * @param obj
     */
    static void setCurrentProject(GtObject* obj);

    /**
     * @brief validBackupDirectories
     * @param project to check
     * @return a list of the valid backup directories.
     * The validity check is done by a check if a project file is
     * inside the directory
     */
    static QList<QDir> validBackupDirectories(GtProject* project);

protected:
    /**
     * @brief specificData
     * @param obj
     * @param role
     * @param column
     * @return
     */
    QVariant specificData(GtObject* obj,
                          int role,
                          int column) const override;

public slots:
    /**
     * @brief testCommit
     * @param obj
     */
    void testCommit(GtObject* obj);

    /**
     * @brief canOpenProject
     * @param obj
     * @return
     */
    bool canTestCommit(GtObject* obj);

    /**
     * @brief testCheckout
     * @param obj
     */
    void testCheckout(GtObject* obj);

    /**
     * @brief canTestCheckout
     * @param obj
     * @return
     */
    bool canTestCheckout(GtObject* obj);
};

#endif // GTPROJECTUI_H
