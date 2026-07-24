/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 */

#ifndef GT_MAINTOOLBAR_H
#define GT_MAINTOOLBAR_H

#include <gt_qmltoolbar.h>
#include <gt_mainwin.h>

/**
 * @brief The main toolbar for the gtlab gui application
 */
class GtMainToolbar : public GtQmlToolbar
{
    Q_OBJECT
public:
    explicit GtMainToolbar(GtMainWin *parent = nullptr);

    /**
     * @brief Sets these actions as the editor context actions
     *
     * To be backwards compatible, the print action is always
     * placed as well into the editor group
     *
     * Call this, whenever you want to modify the mdi toolbar actions
     */
    void setEditorActions(std::vector<GtQmlAction*> actions);

    // System context
    GtQmlAction *homeAction = {}, *newProjectAction = {}, *openProjectAction= {};

    // project context
    GtQmlAction *saveProjectAction = {}, *undoAction= {},
                *redoAction = {}, *infoAction = {};

    // edit actions
    GtQmlAction *printAction = {};

    GtQmlToolbarGroup *projectContext = {}, *editorContext = {};

public slots:
    void onProjectChanged(bool isOpen);
    void showProjectInfo();

private:
    void updateTheme();

    void makeConnections(GtMainWin*);
    bool projectHasInfo();
};

#endif // GT_MAINTOOLBAR_H
