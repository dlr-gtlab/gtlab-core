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
#include <QMouseEvent>
#include <QMessageBox>

#include "gt_propertymodel.h"
#include "gt_propertytreeview.h"
#include "gt_icons.h"

#include "gt_propertyiddelegate.h"

GtPropertyIdDelegate::GtPropertyIdDelegate(GtPropertyTreeView* parent) :
    QStyledItemDelegate(parent),
    m_view(parent)
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

        bool isContainer = index.data(GtPropertyModel::ContainerRole).toBool();

        if (isContainer)
        {
            painter->save();

            QStyleOptionViewItem opt = containerStyleOption(option);

            QApplication::style()->drawPrimitive(QStyle::PE_IndicatorTabClose,
                                                 &opt, painter);

            painter->restore();
        }

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

bool
GtPropertyIdDelegate::editorEvent(QEvent* event,
                                  QAbstractItemModel* model,
                                  const QStyleOptionViewItem& option,
                                  const QModelIndex& index)
{
    if (event->type() == QEvent::MouseButtonPress && index.isValid())
    {
        bool isContainer = index.data(GtPropertyModel::ContainerRole).toBool();

        if (isContainer)
        {
            const QMouseEvent* const me = static_cast<const QMouseEvent*>(event);
            const QPoint p = me->pos();

            QStyleOptionViewItem opt = containerStyleOption(option);

            if (opt.rect.contains(p))
            {
                QMessageBox mb;
                mb.setIcon(QMessageBox::Question);
                mb.setWindowTitle(tr("Delete entry?"));
                mb.setWindowIcon(gt::gui::icon::delete16());
                mb.setText(tr("Delete ") + index.data().toString() + "?");
                mb.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
                mb.setDefaultButton(QMessageBox::Cancel);
                int ret = mb.exec();

                switch (ret)
                {
                case QMessageBox::Yes:
                {
                    emit deleteRequested(index);

                    break;
                }

                default:
                    break;
                }
            }
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QStyleOptionViewItem
GtPropertyIdDelegate::containerStyleOption(
        const QStyleOptionViewItem &option) const
{
    QStyleOptionComboBox box;
    box.rect = option.rect;

    QRect r = QApplication::style()->subControlRect(QStyle::CC_ComboBox,
                                                    &box,
                                                    QStyle::SC_ComboBoxArrow);

    QStyleOptionViewItem opt = option;

    double dx = (r.width() - 10) / 2;
    double dy = (r.height() - 10) / 2;

    opt.rect = QRect(r.x() + dx, r.y() + dy, 10, 10);

    return opt;
}
