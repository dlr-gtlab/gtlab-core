/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTPROCESSDOCKEXTENSIONS_P_H
#define GTPROCESSDOCKEXTENSIONS_P_H

#include "gt_processdockextensions.h"

#include <QPointer>

class QComboBox;
class QMenu;
class QStackedWidget;
class GtTask;
class GtTaskGroup;

namespace gt::gui::detail
{

    struct ProcessDockAction
    {
        QPointer<QObject> owner;
        GtObjectUIAction action;
    };

    struct ProcessDockPage
    {
        QPointer<QObject> owner;
        ProcessDockPageFactory factory;
    };

    GT_GUI_EXPORT QList<ProcessDockPage> processDockPages();
    GT_GUI_EXPORT QList<ProcessDockAction> processDockAddActions();
    GT_GUI_EXPORT QList<ProcessDockAction> processDockContextActions();

    GT_GUI_EXPORT void initializeProcessDockPages(QStackedWidget* stack,
                                                  QComboBox* selector);
    GT_GUI_EXPORT GtObject* processAddTarget(GtObject* selectedComponent,
                                             GtObject* selectedObject,
                                             GtTaskGroup* taskGroup);
    GT_GUI_EXPORT bool hasVisibleProcessDockActions(
        const QList<ProcessDockAction>& actions, GtObject* target);
    GT_GUI_EXPORT void addProcessDockActionsToMenu(
        const QList<ProcessDockAction>& actions, QMenu& menu, GtObject* target);

} // namespace gt::gui::detail

#endif // GTPROCESSDOCKEXTENSIONS_P_H
