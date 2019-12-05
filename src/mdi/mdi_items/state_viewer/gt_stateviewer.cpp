/* GTlab - Gas Turbine laboratory
 * Source File: gt_stateviewer.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QTreeView>
#include <QVBoxLayout>

#include "gt_application.h"
#include "gt_statemodel.h"
#include "gt_statehandler.h"

#include "gt_stateviewer.h"

GtStateViewer::GtStateViewer()
{
    setObjectName(tr("State Viewer"));

    m_tree = new QTreeView(widget());
    m_tree->setAlternatingRowColors(true);

    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    widget()->setLayout(lay);

    lay->addWidget(m_tree);

    m_model = new GtStateModel(this);
    m_tree->setModel(m_model);

    m_tree->expandAll();
    m_tree->setColumnWidth(0, 300);
    m_tree->setColumnWidth(1, 150);
    m_tree->setColumnWidth(2, 100);

    connect(gtStateHandler, SIGNAL(dataChanged(GtObject*)),
            SLOT(reloadModel()));
}

QIcon
GtStateViewer::icon() const
{
    return gtApp->icon("devModeOnly/stateIcon.png");
}

void
GtStateViewer::reloadModel()
{
    m_model->reload();
    m_tree->expandAll();
}
