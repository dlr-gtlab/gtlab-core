/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.07.2026
 *  Author: AI
 *  Tel.: +49 2203 601 2907
 */

#include "gt_taskgroupcomboboxdelegate.h"

#include <QPainter>
#include <QStyleOptionButton>
#include <QStyle>
#include <QApplication>
#include <QMouseEvent>

#include "gt_taskgroupmodel.h"

GtTaskGroupComboBoxDelegate::GtTaskGroupComboBoxDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{
}

QWidget*
GtTaskGroupComboBoxDelegate::createEditor(QWidget* parent,
                                          const QStyleOptionViewItem& /*option*/,
                                          const QModelIndex& /*index*/) const
{
    return nullptr;
}

void
GtTaskGroupComboBoxDelegate::setEditorData(QWidget* /*editor*/,
                                           const QModelIndex& /*index*/) const
{
}

void
GtTaskGroupComboBoxDelegate::setModelData(QWidget* /*editor*/,
                                          QAbstractItemModel* /*model*/,
                                          const QModelIndex& /*index*/) const
{
}

void
GtTaskGroupComboBoxDelegate::paint(QPainter* painter,
                                   const QStyleOptionViewItem& option,
                                   const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    const GtTaskGroupModel* model = qobject_cast<const GtTaskGroupModel*>(index.model());
    if (!model)
    {
        return;
    }

    bool isDeletable = index.data(GtTaskGroupModel::IsDeletableCustomGroupRole).toBool();
    if (!isDeletable)
    {
        return;
    }

    const QStyle* style = QApplication::style();
    QStyleOptionButton deleteButton;
    deleteButton.rect = option.rect.adjusted(option.rect.width() - 25, 2, -2, -2);
    deleteButton.text = tr("X");
    deleteButton.state = QStyle::State_Enabled;

    if (option.state & QStyle::State_MouseOver)
    {
        deleteButton.state |= QStyle::State_MouseOver;
    }

    if (option.state & QStyle::State_Selected)
    {
        deleteButton.palette.setBrush(QPalette::Button,
                                      option.palette.highlight());
    }

    style->drawControl(QStyle::CE_PushButton, &deleteButton, painter, nullptr);
}

bool
GtTaskGroupComboBoxDelegate::editorEvent(QEvent* event,
                                         QAbstractItemModel* model,
                                         const QStyleOptionViewItem& option,
                                         const QModelIndex& index)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        const QRect buttonRect = option.rect.adjusted(option.rect.width() - 25, 2, -2, -2);

        if (buttonRect.contains(mouseEvent->pos()))
        {
            emit deleteRequested(index);
            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QSize
GtTaskGroupComboBoxDelegate::sizeHint(const QStyleOptionViewItem& option,
                                      const QModelIndex& index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(qMax(size.height(), 20));
    return size;
}