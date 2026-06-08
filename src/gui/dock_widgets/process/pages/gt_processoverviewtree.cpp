/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processoverviewtree.cpp
 *
 *  Created on: 03.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QScrollBar>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>

#include "gt_treefiltermodel.h"
#include "gt_processoverviewmodel.h"
#include "gt_abstractprocessitem.h"
#include "gt_icons.h"

#include "gt_processoverviewtree.h"

GtProcessButtonDelegate::GtProcessButtonDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{
}

void
GtProcessButtonDelegate::paint(QPainter* painter,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index) const
{
    bool isButton = index.data(GtProcessOverviewModel::ButtonRole).isValid();

    if (isButton)
    {
        QStyledItemDelegate::paint(painter, option, index);

        QRect rect = option.rect;
        QIcon infoIcon = gt::gui::icon::info();
        infoIcon.paint(painter, rect, Qt::AlignCenter);

        return;
    }

    QStyledItemDelegate::paint(painter, option, index);
}

bool
GtProcessButtonDelegate::editorEvent(QEvent* event,
                                     QAbstractItemModel* model,
                                     const QStyleOptionViewItem& option,
                                     const QModelIndex& index)
{
    if (event->type() != QEvent::MouseButtonPress || !index.isValid())
    {
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

    bool isButton = index.data(GtProcessOverviewModel::ButtonRole).isValid();

    if (!isButton)
    {
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }

    const QMouseEvent* const me = static_cast<const QMouseEvent*>(event);
    const QPoint p = me->pos();

    if (option.rect.contains(p))
    {
        auto* item = qobject_cast<GtAbstractProcessItem*>(index.data(
                GtProcessOverviewModel::ButtonRole).value<QObject*>());

        if (item)
        {
            emit buttonClicked(item, me->globalPos());
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

GtProcessOverviewTree::GtProcessOverviewTree(GtProcessOverviewModel* model,
                                             QWidget* parent) :
    GtTreeView(parent),
    m_model(nullptr),
    m_filterModel(nullptr),
    m_buttonDelegate(nullptr)
{
    setAnimated(true);
    setRootIsDecorated(false);

    m_model = model;

    if (m_model)
    {
        m_model->setUpModel();
        m_filterModel = new GtTreeFilterModel(m_model);
        m_filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        m_filterModel->setSourceModel(m_model);
        setModel(m_filterModel);

        m_buttonDelegate = new GtProcessButtonDelegate(this);
        setItemDelegateForColumn(1, m_buttonDelegate);

        connect(m_buttonDelegate,
                SIGNAL(buttonClicked(GtAbstractProcessItem*, const QPoint&)),
                this,
                SLOT(onButtonClicked(GtAbstractProcessItem*, const QPoint&)));
    }

    connect(this, SIGNAL(collapsed(QModelIndex)),
            SLOT(onCollapsed(QModelIndex)));
    connect(this, SIGNAL(expanded(QModelIndex)),
            SLOT(onExpanded(QModelIndex)));
}

void
GtProcessOverviewTree::resizeColumns()
{
    setColumnWidth(1, 20);

    int delta = 0;

    if (verticalScrollBar()->isVisible())
    {
        delta = verticalScrollBar()->width();
    }

    setColumnWidth(0, width() - 50 - delta);
}

GtTreeFilterModel*
GtProcessOverviewTree::filterModel()
{
    return m_filterModel;
}

GtProcessOverviewModel*
GtProcessOverviewTree::sourceModel()
{
    return m_model;
}

void
GtProcessOverviewTree::sortAlphabetically()
{
    setSortingEnabled(true);
    m_filterModel->sort(0, Qt::AscendingOrder);
    setSortingEnabled(false);
}

void
GtProcessOverviewTree::resizeEvent(QResizeEvent* event)
{
    resizeColumns();

    QTreeView::resizeEvent(event);
}

void
GtProcessOverviewTree::setCollapsed(const QModelIndex& index, bool val)
{
    if (!model()) return;

    bool isCategory = index.data(GtProcessOverviewModel::CategoryRole).toBool();

    if (isCategory)
    {
        model()->setData(index, val, GtProcessOverviewModel::CollapseRole);
    }
}

void
GtProcessOverviewTree::onCollapsed(const QModelIndex& index)
{
    setCollapsed(index, true);
}

void
GtProcessOverviewTree::onExpanded(const QModelIndex& index)
{
    setCollapsed(index, false);
}

void
GtProcessOverviewTree::onButtonClicked(GtAbstractProcessItem* item,
                                       const QPoint& globalPos)
{
    if (m_model)
    {
        m_model->onButtonClicked(item, globalPos);
    }
}
