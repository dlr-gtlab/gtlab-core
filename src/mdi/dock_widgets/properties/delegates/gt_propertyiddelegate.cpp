/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPainter>
#include <QApplication>

#include "gt_propertyiddelegate.h"
#include "gt_propertymodel.h"

GtPropertyIdDelegate::GtPropertyIdDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{

}

void
GtPropertyIdDelegate::paint(QPainter* painter,
                            const QStyleOptionViewItem& option,
                            const QModelIndex& index) const
{
    bool isCategory = index.data(GtPropertyModel::CategoryRole).toBool();

    if (isCategory)
    {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    painter->save();

    QStyleOptionViewItemV3 opt = option;

    QStyledItemDelegate::paint(painter, opt, index);

    QColor color =
        static_cast<QRgb>(
            QApplication::style()->styleHint(
                QStyle::SH_Table_GridLineColor, &option));

    painter->setPen(QPen(color));

    int right = (option.direction == Qt::LeftToRight) ?
                option.rect.right() : option.rect.left();

    painter->drawLine(right, option.rect.y(), right, option.rect.bottom());

    painter->restore();
}
