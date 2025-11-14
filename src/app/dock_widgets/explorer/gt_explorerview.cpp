/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 31.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QScrollBar>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QHeaderView>

#include "gt_projectprovider.h"
#include "gt_project.h"
#include "gt_datamodel.h"
#include "gt_logging.h"
#include "gt_application.h"

#include "gt_explorerview.h"

#include <QTimer>

GtExplorerView::GtExplorerView(QWidget* parent) :
    GtTreeView(parent)
{
    setHeaderHidden(true);
    setAnimated(true);
    setAlternatingRowColors(true);
    setDragEnabled(true);
    setAcceptDrops(true);
    setSelectionMode(ExtendedSelection);
    setContextMenuPolicy(Qt::CustomContextMenu);

    setDragDropMode(DragDrop);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // to allow smooth scrolling
    QScrollBar* hBar = horizontalScrollBar();
    assert(hBar);

    m_hScrollAnim = new QPropertyAnimation(hBar, "value", this);
    m_hScrollAnim->setDuration(200);
    m_hScrollAnim->setEasingCurve(QEasingCurve::InOutQuad);
}

void
GtExplorerView::resizeEvent(QResizeEvent* event)
{
    setColumnWidth(1, 0);

    QTreeView::resizeEvent(event);
}

void
GtExplorerView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QModelIndex index = indexAt(event->pos());

    if (!index.isValid())
    {
        return GtTreeView::mouseDoubleClickEvent(event);
    }

    if (isExpanded(index))
    {
        event->ignore();
    }
    else
    {
        GtTreeView::mouseDoubleClickEvent(event);
    }

    emit mdiItemRequested(index);
}

void
GtExplorerView::keyPressEvent(QKeyEvent *event)
{
    GtTreeView::keyPressEvent(event);
}

void
GtExplorerView::dragEnterEvent(QDragEnterEvent* event)
{
    if (checkMimeData(event->mimeData()))
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void
GtExplorerView::dropEvent(QDropEvent* event)
{
    gtDebug() << "dropEvent...";

    auto urls = event->mimeData()->urls();

    if (urls.size() != 1)
    {
        gtDebug() << "url size != 1";
        return;
    }

    foreach (auto url, urls)
    {
        if (!url.isLocalFile())
        {
            gtDebug() << "url not a local file!";
            continue;
        }

        gtDebug() << "local file found!";
        gtDebug() << "  |-> " << url.fileName();
        gtDebug() << "  |-> " << url.path();

        QFile file(url.toLocalFile());

        if (file.exists())
        {
            gtDebug() << "  |-> is existing file!";

            GtProjectProvider provider(url.toLocalFile());
            GtProject* project = provider.project();

            if (!gtDataModel->newProject(project, true))
            {
                delete project;
            }
        }
    }
}

void
GtExplorerView::dragMoveEvent(QDragMoveEvent* event)
{
    if (checkMimeData(event->mimeData()))
    {
        event->accept();
    }
    else
    {
        GtTreeView::dragMoveEvent(event);
    }
}

bool
GtExplorerView::checkMimeData(const QMimeData* mimeData)
{
    auto urls = mimeData->urls();

    if (urls.size() != 1)
    {
        return false;
    }

    auto url = urls.first();

    if (!url.isLocalFile())
    {
        gtDebug() << "url not a local file!";
        return false;
    }

    QFile file(url.toLocalFile());

    if (!file.exists())
    {
        gtDebug() << "file does not exist!";
        return false;
    }

    QFileInfo fileInfo(file);

    if (fileInfo.suffix() != QStringLiteral("gtlab"))
    {
        gtDebug() << "file not a GTlab project!";
        return false;
    }

    return true;
}

void
GtExplorerView::scrollTo(const QModelIndex& index, ScrollHint hint)
{
    if (!index.isValid())
    {
        QTreeView::scrollTo(index, hint);
        return;
    }

    QScrollBar* hBar = horizontalScrollBar();
    assert(hBar);

    const int viewWidth = viewport()->width();
    const QRect rect = visualRect(index);

    if (!rect.isValid())
    {
        // Fallback to default behavior if we can't determine the item rect
        QTreeView::scrollTo(index, hint);
        return;
    }

    const int indent = indentation();

    int targetX = hBar->value();

    const int itemLeft = rect.x();
    const int itemRight = rect.right();
    const int itemWidth = rect.width();

    // Compute a horizontal target so the item becomes fully visible if possible
    if (itemLeft - indent < 0)
    {
        targetX += itemLeft - indent;
    }
    else if (itemRight > viewWidth)
    {
        if (itemWidth + indent < viewWidth)
            targetX += itemRight - viewWidth;
        else
            targetX += itemLeft - indent;
    }

    targetX = gt::clamp(targetX, hBar->minimum(), hBar->maximum());

    const auto startX = hBar->value();

    // Let QTreeView handle vertical scrolling (and its own logic) first
    QTreeView::scrollTo(index, hint);

    // Restore original horizontal value to neutralize QTreeView's adjustment
    hBar->setValue(startX);

    if (targetX == hBar->value())
        return;

    // Smoothly animate from the original to the computed horizontal position
    m_hScrollAnim->stop();
    m_hScrollAnim->setStartValue(startX);
    m_hScrollAnim->setEndValue(targetX);
    m_hScrollAnim->start();
}
