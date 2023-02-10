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
#include "gt_tableview.h"
#include "gt_application.h"
#include "gt_logging.h"
#include "gt_outputtester.h"
#include "gt_searchwidget.h"
#include "gt_project.h"
#include "gt_datamodel.h"
#include "gt_task.h"
#include "gt_icons.h"

#include <QHeaderView>
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

#include <algorithm>

GtOutputDock::GtOutputDock()
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

    m_logView = new GtTableView;
    m_logView->setFrameStyle(QFrame::NoFrame);
    m_logView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_logView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_logView->setSizeAdjustPolicy(GtTableView::AdjustToContents);
    m_logView->setShowGrid(false);
    m_logView->setModel(m_model);

    // resize the level and time columns as they wont change
    m_logView->resizeColumnToContents(
                GtLogModel::columnFromRole(GtLogModel::LevelRole));
    m_logView->resizeColumnToContents(
                GtLogModel::columnFromRole(GtLogModel::TimeRole));

    // stretch the last section
    m_logView->horizontalHeader()->setStretchLastSection(true);

    // timer to reduce the number of times the view auto resizes itself
    auto* rowResizeTimer = new QTimer{this};
    rowResizeTimer->setInterval(std::chrono::milliseconds{300});
    rowResizeTimer->setSingleShot(true);

    // update row spacing automatically
    connect(rowResizeTimer, &QTimer::timeout, this, [&](){
        m_logView->resizeRowsToContents();
    });

    // resize rows once the section was resized
    connect(m_logView->horizontalHeader(), &QHeaderView::sectionResized,
            this, [&,rowResizeTimer](int idx, int /*oldSize*/, int /*newSize*/){
        // if we have not resized the id column ourselfes we do not
        // want to auto resize the column anymore
        if (!rowResizeTimer->isActive() &&
            idx == GtLogModel::columnFromRole(GtLogModel::IdRole))
        {
            if (!m_resizedColumns)
            {
                m_autoResizeIdColumn = false;
            }
            // clear resize flag
            m_resizedColumns = false;
        }
        // (re-) start timer, which triggers the resize
        rowResizeTimer->start();
    });

    // reset auto resize flag
    connect(gtLogModel, &GtLogModel::logCleared, this, [&](){
        m_autoResizeIdColumn = true;
    });

    // other connections
    connect(gtLogModel, &GtLogModel::rowsInserted,
            this, &GtOutputDock::onRowsInserted);
    connect(m_model, &GtFilteredLogModel::modelReset,
            this, &GtOutputDock::onRowsInserted);
    connect(m_logView, &QWidget::customContextMenuRequested,
            this, &GtOutputDock::openContextMenu);
    connect(m_logView, &GtTableView::copyRequest,
            this, &GtOutputDock::onCopyRequest);
    connect(m_logView, &GtTableView::deleteRequest,
            this, &GtOutputDock::onDeleteRequest);
    connect(m_logView, &GtTableView::searchRequest,
            searchWidget, &GtSearchWidget::enableSearch);

    defaultLayout->addWidget(m_logView);

    connect(searchWidget, &GtSearchWidget::textChanged,
            m_model, &GtFilteredLogModel::filterData);

    filterLayout->addStretch(1);

    /// helper method for setting up an output action button
    const auto setupActionButton =
            [&](QIcon const& icon, QString const& tooltip,
                auto reciever, auto signal){
        auto* button = new QPushButton;
        button->setIcon(icon);
        button->setMaximumSize(QSize(20, 20));
        button->setFlat(true);
        button->setToolTip(tooltip);
        filterLayout->addWidget(button);
        QObject::connect(button, &QPushButton::clicked, reciever, signal);
        return button;
    };

    // clear log button
    setupActionButton(gt::gui::icon::clear(),
                      tr("Clear Output"),
                      gtLogModel, &GtLogModel::clearLog);

    // export log button
    setupActionButton(gt::gui::icon::export_(),
                      tr("Export Output to file"),
                      this, &GtOutputDock::exportLog);

    if (gtApp->devMode())
    {
        // test button
        auto* testButton = setupActionButton(gt::gui::icon::bugPlay(),
                                             tr("Test Output"),
                                             this, &GtOutputDock::testOutput);

        testButton->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(testButton, &QWidget::customContextMenuRequested,
                this, &GtOutputDock::openTestCaseContextMenu);
    }

    /// helper method for setting up an output toggle button
    const auto setupToggleButton =
            [&](QIcon const& icon, QString const& tooltip, auto signal){
        auto* button = new QPushButton;
        button->setIcon(icon);
        button->setMaximumSize(QSize(20, 20));
        button->setFlat(true);
        button->setCheckable(true);
        button->setChecked(true);
        button->setToolTip(tooltip);
        filterLayout->addWidget(button);
        connect(button, &QPushButton::toggled, m_model, signal);
        return button;
    };

    auto loggingLevel = gt::log::Logger::instance().loggingLevel();

    // trace message button
    m_traceButton = setupToggleButton(gt::gui::icon::traceColorized(),
                                      tr("Show/Hide Trace Output"),
                                      &GtFilteredLogModel::filterTraceLevel);

    // debug message button
    m_debugButton = setupToggleButton(gt::gui::icon::bugColorized(),
                                      tr("Show/Hide Debug Output"),
                                      &GtFilteredLogModel::filterDebugLevel);

    // info message button
    m_infoButton = setupToggleButton(gt::gui::icon::infoColorized(),
                                     tr("Show/Hide Info Output"),
                                     &GtFilteredLogModel::filterInfoLevel);

    // warning message button
    m_warningButton = setupToggleButton(gt::gui::icon::warningColorized(),
                                        tr("Show/Hide Warning Output"),
                                        &GtFilteredLogModel::filterWarningLevel);

    // error message button
    m_errorButton = setupToggleButton(gt::gui::icon::errorColorized(),
                                      tr("Show/Hide Error Output"),
                                      &GtFilteredLogModel::filterErrorLevel);

    defaultLayout->addLayout(filterLayout);

    if (loggingLevel > gt::log::TraceLevel)
    {
        m_traceButton->hide();
    }
    if (loggingLevel > gt::log::DebugLevel)
    {
        m_debugButton->hide();
    }

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

    registerShortCut("toggleTraceOutput", QKeySequence(Qt::CTRL + Qt::Key_T));
    registerShortCut("toggleDebugOutput", QKeySequence(Qt::CTRL + Qt::Key_D));
    registerShortCut("toggleInfoOutput", QKeySequence(Qt::CTRL + Qt::Key_I));
    registerShortCut("toggleWarningOutput", QKeySequence(Qt::CTRL + Qt::Key_W));
    registerShortCut("toggleErrorOutput", QKeySequence(Qt::CTRL + Qt::Key_E));
    registerShortCut("clearOutput", QKeySequence(Qt::CTRL + Qt::Key_L));

    m_logView->verticalHeader()->hide();
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
    auto* mimeData = gtLogModel->mimeData(indexes);
    QApplication::clipboard()->setMimeData(mimeData);
}

