/* GTlab - Gas Turbine laboratory
 * Source File: gt_localcollectionwidget.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 07.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QTreeView>
#include <QVBoxLayout>
#include <QMenu>

#include "gt_localcollectionmodel.h"
#include "gt_collectionitem.h"
#include "gt_collectionitemwidget.h"
#include "gt_icons.h"
#include "gt_logging.h"

#include "gt_localcollectionwidget.h"

GtLocalCollectionWidget::GtLocalCollectionWidget(QWidget* parent) :
    QWidget(parent), m_contextMenuEnabled(true)
{
    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);

    m_view = new QTreeView;

    m_model = new GtLocalCollectionModel(this);
    m_view->setModel(m_model);

    m_view->setColumnWidth(0, 300);
    m_view->setColumnWidth(1, 25);
    m_view->setRootIsDecorated(false);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);

    lay->addWidget(m_view);

    setLayout(lay);

    connect(m_view, SIGNAL(clicked(QModelIndex)),
            SLOT(onItemClicked(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(onCustomContextMenuRequested(QPoint)));
    connect(m_view->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(onCurrentChanged(QModelIndex,QModelIndex)),
            Qt::UniqueConnection);
    connect(m_view, SIGNAL(doubleClicked(QModelIndex)),
            SIGNAL(itemDoubleClicked(QModelIndex)));
}

void
GtLocalCollectionWidget::setItems(const QList<GtCollectionItem>& items)
{
    m_model->setItems(items);
}

GtCollectionItem
GtLocalCollectionWidget::getItem(const QModelIndex& index)
{
    return m_model->itemFromIndex(index);
}

void
GtLocalCollectionWidget::setCustomContextMenuEnabled(bool val)
{
    m_contextMenuEnabled = val;
}

void
GtLocalCollectionWidget::setShowItemInfo(bool val)
{
    if (!m_model)
    {
        return;
    }

    m_model->setShowInfoColumns(val);

    if (val)
    {
        m_view->setColumnWidth(0, 300);
        m_view->setColumnWidth(1, 25);
    }
    else
    {
        m_view->resizeColumnToContents(0);
    }
}

QModelIndex
GtLocalCollectionWidget::getModelIndex(int row)
{
    return m_model->index(row);
}

void
GtLocalCollectionWidget::setCurrentIndex(const QModelIndex& index)
{
    m_view->setCurrentIndex(index);
}

void
GtLocalCollectionWidget::onItemClicked(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    if (index.column() != 1)
    {
        emit itemSelected(index);
        return;
    }

    GtCollectionItem item = m_model->itemFromIndex(index);

    if (!item.isValid())
    {
        return;
    }

    gtDebug() << "item clicked!";

    GtCollectionItemWidget* widget = new GtCollectionItemWidget(item);

    widget->show();
}

void
GtLocalCollectionWidget::onCustomContextMenuRequested(const QPoint& /*pos*/)
{
    if (!m_contextMenuEnabled)
    {
        return;
    }

    QModelIndex index = m_view->currentIndex();

    if (!index.isValid())
    {
        return;
    }

    GtCollectionItem item = m_model->itemFromIndex(index);

    if (!item.isValid())
    {
        return;
    }

    QMenu menu;

    QAction* deleteAction = menu.addAction(
                GtGUI::Icon::delete16(),
                tr("Uninstall"));

    QAction* a = menu.exec(QCursor::pos());

    if (a == deleteAction)
    {
        gtDebug() << "deleting item...";
        gtDebug() << "  |-> " << item.localPath();
        gtDebug() << "  |-> " << index;

        if (!m_model->uninstallItem(index))
        {
            gtError() << tr("Could not uninstall collection item!");
        }
    }
}

void
GtLocalCollectionWidget::onCurrentChanged(const QModelIndex& current,
                                          const QModelIndex& /*previous*/)
{
    emit itemSelected(current);
}
