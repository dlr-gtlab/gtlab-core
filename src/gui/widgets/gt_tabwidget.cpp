/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 *  Created on: 25.4.2024
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */


#include "gt_tabwidget.h"

#include <gt_finally.h>

#include <QMouseEvent>
#include <QTabBar>

GtTabWidget::GtTabWidget(QWidget* parent) :
    QTabWidget(parent)
{

}

void
GtTabWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton && tabsClosable())
    {
        m_tabClickedIdx = tabBar()->tabAt(event->pos());
        if (m_tabClickedIdx >= 0)
        {
            event->accept();
            return;
        }
    }

    return QTabWidget::mousePressEvent(event);
}

void
GtTabWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_tabClickedIdx >= 0)
    {
        auto cleanup = gt::finally([this](){
            m_tabClickedIdx = -1;
        });

        // only remove tab if cursor is on the same tab
        int idx = tabBar()->tabAt(event->pos());
        if (idx == m_tabClickedIdx)
        {
            event->accept();
            emit tabCloseRequested(m_tabClickedIdx);
            return;
        }
    }

    return QTabWidget::mouseReleaseEvent(event);
}

