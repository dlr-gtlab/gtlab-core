/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_stateviewer.cpp
 *
 *  Created on: 28.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QVBoxLayout>

#include "gt_icons.h"
#include "gt_statemodel.h"
#include "gt_statehandler.h"
#include "gt_treeview.h"

#include "gt_stateviewer.h"

GtStateViewer::GtStateViewer()
{
    setObjectName(tr("State Viewer"));

    m_tree = new GtTreeView(widget());
    m_tree->setAlternatingRowColors(true);
    m_tree->setFrameStyle(QFrame::NoFrame);

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
    return gt::gui::icon::listFormatted();
}

void
GtStateViewer::onThemeChanged()
{
    /// nothing to do here
}

void
GtStateViewer::reloadModel()
{
    m_model->reload();
    m_tree->expandAll();
}
