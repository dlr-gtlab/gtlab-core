/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 1.3.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTGUIUTILITIES_H
#define GTGUIUTILITIES_H

#include <QModelIndex>

#include "gt_gui_exports.h"

class QMenu;
class QKeyEvent;
class QTreeView;
class QAbstractItemView;

class GtObject;
class GtObjectUIAction;
namespace gt
{

namespace gui
{

/**
 * @brief Appends the actions to the menu.
 * @param actions Actions to append
 * @param menu Menu to append actions to.
 * @param obj Object to pass to action methods
 * @pram parent Optional parent object used to invoke object ui actions on
 */
GT_GUI_EXPORT void addToMenu(const QList<GtObjectUIAction>& actions,
                             QMenu& menu,
                             GtObject* obj,
                             QObject* parent = {});

/**
 * @brief Appends the action to the menu.
 *
 * Usage:
 *
 *  QMenu menu(this);
 *
 *  auto action = gt::gui::makeAction("My Action Text", myActionMethod)
 *      .setIcon(...)
 *      .setVisibility(...);
 *
 *  gt::gui::addToMenu(action, menu, nullptr);
 *
 *  menu.exec(QCursor::pos());
 *
 * @param actions Action to append
 * @param menu Menu to append action to.
 * @param obj Object to pass to action method
 * @pram parent Optional parent object used to invoke object ui actions on
 */
GT_GUI_EXPORT void addToMenu(const GtObjectUIAction& action,
                             QMenu& menu,
                             GtObject* obj,
                             QObject* parent = {});

/**
 * @brief Creates a new import menu based on the registered importers and appends
 * it to the menu. Returns a pointer to new menu, which can be used to hide or
 * disable it. Ownership is already transfered to the base mneu.
 * @param menu Menu to append import menu to
 * @param obj Object to create import menu for
 * @return Pointer to menu. May be null if no importers are registered
 */
GT_GUI_EXPORT QMenu* addImportMenu(QMenu& menu, GtObject& obj);

/**
 * @brief Creates a new export menu based on the registered exporters and appends
 * it to the menu. Returns a pointer to new menu, which can be used to hide or
 * disable it. Ownership is already transfered to the base mneu.
 * @param menu Menu to append export menu to
 * @param obj Object to create import menu for
 * @return Pointer to menu. May be null if no importers are registered
 */
GT_GUI_EXPORT QMenu* addExportMenu(QMenu& menu, GtObject& obj);

/**
 * @brief Creates a default delete action, that deletes the object from the
 * datamodel (only if the object is deletable)
 * @param obj Object to create delete action for
 * @return Delete action. Note: May be empty if the object is not deletable
 */
GT_GUI_EXPORT GtObjectUIAction makeDeleteAction(GtObject& obj);

/**
 * @brief Creates a default rename action, that renames the object from the
 * datamodel (only if the object is renameable)
 * @param obj Object to rename
 * @param idx Index of the object in the view (mapped to view model)
 * @param view View object
 * @return Rename action. Note: May be empty if the object is not renameable or
 * the index is invalid
 */
GT_GUI_EXPORT GtObjectUIAction makeRenameAction(GtObject& obj,
                                                QModelIndex const& idx,
                                                QAbstractItemView& view);

/**
 * @brief Appends the ObjectUI Actions of obj to menu. Returns the number of
 * visible actions. Optionally one may pass an index and view parameter which
 * will be used for renaming the object (if its renamable). Can be used in
 * custom context menus for views that displays GtObjects.
 *
 * Usage:
 *
 *  QMenu menu(this);
 *
 *  if (gt::gui::makeObjectContextMenu(menu, *obj, index, m_view))
 *  {
 *      menu.exec(QCursor::pos());
 *  }
 *
 * @param menu Menu to append the actions to.
 * @param obj Object to fetch the object uis froms.
 * @param idx Index of the object in the view.
 * This may be used to rename the object.
 * @param view View. May be used for renaming the object.
 * @return
 */
GT_GUI_EXPORT int makeObjectContextMenu(QMenu& menu,
                                        GtObject& obj,
                                        const QModelIndex& idx = {},
                                        QAbstractItemView* view = {});

/**
 * @brief Handles a double click on the object using the correct object ui
 * (e.g opens the prefered MDI item).
 * Should be called when implementing a view that displays GtObjects and a
 * double click was triggered.
 * @param obj Object that was double clicked
 */
GT_GUI_EXPORT void handleObjectDoubleClick(GtObject& obj);

/**
 * @brief Handles the key event for a GtObject. (i.e. invokes the correct
 * action of the ObjectUI for the keyevent). Can be used in a custom
 * keyPressEvent for views that displays GtObjects. Optionally one may pass an
 * index and view parameter which are required for certain shortcuts.
 *
 * Usage:
 *
 *
 * MyWidget::keyPressEvent(QKeyEvent* event)
 * {
 *     QModelIndex index = selectedIndex(...);
 *     GtObject* obj = mapToObject(index);
 *
 *     if (!obj) return;
 *
 *     gt::gui::handleObjectKeyEvent(*event, *obj, index, m_view);
 * }
 *
 * @param event Key event.
 * @param obj Object to fetch the object uis froms.
 * @param idx Index of the object in the view.
 * @param view View. View, required for certain shortcuts
 * @return
 */
GT_GUI_EXPORT void handleObjectKeyEvent(QKeyEvent& event,
                                        GtObject& obj,
                                        const QModelIndex& idx = {},
                                        QAbstractItemView* view = {});

} // namespace gui

} // namespace gt

#endif // GTGUIUTILITIES_H
