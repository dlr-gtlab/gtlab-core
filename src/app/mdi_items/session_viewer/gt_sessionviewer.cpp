/* GTlab - Gas Turbine laboratory
 * Source File: gt_sessionviewer.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QTreeView>
#include <QVBoxLayout>

#include "gt_icons.h"
#include "gt_datamodel.h"
#include "gt_styledmodel.h"

#include "gt_sessionviewer.h"

GtSessionViewer::GtSessionViewer()
{
    setObjectName("Session Viewer");

    QTreeView* view = new QTreeView(widget());
    view->setHeaderHidden(true);
    view->setDragEnabled(true);
    view->setDragDropMode(QTreeView::DragOnly);

    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    widget()->setLayout(lay);

    lay->addWidget(view);


    GtStyledModel* styledModel = new GtStyledModel(this);
    styledModel->setSourceModel(gtDataModel);
    view->setModel(styledModel);
}

QIcon
GtSessionViewer::icon() const
{
    return gt::gui::icon::session();
}
