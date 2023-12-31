/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processqueuewidget.cpp
 *
 *  Created on: 13.10.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#include "gt_processqueuewidget.h"

#include <QVBoxLayout>
#include <QFrame>
#include <QMenu>
#include <QListView>
#include <QPushButton>

#include "gt_processqueuemodel.h"
#include "gt_processexecutor.h"
#include "gt_icons.h"
#include "gt_colors.h"
#include "gt_task.h"
#include "gt_stylesheets.h"


GtProcessQueueWidget::GtProcessQueueWidget(GtProcessQueueModel* model) :
    QWidget(), m_model(model)
{
    setWindowTitle("Process Queue");

    setWindowIcon(gt::gui::icon::queue());

    QFrame* frame = new QFrame;
    QVBoxLayout* frameLayout = new QVBoxLayout;

    m_upButton = new QPushButton(tr("Move Up"));
    m_upButton->setIcon(gt::gui::icon::arrowUp());
    m_upButton->setToolTip(tr("Move up task"));
    QString styleSheet = gt::gui::stylesheet::button();

    m_upButton->setStyleSheet(styleSheet);

    m_removeButton = new QPushButton(tr("Remove"));
    m_removeButton->setIcon(gt::gui::icon::delete_());
    m_removeButton->setToolTip(tr("Remove from queue"));
    m_removeButton->setStyleSheet(styleSheet);

    m_downButton = new QPushButton(tr("Move Down"));
    m_downButton->setIcon(gt::gui::icon::arrowDown());
    m_downButton->setToolTip(tr("Move down task"));
    m_downButton->setStyleSheet(styleSheet);

    m_view = new QListView(this);
    m_view->setModel(m_model);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);

    frame->setLayout(frameLayout);
    frame->setAutoFillBackground(true);
    frame->setFrameShape(m_view->frameShape());
    frame->setFrameStyle(m_view->frameStyle());
    frame->setFrameShadow(m_view->frameShadow());

    m_view->setFrameStyle(QListView::NoFrame);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(1);
    btnLayout->addWidget(m_upButton);
    btnLayout->addWidget(m_downButton);
    btnLayout->addWidget(m_removeButton);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    frameLayout->addWidget(m_view);

    layout->addWidget(frame);
    layout->addLayout(btnLayout);

    this->setLayout(layout);

    connect(m_view, &QListView::clicked,
            this, &GtProcessQueueWidget::onSelectionChanged);

    connect(m_removeButton, &QPushButton::pressed,
            this, &GtProcessQueueWidget::onRemove);

    connect(m_upButton, &QPushButton::pressed,
            this, &GtProcessQueueWidget::onMoveUp);

    connect(m_downButton, &QPushButton::pressed,
            this, &GtProcessQueueWidget::onMoveDown);

    connect(m_model, &GtProcessQueueModel::queueChanged,
            this, &GtProcessQueueWidget::clearSelection);

    connect(m_view, &QListView::customContextMenuRequested,
            this, &GtProcessQueueWidget::onContextMenuRequest);

    m_upButton->setDisabled(true);
    m_downButton->setDisabled(true);
    m_removeButton->setDisabled(true);
}

GtProcessQueueWidget::~GtProcessQueueWidget()
{
    delete m_model;
}

void
GtProcessQueueWidget::onMoveUp()
{
    QModelIndex selection =
            m_view->selectionModel()->selectedIndexes().first();

    if (!selection.isValid())
    {
        return;
    }

    GtTask* task = gt::currentProcessExecutor().queue().at(selection.row());

    if (task)
    {
        gt::currentProcessExecutor().moveTaskUp(task);
    }

    m_view->clearSelection();
    QModelIndex index = m_model->index(selection.row()-1, 0);
    if (index.isValid())
    {
        m_view->selectionModel()->select(index,
                                         QItemSelectionModel::ClearAndSelect);
    }

    onSelectionChanged();
}

void
GtProcessQueueWidget::onMoveDown()
{
    QModelIndex selection =
            m_view->selectionModel()->selectedIndexes().first();

    if (!selection.isValid())
    {
        return;
    }

    GtTask* task = gt::currentProcessExecutor().queue().at(selection.row());

    if (task)
    {
        gt::currentProcessExecutor().moveTaskDown(task);
    }

    m_view->clearSelection();
    QModelIndex index = m_model->index(selection.row() + 1, 0);
    if (index.isValid())
    {
        m_view->selectionModel()->select(index,
                                         QItemSelectionModel::ClearAndSelect);
    }

    onSelectionChanged();
}

