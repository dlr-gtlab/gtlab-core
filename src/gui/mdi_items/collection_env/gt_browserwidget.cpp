/* GTlab - Gas Turbine laboratory
 * Source File: gt_browserwidget.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 07.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QTreeView>
#include <QVBoxLayout>
#include <QCheckBox>

#include "gt_collectionbrowsermodel.h"
#include "gt_collectionitemwidget.h"
#include "gt_browserwidget.h"

GtBrowserWidget::GtBrowserWidget(QWidget* parent) :
    GtAbstractBrowserWidget(parent)
{
    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);

    m_view = new QTreeView;

    m_model = new GtCollectionBrowserModel(this);
    m_view->setModel(m_model);

    m_view->setColumnWidth(0, 300);
    m_view->setColumnWidth(1, 25);
//    view->setRootIsDecorated(false);

    lay->addWidget(m_view);

    setLayout(lay);

    connect(m_view, SIGNAL(clicked(QModelIndex)),
            SLOT(onItemClicked(QModelIndex)));

    connect(m_model, SIGNAL(selectionChanged()), SLOT(onItemSelectionChange()));
}

void
GtBrowserWidget::setItems(const QList<GtCollectionNetworkItem>& installedItems,
                          const QList<GtCollectionNetworkItem>& availableItems,
                          const QList<GtCollectionNetworkItem>& updateAvailableItems)
{
    m_model->setCollectionData(installedItems,
                               availableItems,
                               updateAvailableItems);

    m_view->expandAll();
}

QList<GtCollectionNetworkItem>
GtBrowserWidget::itemsToDownload()
{
    return m_model->selectedItems();
}

QList<GtCollectionNetworkItem>
GtBrowserWidget::itemsToUpdate()
{
    return m_model->itemsToUpdate();
}

void
GtBrowserWidget::selectAllItems()
{
    m_model->selectAll();
    m_view->expandAll();
}

void
GtBrowserWidget::unselectAllItems()
{
    m_model->unselectAll();
    m_view->expandAll();
}

void
GtBrowserWidget::onItemClicked(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    if (index.column() != 1)
    {
        return;
    }

    GtCollectionItem item = m_model->itemFromIndex(index);

    if (!item.isValid())
    {
        return;
    }

    auto* widget = new GtCollectionItemWidget(item, this);

    widget->show();
}

void
GtBrowserWidget::onItemSelectionChange()
{
    emit selectionChanged();
}

