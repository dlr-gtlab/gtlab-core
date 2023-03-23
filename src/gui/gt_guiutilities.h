/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
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

namespace gt
{

namespace gui
{

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