void
GtProcessQueueWidget::onRemove()
{
    QModelIndex selection =
            m_view->selectionModel()->selectedIndexes().first();

    if (!selection.isValid())
    {
        return;
    }

    GtTask* task = gt::currentProcessExecutor().queue().at(selection.row());

    if (task)
    {
        gt::currentProcessExecutor().removeFromQueue(task);
    }
}

void
GtProcessQueueWidget::onSelectionChanged()
{
    QModelIndex selection =
            m_view->selectionModel()->selectedIndexes().first();

    if (!selection.isValid())
    {
        m_upButton->setDisabled(true);
        m_downButton->setDisabled(true);
        m_removeButton->setDisabled(true);
    }

    GtTask* task = gt::currentProcessExecutor().queue().at(selection.row());

    if (!task)
    {
        return;
    }

    if (task->currentState() == GtTask::RUNNING)
    {
        m_upButton->setDisabled(true);
        m_downButton->setDisabled(true);
        m_removeButton->setDisabled(true);
    }
    else if (task->currentState() == GtTask::QUEUED)
    {
        if (selection.row() > 1)
        {
            m_upButton->setDisabled(false);
        }
        else
        {
            m_upButton->setDisabled(true);
        }

        if (selection.row() < gt::currentProcessExecutor().queue().size() -1)
        {
            m_downButton->setDisabled(false);
        }
        else
        {
            m_downButton->setDisabled(true);
        }

        m_removeButton->setDisabled(false);
    }
    else if (task->currentState() == GtTask::FINISHED)
    {
        m_upButton->setDisabled(true);
        m_downButton->setDisabled(true);
        m_removeButton->setDisabled(true);
    }
    else if (task->currentState() == GtTask::FAILED)
    {
        m_upButton->setDisabled(true);
        m_downButton->setDisabled(true);
        m_removeButton->setDisabled(true);
    }
    else if (task->currentState() == GtTask::CONNECTING)
    {
        m_upButton->setDisabled(true);
        m_downButton->setDisabled(true);
        m_removeButton->setDisabled(true);
    }
}

void
GtProcessQueueWidget::clearSelection()
{
    m_view->clearSelection();
    m_upButton->setDisabled(true);
    m_downButton->setDisabled(true);
    m_removeButton->setDisabled(true);
}

void
GtProcessQueueWidget::onContextMenuRequest(const QPoint& pos)
{
    if (!m_view->model())
    {
        return;
    }

    QModelIndex index = m_view->indexAt(pos);

    if (!index.isValid())
    {
        return;
    }

    QModelIndex selection =
            m_view->selectionModel()->selectedIndexes().first();

    if (!selection.isValid())
    {
        return;
    }

    GtTask* task = gt::currentProcessExecutor().queue().at(selection.row());

    if (task)
    {
        QMenu menu(this);

        QAction* actMoveUp = menu.addAction("Move up");
        actMoveUp->setIcon(gt::gui::icon::arrowUp());
        menu.addSeparator();

        QAction* actMoveDown = menu.addAction("Move down");
        actMoveDown->setIcon(gt::gui::icon::arrowDown());
        menu.addSeparator();

        QAction* actRemove = menu.addAction("Remove");
        actRemove->setIcon(gt::gui::icon::delete_());

        if (selection.row() == 1)
        {
            actMoveUp->setEnabled(false);
        }
        else if (selection.row() == gt::currentProcessExecutor().queue().size()-1)
        {
            actMoveDown->setEnabled(false);
        }
        else if (selection.row() == 0)
        {
            actMoveUp->setEnabled(false);
            actMoveDown->setEnabled(false);
            actRemove->setEnabled(false);
        }

        QAction* a = menu.exec(QCursor::pos());

        if (a == actMoveUp)
        {
            onMoveUp();
        }
        else if (a == actMoveDown)
        {
            onMoveDown();
        }
        else if (a == actRemove)
        {
            onRemove();
        }
    }
}
