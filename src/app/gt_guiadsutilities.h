/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef GT_GUIADSUTILITIES_H
#define GT_GUIADSUTILITIES_H

#include <qtadvanceddocking-qt5/DockWidget.h>
#include <qtadvanceddocking-qt5/DockAreaWidget.h>
#include <qtadvanceddocking-qt5/DockAreaTitleBar.h>

#include <QPushButton>

namespace gt
{
    namespace gui
    {
        inline
        void applyDefaultStyle(ads::CDockWidget& dock)
        {
            dock.setFrameShape(QFrame::NoFrame);
            auto* scrollArea =
                dock.findChild<QFrame*>(QStringLiteral("dockWidgetScrollArea"),
                                          Qt::FindDirectChildrenOnly);
            if (scrollArea) scrollArea->setFrameShape(QFrame::NoFrame);

            auto* area = dock.dockAreaWidget();
            if (!area) return;

            auto* titleBar = area->titleBar();
            assert(titleBar);

            auto* tabButton = titleBar->findChild<QPushButton*>(
                QStringLiteral("tabCloseButton"));
            if (tabButton)
            {
                tabButton->setFlat(true);
                tabButton->resize(tabButton->height(), tabButton->height());
            }
        }

    } // namespace gui

} // namespace gt

#endif // GT_GUIADSUTILITIES_H

