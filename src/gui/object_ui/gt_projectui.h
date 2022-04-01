/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECTUI_H
#define GTPROJECTUI_H

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
     * @brief openProject
     * @param obj
     */
    static void openProject(GtObject* obj);

    /**
     * @brief canOpenProject
     * @param obj
     * @return
     */
    bool canOpenProject(GtObject* obj);

    /**
     * @brief closeProject
     * @param obj
     */
    static void closeProject(GtObject* obj);

    /**
     * @brief canCloseProject
     * @param obj
     * @return
     */
    bool canCloseProject(GtObject* obj);

    /**
     * @brief saveProject
     * @param obj
     */
    void saveProject(GtObject* obj);

    /**
     * @brief canSaveProject
     * @param obj
     * @return
     */
    bool canSaveProject(GtObject* obj);

    /**
     * @brief saveProjectAs
     * @param obj
     */
    static void saveProjectAs(GtObject* obj);

    /**
     * @brief canSaveProjectAs
     * @param obj
     * @return
     */
    bool canSaveProjectAs(GtObject* obj);

    /**
     * @brief duplicateProject
     * @param obj
     */
    static void duplicateProject(GtObject* obj);

    /**
     * @brief canDuplicateProject
     * @param obj
     * @return
     */
    bool canDuplicateProject(GtObject* obj);

    /**
     * @brief deleteProject
     * @param obj
     */
    static void deleteProject(GtObject* obj);

    /**
     * @brief canDeleteProject
     * @param obj
     * @return
     */
    bool canDeleteProject(GtObject* obj);

    /**
     * @brief setCurrentProject
     * @param obj
     */
    static void setCurrentProject(GtObject* obj);

    /**
     * @brief canSetCurrentProject
     * @param obj
     * @return
     */
    bool canSetCurrentProject(GtObject* obj);

    /**
     * @brief chooseProjectModule
     * @param obj
     */
    void chooseProjectModule(GtObject* obj);

    /**
     * @brief canChooseProjectModule
     * @param obj
     */
    bool canChooseProjectModule(GtObject* obj);

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

    /**
     * @brief enableVersionControl
     * @param obj
     */
    void enableVersionControl(GtObject* obj);

    /**
     * @brief canEnableVersionControl
     * @param obj
     * @return
     */
    bool canEnableVersionControl(GtObject* obj);

    /**
     * @brief exportMetaData
     * @param obj
     */
    void exportMetaData(GtObject* obj);

    /**
     * @brief canExportMetaData
     * @param obj
     * @return
     */
    bool canExportMetaData(GtObject* obj);

    /**
     * @brief showInExplorer
     * @param obj
     */
    void showInExplorer(GtObject* obj);

    /**
     * @brief renameProject
     * @param obj
     */
    void renameProject(GtObject* obj);

    /**
     * @brief canRenameProject
     * @param obj
     * @return
     */
    bool canRenameProject(GtObject* obj);

    /**
     * @brief Shows footprint of selected project.
     * @param obj Selected project
     */
    void showFootprint(GtObject* obj);

    /**
     * @brief Opens comment editor if project ist opened.
     * @param obj Selected project.
     */
    void editComment(GtObject* obj);

    /**
     * @brief Returns true if project is opened so comment can be edited.
     * Otherwise false is returned.
     * @param obj Selected project
     * @return Whether project comment can be edited or not.
     */
    bool canEditComment(GtObject* obj);

    /**
     * @brief openProjectSettings opens the project settings
     * @param obj Selected project
     */
    void openProjectSettings(GtObject* obj);

};

#endif // GTPROJECTUI_H
