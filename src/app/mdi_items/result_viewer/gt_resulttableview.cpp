/* GTlab - Gas Turbine laboratory
 * Source File: gt_resulttableview.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 04.08.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QKeyEvent>
#include <QMenu>
#include <QApplication>
#include <QClipboard>

#include "gt_resulttableview.h"

GtResultTableView::GtResultTableView(QWidget* parent) : QTableView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::ContiguousSelection);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(customMenuRequested(QPoint)));
}

void
GtResultTableView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_C && event->modifiers() & Qt::ControlModifier)
    {
        copyToClipboard();
    }
    else
    {
        QTableView::keyPressEvent(event);
    }
}

void
GtResultTableView::customMenuRequested(QPoint /*point*/)
{
    QModelIndexList indxs = selectionModel()->selectedIndexes();

    QMenu menu;

    QAction* copyAction = menu.addAction(tr("Copy"));

    if (indxs.isEmpty())
    {
        copyAction->setEnabled(false);
    }

    QAction* a = menu.exec(QCursor::pos());

    if (a == copyAction)
    {
        copyToClipboard();
    }
}

void
GtResultTableView::copyToClipboard()
{
    QModelIndexList indxs = selectionModel()->selectedIndexes();

    int row = -1;
    int col = -1;
    QString str;

    foreach (const QModelIndex& index, indxs)
    {
        if (row != index.row() && row >= 0)
        {
            str += QStringLiteral("\n");
            col = -1;
        }

        if (col != index.column() && col >= 0)
        {
            str += QStringLiteral("\t");
        }

        row = index.row();
        col = index.column();

        str += index.data().toString();
    }

    if (!str.isEmpty())
    {
        QApplication::clipboard()->setText(str);
    }
}
