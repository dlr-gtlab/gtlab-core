/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
#include "gt_style.h"

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
        bool isReadonly = index.data(GtPropertyModel::ReadOnlyRole).toBool();

        // draw delete button
        if (isContainer && !isReadonly)
        {
            painter->save();

            QStyleOptionViewItem opt = containerStyleOption(option);

            auto const& deleteIcon = QApplication::style()->standardIcon(
                QStyle::SP_DialogCloseButton);
            deleteIcon.paint(painter, opt.rect);

            painter->restore();
        }

        return;
    }

    QStyledItemDelegate::paint(painter, option, index);
}

bool
GtPropertyIdDelegate::editorEvent(QEvent* event,
                                  QAbstractItemModel* model,
                                  const QStyleOptionViewItem& option,
                                  const QModelIndex& index)
{
    if (event->type() != QEvent::MouseButtonPress && !index.isValid())
    {
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

    bool isContainer = index.data(GtPropertyModel::ContainerRole).toBool();
    bool isReadonly = index.data(GtPropertyModel::ReadOnlyRole).toBool();

    if (!isContainer || isReadonly)
    {
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

    const QMouseEvent* const me = static_cast<const QMouseEvent*>(event);
    const QPoint p = me->pos();

    QStyleOptionViewItem opt = containerStyleOption(option);

    if (opt.rect.contains(p))
    {
        QMessageBox mb;
        mb.setIcon(QMessageBox::Question);
        mb.setWindowTitle(tr("Delete entry?"));
        mb.setWindowIcon(gt::gui::icon::delete_());
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

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QStyleOptionViewItem
GtPropertyIdDelegate::containerStyleOption(
        const QStyleOptionViewItem &option) const
{
    QStyleOptionComboBox box;
    box.rect = option.rect;

    QRect r = QApplication::style()->subControlRect(QStyle::CC_ComboBox, &box,
                                                    QStyle::SC_ComboBoxArrow);

    QStyleOptionViewItem opt = option;

    // scale down rect, otherwise the icon would be too big
    constexpr const double scale = 0.75;
    constexpr const double offset = (1.0 - scale) / 2;
    double dx = (r.width() * offset);
    double dy = (r.height() * offset);

    double const size = opt.rect.height() * scale;
    opt.rect = QRect(r.x() + dx, r.y() + dy, size, size);

    return opt;
}