void
GtOutputDock::removeItems(const QModelIndexList& indexes)
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

    if (QScrollBar* bar = m_logView->verticalScrollBar())
    {
        m_logView->scrollTo(beforeFirst, QAbstractItemView::PositionAtCenter);
        bar->update();
    }
}

void
GtOutputDock::keyPressEvent(QKeyEvent* event)
{
    if (!m_model)
    {
        return;
    }

    const QMetaObject* m = metaObject();
    QString cat = m->className();

    if (gtApp->compareKeyEvent(event, "toggleTraceOutput", cat))
    {
        bool isChecked = m_traceButton->isChecked();
        m_traceButton->setChecked(!isChecked);
        m_model->filterDebugLevel(!isChecked);
        return;
    }
    if (gtApp->compareKeyEvent(event, "toggleDebugOutput", cat))
    {
        bool isChecked = m_debugButton->isChecked();
        m_debugButton->setChecked(!isChecked);
        m_model->filterDebugLevel(!isChecked);
        return;
    }
    if (gtApp->compareKeyEvent(event, "toggleInfoOutput", cat))
    {
        bool isChecked = m_infoButton->isChecked();
        m_infoButton->setChecked(!isChecked);
        m_model->filterDebugLevel(!isChecked);
        return;
    }
    if (gtApp->compareKeyEvent(event, "toggleWarningOutput", cat))
    {
        bool isChecked = m_warningButton->isChecked();
        m_warningButton->setChecked(!isChecked);
        m_model->filterWarningLevel(!isChecked);
        return;
    }
    if (gtApp->compareKeyEvent(event, "toggleErrorOutput", cat))
    {
        bool isChecked = m_errorButton->isChecked();
        m_errorButton->setChecked(!isChecked);
        m_model->filterErrorLevel(!isChecked);
        return;
    }
    if (gtApp->compareKeyEvent(event, "clearOutput", cat))
    {
        return gtLogModel->clearLog();
    }
    if (gtApp->compareKeyEvent(event, "openContextMenu"))
    {
        return openContextMenu();
    }
}

