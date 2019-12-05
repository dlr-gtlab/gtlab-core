/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QScrollBar>
#include <QFileDialog>
#include <QDir>
#include <QMenu>
#include <QApplication>
#include <QClipboard>
#include <QFontDatabase>

#ifdef GT_QML_WIDGETS
#include <QQuickWidget>
#include <QQmlContext>
#endif

#include "gt_logmodel.h"
#include "gt_styledlogmodel.h"
#include "gt_filteredlogmodel.h"
#include "gt_listview.h"
#include "gt_treeview.h"
#include "gt_lineedit.h"
#include "gt_application.h"
#include "gt_logging.h"
#include "gt_outputtester.h"
#include "gt_searchwidget.h"
#include "gt_taskhistorymodel.h"
#include "gt_project.h"
#include "gt_datamodel.h"
#include "gt_task.h"

#include "gt_outputdock.h"

GtOutputDock::GtOutputDock() :
    m_listView(Q_NULLPTR),
    m_taskPageView(Q_NULLPTR),
    m_historyModel(Q_NULLPTR)
{
    setObjectName(tr("Output"));

    QWidget* widget = new QWidget(this);
    widget->setObjectName("centralWidget");
    setWidget(widget);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);

    GtStyledLogModel* styleModel = new GtStyledLogModel(this);
    m_model = new GtFilteredLogModel(styleModel);
    styleModel->setSourceModel(gtLogModel);
    m_model->setSourceModel(styleModel);
    m_model->setFilterCaseSensitivity(Qt::CaseInsensitive);

    QTabWidget* tab = new QTabWidget;
    tab->setObjectName("tabWidget");

    // default page
    QWidget* defaultPage = new QWidget(this);
    tab->addTab(defaultPage, "Application");
    QVBoxLayout* defaultLayout = new QVBoxLayout;
    defaultLayout->setContentsMargins(0, 0, 0, 0);
    defaultLayout->setSpacing(0);
    defaultPage->setLayout(defaultLayout);

    QHBoxLayout* filterLayout = new QHBoxLayout;
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(0);

    GtSearchWidget* searchWidget = new GtSearchWidget;

    filterLayout->addWidget(searchWidget);

#ifdef GT_QML_WIDGETS
    // qml component
    QQuickWidget* qmlListView = new QQuickWidget;
    qmlListView->resize(QSize(defaultPage->width(), 100));
    qmlListView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    qmlListView->rootContext()->setContextProperty("_cppModel",
                                                   m_model);
    qmlListView->setSource(QUrl("qrc:/qml/outputdock.qml"));
    defaultLayout->addWidget(qmlListView);
#else
    m_listView = new GtListView;
    m_listView->setFrameStyle(QFrame::NoFrame);
//    m_listView->setWrapping(true);
    m_listView->setWordWrap(true);
    m_listView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_listView->setUniformItemSizes(true);
//    m_listView->setDragEnabled(true);
//    m_listView->setDragDropMode(QListView::DragOnly);
//    listView->setLayoutMode(QListView::Batched);
//    listView->setResizeMode(QListView::Adjust);
    m_listView->setModel(m_model);

    connect(gtLogModel,
            SIGNAL(rowsInserted(QModelIndex,int,int)),
            SLOT(scrollToBottom()));
    connect(m_listView, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(openContextMenu()));
    connect(m_listView, SIGNAL(searchRequest()), searchWidget,
            SLOT(enableSearch()));
    connect(m_listView, SIGNAL(copyRequest()), SLOT(onCopyRequest()));

    connect(m_listView, SIGNAL(deleteRequest()), SLOT(onDeleteRequest()));

    defaultLayout->addWidget(m_listView);
