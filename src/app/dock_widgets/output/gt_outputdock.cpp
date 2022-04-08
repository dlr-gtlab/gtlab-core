/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_outputdock.h"

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
#include "gt_icons.h"

#ifdef GT_QML_WIDGETS
#include <QQuickWidget>
#include <QQmlContext>
#endif

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
#include <QLabel>

#include <algorithm>

GtOutputDock::GtOutputDock() :
    m_listView(nullptr),
    m_taskPageView(nullptr),
    m_historyModel(nullptr),
    m_debugButton(nullptr),
    m_infoButton(nullptr),
    m_warningButton(nullptr),
    m_errorButton(nullptr)
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
    tab->addTab(defaultPage, tr("Application"));
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
    clearButton->setIcon(GtGUI::Icon::clear16());
    clearButton->setMaximumSize(QSize(20, 20));
    clearButton->setFlat(true);
    clearButton->setToolTip(tr("Clear Output"));
    filterLayout->addWidget(clearButton);
    connect(clearButton, SIGNAL(clicked(bool)),
            gtLogModel, SLOT(clearLog()));

    // export log button
    QPushButton* exportButton = new QPushButton;
    exportButton->setIcon(GtGUI::Icon::export16());
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
        testButton->setIcon(GtGUI::Icon::file16());
        testButton->setMaximumSize(QSize(20, 20));
        testButton->setFlat(true);
        testButton->setToolTip(tr("Test Output"));
        filterLayout->addWidget(testButton);
        connect(testButton, SIGNAL(clicked(bool)), SLOT(testOutput()));

        // debug message button
        m_debugButton = new QPushButton;
        m_debugButton->setIcon(GtGUI::Icon::bug());
        m_debugButton->setMaximumSize(QSize(20, 20));
        m_debugButton->setFlat(true);
        m_debugButton->setCheckable(true);
        m_debugButton->setChecked(true);
        m_debugButton->setToolTip(tr("Show/Hide Debug Output"));
        filterLayout->addWidget(m_debugButton);
        connect(m_debugButton, SIGNAL(toggled(bool)),
                m_model, SLOT(toggleDebugLevel(bool)));
    }

    // info message button
    m_infoButton = new QPushButton;
    m_infoButton->setIcon(GtGUI::Icon::infoBlue16());
    m_infoButton->setMaximumSize(QSize(20, 20));
    m_infoButton->setFlat(true);
    m_infoButton->setCheckable(true);
    m_infoButton->setChecked(true);
    m_infoButton->setToolTip(tr("Show/Hide Info Output"));
    filterLayout->addWidget(m_infoButton);
    connect(m_infoButton, SIGNAL(toggled(bool)),
            m_model, SLOT(toggleInfoLevel(bool)));

    // warning message button
    m_warningButton = new QPushButton;
    m_warningButton->setIcon(GtGUI::Icon::processFailed16());
    m_warningButton->setMaximumSize(QSize(20, 20));
    m_warningButton->setFlat(true);
    m_warningButton->setCheckable(true);
    m_warningButton->setChecked(true);
    m_warningButton->setToolTip(tr("Show/Hide Warning Output"));
    filterLayout->addWidget(m_warningButton);
    connect(m_warningButton, SIGNAL(toggled(bool)),
            m_model, SLOT(toggleWarningLevel(bool)));

    // error message button
    m_errorButton = new QPushButton;
    m_errorButton->setIcon(GtGUI::Icon::error16());
    m_errorButton->setMaximumSize(QSize(20, 20));
    m_errorButton->setFlat(true);
    m_errorButton->setCheckable(true);
    m_errorButton->setChecked(true);
    m_errorButton->setToolTip(tr("Show/Hide Error Output"));
    filterLayout->addWidget(m_errorButton);
    connect(m_errorButton, SIGNAL(toggled(bool)),
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

//    if (!project)
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
    if (bar)
    {
        m_listView->scrollTo(beforeFirst,
                             QListView::ScrollHint::PositionAtCenter);
        bar->update();
    }
}

void
GtOutputDock::keyPressEvent(QKeyEvent* event)
{
    if (m_model)
    {
        if (gtApp->compareKeyEvent(event, "toggleDebugOutput"))
        {
            if (m_debugButton)
            {
                if (m_debugButton->isChecked())
                {
                    m_debugButton->setChecked(false);
                    m_model->toggleDebugLevel(false);
                }
                else
                {
                    m_debugButton->setChecked(true);
                    m_model->toggleDebugLevel(true);
                }
            }
        }
        if (gtApp->compareKeyEvent(event, "toggleInfoOutput"))
        {
            if (m_infoButton)
            {
                if (m_infoButton->isChecked())
                {
                    m_infoButton->setChecked(false);
                    m_model->toggleInfoLevel(false);
                }
                else
                {
                    m_infoButton->setChecked(true);
                    m_model->toggleInfoLevel(true);
                }
            }
        }
        if (gtApp->compareKeyEvent(event, "toggleWarningOutput"))
        {
            if (m_warningButton)
            {
                if (m_warningButton->isChecked())
                {
                    m_warningButton->setChecked(false);
                    m_model->toggleWarningLevel(false);
                }
                else
                {
                    m_warningButton->setChecked(true);
                    m_model->toggleWarningLevel(true);
                }
            }
        }
        if (gtApp->compareKeyEvent(event, "toggleErrorOutput"))
        {
            if (m_errorButton)
            {
                if (m_errorButton->isChecked())
                {
                    m_errorButton->setChecked(false);
                    m_model->toggleErrorLevel(false);
                }
                else
                {
                    m_errorButton->setChecked(true);
                    m_model->toggleErrorLevel(true);
                }
            }
        }
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

    if (bar)
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

    std::sort(std::begin(indexes), std::end(indexes));

    QMenu menu(this);

    QAction* actionCopy = menu.addAction(GtGUI::Icon::copy16(), tr("Copy"));

    QAction* actionRemove = menu.addAction(GtGUI::Icon::clear16(),
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

    std::sort(std::begin(indexes), std::end(indexes));

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

    std::sort(std::begin(indexes), std::end(indexes));

    removeItems(indexes);
}
