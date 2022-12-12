/* GTlab - Gas Turbine laboratory
 * Source File: gt_postdock.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 26.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QFrame>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QDebug>
#include <QDomDocument>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QKeyEvent>

#include "gt_listview.h"
#include "gt_searchwidget.h"
#include "gt_application.h"
#include "gt_postmodel.h"
#include "gt_project.h"
#include "gt_mdilauncher.h"
#include "gt_templateviewer.h"
#include "gt_deleteitemmessagebox.h"
#include "gt_icons.h"

#include "gt_postdock.h"
#include "gt_postdelegate.h"
#include "gt_posttemplatepath.h"

#include "gt_logging.h"

GtPostDock::GtPostDock() : m_project(nullptr)
{
    setObjectName(tr("Post Processing"));

    QFrame* widget = new QFrame(this);
    setWidget(widget);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    widget->setLayout(layout);

    m_listView = new GtListView;
    m_listView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_listView->setEditTriggers(GtListView::SelectedClicked);
    m_listView->setItemDelegate(new GtPostDelegate(this));

    widget->setFrameShape(m_listView->frameShape());
    widget->setFrameStyle(m_listView->frameStyle());
    widget->setFrameShadow(m_listView->frameShadow());

    widget->setAutoFillBackground(true);

    m_listView->setFrameStyle(QListView::NoFrame);
    m_listView->setIconSize(QSize(20, 20));
    m_listView->setDragEnabled(true);
    m_listView->setDragDropMode(QListView::DragOnly);
    m_listView->setAlternatingRowColors(true);
    m_listView->installEventFilter(this);

    m_datamodel = new GtPostModel(this);
    m_model = new QSortFilterProxyModel(this);
    m_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_model->setSourceModel(m_datamodel);

    m_listView->setModel(m_model);

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

    if (gtApp->devMode())
    {
        // export button
        m_exportButton = new QPushButton;
        m_exportButton->setIcon(gt::gui::icon::export16());
        m_exportButton->setMaximumSize(QSize(20, 20));
        m_exportButton->setFlat(true);
        m_exportButton->setToolTip(tr("Export templates to file"));
        m_exportButton->setEnabled(false);
        filterLayout->addWidget(m_exportButton);
        //    connect(exportButton, SIGNAL(clicked(bool)),
        //           SLOT(exportLog()));

        // import button
        m_importButton = new QPushButton;
        m_importButton->setIcon(gt::gui::icon::import16());
        m_importButton->setMaximumSize(QSize(20, 20));
        m_importButton->setFlat(true);
        m_importButton->setToolTip(tr("Import templates from file"));
        m_importButton->setEnabled(false);
        filterLayout->addWidget(m_importButton);
        //    connect(exportButton, SIGNAL(clicked(bool)),
        //           SLOT(exportLog()));
    }

    // add button
    m_addButton = new QPushButton;
    m_addButton->setIcon(gt::gui::icon::add16());
    m_addButton->setMaximumSize(QSize(20, 20));
    m_addButton->setFlat(true);
    m_addButton->setToolTip(tr("Add new template"));
    m_addButton->setEnabled(false);
    filterLayout->addWidget(m_addButton);
    connect(m_addButton, SIGNAL(clicked(bool)), SLOT(newPostTemplate()));

    layout->addLayout(filterLayout);

    connect(m_search, SIGNAL(textEdited(QString)),
            SLOT(filterData(QString)));
    connect(m_search, SIGNAL(textChanged(QString)),
            SLOT(filterData(QString)));
    connect(m_listView, SIGNAL(searchRequest()), m_search,
            SLOT(enableSearch()));

    connect(m_listView, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(onDoubleClicked(QModelIndex)));
}

Qt::DockWidgetArea
GtPostDock::getDockWidgetArea()
{
    return Qt::LeftDockWidgetArea;
}

bool
GtPostDock::renameFile(const QString& oldName, const QString& newName)
{
    return m_datamodel->renameFile(oldName, newName);
}

void
GtPostDock::projectChangedEvent(GtProject* project)
{
    if (gtApp->devMode())
    {
        m_importButton->setEnabled(false);
        m_exportButton->setEnabled(false);
    }

    m_addButton->setEnabled(false);

    m_project = project;

    //    refreshData();

    m_datamodel->refreshData(m_project);

    // enable buttons

    if (m_project)
    {
        m_addButton->setEnabled(true);
    }
}

bool
GtPostDock::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_listView)
    {
        if (event->type() == QEvent::KeyRelease)
        {
            QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);

            if (keyEvent)
            {

                if (gtApp->compareKeyEvent(keyEvent, "delete"))
                {
                    QModelIndexList selIdx =
                        m_listView->selectionModel()->selectedIndexes();

                    foreach (const QModelIndex& idx, selIdx)
                    {
                        deleteEntry(mapToSource(idx));
                    }

                    return true;
                }
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

QModelIndex
GtPostDock::mapToSource(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return {};
    }

    return m_model->mapToSource(index);
}

QModelIndex
GtPostDock::mapFromSource(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return {};
    }

    return m_model->mapFromSource(index);
}

void
GtPostDock::openTemplateViewer(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    QModelIndex srcIndex = mapToSource(index);

    if (!srcIndex.isValid())
    {
        return;
    }

    GtPostTemplatePath* path = m_datamodel->createTemplatePath(srcIndex);

    if (path)
    {
        gtMdiLauncher->open(GtTemplateViewer::staticMetaObject.className(),
                            path, path->path());
    }
}

void
GtPostDock::deleteEntry(const QModelIndex& index)
{
    GtDeleteItemMessageBox mb(tr("Delete Item"), index.data().toString());

    int ret = mb.exec();

    switch (ret)
    {
        case QMessageBox::Yes:
        {
            m_datamodel->deleteEntry(index);
            break;
        }

        case QMessageBox::Cancel:
        {
            break;
        }

        default:
            break;
    }
}

const QStringList&
GtPostDock::entries()
{
    return m_datamodel->entries();
}

void
GtPostDock::newPostTemplate()
{
    QModelIndex srcIndex = m_datamodel->newPostTemplate();

    if (!srcIndex.isValid())
    {
        return;
    }

    QModelIndex index = mapFromSource(srcIndex);

    if (index.isValid())
    {
        m_listView->edit(index);
    }
}

void
GtPostDock::customContextMenu(QPoint const& pos)
{
    QModelIndex index = m_listView->indexAt(pos);

    customContextMenu(index);
}

void
GtPostDock::customContextMenu(QModelIndex const& index)
{
    if (!gtApp->currentProject())
    {
        return;
    }

    if (index.isValid())
    {
        QMenu menu(this);

        QAction* actrename = menu.addAction(tr("Rename"));
        actrename->setIcon(gt::gui::icon::input16());


        menu.addSeparator();

        QAction* actdelete = menu.addAction(tr("Delete"));
        actdelete->setIcon(gt::gui::icon::delete16());

        QAction* a = menu.exec(QCursor::pos());

        if (a == actrename)
        {
            m_listView->edit(index);
        }
        else if (a == actdelete)
        {
            QModelIndex srcIndex = mapToSource(index);

            deleteEntry(srcIndex);
        }
    }
    else
    {
        QMenu menu(this);

        QAction* addtemplate = menu.addAction(tr("Add new template"));
        addtemplate->setIcon(gt::gui::icon::add16());

        QAction* a = menu.exec(QCursor::pos());

        if (a == addtemplate)
        {
            newPostTemplate();
        }

    }
}

void
GtPostDock::filterData(const QString& val)
{
    m_model->setFilterRegExp(val);
}

void
GtPostDock::onDoubleClicked(const QModelIndex& index)
{
    openTemplateViewer(index);
}

void
GtPostDock::keyPressEvent(QKeyEvent* event)
{
    if (m_listView->model())
    {
        QModelIndexList indexes;

        // multiselection
        if (m_listView->selectionModel())
        {
            indexes = m_listView->selectionModel()->selectedIndexes();
        }

        if (gtApp->compareKeyEvent(event, "openContextMenu"))
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
