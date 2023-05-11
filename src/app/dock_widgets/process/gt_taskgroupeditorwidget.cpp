/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2023 by DLR
 *
 *  Created on: 02.02.2023
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include <QPushButton>
#include <QHBoxLayout>
#include <QListView>
#include <QStandardItemModel>
#include <QStandardItem>

#include "gt_processdata.h"
#include "gt_coredatamodel.h"

#include "gt_logging.h"

#include "gt_taskgroupeditorwidget.h"

class TaskGroupItem : public QStandardItem
{
public:
    TaskGroupItem(const QString& id) : QStandardItem(id),
    m_storedId(id) {}

    QString m_storedId;
};

GtTaskGroupEditorWidget::GtTaskGroupEditorWidget(GtProcessData& processData,
                                                 const QString& projectPath,
                                                 QWidget* parWid) :
    GtDialog(parWid),
    m_processData(&processData),
    m_projectPath(projectPath)
{
    auto addButton = new QPushButton(tr("Add"));
    auto closeButton = new QPushButton(tr("Ok"));
    auto mainLayout = new QVBoxLayout;
    auto buttonsLayout = new QHBoxLayout;
    m_view = new QListView;

    buttonsLayout->setContentsMargins(4, 4, 4, 4);
    buttonsLayout->addWidget(addButton);
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(closeButton);

    mainLayout->addWidget(m_view);
    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);

    m_model = new QStandardItemModel(this);
    m_view->setModel(m_model);

    fillList();

    connect(addButton, SIGNAL(clicked(bool)), SLOT(createNewTaskGroup()));
    connect(m_model, SIGNAL(itemChanged(QStandardItem*)),
            SLOT(onItemChanged(QStandardItem*)));
}

void
GtTaskGroupEditorWidget::fillList()
{
    if (!m_processData) return;

    for (const auto& id : m_processData->customGroupIds())
    {
        addNewItem(id);
    }
}

QStandardItem*
GtTaskGroupEditorWidget::addNewItem(const QString& id)
{
    auto item = new TaskGroupItem(id);
    m_model->appendRow(item);

    return item;
}

void
GtTaskGroupEditorWidget::createNewTaskGroup()
{
    if (!m_processData) return;

    QString newId = getUniqueName("New Task Group",
                                  m_processData->customGroupIds());

    if (newId.isEmpty()) return;

    auto group = m_processData->createNewTaskGroup(newId, GtTaskGroup::CUSTOM,
                                                   m_projectPath);

    if (!group) return;

    auto item = addNewItem(newId);

    m_view->edit(m_model->indexFromItem(item));
}

void
GtTaskGroupEditorWidget::onItemChanged(QStandardItem* item)
{
    if (!m_processData) return;

    TaskGroupItem* tgItem = dynamic_cast<TaskGroupItem*>(item);

    if (!tgItem) return;

    gtInfo() << "item change!";
    gtInfo() << "|-> " << tgItem->text();
    gtInfo() << "|-> " << tgItem->m_storedId;

    if (tgItem->text() == tgItem->m_storedId) return;

    // task group id changed. rename folder
    auto suc = m_processData->renameTaskGroup(tgItem->m_storedId,
                                              tgItem->text(),
                                              GtTaskGroup::CUSTOM,
                                              m_projectPath);

    if (!suc)
    {
        item->setText(tgItem->m_storedId);
    }
}
