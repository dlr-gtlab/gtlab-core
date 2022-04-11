/* GTlab - Gas Turbine laboratory
 * Source File: gt_labelusagedelegate.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 22.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPainter>
#include <QApplication>

#include "gt_labelusagedelegate.h"

GtLabelUsageDelegate::GtLabelUsageDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{

}

void
GtLabelUsageDelegate::paint(QPainter* painter,
                            const QStyleOptionViewItem& option,
                            const QModelIndex& index) const
{
    option.widget->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option,
                                          painter, option.widget);

    QString str = index.data().toString();

    QStringList subStrs = str.split(QStringLiteral(";"));
    subStrs.removeFirst();

    QFont font = painter->font();
    QFontMetrics fm(font);

    int lineHeight = fm.height();

    const QRect rect = option.rect;
    int current = 0;

    foreach (const QString& s, subStrs)
    {

        int lineWidth = fm.width(s);

        const QRect strRect(rect.left() + current,
                            rect.top(),
                            lineWidth,
                            lineHeight);

        painter->drawText(strRect, s);

        // arrow

        if (s == subStrs.last())
        {
            return;
        }

        int arrow = 8;
        int pad = (rect.height() - arrow) / 2;
        QStyleOption arrowOption;
        arrowOption = option;
        arrowOption.rect = QRect(rect.left() + current + lineWidth + 4,
                                 rect.top() + pad, arrow, arrow);
        QApplication::style()->drawPrimitive(QStyle::PE_IndicatorArrowRight,
                                             &arrowOption, painter);

        current += lineWidth + 16;
    }
}