void
GtOutputDock::onRowsInserted()
{
    // check if we should autoresize the id column
    if (m_autoResizeIdColumn)
    {
        // indicate that we have resized the column ourselfes
        m_resizedColumns = true;
        m_logView->resizeColumnToContents(
                    GtLogModel::columnFromRole(GtLogModel::IdRole));
    }
    // resize rows
    m_logView->resizeRowsToContents();

    // only scroll to bottom if scroll bar is at its maximum
    if (QScrollBar* bar = m_logView->verticalScrollBar())
    {
        if (bar->value() == bar->maximum())
        {
            m_logView->scrollToBottom();
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
            m_logView->selectionModel()->selectedIndexes();

    if (rawIndexes.isEmpty())
    {
        return;
    }

    QModelIndexList indexes;

    for (const QModelIndex& index : rawIndexes)
    {
        indexes << m_model->mapToSource(index);
    }

    std::sort(std::begin(indexes), std::end(indexes));

    QMenu menu(this);

    QAction* actionCopy = menu.addAction(gt::gui::icon::copy(), tr("Copy"));

    QAction* actionRemove = menu.addAction(gt::gui::icon::delete_(),
                                           tr("Delete"));

    QAction* a = menu.exec(QCursor::pos());

    if (a == actionCopy)
    {
        return copyToClipboard(indexes);
    }
    if (a == actionRemove)
    {
        return removeItems(indexes);
    }
}

void
GtOutputDock::openTestCaseContextMenu()
{
    QMenu menu(this);
    QVector<QAction*> actions;

    auto const createTestCase = [&](const char* test){
        actions << menu.addAction(tr(test));
    };

    createTestCase("Test log Levels");
    createTestCase("Test custom Ids");
    createTestCase("Test Lorem Ipsum");
    createTestCase("Test message spam");

    QAction* a = menu.exec(QCursor::pos());

    int testCase = 1;
    for (auto* action : qAsConst(actions))
    {
        if (a == action)
        {
            return testOutput(testCase);
        }
        ++testCase;
    }
}

void
GtOutputDock::onCopyRequest()
{
    const QModelIndexList rawIndexes =
            m_logView->selectionModel()->selectedIndexes();

    if (rawIndexes.isEmpty())
    {
        return;
    }

    QModelIndexList indexes;

    for (const QModelIndex& index : rawIndexes)
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
            m_logView->selectionModel()->selectedIndexes();

    if (rawIndexes.isEmpty())
    {
        return;
    }

    QModelIndexList indexes;

    for (const QModelIndex& index : rawIndexes)
    {
        indexes << m_model->mapToSource(index);
    }

    std::sort(std::begin(indexes), std::end(indexes));

    removeItems(indexes);
}

void
GtOutputDock::testOutput(int testCase)
{
    switch (testCase)
    {
    default:
    case 1:
    {
        gtTrace()   << "Trace!";
        gtDebug()   << "Debug!";
        gtInfo()    << "Info!";
        gtWarning() << "Warning!";
        gtError()   << "Error!";
        gtFatal()   << "Fatal!";
        break;
    }
    case 2:
    {
        gtTraceId("Trace Id")     << "My Trace Log!";
        gtDebugId("Debug Id")     << "My Debug Log!";
        gtInfoId("Info Id")       << "My Info Log!";
        gtWarningId("Warning Id") << "My Warning Log!";
        gtErrorId("Error Id")     << "My Error Log!";
        gtFatalId("Fatal Id")     << "My Fatal Log!";
        break;
    }
    case 3:
    {
        gtInfo() << "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, "
                    "sed diam nonumy eirmod tempor invidunt ut labore et "
                    "dolore magna aliquyam erat, sed diam voluptua.\n"
                    "At vero eos et accusam et justo duo dolores et ea rebum.\n"
                    "Stet clita kasd gubergren, no sea takimata sanctus est "
                    "Lorem ipsum dolor sit amet.";
        break;
    }
    case 4:
    {
        GtOutputTester* tester = new GtOutputTester;
        connect(tester, &GtOutputTester::finished,
                tester, &GtOutputTester::deleteLater);
        tester->start();
        break;
    }
    }
}

