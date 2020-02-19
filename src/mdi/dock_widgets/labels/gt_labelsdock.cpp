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

#include "gt_searchwidget.h"
#include "gt_listview.h"
#include "gt_application.h"
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
    m_labelData(Q_NULLPTR),
    m_project(Q_NULLPTR)
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

    widget->setFrameShape(m_listView->frameShape());
    widget->setFrameStyle(m_listView->frameStyle());
    widget->setFrameShadow(m_listView->frameShadow());

    QPalette palette = widget->palette();
    palette.setColor(backgroundRole(), QColor(255, 255, 255));
    widget->setPalette(palette);

    widget->setAutoFillBackground(true);

    m_listView->setFrameStyle(QListView::NoFrame);
    m_listView->setIconSize(QSize(20, 20));
    m_listView->setDragEnabled(true);
    m_listView->setDragDropMode(QListView::DragOnly);
    m_listView->setAlternatingRowColors(true);

    layout->addWidget(m_listView);

    connect(m_listView, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(customContextMenu(QPoint)));

    QHBoxLayout* filterLayout = new QHBoxLayout;
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(0);

    m_search = new GtSearchWidget;
    filterLayout->addWidget(m_search);

    filterLayout->addStretch(1);

    // export button
    m_exportButton = new QPushButton;
    m_exportButton->setIcon(gtApp->icon("exportIcon_16.png"));
    m_exportButton->setMaximumSize(QSize(20, 20));
    m_exportButton->setFlat(true);
    m_exportButton->setToolTip(tr("Export labels to file"));
    m_exportButton->setEnabled(false);
    filterLayout->addWidget(m_exportButton);
    //    connect(exportButton, SIGNAL(clicked(bool)),
    //           SLOT(exportLog()));

    // import button
    m_importButton = new QPushButton;
    m_importButton->setIcon(gtApp->icon("importIcon_16.png"));
    m_importButton->setMaximumSize(QSize(20, 20));
    m_importButton->setFlat(true);
    m_importButton->setToolTip(tr("Import labels from file"));
    m_importButton->setEnabled(false);
    filterLayout->addWidget(m_importButton);
    //    connect(exportButton, SIGNAL(clicked(bool)),
    //           SLOT(exportLog()));

    // add button
    m_addButton = new QPushButton;
    m_addButton->setIcon(gtApp->icon("addIcon_16.png"));
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

    m_listView->setModel(Q_NULLPTR);
    m_styledModel->setSourceModel(gtDataModel);

    m_labelData = Q_NULLPTR;
    m_project = project;

    if (m_project != Q_NULLPTR)
    {
        m_addButton->setEnabled(true);

        m_labelData = m_project->labelData();

        if (m_labelData != Q_NULLPTR)
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
        return QModelIndex();
    }

    QModelIndex i1 = m_model->mapToSource(index);

    return m_styledModel->mapToSource(i1);
}

QModelIndex
GtLabelsDock::mapFromSource(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return QModelIndex();
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

    if (m_project == Q_NULLPTR)
    {
        return;
    }

    GtLabel* label = labelFromIndex(index);

    if (label == Q_NULLPTR)
    {
        return;
    }

    int nou = m_project->numberOfLabelUsages(label);

    if (nou <= 0)
    {
        QMessageBox mb;
        mb.setIcon(QMessageBox::Information);
        mb.setWindowTitle(tr("Find Usages"));
        mb.setWindowIcon(gtApp->icon(QStringLiteral("infoIcon_16.png")));
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

    if (m_project == Q_NULLPTR)
    {
        return;
    }

    GtLabel* label = labelFromIndex(index);

    if (label == Q_NULLPTR)
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
    mb.setWindowIcon(gtApp->icon(QStringLiteral("closeIcon_16.png")));
    mb.setText(msg);
    mb.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    mb.setDefaultButton(QMessageBox::Yes);
    int ret = mb.exec();

    switch (ret)
    {
        case QMessageBox::Yes:
        {
//            GtProject* project = label->findParent<GtProject*>();

//            if (project != Q_NULLPTR)
//            {
//                QString delMsg = tr("Delete Label") + QStringLiteral(" - ") +
//                                 label->objectName();
//                gtApp->startCommand(project, delMsg);
//            }

        QList<GtObject*> objsToDelete;

        if (deleteLabel)
        {
            //            gtDataModel->deleteFromModel(label);
            objsToDelete << label;
        }



        foreach (GtLabel* l, labels)
        {
            objsToDelete << l;
        }

        gtDataModel->deleteFromModel(objsToDelete);

        //            if (project != Q_NULLPTR)
//            {
//                gtApp->endCommand();
//            }

            break;
        }

        case QMessageBox::Cancel:
        {
            break;
        }
    }
}

GtLabel*
GtLabelsDock::labelFromIndex(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return Q_NULLPTR;
    }

    QModelIndex srcIndex = mapToSource(index);

    if (!srcIndex.isValid())
    {
        return Q_NULLPTR;
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

    m_listView->setModel(Q_NULLPTR);

    if (m_labelData != Q_NULLPTR)
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
    if (m_labelData == Q_NULLPTR)
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
    if (m_listView->model() == Q_NULLPTR)
    {
        return;
    }

    QModelIndex index = m_listView->indexAt(pos);

    QModelIndex srcIndex = mapToSource(index);

    if (srcIndex.isValid())
    {
        QMenu menu(this);

        QAction* actrename = menu.addAction(tr("Rename"));
        actrename->setIcon(gtApp->icon(QStringLiteral("inputIcon_16.png")));

        QAction* actfindusages = menu.addAction(tr("Find Usages"));
        actfindusages->setIcon(gtApp->icon(QStringLiteral("resultsIcon_16.png")));

        QAction* actdeleteusages = menu.addAction(tr("Delete Usages"));
        actdeleteusages->setIcon(gtApp->icon(QStringLiteral("closeIcon_16.png")));

        menu.addSeparator();

        QAction* actdelete = menu.addAction(tr("Delete Label"));
        actdelete->setIcon(gtApp->icon(QStringLiteral("closeIcon_16.png")));

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
    }
}

void
GtLabelsDock::resetModel()
{
    projectChangedEvent(m_project);
}