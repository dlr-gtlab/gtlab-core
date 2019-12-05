/* GTlab - Gas Turbine laboratory
 * Source File: gt_processmonitoringdelegate.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 17.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QApplication>
#include <QPainter>

#include "gt_processmonitoringdelegate.h"

GtProcessMonitoringDelegate::GtProcessMonitoringDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{

}

QSize
GtProcessMonitoringDelegate::sizeHint(const QStyleOptionViewItem& option,
                                      const QModelIndex& index) const
{
    if (index.column() > 0)
    {
        const int totalWidth = 50;
        const int totalHeight = 22;

        return QSize(totalWidth, totalHeight);
    }

    return QStyledItemDelegate::sizeHint(option, index);
}

void
GtProcessMonitoringDelegate::paint(QPainter* painter,
                                   const QStyleOptionViewItem& option,
                                   const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();

    QColor color = static_cast<QRgb>(
                       QApplication::style()->styleHint(
                           QStyle::SH_Table_GridLineColor, &option));
    painter->setPen(QPen(color));

    int right =
        (option.direction ==
         Qt::LeftToRight) ? option.rect.right() : option.rect.left();

    painter->drawLine(right, option.rect.y(), right, option.rect.bottom());

    painter->restore();
}
