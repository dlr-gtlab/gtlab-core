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
