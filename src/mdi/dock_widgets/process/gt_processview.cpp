/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 02.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QScrollBar>
#include <QKeyEvent>

#include "gt_application.h"

#include "gt_processview.h"

GtProcessView::GtProcessView(QWidget* parent) : GtTreeView(parent)
{
    setHeaderHidden(true);
    setAnimated(true);
    setAlternatingRowColors(true);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropMode(InternalMove);
    setDefaultDropAction(Qt::MoveAction);
    setExpandsOnDoubleClick(false);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void
GtProcessView::resizeEvent(QResizeEvent* event)
{
    resizeColumns();

    QTreeView::resizeEvent(event);
}

void
GtProcessView::keyPressEvent(QKeyEvent* event)
{
    if (selectionModel() != Q_NULLPTR)
    {
        QList<QModelIndex> indexes = selectionModel()->selectedIndexes();

        if (indexes.size() == 2)
        {
            QModelIndex index = indexes.first();

            if (gtApp->compareKeyEvent(event, "runProcess"))
            {
                if (index.isValid())
                {
                    emit runTaskElement(index);
                    event->accept();
                    return;
                }
            }

            if (gtApp->compareKeyEvent(event, "copy"))
            {
                if (index.isValid())
                {
                    emit copyProcessElement(index);
                    event->accept();
                    return;
                }
            }
            if (gtApp->compareKeyEvent(event, "paste"))
            {
                emit pasteProcessElement(index);
            }

            if (gtApp->compareKeyEvent(event, "cut"))
            {
                if (index.isValid())
                {
                    emit cutProcessElement(index);
                    event->accept();
                    return;
                }
            }

            if (gtApp->compareKeyEvent(event, "skipProcess"))
            {
                if (index.isValid())
                {
                    emit skipCalcultorElement(index, true);
                    event->accept();
                    return;
                }
            }

            if (gtApp->compareKeyEvent(event, "unskipProcess"))
            {
                if (index.isValid())
                {
                    emit skipCalcultorElement(index, false);
                    event->accept();
                    return;
                }
            }

            if (gtApp->compareKeyEvent(event, "rename"))
            {
                if (index.isValid())
                {
                    emit renameProcessElement(index);
                    event->accept();
                    return;
                }
            }
        }

        if (gtApp->compareKeyEvent(event, "delete"))
        {
            emit deleteProcessElements(indexes);
        }
    }

    GtTreeView::keyPressEvent(event);
}

void
GtProcessView::mousePressEvent(QMouseEvent* event)
{
    QTreeView::mousePressEvent(event);

    QModelIndex index = indexAt(event->pos());

    if (!index.isValid())
    {
        if (gtApp->currentProject() == Q_NULLPTR)
        {
            return;
        }

        clearSelection();
        selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
        emit clicked(index);
    }
}

void
GtProcessView::resizeColumns()
{
    setColumnWidth(1, 20);

    int delta = 0;

    if (verticalScrollBar()->isVisible())
    {
        delta = verticalScrollBar()->width();
    }

    setColumnWidth(0, width() - 33 - delta);
}
