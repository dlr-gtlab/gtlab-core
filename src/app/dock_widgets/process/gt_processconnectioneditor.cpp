/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processconnectioneditor.cpp
 *
 *  Created on: 19.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QVBoxLayout>
#include <QPushButton>

#include "gt_icons.h"
#include "gt_processconnectionmodel.h"
#include "gt_processconnectionview.h"
#include "gt_processconnectiongraphicsview.h"
#include "gt_task.h"
#include "gt_objectmemento.h"

#include "gt_processconnectioneditor.h"

GtProcessConnectionEditor::GtProcessConnectionEditor(GtTask* task,
                                                     QWidget* parent) :
    GtDialog(parent),
    m_task(nullptr)
{
    setWindowTitle(tr("Connections"));

    setWindowIcon(gt::gui::icon::connection());

    QVBoxLayout* lay = new QVBoxLayout;

    QHBoxLayout* mainLay = new QHBoxLayout;

    mainLay->setSpacing(0);

    m_outputView = new GtProcessConnectionView;

    m_outputModel = new GtProcessConnectionModel(this);
    m_outputView->setModel(m_outputModel);
    m_outputView->setHeaderHidden(true);


    m_inputView = new GtProcessConnectionView;

    m_inputModel = new GtProcessConnectionModel(this);
    m_inputView->setModel(m_inputModel);
    m_inputModel->setMode(GtProcessConnectionModel::WRITE_ONLY);
    m_inputView->setHeaderHidden(true);

    m_connectionView = new GtProcessConnectionGraphicsView(m_outputView,
                                                           m_inputView);
    m_connectionView->setFrameStyle(GtProcessConnectionGraphicsView::NoFrame);
    m_connectionView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_connectionView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_outputView->setGraphicsView(m_connectionView);
    m_inputView->setGraphicsView(m_connectionView);

    mainLay->addWidget(m_outputView);
    mainLay->addWidget(m_connectionView);
    mainLay->addWidget(m_inputView);

    lay->addLayout(mainLay);

    auto* removeAllButton =
            new QPushButton(tr("Remove all connections"));
    removeAllButton->setIcon(gt::gui::icon::delete_());
    auto* saveButton = new QPushButton(tr("Ok"));
    saveButton->setIcon(gt::gui::icon::check());
    auto* closeButton = new QPushButton(tr("Cancel"));
    closeButton->setIcon(gt::gui::icon::cancel());

    connect(closeButton, SIGNAL(clicked()), SLOT(reject()));
    connect(saveButton, SIGNAL(clicked()), SLOT(accept()));
    connect(removeAllButton, SIGNAL(clicked()), SLOT(deleteAllConnections()));

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(removeAllButton);
    buttonsLayout->setContentsMargins(4, 4, 4, 4);
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(saveButton);
    buttonsLayout->addWidget(closeButton);

    lay->addLayout(buttonsLayout);

    setLayout(lay);

    if (task)
    {
        GtObject* obj = task->clone();

        if (obj)
        {
            m_task = qobject_cast<GtTask*>(obj);

            if (!m_task)
            {
                delete obj;
            }
        }
    }

    fillData();
}

GtProcessConnectionEditor::~GtProcessConnectionEditor()
{
    delete m_task;
}

GtObjectMemento
GtProcessConnectionEditor::connectionData()
{
    if (m_task)
    {
        return m_task->toMemento();
    }

    return GtObjectMemento();
}

void
GtProcessConnectionEditor::deleteAllConnections()
{
    if (!m_connectionView->root()) return;

    m_connectionView->removeAllConnections(m_connectionView->root()->uuid());
}

void
GtProcessConnectionEditor::fillData()
{
    // check task
    if (!m_task) return;

    // fill output model and expand view
    m_outputModel->setRoot(m_task);
    m_outputView->expandAll();

    // fill input model and expand view
    m_inputModel->setRoot(m_task);
    m_inputView->expandAll();

    // create existing connections
    m_connectionView->setRoot(m_task);
    m_connectionView->createConnections(m_task->collectPropertyConnections());
}
