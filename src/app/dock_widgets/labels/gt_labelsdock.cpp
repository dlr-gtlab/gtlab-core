/* GTlab - Gas Turbine laboratory
 * Source File: gt_labelsdock.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 18.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QFrame>
#include <QVBoxLayout>
#include <QSortFilterProxyModel>
#include <QPushButton>
#include <QMenu>
#include <QMessageBox>
#include <QKeyEvent>

#include "gt_searchwidget.h"
#include "gt_listview.h"
#include "gt_application.h"
#include "gt_icons.h"
#include "gt_project.h"
#include "gt_mdilauncher.h"
#include "gt_datamodel.h"
#include "gt_styledmodel.h"
#include "gt_treefiltermodel.h"

#include "gt_labelsdock.h"
#include "gt_labeldata.h"
#include "gt_label.h"
#include "gt_labelusageviewer.h"
#include "gt_labelcontainer.h"
#include "gt_labeldelegate.h"

#include "gt_logging.h"

GtLabelsDock::GtLabelsDock() :
    m_labelData(nullptr),
    m_project(nullptr)
{
    setObjectName(tr("Labels"));

    QFrame* widget = new QFrame(this);
    setWidget(widget);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    widget->setLayout(layout);

    m_styledModel = new GtStyledModel(this);
    m_styledModel->setSourceModel(gtDataModel);

    m_model = new GtTreeFilterModel(this);
    m_model->setFilterCaseSensitivity(Qt::CaseInsensitive);

    m_model->setSourceModel(m_styledModel);

    m_listView = new GtListView;
    m_listView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_listView->setItemDelegate(new GtLabelDelegate(this));
    m_listView->setSelectionMode(GtListView::ExtendedSelection);

    widget->setFrameShape(m_listView->frameShape());
    widget->setFrameStyle(m_listView->frameStyle());
    widget->setFrameShadow(m_listView->frameShadow());

    widget->setAutoFillBackground(true);

    m_listView->setFrameStyle(QListView::NoFrame);
    m_listView->setIconSize(QSize(20, 20));
    m_listView->setDragEnabled(true);
    m_listView->setDragDropMode(QListView::DragOnly);
    m_listView->setAlternatingRowColors(true);

    layout->addWidget(m_listView);

    connect(m_listView, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(customContextMenu(QPoint)));

    connect(this, SIGNAL(contextMenuKeyPressSignal(QModelIndex)),
            SLOT(customContextMenu(QModelIndex)));

    QHBoxLayout* filterLayout = new QHBoxLayout;
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(0);

    m_search = new GtSearchWidget;
    filterLayout->addWidget(m_search);

    filterLayout->addStretch(1);

    // export button
    m_exportButton = new QPushButton;
    m_exportButton->setIcon(gt::gui::icon::export16());
    m_exportButton->setMaximumSize(QSize(20, 20));
    m_exportButton->setFlat(true);
    m_exportButton->setToolTip(tr("Export labels to file"));
    m_exportButton->setEnabled(false);
    filterLayout->addWidget(m_exportButton);
    //    connect(exportButton, SIGNAL(clicked(bool)),
    //           SLOT(exportLog()));

    // import button
    m_importButton = new QPushButton;
    m_importButton->setIcon(gt::gui::icon::import16());
    m_importButton->setMaximumSize(QSize(20, 20));
    m_importButton->setFlat(true);
    m_importButton->setToolTip(tr("Import labels from file"));
    m_importButton->setEnabled(false);
    filterLayout->addWidget(m_importButton);
    //    connect(exportButton, SIGNAL(clicked(bool)),
    //           SLOT(exportLog()));

    // add button
    m_addButton = new QPushButton;
    m_addButton->setIcon(gt::gui::icon::add16());
    m_addButton->setMaximumSize(QSize(20, 20));
    m_addButton->setFlat(true);
    m_addButton->setToolTip(tr("Add new label"));
    m_addButton->setEnabled(false);
    filterLayout->addWidget(m_addButton);
    connect(m_addButton, SIGNAL(clicked(bool)), SLOT(newLabel()));

    layout->addLayout(filterLayout);

    connect(m_search, SIGNAL(textEdited(QString)),
            SLOT(filterData(QString)));
    connect(m_search, SIGNAL(textChanged(QString)),
            SLOT(filterData(QString)));
    connect(m_listView, SIGNAL(searchRequest()), m_search,
            SLOT(enableSearch()));
    connect(gtDataModel, SIGNAL(triggerEndResetDataModelView()),
            SLOT(resetModel()));
}

Qt::DockWidgetArea
GtLabelsDock::getDockWidgetArea()
{
    return Qt::RightDockWidgetArea;
}

void
GtLabelsDock::projectChangedEvent(GtProject* project)
{
    m_importButton->setEnabled(false);
    m_exportButton->setEnabled(false);
    m_addButton->setEnabled(false);

    m_listView->setModel(nullptr);
    m_styledModel->setSourceModel(gtDataModel);

    m_labelData = nullptr;
    m_project = project;

    if (m_project)
    {
        m_addButton->setEnabled(true);

        m_labelData = m_project->labelData();

        if (m_labelData)
        {
            filterData(m_search->text());
        }
    }
}

QModelIndex
GtLabelsDock::mapToSource(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return {};
    }

    QModelIndex i1 = m_model->mapToSource(index);

    return m_styledModel->mapToSource(i1);
}

QModelIndex
GtLabelsDock::mapFromSource(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return {};
    }

    QModelIndex i1 = m_styledModel->mapFromSource(index);

    return  m_model->mapFromSource(i1);
}

void
GtLabelsDock::findUsages(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    if (!m_project)
    {
        return;
    }

    GtLabel* label = labelFromIndex(index);

    if (!label)
    {
        return;
    }

    int nou = m_project->numberOfLabelUsages(label);

    if (nou <= 0)
    {
        QMessageBox mb;
        mb.setIcon(QMessageBox::Information);
        mb.setWindowTitle(tr("Find Usages"));
        mb.setWindowIcon(gt::gui::icon::info16());
        mb.setText(QString(tr("No usages found") + QStringLiteral(" !")));
        mb.setStandardButtons(QMessageBox::Ok);
        mb.setDefaultButton(QMessageBox::Ok);
        mb.exec();
        return;
    }

    GtLabelContainer* container = new GtLabelContainer(m_project, label);

    gtMdiLauncher->open(GtLabelUsageViewer::staticMetaObject.className(),
                        container);
}

void
GtLabelsDock::deleteUsages(const QModelIndex& index, bool deleteLabel)
{
    if (!index.isValid())
    {
        return;
    }

    if (!m_project)
    {
        return;
    }

    GtLabel* label = labelFromIndex(index);

    if (!label)
    {
        return;
    }

    QList<GtLabel*> labels = m_project->findLabelUsages(label);

    int nou = labels.size();

    QString msg;

    if (nou <= 0)
    {
        if (!deleteLabel)
        {
            QMessageBox::information(this, tr("No usages found"),
                                     tr("No usages found!"), QMessageBox::Ok,
                                     QMessageBox::Ok);
            return;
        }

        msg = tr("Delete") + QStringLiteral(" ") + label->objectName() +
              QStringLiteral("?");
    }
    else
    {
        msg = tr("Delete all") + QStringLiteral(" <b>") + QString::number(nou) +
              QStringLiteral("</b> ") + tr("data entries of ") +
              label->objectName() + QStringLiteral("?");
    }

    QMessageBox mb;
    mb.setIcon(QMessageBox::Question);
    mb.setWindowTitle(tr("Delete Label"));
    mb.setWindowIcon(gt::gui::icon::delete16());
    mb.setText(msg);
    mb.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    mb.setDefaultButton(QMessageBox::Yes);
    int ret = mb.exec();

    switch (ret)
    {
        case QMessageBox::Yes:
        {
            QList<GtObject*> objsToDelete;

            if (deleteLabel)
            {
                objsToDelete << label;
            }

            foreach (GtLabel* l, labels)
            {
                objsToDelete << l;
            }

            gtDataModel->deleteFromModel(objsToDelete);

            break;
        }
        case QMessageBox::Cancel:
        {
            break;
        }
    }
}

void
GtLabelsDock::deleteMultipleUsages(const QModelIndexList& indexes,
                                   bool deleteLabels)
{
    if (!m_project)
    {
        return;
    }

    QList<GtObject*> labels;
    QList<GtLabel*> usages;

    for (const QModelIndex& idx : indexes)
    {
        if (!idx.isValid())
        {
            continue;
        }

        GtLabel* label = labelFromIndex(idx);

        if (!label)
        {
            continue;
        }

        usages.append(m_project->findLabelUsages(label));
        labels.append(label);
    }

    int nou = usages.size();

    QString msg;

    if (nou <= 0)
    {
        if (!deleteLabels)
        {
            QMessageBox::information(this, tr("No usages found"),
                                     tr("No usages found!"), QMessageBox::Ok,
                                     QMessageBox::Ok);
            return;
        }

        msg = tr("Delete selected labels?");
    }
    else
    {
        msg = tr("Delete all") + QStringLiteral(" <b>") + QString::number(nou) +
              QStringLiteral("</b> ") + tr("data entries of selected labels?");
    }

    QMessageBox mb;
    mb.setIcon(QMessageBox::Question);
    mb.setWindowTitle(tr("Delete selected Labels"));
    mb.setWindowIcon(gt::gui::icon::delete16());
    mb.setText(msg);
    mb.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    mb.setDefaultButton(QMessageBox::Yes);
    int ret = mb.exec();

    switch (ret)
    {
        case QMessageBox::Yes:
        {
            QList<GtObject*> objsToDelete;

            if (deleteLabels)
            {
                objsToDelete << labels;
            }

            for (GtLabel* l : usages)
            {
                objsToDelete << l;
            }

            gtDataModel->deleteFromModel(objsToDelete);

            break;
        }
        case QMessageBox::Cancel:
        {
            break;
        }
    }
}

void
GtLabelsDock::keyPressEvent(QKeyEvent* event)
{
    if (m_listView->model())
    {
        QModelIndexList indexes;

        // multiselection
        if (m_listView->selectionModel())
        {
            indexes = m_listView->selectionModel()->selectedIndexes();
        }

        if (gtApp->compareKeyEvent(event, "OpenContextMenu"))
        {
            if (!indexes.isEmpty())
            {
                QModelIndex first = indexes.first();

                if (first.isValid())
                {
                    emit contextMenuKeyPressSignal(first);
                    event->accept();
                    return;
                }

            }
        }
    }

    GtDockWidget::keyPressEvent(event);
}

GtLabel*
GtLabelsDock::labelFromIndex(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return nullptr;
    }

    QModelIndex srcIndex = mapToSource(index);

    if (!srcIndex.isValid())
    {
        return nullptr;
    }

    GtObject* obj = gtDataModel->objectFromIndex(srcIndex);

    return qobject_cast<GtLabel*>(obj);
}

GtProject*
GtLabelsDock::project()
{
    return m_project;
}

void
GtLabelsDock::filterData(const QString& val)
{
    m_model->setFilterRegExp(val);

    m_listView->setModel(nullptr);

    if (m_labelData)
    {
        QModelIndex srcIndex = gtDataModel->indexFromObject(m_labelData);
        QModelIndex index = mapFromSource(srcIndex);
        m_rootIndex = QPersistentModelIndex(index);

        if (m_rootIndex.isValid())
        {
            m_listView->setModel(m_model);
            m_listView->setRootIndex(m_rootIndex);
        }
    }
}

void
GtLabelsDock::newLabel()
{
    if (!m_labelData)
    {
        return;
    }

    QModelIndex parentIndex = gtDataModel->indexFromObject(m_labelData);

    if (!parentIndex.isValid())
    {
        return;
    }

    QString id = gtDataModel->uniqueObjectName(tr("New Label"), parentIndex);

    QModelIndex index = gtDataModel->appendChild(new GtLabel(id), m_labelData);

    if (index.isValid())
    {
        m_listView->edit(mapFromSource(index));
    }
}

void
GtLabelsDock::customContextMenu(const QPoint& pos)
{
    QModelIndex indexUnderMouse = m_listView->indexAt(pos);

    if (!indexUnderMouse.isValid())
    {
        gtDebug() << "invalid list view";
        return;
    }

    customContextMenu(indexUnderMouse);
}


void
GtLabelsDock::customContextMenu(const QModelIndex& index)
{
    if (!m_listView->model())
    {
        gtDebug() << "invalid list view";
        return;
    }

    QModelIndexList indexes;

    // multiselection
    if (m_listView->selectionModel())
    {
        indexes = m_listView->selectionModel()->selectedIndexes();
    }
    else
    {
        if (!index.isValid())
        {
            gtDebug() << "Invalid index";
            return;
        }

        indexes.append(index);
    }

    if (indexes.isEmpty())
    {
        gtDebug() << "Indexes is empty";
        return;
    }

    // conetx menu for single label
    if (indexes.length() == 1)
    {
        //QModelIndex index = indexes.at(0);

        QMenu menu(this);

        QAction* actrename = menu.addAction(tr("Rename"));
        actrename->setIcon(gt::gui::icon::input16());

        QAction* actfindusages = menu.addAction(tr("Find Usages"));
        actfindusages->setIcon(gt::gui::icon::results16());

        QAction* actdeleteusages = menu.addAction(tr("Delete Usages"));
        actdeleteusages->setIcon(gt::gui::icon::delete16());

        menu.addSeparator();

        QAction* actdelete = menu.addAction(tr("Delete Label"));
        actdelete->setIcon(gt::gui::icon::delete16());

        QAction* a = menu.exec(QCursor::pos());

        if (a == actrename)
        {
            m_listView->edit(index);
        }
        else if (a == actfindusages)
        {
            findUsages(index);
        }
        else if (a == actdeleteusages)
        {
            deleteUsages(index, false);
        }
        else if (a == actdelete)
        {
            deleteUsages(index);
        }

        return;
    }

    // conetx menu for multiple labels
    QMenu menu(this);

    QAction* actdeleteusages = menu.addAction(tr("Delete Usages"));
    actdeleteusages->setIcon(gt::gui::icon::delete16());

    menu.addSeparator();

    QAction* actdelete = menu.addAction(tr("Delete Labels"));
    actdelete->setIcon(gt::gui::icon::delete16());

    QAction* a = menu.exec(QCursor::pos());

    if (a == actdeleteusages)
    {
        deleteMultipleUsages(indexes, false);
    }
    else if (a == actdelete)
    {
        deleteMultipleUsages(indexes, true);
    }
}

void
GtLabelsDock::resetModel()
{
    projectChangedEvent(m_project);
}
