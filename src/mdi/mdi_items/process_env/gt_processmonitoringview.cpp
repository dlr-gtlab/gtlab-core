/* GTlab - Gas Turbine laboratory
 * Source File: gt_processmonitoringview.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 17.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPainter>
#include <QApplication>
#include <QScrollBar>
#include <QMouseEvent>
#include <QDebug>

#include "gt_processmonitoringmodel.h"
#include "gt_processmonitoringdelegate.h"

#include "gt_processmonitoringview.h"

GtProcessMonitoringView::GtProcessMonitoringView(QWidget* parent) :
    GtTreeView(parent)
{
    GtProcessMonitoringDelegate* del = new GtProcessMonitoringDelegate(this);
    setItemDelegateForColumn(0, del);
    setItemDelegateForColumn(1, del);
    setItemDelegateForColumn(2, del);
}

void
GtProcessMonitoringView::resizeColumns()
{
    const int fcw = columnWidth(0);
    resizeColumnToContents(0);

    if (model() != Q_NULLPTR && model()->columnCount() == 1)
    {
        return;
    }

    setColumnWidth(0, columnWidth(0) + 40);

    if (columnWidth(0) < fcw)
    {
        setColumnWidth(0, fcw);
    }

    const int uwidth = 100;
    setColumnWidth(2, uwidth);

    const int swidth = 70;
    setColumnWidth(3, swidth);

    const int iwidth = columnWidth(0);

    int delta = 0;

    if (verticalScrollBar()->isVisible())
    {
        delta = verticalScrollBar()->width();
    }

    int sum = width() - iwidth - uwidth - swidth - delta;

    if (sum < 30)
    {
        sum = 30;
    }

    setColumnWidth(1, sum);
}

void
GtProcessMonitoringView::mouseMoveEvent(QMouseEvent* event)
{
//    QPoint pos = event->pos();

//    QModelIndex index = indexAt(pos);

//    if (index.isValid())
//    {
//        bool isProperty =
//                index.data(GtProcessMonitoringModel::PropertyRole).toBool();

//        if (isProperty)
//        {
//            qDebug() << index;
//            QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
//            GtTreeView::mouseMoveEvent(event);
//            return;
//        }
//    }

//    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));

    GtTreeView::mouseMoveEvent(event);
}

void
GtProcessMonitoringView::drawRow(QPainter* painter,
                                 const QStyleOptionViewItem& option,
                                 const QModelIndex& index) const
{
    QStyleOptionViewItemV3 opt = option;

    // check whether index has property role
    bool isProperty =
            index.data(GtProcessMonitoringModel::PropertyRole).toBool();

    if (isProperty)
    {
        opt.palette.setColor(QPalette::AlternateBase, QColor(Qt::white));
    }
    else
    {
        const QColor c(240, 240, 240);
        painter->fillRect(option.rect, c);
//        opt.palette.setColor(QPalette::AlternateBase, c.lighter(104));
    }

    GtTreeView::drawRow(painter, opt, index);

    QColor color = static_cast<QRgb>(QApplication::style()->styleHint(
                                         QStyle::SH_Table_GridLineColor, &opt));
    painter->save();
    painter->setPen(QPen(color));
    painter->drawLine(opt.rect.x(), opt.rect.bottom(),
                      opt.rect.right(), opt.rect.bottom());
    painter->restore();
}

void
GtProcessMonitoringView::resizeEvent(QResizeEvent* event)
{
    resizeColumns();

    GtTreeView::resizeEvent(event);
}
