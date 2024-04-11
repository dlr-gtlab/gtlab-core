/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
#include "gt_colors.h"
#include "gt_icons.h"
#include "gt_statehandler.h"
#include "gt_state.h"

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

#ifdef QT_DEBUG
#include <QAbstractItemModelTester>
#endif

#include <algorithm>

GtOutputDock::GtOutputDock()
{
    setObjectName(tr("Output"));

    QWidget* widget = new QWidget(this);
    widget->setObjectName("centralWidget");
    setWidget(widget);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);

    // run model test only in debug mode
#ifdef QT_DEBUG
    new QAbstractItemModelTester(
                gtLogModel,
                QAbstractItemModelTester::FailureReportingMode::Fatal,
                this);
#endif

    GtStyledLogModel* styleModel = new GtStyledLogModel(this);
    styleModel->setSourceModel(gtLogModel);
    m_model = new GtFilteredLogModel(styleModel);
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
    m_logView->setFrameStyle(QFrame::NoFrame);
    m_logView->setModel(m_model);

    // stretch the last section
    m_logView->horizontalHeader()->setStretchLastSection(true);

    QFontMetrics metrics{QFont()};
    m_logView->verticalHeader()->setDefaultSectionSize(metrics.height());

    // resize the level and time columns as they wont change
    m_logView->resizeColumnToContents(
        GtLogModel::columnFromRole(GtLogModel::LevelRole));
    m_logView->resizeColumnToContents(
        GtLogModel::columnFromRole(GtLogModel::TimeRole));

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
        // (re-) start timer, which triggers the resize
        rowResizeTimer->start();
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

    const auto setupButton = [&](QIcon const& icon = {},
                                 QString const& tooltip = {}){
        auto* button = new QPushButton;
        button->setIcon(icon);
        button->setMaximumSize(QSize(20, 20));
        button->setFlat(true);
        button->setToolTip(tooltip);
        filterLayout->addWidget(button);
        return button;
    };

    /// helper method for setting up an output action button
    const auto setupActionButton = [&](QIcon const& icon,
                                       QString const& tooltip,
                                       auto* reciever,
                                       auto signal){
        auto* button = setupButton(icon, tooltip);
        QObject::connect(button, &QPushButton::clicked, reciever, signal);
        return button;
    };

    GtObject* guardian = new GtObject;
    guardian->setParent(this);

    /// helper method for setting up an output toggle button
    const auto setupToggleButton = [&](QIcon const& icon,
                                       QString const& type,
                                       QString const& tooltip,
                                       auto* reciever,
                                       auto signal){

        using gt::gui::color::lighten;
        using gt::gui::color::disabled;
        using gt::gui::color::text;
        using gt::gui::colorize; // use custom colors for icon

        auto* button = setupButton(icon, tooltip);
        button->setCheckable(true);

        // checked button do not use On/Off Icons, thus we have to update the
        // icon ourselfes
        auto const updateIconColor = [b = QPointer<QPushButton>(button)](){
            assert (b);
            return b->isChecked() ? text() : lighten(disabled(), 15);
        };
        button->setIcon(colorize(icon, gt::gui::SvgColorData{ updateIconColor }));

        connect(button, &QPushButton::toggled, reciever, signal);

        GtState* state =
            gtStateHandler->initializeState(staticMetaObject.className(),
                                            type, type.toLower(),
                                            QVariant::fromValue(true),
                                            guardian);

        // apply value from state
        bool isChecked = state->getValue().toBool();
        button->setChecked(isChecked);

        connect(button, &QPushButton::toggled, state, [=](bool checked){
            state->setValue(checked, false);
        });

        // trigger signal
        emit button->toggled(isChecked);

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

        // ingore incoming messages button
        auto* ignoreButton = setupButton();
        ignoreButton->setCheckable(true);

        auto updateIgnoreButton = [=](bool isChecked){
            auto& model = GtLogModel::instance();
            model.setEnabled(!isChecked);
            ignoreButton->setIcon(isChecked ? gt::gui::icon::sleepOff() :
                                              gt::gui::icon::sleep());
            ignoreButton->setToolTip(isChecked ? tr("Enable log model") :
                                                 tr("Disable log model"));
        };

        updateIgnoreButton(false);

        connect(ignoreButton, &QPushButton::toggled, this, updateIgnoreButton);
    }

    // add spacer to distinguish filter buttons from the action buttons
    filterLayout->addSpacing(8);

    auto loggingLevel = gt::log::Logger::instance().loggingLevel();

    // trace message button
    m_traceButton = setupToggleButton(gt::gui::icon::trace(),
                                      QStringLiteral("Trace"),
                                      tr("Show/Hide Trace Output"),
                                      m_model,
                                      &GtFilteredLogModel::filterTraceLevel);

    // debug message button
    m_debugButton = setupToggleButton(gt::gui::icon::bug(),
                                      QStringLiteral("Debug"),
                                      tr("Show/Hide Debug Output"),
                                      m_model,
                                      &GtFilteredLogModel::filterDebugLevel);

    // info message button
    m_infoButton = setupToggleButton(gt::gui::icon::info(),
                                     QStringLiteral("Info"),
                                     tr("Show/Hide Info Output"),
                                     m_model,
                                     &GtFilteredLogModel::filterInfoLevel);

    // warning message button
    m_warningButton = setupToggleButton(gt::gui::icon::warning(),
                                        QStringLiteral("Warning"),
                                        tr("Show/Hide Warning Output"),
                                        m_model,
                                        &GtFilteredLogModel::filterWarningLevel);

    // error message button
    m_errorButton = setupToggleButton(gt::gui::icon::error(),
                                      QStringLiteral("Error"),
                                      tr("Show/Hide Error Output"),
                                      m_model,
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

    layout->addWidget(tab);

    widget->setLayout(layout);

    registerShortCut("toggleTraceOutput", QKeySequence(Qt::CTRL + Qt::Key_T));
    registerShortCut("toggleDebugOutput", QKeySequence(Qt::CTRL + Qt::Key_D));
    registerShortCut("toggleInfoOutput", QKeySequence(Qt::CTRL + Qt::Key_I));
    registerShortCut("toggleWarningOutput", QKeySequence(Qt::CTRL + Qt::Key_W));
    registerShortCut("toggleErrorOutput", QKeySequence(Qt::CTRL + Qt::Key_E));
    registerShortCut("clearOutput", QKeySequence(Qt::CTRL + Qt::Key_L));

    m_logView->verticalHeader()->hide();

    QScrollBar* bar = m_logView->verticalScrollBar();
    assert(bar);

    bar->setTracking(false);
    connect(bar, &QScrollBar::valueChanged, this, [this, bar](int value) {
        this->m_autoScrollToBottom = bar->maximum() == value;
    });
}

Qt::DockWidgetArea
GtOutputDock::getDockWidgetArea()
{
    return Qt::BottomDockWidgetArea;
}

void
GtOutputDock::projectChangedEvent(GtProject* /*project*/) { }

void
GtOutputDock::copyToClipboard(const QModelIndexList& indexes)
{
    auto* mimeData = gtLogModel->mimeData(indexes);
    QApplication::clipboard()->setMimeData(mimeData);
}

void
GtOutputDock::removeItems(const QModelIndexList& indexes)
{
    gtLogModel->removeElementList(indexes);
}

void
GtOutputDock::keyPressEvent(QKeyEvent* event)
{
    if (!m_model)
    {
        return;
    }

    QString cat = staticMetaObject.className();

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
    // TODO: fix and remove with #1175
    if (gtApp->compareKeyEvent(event, "copy"))
    {
        return onCopyRequest();
    }

    return GtDockWidget::keyPressEvent(event);
}

void
GtOutputDock::onRowsInserted()
{
    if (m_autoScrollToBottom)
    {
        m_logView->scrollToBottom();
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

