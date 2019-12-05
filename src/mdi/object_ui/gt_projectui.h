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

#include "gt_mdi_exports.h"

#include "gt_objectui.h"

/**
 * @brief The GtProjectUI class
 */
class GT_MDI_EXPORT GtProjectUI : public GtObjectUI
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
    virtual QIcon icon(GtObject* obj) const Q_DECL_OVERRIDE;

    /**
     * @brief doubleClicked
     * @param obj
     */
    virtual void doubleClicked(GtObject* obj);

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
                          int column) const Q_DECL_OVERRIDE;

public slots:
    /**
     * @brief openProject
     * @param obj
     */
    void openProject(GtObject* obj);

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
    void closeProject(GtObject* obj);

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
    void saveProjectAs(GtObject* obj);

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
    void duplicateProject(GtObject* obj);

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
    void deleteProject(GtObject* obj);

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
    void setCurrentProject(GtObject* obj);

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

};

#endif // GTPROJECTUI_H
