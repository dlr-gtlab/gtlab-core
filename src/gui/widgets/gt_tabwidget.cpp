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
#include <gt_qtutilities.h>

#include <QMouseEvent>
#include <QTabBar>

GtTabWidget::GtTabWidget(QWidget* parent) :
    QTabWidget(parent)
{

}

int
GtTabWidget::tabIndex(QWidget const* widget) const
{
    if (!widget) return -1;

    int n = count();
    for (int i = 0; i < n; ++i)
    {
        if (this->widget(i) == widget)
        {
            return i;
        }
    }

    return -1;
}

bool
GtTabWidget::contains(QWidget const* widget) const
{
    return tabIndex(widget) >= 0;
}

QList<QWidget*>
GtTabWidget::widgets()
{
    QList<QWidget*> list;
    int n = count();
    for (int i = 0; i < n; ++i)
    {
        if (auto* w = widget(i)) list.append(w);
    }
    return list;
}

QList<QWidget const*>
GtTabWidget::widgets() const
{
    return gt::container_const_cast(const_cast<GtTabWidget*>(this)->widgets());
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