#endif


    connect(searchWidget, SIGNAL(textEdited(QString)),
            m_model, SLOT(filterData(QString)));
    connect(searchWidget, SIGNAL(textChanged(QString)),
            m_model, SLOT(filterData(QString)));

    filterLayout->addStretch(1);

    // clear log button
    QPushButton* clearButton = new QPushButton;
    clearButton->setIcon(gtApp->icon("clearIcon_16.png"));
    clearButton->setMaximumSize(QSize(20, 20));
    clearButton->setFlat(true);
    clearButton->setToolTip(tr("Clear Output"));
    filterLayout->addWidget(clearButton);
    connect(clearButton, SIGNAL(clicked(bool)),
            gtLogModel, SLOT(clearLog()));

    // export log button
    QPushButton* exportButton = new QPushButton;
    exportButton->setIcon(gtApp->icon("exportIcon_16.png"));
    exportButton->setMaximumSize(QSize(20, 20));
    exportButton->setFlat(true);
    exportButton->setToolTip(tr("Export Output to file"));
    filterLayout->addWidget(exportButton);
    connect(exportButton, SIGNAL(clicked(bool)),
           SLOT(exportLog()));

    if (gtApp->devMode())
    {
        // test message button
        QPushButton* testButton = new QPushButton;
        testButton->setIcon(gtApp->icon("fileIcon_16.png"));
        testButton->setMaximumSize(QSize(20, 20));
        testButton->setFlat(true);
        testButton->setToolTip(tr("Test Output"));
        filterLayout->addWidget(testButton);
        connect(testButton, SIGNAL(clicked(bool)), SLOT(testOutput()));

        // debug message button
        QPushButton* debugButton = new QPushButton;
        debugButton->setIcon(gtApp->icon("bugIcon.png"));
        debugButton->setMaximumSize(QSize(20, 20));
        debugButton->setFlat(true);
        debugButton->setCheckable(true);
        debugButton->setChecked(true);
        debugButton->setToolTip(tr("Show/Hide Debug Output"));
        filterLayout->addWidget(debugButton);
        connect(debugButton, SIGNAL(toggled(bool)),
                m_model, SLOT(toggleDebugLevel(bool)));
    }

    // info message button
    QPushButton* infoButton = new QPushButton;
    infoButton->setIcon(gtApp->icon("infoBlueIcon_16.png"));
    infoButton->setMaximumSize(QSize(20, 20));
    infoButton->setFlat(true);
    infoButton->setCheckable(true);
    infoButton->setChecked(true);
    infoButton->setToolTip(tr("Show/Hide Info Output"));
    filterLayout->addWidget(infoButton);
    connect(infoButton, SIGNAL(toggled(bool)),
            m_model, SLOT(toggleInfoLevel(bool)));

    // warning message button
    QPushButton* warningButton = new QPushButton;
    warningButton->setIcon(gtApp->icon("processFailedIcon_16.png"));
    warningButton->setMaximumSize(QSize(20, 20));
    warningButton->setFlat(true);
    warningButton->setCheckable(true);
    warningButton->setChecked(true);
    warningButton->setToolTip(tr("Show/Hide Warning Output"));
    filterLayout->addWidget(warningButton);
    connect(warningButton, SIGNAL(toggled(bool)),
            m_model, SLOT(toggleWarningLevel(bool)));

    // error message button
    QPushButton* errorButton = new QPushButton;
    errorButton->setIcon(gtApp->icon("errorIcon_16.png"));
    errorButton->setMaximumSize(QSize(20, 20));
    errorButton->setFlat(true);
    errorButton->setCheckable(true);
    errorButton->setChecked(true);
    errorButton->setToolTip(tr("Show/Hide Error Output"));
    filterLayout->addWidget(errorButton);
    connect(errorButton, SIGNAL(toggled(bool)),
            m_model, SLOT(toggleErrorLevel(bool)));

    defaultLayout->addLayout(filterLayout);

    // task history overview page

    // Temporarily removed! Do not touch!

//    QWidget* taskPage = new QWidget(this);
//    tab->addTab(taskPage, "Task History");

//    QVBoxLayout* taskPageLayout = new QVBoxLayout;
//    taskPageLayout->setContentsMargins(0, 0, 0, 0);
//    taskPageLayout->setSpacing(0);
//    taskPage->setLayout(taskPageLayout);

//    m_taskPageView = new GtTreeView;
//    m_taskPageView->setFrameStyle(QFrame::NoFrame);
//    m_taskPageView->setWordWrap(true);
//    m_taskPageView->setContextMenuPolicy(Qt::CustomContextMenu);
//    m_taskPageView->setSelectionMode(QAbstractItemView::ContiguousSelection);
//    m_taskPageView->setAlternatingRowColors(true);
//    m_taskPageView->setUniformRowHeights(true);
//    m_taskPageView->setRootIsDecorated(false);

//    m_historyModel = new GtTaskHistoryModel(this);

//    m_taskPageView->setModel(m_historyModel);

//    m_taskPageView->setColumnWidth(0, 200);
//    m_taskPageView->setColumnWidth(1, 100);
//    m_taskPageView->setColumnWidth(2, 150);
//    m_taskPageView->setColumnWidth(3, 150);

//    taskPageLayout->addWidget(m_taskPageView);

    layout->addWidget(tab);

    widget->setLayout(layout);
}

Qt::DockWidgetArea
GtOutputDock::getDockWidgetArea()
{
    return Qt::BottomDockWidgetArea;
}

