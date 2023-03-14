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
#include "gt_icons.h"
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
    if (selectionModel())
    {
        QList<QModelIndex> indexes = selectionModel()->selectedIndexes();

        if (indexes.size() == 2)
        {
            QModelIndex index = indexes.first();

            if (gtApp->compareKeyEvent(event, "runProcess", "GtProcessDock"))
            {
                if (index.isValid())
                {
                    emit runTaskElement(index);
                    event->accept();
                    return;
                }
            }

            if (gtApp->compareKeyEvent(event, "copy", "Core"))
            {
                if (index.isValid())
                {
                    emit copyProcessElement(index);
                    event->accept();
                    return;
                }
            }
            if (gtApp->compareKeyEvent(event, "paste", "Core"))
            {
                emit pasteProcessElement(index);
                return;
            }

            if (gtApp->compareKeyEvent(event, "cut", "Core"))
            {
                if (index.isValid())
                {
                    emit cutProcessElement(index);
                    event->accept();
                    return;
                }
            }

            if (gtApp->compareKeyEvent(event, "skipProcess", "GtProcessDock"))
            {
                if (index.isValid())
                {
                    emit skipCalcultorElement(index, true);
                    event->accept();
                    return;
                }
            }

            if (gtApp->compareKeyEvent(event, "unskipProcess", "GtProcessDock"))
            {
                if (index.isValid())
                {
                    emit skipCalcultorElement(index, false);
                    event->accept();
                    return;
                }
            }

            if (gtApp->compareKeyEvent(event, "rename", "Core"))
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
            return;
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
        if (!gtApp->currentProject())
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

    int stateIconSpace = 2 * gt::gui::icon::standardSizeSmall().width();

    setColumnWidth(0, width() - stateIconSpace - delta - 10);
}
