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

class QMenu;
class GtObject;
class QAbstractItemView;

#include "gt_gui_exports.h"

namespace gt
{

namespace gui
{

/**
 * @brief Appends the ObjectUI Actions of obj to menu. Returns the number of
 * visible actions. Optionally one may pass an index and view parameter which
 * will be used for renaming the object (if its renamable).
 *
 * Usage:
 *
 *  QMenu menu(this);
 *
 *  if (gt::gui::makeObjectContextMenu(menu, *obj, mapToSource(index), m_view))
 *  {
 *      menu.exec(QCursor::pos());
 *  }
 *
 * @param menu Menu to append the actions to
 * @param obj Object to fetch the object uis froms
 * @param idx Index of the object int the view (must be mapped to source).
 * This may be used to rename the object.
 * @param view View. May be used for renaming the object.
 * @return
 */
GT_GUI_EXPORT int makeObjectContextMenu(QMenu& menu,
                                        GtObject& obj,
                                        const QModelIndex& idx = {},
                                        QAbstractItemView* view = {});

} // namespace gui

} // namespace gt

#endif // GTGUIUTILITIES_H
