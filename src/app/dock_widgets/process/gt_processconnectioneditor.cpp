/* GTlab - Gas Turbine laboratory
 * Source File: gt_processconnectioneditor.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QVBoxLayout>
#include <QPushButton>

#include "gt_application.h"
#include "gt_processconnectionmodel.h"
#include "gt_processconnectionview.h"
#include "gt_processconnectionscene.h"
#include "gt_processconnectiongraphicsview.h"
#include "gt_task.h"
#include "gt_objectmemento.h"

#include "gt_processconnectioneditor.h"

GtProcessConnectionEditor::GtProcessConnectionEditor(GtTask* task,
                                                     QWidget* parent) :
    QDialog(parent),
    m_task(nullptr)
{
    setWindowTitle(tr("Connections"));

    setWindowIcon(gtApp->icon(QStringLiteral("connectionIcon_16.png")));

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);

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

    QPushButton* saveButton = new QPushButton(tr("Ok"));
    saveButton->setIcon(gtApp->icon(QStringLiteral("checkIcon_16.png")));
    QPushButton* closeButton = new QPushButton(tr("Cancel"));
    closeButton->setIcon(gtApp->icon(QStringLiteral("closeIcon_16.png")));

    connect(closeButton, SIGNAL(clicked()), SLOT(reject()));
    connect(saveButton, SIGNAL(clicked()), SLOT(accept()));

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
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
GtProcessConnectionEditor::fillData()
{
    // check task
    if (!m_task)
    {
        return;
    }

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