void
GtOutputDock::projectChangedEvent(GtProject* /*project*/)
{
    // Temporarily removed! Do not touch!

//    gtDebug() << "GtOutputDock::projectChangedEvent";

//    if (project == Q_NULLPTR)
//    {
//        m_historyModel->clear();
//    }
//    else
//    {
//        m_historyModel->setPath(project->path());
//    }
}

void
GtOutputDock::copyToClipboard(const QModelIndexList& indexes)
{
    QMimeData* mimeData = gtLogModel->mimeData(indexes);
    QApplication::clipboard()->setMimeData(mimeData);
}

void
GtOutputDock::removeItems(const QModelIndexList &indexes)
{
    int first = indexes.first().row();
    int last = indexes.last().row();

    int indexBeforeFirst = 0;

    if (first > 0)
    {
        indexBeforeFirst = first - 1;
    }

    QModelIndex beforeFirst = m_model->index(indexBeforeFirst,
                                             indexes.first().column());


    gtLogModel->removeElementList(indexes, first, last);

    QScrollBar* bar = m_listView->verticalScrollBar();
    if (bar != Q_NULLPTR)
    {
        m_listView->scrollTo(beforeFirst,
                             QListView::ScrollHint::PositionAtCenter);
        bar->update();
    }
}

void
GtOutputDock::testOutput()
{
    const int testCase = 3;

    switch (testCase)
    {
    case 1:
    {
        gtDebug() << "test";
        break;
    }
    case 2:
    {
        gtDebug() << "test! fshaopfd khlsad klhsdl okhla klhs ldkh slikahdl"
                  << " jhbsdh sahkd  ihdskadk ll salhdkl sklhd k skhdk "
                  << "test! fshaopfd khlsad klhsdl okhla klhs ldkh slikahdl"
                  << " jhbsdh sahkd  ihdskadk ll salhdkl sklhd k skhdk ";
        break;
    }
    case 3:
    {
        gtTrace() << "trace!";
        gtDebug() << "debug!";
        gtInfo() << "info!";
        gtWarning() << "warning!";
        gtError() << "error!";
        gtFatal() << "fatal!";
        break;
    }
    case 4:
    {
        GtOutputTester* tester = new GtOutputTester;
        connect(tester, SIGNAL(finished()), tester, SLOT(deleteLater()));
        tester->start();
        break;
    }
    default:
        break;
    }
}

void
GtOutputDock::scrollToBottom()
{
    QScrollBar* bar = m_listView->verticalScrollBar();

    if (bar != Q_NULLPTR)
    {
        if (bar->value() == bar->maximum())
        {
            m_listView->scrollToBottom();
        }
    }
}

void
GtOutputDock::exportLog()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Log Output"),
                                                    QDir::homePath(),
                                                    tr("Text (*.txt)"));

    if (!fileName.isEmpty())
    {
        gtLogModel->exportLogToFile(fileName);
    }
}

void
GtOutputDock::openContextMenu()
{
    const QModelIndexList rawIndexes =
            m_listView->selectionModel()->selectedIndexes();

    if (rawIndexes.isEmpty())
    {
        return;
    }

    QModelIndexList indexes;

    foreach (const QModelIndex& index, rawIndexes)
    {
        indexes << m_model->mapToSource(index);
    }

    qSort(indexes);

    QMenu menu(this);

    QAction* actionCopy = menu.addAction(gtApp->icon("copyIcon_16.png"),
                           tr("Copy"));

    QAction* actionRemove = menu.addAction(gtApp->icon("clearIcon_16.png"),
                           tr("Delete"));

    QAction* a = menu.exec(QCursor::pos());

    if (a == actionCopy)
    {
        copyToClipboard(indexes);
    }
    else if (a == actionRemove)
    {
        removeItems(indexes);
        //gtDebug() << "Want to delete this";
        //actionRemove(indexes);
    }
}

void
GtOutputDock::onCopyRequest()
{
    const QModelIndexList rawIndexes =
            m_listView->selectionModel()->selectedIndexes();

    if (rawIndexes.isEmpty())
    {
        return;
    }

    QModelIndexList indexes;

    foreach (const QModelIndex& index, rawIndexes)
    {
        indexes << m_model->mapToSource(index);
    }

    qSort(indexes);

    copyToClipboard(indexes);
}

void
GtOutputDock::onDeleteRequest()
{
    const QModelIndexList rawIndexes =
            m_listView->selectionModel()->selectedIndexes();

    if (rawIndexes.isEmpty())
    {
        return;
    }

    QModelIndexList indexes;


    foreach (const QModelIndex& index, rawIndexes)
    {
        indexes << m_model->mapToSource(index);
    }

    qSort(indexes);

    removeItems(indexes);
}
