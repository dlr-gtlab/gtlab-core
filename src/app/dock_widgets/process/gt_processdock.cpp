/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 02.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QMimeData>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QLineEdit>
#include <QInputDialog>
#include <QStackedWidget>
#include <QSet>
#include <QPointer>

#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>

#include "gt_algorithms.h"
#include "gt_mainwin.h"
#include "gt_processview.h"
#include "gt_datamodel.h"
#include "gt_project.h"
#include "gt_processdata.h"
#include "gt_processcomponentmodel.h"
#include "gt_searchwidget.h"
#include "gt_task.h"
#include "gt_calculator.h"
#include "gt_processfactory.h"
#include "gt_objectmemento.h"
#include "gt_processwizard.h"
#include "gt_calculatorprovider.h"
#include "gt_processfiltermodel.h"
#include "gt_processexecutor.h"
#include "gt_processrunner.h"
#include "gt_processconnectioneditor.h"
#include "gt_objectmementodiff.h"
#include "gt_taskprovider.h"
#include "gt_propertyconnection.h"
#include "gt_application.h"
#include "gt_settings.h"
#include "gt_extendedtaskdata.h"
#include "gt_extendedcalculatordata.h"
#include "gt_taskfactory.h"
#include "gt_calculatorfactory.h"
#include "gt_textfilterdelegate.h"
#include "gt_deleteitemmessagebox.h"
#include "gt_objectui.h"
#include "gt_stylesheets.h"
#include "gt_importmenu.h"
#include "gt_propertyconnectionfunctions.h"
#include "gt_icons.h"
#include "gt_utilities.h"
#include "gt_guiutilities.h"
#include "gt_taskgroupmodel.h"
#include "gt_statehandler.h"
#include "gt_state.h"
#include "gt_command.h"
#include "gt_logging.h"
#include "gt_processdockextensions.h"
#include "internal/gt_processdockextensions_p.h"

#include "gt_processdock.h"

#include "gt_taskgrouprenamedialog.h"

using namespace gt::gui;

namespace
{

inline bool
useExtendedProcessExecutor()
{
    return gtApp->devMode() && gtApp->settings()->useExtendedProcessExecutor();
}

} // namespace

class GtProcessDock::Impl
{
public:
    QComboBox* taskGroupSelection{nullptr};
    QComboBox* pageSelection{nullptr};
    QStackedWidget* pageStack{nullptr};
    bool pagesInitialized{false};
    QPushButton* runButton{nullptr};
    QPushButton* addElementButton{nullptr};
    QPushButton* processQueueButton{nullptr};
    GtProcessView* view{nullptr};
    GtProcessComponentModel* model{nullptr};
    GtTaskGroupModel* taskGroupModel{nullptr};
    QPointer<GtTreeFilterModel> filterModel;
    GtSearchWidget* search{nullptr};
    QPointer<GtTaskGroup> taskGroup;
    QPointer<GtTask> currentProcess;
    QPointer<GtProject> project;
    GtCommand command{};
    GtState* expandedItemUuidsState{nullptr};
    GtState* lastTaskGroupScopeState{nullptr};
    GtState* lastTaskGroupIdState{nullptr};
};

GtProcessDock::GtProcessDock() :
    m_impl(std::make_unique<Impl>())
{
    setObjectName(tr("Processes/Calculators"));

    auto widget = new QWidget(this);
    setWidget(widget);

    auto processPage = new QWidget;

    auto frame = new QFrame;
    auto frameLayout = new QVBoxLayout;

    frameLayout->setContentsMargins(0, 0, 0, 0);
    frameLayout->setSpacing(0);

    m_impl->runButton = new QPushButton(tr("Run"));
    m_impl->runButton->setIcon(gt::gui::icon::processRun());
    m_impl->runButton->setToolTip(tr("Run Selected Process"));
    m_impl->runButton->setEnabled(false);

    m_impl->runButton->setStyleSheet(gt::gui::stylesheet::processRunButton(
        gt::gui::stylesheet::RunButtonState::NotSelected));

    m_impl->addElementButton = new QPushButton(tr("Add..."));
    m_impl->addElementButton->setIcon(gt::gui::icon::add());
    m_impl->addElementButton->setToolTip(tr("Add New Element"));
    m_impl->addElementButton->setEnabled(false);
    m_impl->addElementButton->setStyleSheet(gt::gui::stylesheet::button());

    auto btnLayout = new QHBoxLayout;
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(1);
    btnLayout->addWidget(m_impl->runButton);
    btnLayout->addWidget(m_impl->addElementButton);

    m_impl->processQueueButton = new QPushButton();
    m_impl->processQueueButton->setIcon(gt::gui::icon::queue());
    m_impl->processQueueButton->setMaximumSize(QSize(20, 20));
    m_impl->processQueueButton->setFlat(true);
    m_impl->processQueueButton->setToolTip(tr("View Process Queue"));
    m_impl->processQueueButton->setEnabled(false);

    m_impl->search = new GtSearchWidget;

    QHBoxLayout* filterLayout = new QHBoxLayout;
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(0);
    filterLayout->addWidget(m_impl->search);
    filterLayout->addStretch(1);
    filterLayout->addWidget(m_impl->processQueueButton);

    m_impl->view = new GtProcessView(this);
    frameLayout->addWidget(m_impl->view);
    frameLayout->addLayout(filterLayout);

    frame->setLayout(frameLayout);
    frame->setAutoFillBackground(true);

    auto delegate =
        new GtTextFilterDelegate(this, GtTextFilterDelegate::allowSpaces);

    m_impl->view->setFrameStyle(QTreeView::NoFrame);
    m_impl->view->setItemDelegate(delegate);
    m_impl->view->setEditTriggers(QTreeView::SelectedClicked);

    // task group overview and selection
    m_impl->taskGroupSelection = new QComboBox;
    m_impl->taskGroupSelection->setEnabled(false);

    m_impl->taskGroupModel = new GtTaskGroupModel(this);
    m_impl->taskGroupSelection->setModel(m_impl->taskGroupModel);

    auto* addTaskGroupButton = new QPushButton;
    addTaskGroupButton->setIcon(gt::gui::icon::add());
    addTaskGroupButton->setToolTip(tr("Add new custom task group"));
    addTaskGroupButton->setMaximumWidth(40);
    connect(addTaskGroupButton, &QPushButton::clicked, this,
            &GtProcessDock::addNewCustomTaskGroup);

    auto* deleteTaskGroupButton = new QPushButton;
    deleteTaskGroupButton->setObjectName(
        QStringLiteral("deleteTaskGroupButton"));
    deleteTaskGroupButton->setIcon(gt::gui::icon::remove());
    deleteTaskGroupButton->setToolTip(tr("Delete selected custom task group"));
    deleteTaskGroupButton->setEnabled(false);
    deleteTaskGroupButton->setMaximumWidth(40);
    connect(deleteTaskGroupButton, &QPushButton::clicked, this,
            &GtProcessDock::onDeleteTaskGroupButtonClicked);

    auto* renameTaskGroupButton = new QPushButton;
    renameTaskGroupButton->setIcon(gt::gui::icon::rename());
    renameTaskGroupButton->setToolTip(tr("Rename selected custom task group"));
    renameTaskGroupButton->setEnabled(false);
    renameTaskGroupButton->setMaximumWidth(40);
    renameTaskGroupButton->setObjectName(
        QStringLiteral("renameTaskGroupButton"));
    connect(renameTaskGroupButton, &QPushButton::clicked, this,
            &GtProcessDock::onRenameTaskGroupButtonClicked);

    auto* taskGroupLayout = new QHBoxLayout;
    taskGroupLayout->setContentsMargins(0, 0, 0, 0);
    taskGroupLayout->setSpacing(2);
    taskGroupLayout->addWidget(m_impl->taskGroupSelection);
    taskGroupLayout->addWidget(addTaskGroupButton);
    taskGroupLayout->addWidget(deleteTaskGroupButton);
    taskGroupLayout->addWidget(renameTaskGroupButton);

    auto layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    layout->addLayout(taskGroupLayout);
    layout->addLayout(btnLayout);

    layout->addWidget(frame);

    processPage->setLayout(layout);

    m_impl->pageSelection = new QComboBox(widget);
    m_impl->pageSelection->setObjectName(
        QStringLiteral("processDockPageSelection"));
    m_impl->pageSelection->addItem(tr("Processes"),
                                   QStringLiteral("processes"));
    m_impl->pageSelection->hide();

    m_impl->pageStack = new QStackedWidget(widget);
    m_impl->pageStack->setObjectName(QStringLiteral("processDockPages"));
    m_impl->pageStack->addWidget(processPage);

    auto* dockLayout = new QVBoxLayout(widget);
    dockLayout->setContentsMargins(0, 0, 0, 0);
    dockLayout->setSpacing(0);
    dockLayout->addWidget(m_impl->pageSelection);
    dockLayout->addWidget(m_impl->pageStack);

    m_impl->model = new GtProcessComponentModel(this);
    m_impl->filterModel = new GtProcessFilterModel(m_impl->model);
    m_impl->model->setSourceModel(gtDataModel);
    m_impl->filterModel->setSourceModel(m_impl->model);
    m_impl->view->setModel(m_impl->filterModel);

    connect(m_impl->model,
            SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)),
            SLOT(onRowsAboutToBeMoved()));
    connect(m_impl->model,
            SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)),
            SLOT(onRowsMoved()));

    // Current changed (keyboard navigation), Mouse clicks are filtered
    // to improve drag and drop functionality
    connect(m_impl->view->selectionModel(),
            &QItemSelectionModel::currentChanged, this,
            [this](const QModelIndex& current, const QModelIndex&) {
                if (QApplication::mouseButtons() == Qt::NoButton)
                    onCurrentChanged(current);
            });

    connect(m_impl->view, SIGNAL(collapsed(QModelIndex)), this,
            SLOT(itemCollapsed(QModelIndex)));
    connect(m_impl->view, SIGNAL(expanded(QModelIndex)), this,
            SLOT(itemExpanded(QModelIndex)));

    connect(gtDataModel, SIGNAL(triggerEndResetDataModelView()),
            SLOT(endResetView()), Qt::DirectConnection);

    connect(gtApp, &GtApplication::themeChanged, this, [&]() {
        m_impl->addElementButton->setStyleSheet(gt::gui::stylesheet::button());
        updateRunButton();
    });

    connect(m_impl->search, SIGNAL(textEdited(QString)),
            SLOT(filterData(QString)));
    connect(m_impl->search, SIGNAL(textChanged(QString)),
            SLOT(filterData(QString)));
    connect(m_impl->view, SIGNAL(searchRequest()), m_impl->search,
            SLOT(enableSearch()));
    connect(m_impl->view, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(customContextMenu(QPoint)));
    connect(m_impl->view, SIGNAL(clicked(QModelIndex)),
            SLOT(onClicked(QModelIndex)));
    connect(m_impl->view, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(onDoubleClicked(QModelIndex)));
    connect(m_impl->view, SIGNAL(copyProcessElement(QModelIndex)),
            SLOT(copyElement(QModelIndex)));
    connect(m_impl->view, SIGNAL(cutProcessElement(QModelIndex)),
            SLOT(cutElement(QModelIndex)));
    connect(m_impl->view, SIGNAL(cloneProcessElement(QModelIndex)),
            SLOT(cloneElement(QModelIndex)));
    connect(m_impl->view, SIGNAL(deleteProcessElements(QList<QModelIndex>)),
            SLOT(deleteProcessElements(QList<QModelIndex>)));
    connect(m_impl->view, SIGNAL(pasteProcessElement(QModelIndex)),
            SLOT(pasteElement(QModelIndex)));
    connect(m_impl->view, SIGNAL(runTaskElement(QModelIndex)),
            SLOT(runProcess()));
    connect(m_impl->view,
            SIGNAL(skipCalculatorElements(const QList<QModelIndex>&, bool)),
            SLOT(skipComponents(const QList<QModelIndex>&, bool)));
    connect(m_impl->view, SIGNAL(renameProcessElement(QModelIndex)),
            SLOT(renameElement()));

    connect(m_impl->runButton, SIGNAL(clicked(bool)), SLOT(runProcess()));
    connect(m_impl->addElementButton, SIGNAL(clicked(bool)),
            SLOT(addElement()));

    connect(m_impl->taskGroupSelection, SIGNAL(currentIndexChanged(int)),
            SLOT(currentTaskGroupIndexChanged(int)));

    // open process queue via main window
    connect(m_impl->processQueueButton, &QPushButton::clicked, this, [&](bool) {
        // main window may not yet exist
        if (auto mainwin = qobject_cast<GtMainWin*>(parentWidget()))
        {
            mainwin->openProcessQueue();
        }
    });

    // connect each executor as it becomes available
    connect(&gt::processExecutorManager(),
            &GtProcessExecutorManager::executorChanged, this,
            &GtProcessDock::onExecutorChanged, Qt::UniqueConnection);

    auto* processRunner = gt::findExecutorByType<GtProcessRunner>();
    assert(processRunner);
    connect(processRunner, &GtProcessRunner::connectionStateChanged, this,
            [this]() { updateRunButton(); });

    connect(this, SIGNAL(selectedObjectChanged(GtObject*)), gtApp,
            SIGNAL(objectSelected(GtObject*)));

    registerShortCut("runProcess", QKeySequence(Qt::CTRL | Qt::Key_R));
    registerShortCut("unskipProcess", QKeySequence(Qt::CTRL | Qt::Key_T));
    registerShortCut("skipProcess", QKeySequence(Qt::CTRL | Qt::Key_G));

    // udpate executor
    onExecutorChanged(&gt::currentProcessExecutor());
}

GtProcessDock::~GtProcessDock() = default;

Qt::DockWidgetArea
GtProcessDock::getDockWidgetArea()
{
    return Qt::RightDockWidgetArea;
}

void
GtProcessDock::initAfterStartup()
{
    GtDockWidget::initAfterStartup();

    if (m_impl->pagesInitialized)
    {
        return;
    }
    m_impl->pagesInitialized = true;

    gt::gui::detail::initializeProcessDockPages(m_impl->pageStack,
                                                m_impl->pageSelection, this);
}

void
GtProcessDock::setCurrentProcess(GtTask* process)
{
    if (process == m_impl->currentProcess)
    {
        return;
    }

    if (m_impl->currentProcess)
    {
        disconnect(m_impl->currentProcess, SIGNAL(destroyed()), this,
                   SLOT(onProcessDestroyed()));
        disconnect(m_impl->currentProcess, SIGNAL(objectNameChanged(QString)),
                   this, SLOT(updateRunButton()));
        disconnect(m_impl->currentProcess, SIGNAL(skipPropertyChanged()), this,
                   SLOT(updateRunButton()));
        disconnect(m_impl->currentProcess, SIGNAL(dataChanged(GtObject*)), this,
                   SLOT(onTaskDataChanged(GtObject*)));
    }

    m_impl->currentProcess = process;

    if (process)
    {
        connect(process, SIGNAL(destroyed()), SLOT(onProcessDestroyed()));
        connect(process, SIGNAL(objectNameChanged(QString)),
                SLOT(updateRunButton()));
        connect(process, SIGNAL(skipPropertyChanged()),
                SLOT(updateRunButton()));
        connect(process, SIGNAL(dataChanged(GtObject*)),
                SLOT(onTaskDataChanged(GtObject*)));
    }
    else
    {
        m_impl->runButton->setEnabled(false);
    }
}

void
GtProcessDock::projectChangedEvent(GtProject* project)
{
    m_impl->processQueueButton->setEnabled(project);
    m_impl->addElementButton->setEnabled(project);

    if (project != m_impl->project)
    {
        m_impl->project = project;
        m_impl->taskGroupSelection->clear();

        if (m_impl->project && m_impl->project->processData())
        {
            resetTaskGroupModel();

            m_impl->taskGroupSelection->setEnabled(true);

            m_impl->expandedItemUuidsState = gtStateHandler->initializeState(
                m_impl->project, QStringLiteral("ProcessDock"),
                QStringLiteral("Expanded Process Dock Item UUIDs"),
                "default;ProcessDock;expandPdItemUuids", QStringList{},
                m_impl->project);

            m_impl->lastTaskGroupScopeState = gtStateHandler->initializeState(
                m_impl->project, QStringLiteral("ProcessDock"),
                QStringLiteral("Scope of the last selected Task Group"),
                "default;ProcessDock;lastTaskGroupScope", GtTaskGroup::USER,
                m_impl->project);

            m_impl->lastTaskGroupIdState = gtStateHandler->initializeState(
                m_impl->project, QStringLiteral("ProcessDock"),
                QStringLiteral("ID of the last selected Task Group"),
                "default;ProcessDock;lastTaskGroupId",
                GtTaskGroup::defaultUserGroupId(), m_impl->project);

            m_impl->project->processData()->switchCurrentTaskGroup(
                lastTaskGroupId(), lastTaskGroupScope(),
                m_impl->project->path());
        }
        else
        {
            m_impl->taskGroupModel->init({}, {});

            m_impl->taskGroupSelection->setEnabled(false);

            m_impl->expandedItemUuidsState = nullptr;
            m_impl->lastTaskGroupScopeState = nullptr;
            m_impl->lastTaskGroupIdState = nullptr;
        }

        // update current task group
        updateCurrentTaskGroup();

        if (m_impl->taskGroup)
        {
            m_impl->taskGroupSelection->setCurrentText(
                m_impl->taskGroup->objectName());
        }

        if (m_impl->project && m_impl->project->processData())
        {
            updateCustomTaskGroupsButtons();
        }
    }
}

void
GtProcessDock::updateCurrentTaskGroup()
{
    auto taskGroup = m_impl->project && m_impl->project->processData()
                         ? m_impl->project->processData()->taskGroup()
                         : nullptr;

    if (taskGroup && m_impl->taskGroup == taskGroup)
    {
        return;
    }

    m_impl->taskGroup = taskGroup;

    setCurrentProcess();

    updateButtons(m_impl->taskGroup);

    updateProcessViewRootIndex();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    m_impl->filterModel->setFilterRegExp(m_impl->search->text());
#else
    m_impl->filterModel->setFilterRegularExpression(m_impl->search->text());
#endif

    m_impl->view->resizeColumns();
}

void
GtProcessDock::updateProcessViewRootIndex()
{
    if (!m_impl->taskGroup)
    {
        return;
    }

    auto index = mapFromSource(gtDataModel->indexFromObject(m_impl->taskGroup));

    if (index.isValid() && m_impl->view->rootIndex() != index)
    {
        m_impl->view->setRootIndex(index);
        restoreExpandStates();
        m_impl->view->setCurrentIndex({});
    }
}

QStringList
GtProcessDock::expandedItemUuids() const
{
    if (!m_impl->expandedItemUuidsState)
    {
        return {};
    }

    return m_impl->expandedItemUuidsState->getValue().toStringList();
}

void
GtProcessDock::setExpandedItemUuids(const QStringList& uuids)
{
    if (m_impl->expandedItemUuidsState)
    {
        m_impl->expandedItemUuidsState->setValue(uuids, false);
    }
}

GtTaskGroup::SCOPE
GtProcessDock::lastTaskGroupScope() const
{
    if (!m_impl->lastTaskGroupScopeState)
    {
        return {};
    }

    return m_impl->lastTaskGroupScopeState->getValue()
        .value<GtTaskGroup::SCOPE>();
}

void
GtProcessDock::setLastTaskGroupScope(GtTaskGroup::SCOPE scope)
{
    if (m_impl->lastTaskGroupScopeState)
    {
        m_impl->lastTaskGroupScopeState->setValue(scope, false);
    }
}

QString
GtProcessDock::lastTaskGroupId() const
{
    if (!m_impl->lastTaskGroupIdState)
    {
        return {};
    }

    return m_impl->lastTaskGroupIdState->getValue().toString();
}

void
GtProcessDock::setLastTaskGroupId(const QString& groupId)
{
    if (m_impl->lastTaskGroupIdState)
    {
        m_impl->lastTaskGroupIdState->setValue(groupId, false);
    }
}

void
GtProcessDock::resetTaskGroupModel()
{
    if (!m_impl->project || !m_impl->project->processData())
    {
        return;
    }

    auto userGroups = m_impl->project->processData()->userGroupIds();
    auto customGroups = m_impl->project->processData()->customGroupIds();

    userGroups.sort(Qt::CaseInsensitive);
    customGroups.sort(Qt::CaseInsensitive);

    m_impl->taskGroupModel->init(userGroups, customGroups);
}

void
GtProcessDock::addEmptyTask(GtObject* root)
{
    if (!root) return;

    QString taskId = gt::makeUniqueName(tr("New Task"), root);

    if (taskId.isEmpty()) return;

    m_impl->view->setFocus();

    auto task = std::make_unique<GtTask>();
    task->setObjectName(taskId);
    task->setFactory(gtProcessFactory);

    QModelIndex srcIndex = gtDataModel->appendChild(std::move(task), root);

    QModelIndex index = mapFromSource(srcIndex);

    if (!index.isValid()) return;

    QModelIndex tmpIndex =
        m_impl->filterModel->index(index.row(), 1, index.parent());
    QItemSelection itemSel(index, tmpIndex);

    m_impl->view->scrollTo(index);
    m_impl->view->selectionModel()->select(itemSel,
                                           QItemSelectionModel::ClearAndSelect);
    m_impl->view->setCurrentIndex(index);
    m_impl->view->edit(index);
}

GtProcessComponent*
GtProcessDock::componentByModelIndex(const QModelIndex& index)
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

    if (srcIndex.model() != gtDataModel)
    {
        return nullptr;
    }

    return qobject_cast<GtProcessComponent*>(
        gtDataModel->objectFromIndex(srcIndex));
}

void
GtProcessDock::addCalculator(GtObject* parentObj)
{
    QPointer<GtObject> guardedParent{parentObj};
    if (!guardedParent || !qobject_cast<GtTask*>(parentObj))
    {
        return;
    }

    QPointer<GtProject> project{parentObj->findParent<GtProject*>()};
    if (!project)
    {
        return;
    }

    GtCalculatorProvider provider;
    GtProcessWizard wizard(project.data(), &provider, this);
    wizard.resize(560, 500);

    if (!wizard.exec() || !guardedParent || !project ||
        project != gtApp->currentProject())
    {
        return;
    }

    GtObjectMemento memento = provider.componentData();

    if (memento.isNull())
    {
        return;
    }

    auto newObj = memento.restore_unique<GtProcessComponent>(gtProcessFactory);

    if (!newObj)
    {
        return;
    }

    gtDebug() << "Calculator appended! (" << newObj->metaObject()->className()
              << ")";

    updateLastUsedElementList(newObj->metaObject()->className());

    QModelIndex newIndex =
        gtDataModel->appendChild(std::move(newObj), guardedParent.data());

    QModelIndex index = mapFromSource(newIndex);

    if (!index.isValid())
    {
        return;
    }

    m_impl->view->setFocus();

    QModelIndex tmpIndex =
        m_impl->filterModel->index(index.row(), 1, index.parent());
    QItemSelection itemSel(index, tmpIndex);

    m_impl->view->expand(
        mapFromSource(gtDataModel->indexFromObject(guardedParent.data())));
    m_impl->view->scrollTo(index);
    m_impl->view->selectionModel()->select(itemSel,
                                           QItemSelectionModel::ClearAndSelect);
    m_impl->view->setCurrentIndex(index);
}

void
GtProcessDock::addTask(GtObject* parentObj)
{
    addTaskToParent(parentObj);
}

void
GtProcessDock::addRootTask(GtTaskGroup* root)
{
    addTaskToParent(root);
}

void
GtProcessDock::addTaskToParent(GtObject* parentObj)
{
    QPointer<GtObject> guardedParent{parentObj};
    if (!guardedParent)
    {
        return;
    }

    QPointer<GtProject> project{parentObj->findParent<GtProject*>()};
    if (!project)
    {
        gtError() << "project == nullptr";
        return;
    }

    GtTaskProvider provider;
    GtProcessWizard wizard(project.data(), &provider, this);
    wizard.resize(560, 500);

    if (!wizard.exec() || !guardedParent || !project ||
        project != gtApp->currentProject())
    {
        return;
    }

    GtObjectMemento memento = provider.componentData();

    if (memento.isNull()) return;

    auto newObj = memento.restore_unique<GtProcessComponent>(gtProcessFactory);

    if (!newObj) return;

    QString taskId =
        gt::makeUniqueName(newObj->objectName(), guardedParent.data());
    newObj->setObjectName(taskId);

    gtDebug().verbose()
        << tr("Task appended! (%1)").arg(newObj->metaObject()->className());

    updateLastUsedElementList(newObj->metaObject()->className());

    QModelIndex newIndex =
        gtDataModel->appendChild(std::move(newObj), guardedParent.data());

    QModelIndex index = mapFromSource(newIndex);

    if (!index.isValid()) return;

    m_impl->view->setFocus();

    QModelIndex tmpIndex =
        m_impl->filterModel->index(index.row(), 1, index.parent());
    QItemSelection itemSel(index, tmpIndex);

    m_impl->view->expand(
        mapFromSource(gtDataModel->indexFromObject(guardedParent.data())));
    m_impl->view->scrollTo(index);
    m_impl->view->selectionModel()->select(itemSel,
                                           QItemSelectionModel::ClearAndSelect);
    m_impl->view->setCurrentIndex(index);
    m_impl->view->edit(index);
}

GtTask*
GtProcessDock::findRootTaskHelper(GtObject* obj)
{
    GtTask* retval = nullptr;

    if (!obj)
    {
        return retval;
    }

    retval = qobject_cast<GtTask*>(obj);

    if (!retval)
    {
        // object not a task
        return findRootTaskHelper(obj->findParent<GtTask*>());
    }

    // object a task
    if (!qobject_cast<GtTaskGroup*>(obj->parent()))
    {
        return findRootTaskHelper(obj->findParent<GtTask*>());
    }

    return retval;
}

bool
GtProcessDock::componentIsReady(GtProcessComponent* comp)
{
    if (!comp)
    {
        return false;
    }

    return comp->isReady();
}

void
GtProcessDock::filterData(const QString& val)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    m_impl->filterModel->setFilterRegExp(val);
#else
    m_impl->filterModel->setFilterRegularExpression(val);
#endif

    if (!m_impl->view->rootIndex().isValid())
    {
        updateProcessViewRootIndex();
    }
    else
    {
        restoreExpandStates();
    }
}

void
GtProcessDock::updateButtons(GtObject* obj)
{
    m_impl->addElementButton->setEnabled(m_impl->taskGroup);

    setCurrentProcess(findRootTaskHelper(obj));

    if (obj && (qobject_cast<GtCalculator*>(obj) || obj->isDummy() ||
                obj->hasDummyParents()))
    {
        m_impl->addElementButton->setEnabled(false);
    }

    updateRunButton();

    updateCustomTaskGroupsButtons();
}

void
GtProcessDock::updateRunButton()
{
    // no process selected
    if (!m_impl->currentProcess)
    {
        m_impl->runButton->setIcon(gt::gui::icon::processRun());

        m_impl->runButton->setStyleSheet(gt::gui::stylesheet::processRunButton(
            gt::gui::stylesheet::RunButtonState::NotSelected));

        m_impl->runButton->setEnabled(false);
        m_impl->runButton->setText(tr("Run"));
        m_impl->runButton->setToolTip(tr("No Process selected"));
        return;
    }

    const QString& str = m_impl->currentProcess->objectName();

    const GtTask* current = gt::currentProcessExecutor().currentRunningTask();

    // no task is running
    if (!current)
    {

        m_impl->runButton->setIcon(gt::gui::icon::processRun());
        m_impl->runButton->setStyleSheet(gt::gui::stylesheet::processRunButton(
            gt::gui::stylesheet::RunButtonState::RunProcess));

        if (str.isEmpty())
        {
            m_impl->runButton->setEnabled(false);
            m_impl->runButton->setToolTip(tr("No Process selected"));
            m_impl->runButton->setText(tr("Run"));
        }
        else
        {
            m_impl->runButton->setEnabled(true);
            m_impl->runButton->setToolTip(tr("Run Selected Process"));

            QString additional = gt::squoted(str);

            if (useExtendedProcessExecutor() &&
                &gt::currentProcessExecutor() ==
                    gt::findExecutorByType<GtProcessRunner>())
            {
                additional += QStringLiteral(" (detached)");
            }

            m_impl->runButton->setText(tr("Run") + ' ' + additional);
        }
    }
    // a task is running
    else if (current != m_impl->currentProcess)
    {
        // queue
        m_impl->runButton->setIcon(gt::gui::icon::queue());

        m_impl->runButton->setStyleSheet(gt::gui::stylesheet::processRunButton(
            gt::gui::stylesheet::RunButtonState::QueueProcess));

        if (gt::currentProcessExecutor().taskQueued(m_impl->currentProcess))
        {
            m_impl->runButton->setEnabled(false);
            m_impl->runButton->setText(tr("'%1' already queued").arg(str));
            m_impl->runButton->setToolTip(tr("Process already queued"));
        }
        else
        {
            m_impl->runButton->setEnabled(true);
            m_impl->runButton->setText(tr("Add '%1' to queue").arg(str));
            m_impl->runButton->setToolTip(tr("Add Selected Process to Queue"));
        }
    }
    // selected task is running
    else
    {
        m_impl->runButton->setStyleSheet(gt::gui::stylesheet::processRunButton(
            gt::gui::stylesheet::RunButtonState::StopProcess));

        // stopping
        if (m_impl->currentProcess->currentState() ==
            GtProcessComponent::TERMINATION_REQUESTED)
        {
            m_impl->runButton->setIcon(gt::gui::icon::timer());
            m_impl->runButton->setEnabled(false);
            m_impl->runButton->setToolTip(tr("Terminating process..."));
            m_impl->runButton->setText(str + tr(" terminating..."));
        }
        else
        {
            m_impl->runButton->setIcon(gt::gui::icon::stop());
            m_impl->runButton->setEnabled(true);

            QString text;
            QString info = gt::brackets(str);

            auto* processRunner = gt::findExecutorByType<GtProcessRunner>();
            assert(processRunner);

            switch (processRunner->connectionState())
            {
            case GtProcessRunnerTransceiver::StartingRunner:
                text = tr("Starting Runner...");
                break;
            case GtProcessRunnerTransceiver::Connecting:
                text = tr("Connecting...");
                break;
            case GtProcessRunnerTransceiver::Transmitting:
                text = tr("Transmitting...");
                break;
            case GtProcessRunnerTransceiver::Recieving:
                text = tr("Recieving...");
                break;
            case GtProcessRunnerTransceiver::ConnectionLost:
                text = tr("Connection lost");
                break;
            default:
                text = tr("Stop");
                break;
            }

            m_impl->runButton->setToolTip(tr("Stop Selected Process"));
            m_impl->runButton->setText(text + ' ' + info);
        }
    }

    // disable run button if process is not ready, skip running task
    if (current != m_impl->currentProcess &&
        (m_impl->currentProcess->hasDummyChildren() ||
         m_impl->currentProcess->isSkipped()))
    {
        // text, style and icon are already set
        m_impl->runButton->setEnabled(false);
        m_impl->runButton->setToolTip(tr("Invalid Process selected"));
        m_impl->runButton->setStyleSheet(gt::gui::stylesheet::processRunButton(
            gt::gui::stylesheet::RunButtonState::NotSelected));
    }
}

void
GtProcessDock::onProcessDestroyed()
{
    setCurrentProcess();
}

void
GtProcessDock::onCurrentChanged(const QModelIndex& current,
                                const QModelIndex& /*previous*/)
{
    QModelIndex srcIndex = mapToSource(current);
    GtObject* obj = gtDataModel->objectFromIndex(srcIndex);
    updateButtons(obj);

    // Restoring an index after a model reset can select the same object again.
    if (obj && obj == gtApp->selectedObject()) return;

    emit selectedObjectChanged(obj);
}

void
GtProcessDock::onClicked(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    onCurrentChanged(index);
}

void
GtProcessDock::onDoubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    QModelIndex srcIndex = mapToSource(index);

    GtObject* obj = gtDataModel->objectFromIndex(srcIndex);

    if (auto task = qobject_cast<GtTask*>(obj))
    {
        if (!task->isReady())
        {
            return;
        }

        configTask(task);
    }
    else if (auto calc = qobject_cast<GtCalculator*>(obj))
    {

        if (!calc->isReady())
        {
            return;
        }

        configCalculator(calc);
    }
}

void
GtProcessDock::runProcess()
{
    QModelIndex srcIndex = mapToSource(m_impl->view->currentIndex());

    if (srcIndex.isValid())
    {
        if (GtObject* obj = gtDataModel->objectFromIndex(srcIndex))
        {
            if (GtTask* task = findRootTaskHelper(obj))
            {
                if (!task->isReady())
                {
                    terminateProcess();
                }
                else
                {
                    gt::currentProcessExecutor().startTask(task);
                }
            }
        }
    }
}

void
GtProcessDock::terminateProcess()
{
    if (!m_impl->currentProcess)
    {
        return;
    }

    gt::currentProcessExecutor().terminateTask(m_impl->currentProcess);
}

void
GtProcessDock::addElement()
{
    QMenu menu(this);

    makeAddMenu(menu);

    menu.exec(QCursor::pos());
}

void
GtProcessDock::makeAddMenu(QMenu& menu, GtObject* selectedComponent)
{
    const QModelIndex srcIdx = mapToSource(m_impl->view->currentIndex());
    GtObject* selectedObject =
        srcIdx.isValid() ? gtDataModel->objectFromIndex(srcIdx) : nullptr;

    // Snapshot the insertion context because a context click may differ from
    // the current selection while the menu is open.
    const QPointer<GtObject> insertionTarget{gt::gui::detail::processAddTarget(
        selectedComponent, selectedObject, m_impl->taskGroup)};
    const QPointer<GtTaskGroup> rootTarget = m_impl->taskGroup;

    auto addEmptyRootTask = [this, rootTarget](GtObject*) {
        if (rootTarget)
        {
            addEmptyTask(rootTarget.data());
        }
    };
    auto addemptytask =
        gt::gui::makeAction(tr("Empty Root Task"), addEmptyRootTask)
            .setIcon(gt::gui::icon::processAdd());
    gt::gui::addToMenu(addemptytask, menu, nullptr);

    menu.addSeparator();

    if (qobject_cast<GtTask*>(insertionTarget.data()))
    {
        auto addCalculator = [this, insertionTarget](GtObject*) {
            if (insertionTarget)
            {
                this->addCalculator(insertionTarget.data());
            }
        };
        auto addcalc =
            gt::gui::makeAction(tr("New Calculator..."), addCalculator)
                .setIcon(gt::gui::icon::calculator());
        gt::gui::addToMenu(addcalc, menu, nullptr);

        auto addRootTask = [this, rootTarget](GtObject*) {
            if (rootTarget)
            {
                this->addRootTask(rootTarget.data());
            }
        };
        auto addroottask =
            gt::gui::makeAction(tr("New Root Task..."), addRootTask)
                .setIcon(gt::gui::icon::processAdd());
        gt::gui::addToMenu(addroottask, menu, nullptr);
    }

    if (insertionTarget)
    {
        auto addTask = [this, insertionTarget](GtObject*) {
            if (insertionTarget)
            {
                this->addTask(insertionTarget.data());
            }
        };
        auto addtask = gt::gui::makeAction(tr("New Task..."), addTask)
                           .setIcon(gt::gui::icon::processAdd());
        gt::gui::addToMenu(addtask, menu, nullptr);
    }

    if (!gtApp->settings()->lastProcessElements().isEmpty())
    {
        menu.addSeparator();
        generateLastUsedElementMenu(&menu, insertionTarget.data());
    }

    gt::gui::detail::addProcessDockActionsToMenu(
        gt::gui::detail::processDockAddActions(), menu, insertionTarget.data());
}

void
GtProcessDock::customContextMenu(const QModelIndex& srcIndex)
{
    // invalid index selected
    if (!srcIndex.isValid())
    {
        return defaultContextMenu();
    }

    // multi selection
    if (m_impl->view->selectionModel()->selectedRows(0).size() > 1)
    {
        multiSelectionContextMenu(
            m_impl->view->selectionModel()->selectedIndexes());
        return;
    }

    // single selection
    GtObject* obj = gtDataModel->objectFromIndex(srcIndex);
    if (!obj) return;

    auto* task = qobject_cast<GtTask*>(obj);
    auto* calc = qobject_cast<GtCalculator*>(obj);

    if (!task && !calc) return;

    processContextMenu(*static_cast<GtProcessComponent*>(obj), srcIndex, task);
}

void
GtProcessDock::customContextMenu(const QPoint& pos)
{
    if (!m_impl->view->model())
    {
        return;
    }

    QModelIndex index = m_impl->view->indexAt(pos);
    QModelIndex srcIndex = mapToSource(index);

    customContextMenu(srcIndex);
}

/// helper method for creating paste action
GtObjectUIAction
makePasteAction(GtProcessDock* dock, GtObject* obj,
                bool allowCalculator = false)
{
    auto doPaste = [=](GtObject*) { dock->pasteElement(obj); };

    auto isEnabled = [=](GtObject*) {
        auto* clipboard = QApplication::clipboard();
        const auto* mimeData = clipboard->mimeData();

        if (mimeData && mimeData->hasFormat(GT_CLASSNAME(GtObject)))
        {
            GtObjectMemento memento{mimeData->data(GT_CLASSNAME(GtObject))};

            if (allowCalculator)
            {
                return memento.canCastTo(GT_CLASSNAME(GtCalculator),
                                         gtProcessFactory);
            }

            return memento.canCastTo(GT_CLASSNAME(GtTask), gtProcessFactory);
        }
        return false;
    };

    return gt::gui::makeAction(QObject::tr("Paste"), doPaste)
        .setIcon(gt::gui::icon::paste())
        .setShortCut(gtApp->getShortCutSequence(QStringLiteral("paste")))
        .setVerificationMethod(isEnabled);
}

void
GtProcessDock::defaultContextMenu()
{
    QMenu menu(this);

    QMenu* addMenu = menu.addMenu(gt::gui::icon::add(), tr("Add..."));

    // add menu
    makeAddMenu(*addMenu);

    menu.addSeparator();

    if (m_impl->taskGroup)
    {
        // import action
        auto imenu = new GtImportMenu(m_impl->taskGroup, &menu);
        menu.addMenu(imenu);
        menu.addSeparator();
    }

    auto paste = makePasteAction(this, m_impl->taskGroup);

    gt::gui::addToMenu(paste, menu, m_impl->taskGroup);

    menu.exec(QCursor::pos());
}

void
GtProcessDock::processContextMenu(GtProcessComponent& obj,
                                  const QModelIndex& index, bool isTask)
{
    // incomplete processelements should not be modified
    bool hasInvalidParents = obj.hasDummyParents();
    bool isReady = componentIsReady(&obj) && !hasInvalidParents;

    QMenu menu(this);

    // run and stop root task
    bool isRootTask = isTask && qobject_cast<GtTaskGroup*>(obj.parent());
    if (isRootTask)
    {
        const auto doRun = [this](auto ID) {
            if (gt::processExecutorManager().setCurrentExecutor(ID))
            {
                runProcess();
            }
        };

        bool isTaskReady = isReady && !obj.isSkipped();

        auto runremote =
            gt::gui::makeAction(
                tr("Run Task - detached execution"),
                [doRun](GtObject*) { doRun(GtProcessRunner::S_ID); })
                .setIcon(gt::gui::icon::processRun())
                .setEnabled(isTaskReady)
                .setVisible(useExtendedProcessExecutor());

        auto runlocal =
            gt::gui::makeAction(
                tr("Run Task"),
                [doRun](GtObject*) { doRun(GtProcessExecutor::S_ID); })
                .setIcon(gt::gui::icon::processRun())
                .setEnabled(isTaskReady)
                .setShortCut(getShortCut(QStringLiteral("runProcess")));

        auto stop = gt::gui::makeAction(tr("Stop Task"),
                                        [=](GtObject*) { terminateProcess(); })
                        .setIcon(gt::gui::icon::stop())
                        .setVisible(!isReady);

        gt::gui::addToMenu({runremote, runlocal, stop}, menu, &obj);

        menu.addSeparator();
    }

    auto config = gt::gui::makeAction(
                      tr("Config..."),
                      [=, o = &obj](GtObject*) {
                          isTask
                              ? configTask(static_cast<GtTask*>(o))
                              : configCalculator(static_cast<GtCalculator*>(o));
                      })
                      .setIcon(gt::gui::icon::config())
                      .setEnabled(isReady);

    auto connect =
        gt::gui::makeAction(tr("Connection Editor"),
                            [=](GtObject*) { openConnectionEditor(index); })
            .setIcon(gt::gui::icon::connection())
            .setEnabled(isReady);

    gt::gui::addToMenu({config, connect}, menu, &obj);

    menu.addSeparator();

    if (isTask)
    {
        QMenu* addMenu = menu.addMenu(gt::gui::icon::add(), tr("Add..."));
        addMenu->setEnabled(isReady);

        makeAddMenu(*addMenu, &obj);

        menu.addSeparator();
    }

    auto skip =
        gt::gui::makeAction(
            tr("Skip"), [=](GtObject*) { skipComponent(mapFromSource(index)); })
            .setIcon(gt::gui::icon::skip())
            .setShortCut(getShortCut(QStringLiteral("skipProcess")))
            .setVisible(!obj.isSkipped());

    auto unskip =
        gt::gui::makeAction(
            tr("Unskip"),
            [=](GtObject*) { skipComponent(mapFromSource(index), false); })
            .setIcon(gt::gui::icon::unskip())
            .setShortCut(getShortCut(QStringLiteral("unskipProcess")))
            .setVisible(obj.isSkipped());

    gt::gui::addToMenu({skip, unskip}, menu, &obj);

    menu.addSeparator();

    // importer menu
    if (auto* imenu = gt::gui::addImportMenu(menu, obj))
    {
        imenu->setEnabled(!hasInvalidParents);
        menu.addSeparator();
    }

    // exporter menu
    if (auto* emenu = gt::gui::addExportMenu(menu, obj))
    {
        emenu->setEnabled(!hasInvalidParents);
        menu.addSeparator();
    }

    auto rename =
        gt::gui::makeRenameAction(obj, mapFromSource(index), *m_impl->view);

    if (!rename.isEmpty())
    {
        rename.setEnabled(isReady);
        gt::gui::addToMenu(rename, menu, &obj);
    }

    auto clone =
        gt::gui::makeAction(
            tr("Clone"), [=](GtObject*) { cloneElement(mapFromSource(index)); })
            .setIcon(gt::gui::icon::clone())
            .setShortCut(gtApp->getShortCutSequence(QStringLiteral("clone")))
            .setEnabled(isReady);

    auto cut =
        gt::gui::makeAction(
            tr("Cut"), [=](GtObject*) { cutElement(mapFromSource(index)); })
            .setIcon(gt::gui::icon::cut())
            .setShortCut(gtApp->getShortCutSequence(QStringLiteral("cut")))
            .setEnabled(isReady);

    auto copy =
        gt::gui::makeAction(
            tr("Copy"), [=](GtObject*) { copyElement(mapFromSource(index)); })
            .setIcon(gt::gui::icon::copy())
            .setShortCut(gtApp->getShortCutSequence(QStringLiteral("copy")))
            .setEnabled(isReady);

    gt::gui::addToMenu({clone, cut, copy}, menu, &obj);

    if (isTask)
    {
        auto paste = makePasteAction(this, &obj, true);

        gt::gui::addToMenu(paste, menu, &obj);
    }

    menu.addSeparator();

    auto delete_ =
        gt::gui::makeAction(
            tr("Delete"),
            [=](GtObject*) {
                deleteProcessElements(
                    m_impl->view->selectionModel()->selectedIndexes());
            })
            .setIcon(gt::gui::icon::delete_())
            .setShortCut(gtApp->getShortCutSequence(QStringLiteral("delete")))
            .setEnabled(isReady);

    gt::gui::addToMenu(delete_, menu, &obj);

    gt::gui::detail::addProcessDockActionsToMenu(
        gt::gui::detail::processDockContextActions(), menu, &obj);

    menu.exec(QCursor::pos());
}

void
GtProcessDock::multiSelectionContextMenu(const QList<QModelIndex>& indexList)
{

    QMenu menu(this);

    bool hideSkip = true;
    bool hideUnskipped = true;
    // counter for dummy objects
    int dummyObjects = 0;

    GtProcessComponent* first = nullptr;

    for (const QModelIndex& index : indexList)
    {
        GtProcessComponent* pc = componentByModelIndex(index);

        if (!first) first = pc;

        if (!pc || pc->isDummy() || pc->hasDummyParents())
        {
            dummyObjects += 1;
            continue;
        }

        dummyObjects += (pc->hasDummyParents());

        if (!pc->isSkipped())
        {
            hideSkip = false;
        }
        else
        {
            hideUnskipped = false;
        }
    }

    if (!first) return;

    // dummy objects cannot be skipped
    if (dummyObjects == indexList.length())
    {
        hideSkip = true;
        hideUnskipped = true;
    }

    auto skip = gt::gui::makeAction(
                    tr("Skip"), [=](GtObject*) { skipComponents(indexList); })
                    .setIcon(gt::gui::icon::skip())
                    .setShortCut(getShortCut(QStringLiteral("skipProcess")))
                    .setVisible(!hideSkip);

    auto unskip =
        gt::gui::makeAction(
            tr("Unskip"), [=](GtObject*) { skipComponents(indexList, false); })
            .setIcon(gt::gui::icon::unskip())
            .setShortCut(getShortCut(QStringLiteral("unskipProcess")))
            .setVisible(!hideUnskipped);

    gt::gui::addToMenu({skip, unskip}, menu, first, this);

    menu.addSeparator();

    bool hideDelete = dummyObjects > 0;

    auto delete_ =
        gt::gui::makeAction(
            tr("Delete"), [=](GtObject*) { deleteProcessElements(indexList); })
            .setIcon(gt::gui::icon::delete_())
            .setShortCut(gtApp->getShortCutSequence(QStringLiteral("delete")))
            .setEnabled(!hideDelete);

    gt::gui::addToMenu(delete_, menu, first, this);

    menu.exec(QCursor::pos());
}

void
GtProcessDock::restoreExpandStates()
{
    auto rootIndex = m_impl->view->rootIndex();

    if (rootIndex.isValid() && rootIndex.model() == m_impl->filterModel)
    {
        m_impl->view->setUpdatesEnabled(false);

        restoreExpandStatesHelper(expandedItemUuids(),
                                  m_impl->filterModel->index(0, 0, rootIndex));

        m_impl->view->setUpdatesEnabled(true);
    }
}

void
GtProcessDock::restoreExpandStatesHelper(const QStringList& expandedUuids,
                                         const QModelIndex& startIndex)
{
    if (!startIndex.isValid()) return;

    auto model = startIndex.model();

    for (const auto& uuid : expandedUuids)
    {
        auto matchedIndices = model->match(
            startIndex, GtCoreDatamodel::UuidRole, QVariant::fromValue(uuid));

        for (const auto& index : qAsConst(matchedIndices))
        {
            m_impl->view->setExpanded(index, true);
            restoreExpandStatesHelper(expandedUuids, model->index(0, 0, index));
        }
    }
}

void
GtProcessDock::copyElement(const QModelIndex& index)
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

    if (srcIndex.model() != gtDataModel)
    {
        return;
    }

    GtObject* obj = gtDataModel->objectFromIndex(srcIndex);

    if (!obj)
    {
        return;
    }

    auto pComp = qobject_cast<GtProcessComponent*>(obj);

    if (!pComp)
    {
        return;
    }

    if (!pComp->isReady())
    {
        return;
    }

    GtObject* copy = obj->copy();

    auto origTask = qobject_cast<GtTask*>(pComp);
    auto newTask = qobject_cast<GtTask*>(copy);

    if (newTask && origTask)
    {
        if (qobject_cast<GtTaskGroup*>(origTask->parent()))
        {
            mapPropertyConnections(origTask, newTask);
        }
        else
        {
            auto origTaskClone = qobject_cast<GtTask*>(origTask->clone());

            if (!origTaskClone)
            {
                return;
            }

            QList<GtPropertyConnection*> validPropCons =
                detail::internalPropertyConnections(origTask);

            foreach (GtPropertyConnection* propCon, validPropCons)
            {
                GtObject* conClone = propCon->clone();
                GtObject* conCopy = propCon->copy();

                if (conClone && conCopy)
                {
                    origTaskClone->appendChild(conClone);
                    newTask->appendChild(conCopy);
                }
            }

            detail::setOffLostConnectionWarnings(
                detail::lostPropertyConnections(origTask),
                detail::highestParentTask(origTask));

            mapPropertyConnections(origTaskClone, newTask);

            delete origTaskClone;
        }

        detail::updateRelativeObjectLinks(origTask, newTask);
    }

    QMimeData* mimeData = gtDataModel->mimeDataFromObject(copy, false);
    QApplication::clipboard()->setMimeData(mimeData);

    delete copy;
}

void
GtProcessDock::renameElement()
{
    QModelIndex srcIndex = m_impl->view->currentIndex();

    if (srcIndex.isValid())
    {
        m_impl->view->edit(m_impl->view->currentIndex());
    }
}

void
GtProcessDock::cloneElement(const QModelIndex& index)
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

    if (srcIndex.model() != gtDataModel)
    {
        return;
    }

    GtObject* obj = gtDataModel->objectFromIndex(srcIndex);

    if (!obj)
    {
        return;
    }

    auto pComp = qobject_cast<GtProcessComponent*>(obj);

    if (!pComp)
    {
        return;
    }

    if (!pComp->isReady())
    {
        return;
    }

    GtObject* parent = obj->parentObject();

    if (!parent)
    {
        return;
    }

    GtObject* cloned = obj->copy();

    // check cloned object
    if (!cloned)
    {
        return;
    }

    auto origTask = qobject_cast<GtTask*>(obj);
    auto newTask = qobject_cast<GtTask*>(cloned);

    if (newTask && origTask)
    {
        if (qobject_cast<GtTaskGroup*>(origTask->parent()))
        {
            mapPropertyConnections(origTask, newTask);
        }
        else
        {
            GtTask* origTaskClone = qobject_cast<GtTask*>(origTask->clone());

            if (!origTaskClone)
            {
                return;
            }

            QList<GtPropertyConnection*> validPropCons =
                detail::internalPropertyConnections(origTask);

            foreach (GtPropertyConnection* propCon, validPropCons)
            {
                GtObject* conClone = propCon->clone();
                GtObject* conCopy = propCon->copy();

                if (conClone && conCopy)
                {
                    origTaskClone->appendChild(conClone);
                    newTask->appendChild(conCopy);
                }
            }

            detail::setOffLostConnectionWarnings(
                detail::lostPropertyConnections(origTask),
                detail::highestParentTask(origTask));

            mapPropertyConnections(origTaskClone, newTask);

            delete origTaskClone;
        }

        detail::updateRelativeObjectLinks(origTask, newTask);
    }

    // paste new element
    pasteElement(cloned, parent);
}

void
GtProcessDock::cutElement(const QModelIndex& index)
{
    if (!index.isValid()) return;

    QModelIndex srcIndex = mapToSource(index);

    if (!srcIndex.isValid()) return;

    if (srcIndex.model() != gtDataModel) return;

    GtObject* obj = gtDataModel->objectFromIndex(srcIndex);

    if (!obj) return;

    auto pComp = qobject_cast<GtProcessComponent*>(obj);

    if (!pComp) return;

    if (!pComp->isReady()) return;

    if (!m_impl->taskGroup) return;

    copyElement(index);

    QList<GtObject*> toDelete;

    if (GtTask* child = qobject_cast<GtTask*>(pComp))
    {
        if (!qobject_cast<GtTaskGroup*>(pComp->parent()))
        {
            QList<GtPropertyConnection*> relatedCons =
                detail::relatedPropertyConnections(child);

            std::copy(relatedCons.cbegin(), relatedCons.cend(),
                      std::back_inserter(toDelete));

            detail::setOffLostConnectionWarnings(
                relatedCons, detail::highestParentTask(child));
        }
    }

    toDelete.append(pComp);

    auto* commonParent =
        gt::find_lowest_ancestor(toDelete, gt::get_parent_object);
    assert(commonParent);

    auto command = gtApp->makeCommand(
        commonParent, tr("Cut Process Element") + QStringLiteral(" (") +
                          obj->objectName() + QStringLiteral(")"));
    Q_UNUSED(command)

    gtDataModel->deleteFromModel(toDelete);
}

void
GtProcessDock::deleteProcessElements(const QList<QModelIndex>& indexList)
{
    if (indexList.isEmpty())
    {
        return;
    }

    QList<GtObject*> objects;

    foreach (QModelIndex index, indexList)
    {
        if (!index.isValid())
        {
            continue;
        }

        QModelIndex srcIndex = mapToSource(index);

        if (!srcIndex.isValid())
        {
            continue;
        }

        if (srcIndex.model() != gtDataModel)
        {
            continue;
        }

        GtObject* obj = gtDataModel->objectFromIndex(srcIndex);

        if (!obj)
        {
            continue;
        }

        if (!componentIsReady(qobject_cast<GtProcessComponent*>(obj)))
        {
            return;
        }

        if (!objects.contains(obj))
        {
            objects.append(obj);
        }
    }

    QString str1;
    QString str2;

    if (objects.size() == 1)
    {
        str1 = tr("Delete Item");
        str2 = objects.first()->objectName();
    }
    else
    {
        str1 = tr("Delete Items");
        str2 = tr("Process Elements");
    }

    GtDeleteItemMessageBox mb(str1, str2);

    int ret = mb.exec();

    switch (ret)
    {
    case QMessageBox::Yes: {
        gtDataModel->reduceToParents(objects);

        auto* commonParent =
            gt::find_lowest_ancestor(objects, gt::get_parent_object);
        assert(commonParent);

        auto cmd =
            gtApp->makeCommand(commonParent, tr("Delete Process Elements"));
        Q_UNUSED(cmd)

        for (GtObject* obj : qAsConst(objects))
        {
            deleteProcessComponent(obj);
        }
    }

    case QMessageBox::Cancel: {
        break;
    }

    default:
        break;
    }
}

void
GtProcessDock::pasteElement(GtObject* parent)
{
    QClipboard* clipboard = QApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();

    GtObject* obj =
        gtDataModel->objectFromMimeData(mimeData, true, gtProcessFactory);

    if (!obj)
    {
        gtError() << tr("Could not restore object from clipboard!");
        return;
    }

    if (!qobject_cast<GtTaskGroup*>(parent))
    {
        auto pComp = qobject_cast<GtProcessComponent*>(parent);

        if (!pComp)
        {
            return;
        }

        if (!pComp->isReady())
        {
            return;
        }
    }

    pasteElement(obj, parent);
}

void
GtProcessDock::pasteElement(GtObject* obj, GtObject* parent)
{
    if (!obj || !parent) return;

    if (qobject_cast<GtCalculator*>(parent)) return;

    if (!qobject_cast<GtTaskGroup*>(parent))
    {
        auto pComp = qobject_cast<GtProcessComponent*>(parent);

        if (!pComp) return;

        if (!pComp->isReady()) return;
    }

    // create new unique identification string
    QString newId = gt::makeUniqueName(obj->objectName(), parent);

    // set new identification string
    obj->setObjectName(newId);

    m_impl->view->setFocus();

    auto pasteCmd = gtApp->makeCommand(parent, tr("Paste process element"));

    const auto propCons = obj->findChildren<GtPropertyConnection*>();

    for (GtPropertyConnection* propCon : propCons)
    {
        propCon->setParent(nullptr);
    }

    QModelIndex srcIndex = gtDataModel->appendChild(obj, parent);

    auto task = qobject_cast<GtTask*>(obj);

    if (task)
    {
        GtTask* highestParent = detail::highestParentTask(task);

        if (highestParent)
        {
            for (GtPropertyConnection* propCon : propCons)
            {
                gtDataModel->appendChild(propCon, highestParent);
            }
        }
    }

    pasteCmd.finalize();

    QModelIndex index = mapFromSource(srcIndex);

    if (index.isValid())
    {
        QModelIndex tmpIndex =
            m_impl->filterModel->index(index.row(), 1, index.parent());
        QItemSelection itemSel(index, tmpIndex);

        m_impl->view->scrollTo(index);
        m_impl->view->selectionModel()->select(
            itemSel, QItemSelectionModel::ClearAndSelect);
        m_impl->view->setCurrentIndex(index);
    }
}

void
GtProcessDock::pasteElement(const QModelIndex& parentIndex)
{
    if (!parentIndex.isValid())
    {
        return;
    }

    QModelIndex srcParentIndex = mapToSource(parentIndex);

    if (!srcParentIndex.isValid())
    {
        return;
    }

    if (srcParentIndex.model() != gtDataModel)
    {
        return;
    }

    GtObject* parent = gtDataModel->objectFromIndex(srcParentIndex);

    if (!parent)
    {
        return;
    }

    pasteElement(parent);
}

void
GtProcessDock::skipComponent(const QModelIndex& index, bool skip)
{
    skipComponents({index}, skip);
}

void
GtProcessDock::skipComponents(const QList<QModelIndex>& indexList, bool skip)
{
    QList<GtProcessComponent*> pcs;

    for (QModelIndex index : indexList)
    {
        GtProcessComponent* pc = componentByModelIndex(index);

        if (!pc) continue;

        if (!pcs.contains(pc) && pc->isSkipped() != skip)
        {
            pcs.append(pc);
        }
    }

    QString msg = tr("%1 selected Process Elements")
                      .arg(skip ? tr("Skip") : tr("Unskip"));

    auto cmd = gtApp->makeCommand(m_impl->taskGroup, msg);
    Q_UNUSED(cmd)

    for (GtProcessComponent* pc : qAsConst(pcs))
    {
        skipComponent(pc, skip);
    }
}

void
GtProcessDock::skipComponent(GtProcessComponent* comp, bool skip)
{
    if (comp) comp->setSkipped(skip);
}

void
GtProcessDock::configCalculator(GtCalculator* calc)
{
    if (!calc)
    {
        return;
    }

    auto project = calc->findParent<GtProject*>();

    if (!project)
    {
        return;
    }

    //    gtDebug() << "config calculator " << calc->metaObject()->className() <<
    //                 "...";

    GtCalculatorProvider provider(calc);
    GtProcessWizard wizard(project, &provider, this);

    if (!wizard.exec())
    {
        return;
    }

    GtObjectMemento memento = provider.componentData();

    if (memento.isNull())
    {
        return;
    }

    auto command = gtApp->makeCommand(calc, calc->objectName() +
                                                tr(" configuration changed"));
    Q_UNUSED(command)

    calc->fromMemento(memento);
}

void
GtProcessDock::configTask(GtTask* task)
{
    if (!task)
    {
        return;
    }

    if (!gtTaskFactory->taskDataExists(task->metaObject()->className()))
    {
        return;
    }

    GtProject* project = gtApp->currentProject();

    if (!project)
    {
        return;
    }

    GtTaskProvider provider(task);
    GtProcessWizard wizard(project, &provider, this);

    if (!wizard.exec())
    {
        return;
    }

    GtObjectMemento memento = provider.componentData();

    if (memento.isNull())
    {
        return;
    }

    auto command = gtApp->makeCommand(task, task->objectName() +
                                                tr(" configuration changed"));
    Q_UNUSED(command)

    task->fromMemento(memento);
}

QModelIndex
GtProcessDock::mapToSource(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return {};
    }

    QModelIndex tmp1 = m_impl->filterModel->mapToSource(index);

    return m_impl->model->mapToSource(tmp1);
}

QModelIndex
GtProcessDock::mapFromSource(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return {};
    }

    QModelIndex tmp1 = m_impl->model->mapFromSource(index);

    return m_impl->filterModel->mapFromSource(tmp1);
}

void
GtProcessDock::openConnectionEditor(const QModelIndex& index)
{
    // check index
    if (!index.isValid())
    {
        return;
    }

    // get object from index
    GtObject* obj = gtDataModel->objectFromIndex(index);

    // check object
    if (!obj)
    {
        return;
    }

    // cast object to process component
    auto comp = qobject_cast<GtProcessComponent*>(obj);

    // check casted process component
    if (!comp)
    {
        return;
    }

    // get root task
    GtTask* rootTask = comp->rootTask();

    // check root task
    if (!rootTask)
    {
        return;
    }

    auto project = rootTask->findParent<GtProject*>();

    if (!project)
    {
        return;
    }

    // create process connection editor
    GtProcessConnectionEditor editor(rootTask);

    // resice connection editor
    editor.resize(800, 500);

    GtObjectMemento mementoOrigin = rootTask->toMemento();

    // execute connection editor
    if (editor.exec())
    {
        GtObjectMemento memento = editor.connectionData();

        if (!memento.isNull())
        {
            GtObjectMementoDiff diff(mementoOrigin, memento);

            if (!diff.isNull())
            {
                QString delMsg = tr("Update Task Connections") +
                                 QStringLiteral(" - ") + rootTask->objectName();

                auto command = gtApp->makeCommand(project, delMsg);
                Q_UNUSED(command)

                rootTask->applyDiff(diff);
            }
        }
    }
}

void
GtProcessDock::onRowsAboutToBeMoved()
{
    assert(m_impl->taskGroup);

    m_impl->command =
        gtApp->startCommand(m_impl->taskGroup, tr("Process elements moved"));
}

void
GtProcessDock::onRowsMoved()
{
    gtApp->endCommand(m_impl->command);
}

void
GtProcessDock::onTaskDataChanged(GtObject* obj)
{
    if (obj == m_impl->currentProcess)
    {
        updateRunButton();
    }
}

void
GtProcessDock::actionTriggered(QObject* obj, GtObject* insertionTarget)
{
    QPointer<GtObject> guardedTarget{insertionTarget};
    if (!gtApp->currentProject() || !guardedTarget)
    {
        return;
    }

    QPointer<GtProject> project{guardedTarget->findParent<GtProject*>()};
    if (!project || project != gtApp->currentProject())
    {
        return;
    }

    auto act = qobject_cast<QAction*>(obj);

    if (!act)
    {
        return;
    }

    QString className = act->data().toString();

    if (className.isEmpty())
    {
        return;
    }

    if (gtTaskFactory->taskDataExists(className))
    {
        GtTaskData taskData = gtTaskFactory->taskData(className);

        if (!taskData->isValid())
        {
            return;
        }

        QObject* newObj = taskData->metaData().newInstance();

        if (!newObj)
        {
            return;
        }

        GtTask* task = qobject_cast<GtTask*>(newObj);

        if (!task)
        {
            delete newObj;
            return;
        }

        task->setFactory(gtProcessFactory);

        GtTaskProvider provider(task);
        GtProcessWizard wizard(project.data(), &provider, this);
        wizard.setWindowTitle(tr("Config ") + taskData->id);

        if (!wizard.exec() || !guardedTarget || !project ||
            project != gtApp->currentProject())
        {
            delete task;
            return;
        }

        GtObjectMemento memento = provider.componentData();

        if (memento.isNull())
        {
            delete task;
            return;
        }

        task->fromMemento(memento);

        GtObject* parentObj = guardedTarget.data();

        updateLastUsedElementList(task->metaObject()->className());

        QModelIndex newIndex = gtDataModel->appendChild(task, parentObj);

        QModelIndex index = mapFromSource(newIndex);

        if (!index.isValid())
        {
            return;
        }

        m_impl->view->setFocus();

        QModelIndex tmpIndex =
            m_impl->filterModel->index(index.row(), 1, index.parent());
        QItemSelection itemSel(index, tmpIndex);

        m_impl->view->expand(
            mapFromSource(gtDataModel->indexFromObject(guardedTarget.data())));
        m_impl->view->scrollTo(index);
        m_impl->view->selectionModel()->select(
            itemSel, QItemSelectionModel::ClearAndSelect);
        m_impl->view->setCurrentIndex(index);
        m_impl->view->edit(index);
    }
    else if (gtCalculatorFactory->calculatorDataExists(className))
    {
        GtObject* currentObj = guardedTarget.data();
        if (!qobject_cast<GtTask*>(currentObj))
        {
            return;
        }

        GtCalculatorData calcData =
            gtCalculatorFactory->calculatorData(className);

        if (!calcData->isValid())
        {
            return;
        }

        QObject* newObj = calcData->metaData().newInstance();

        if (!newObj)
        {
            return;
        }

        auto calc = qobject_cast<GtCalculator*>(newObj);

        if (!calc)
        {
            delete newObj;
            return;
        }

        calc->setFactory(gtProcessFactory);

        GtCalculatorProvider provider(calc);
        GtProcessWizard wizard(project.data(), &provider, this);
        wizard.setWindowTitle(tr("Config ") + calcData->id);

        if (!wizard.exec() || !guardedTarget || !project ||
            project != gtApp->currentProject())
        {
            delete calc;
            return;
        }

        GtObjectMemento memento = provider.componentData();

        if (memento.isNull())
        {
            delete calc;
            return;
        }

        calc->fromMemento(memento);

        updateLastUsedElementList(newObj->metaObject()->className());

        QModelIndex newIndex =
            gtDataModel->appendChild(calc, guardedTarget.data());

        QModelIndex index = mapFromSource(newIndex);

        if (!index.isValid())
        {
            return;
        }

        m_impl->view->setFocus();

        QModelIndex tmpIndex =
            m_impl->filterModel->index(index.row(), 1, index.parent());
        QItemSelection itemSel(index, tmpIndex);

        m_impl->view->expand(m_impl->view->currentIndex());
        m_impl->view->scrollTo(index);
        m_impl->view->selectionModel()->select(
            itemSel, QItemSelectionModel::ClearAndSelect);
        m_impl->view->setCurrentIndex(index);
    }
}

void
GtProcessDock::onExecutorChanged(GtCoreProcessExecutor* exec)
{
    if (exec)
    {
        connect(exec, &GtCoreProcessExecutor::queueChanged, this,
                [this]() { updateRunButton(); });
    }
}

void
GtProcessDock::currentTaskGroupIndexChanged(int index)
{
    if (!m_impl->project || !m_impl->project->processData())
    {
        return;
    }

    const GtTaskGroup::SCOPE scope = m_impl->taskGroupModel->rowScope(index);

    if (scope == GtTaskGroup::UNDEFINED)
    {
        return;
    }

    GtTaskGroup* currentGroup = m_impl->project->processData()->taskGroup();
    const QString groupId = m_impl->taskGroupSelection->itemText(index);

    // check if selection matches current task
    if (currentGroup && currentGroup->objectName() == groupId)
    {
        return;
    }

    if (m_impl->project->processData()->switchCurrentTaskGroup(
            groupId, scope, m_impl->project->path()))
    {
        setLastTaskGroupScope(scope);
        setLastTaskGroupId(groupId);
    }

    updateButtons();

    updateCurrentTaskGroup();
}

void
GtProcessDock::onDeleteTaskGroupButtonClicked()
{
    int currentIndex = m_impl->taskGroupSelection->currentIndex();

    if (currentIndex < 0) return;

    GtTaskGroup::SCOPE scope = m_impl->taskGroupModel->rowScope(currentIndex);

    if (scope != GtTaskGroup::CUSTOM) return;

    QString groupName = m_impl->taskGroupSelection->itemText(currentIndex);

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Delete Task Group"),
        tr("Are you sure you want to delete the task group '%1'?")
            .arg(groupName),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        deleteCustomTaskGroup(groupName);
    }
}

void
GtProcessDock::onRenameTaskGroupButtonClicked()
{
    int currentIndex = m_impl->taskGroupSelection->currentIndex();

    if (currentIndex < 0) return;

    GtTaskGroup::SCOPE scope = m_impl->taskGroupModel->rowScope(currentIndex);

    if (scope != GtTaskGroup::CUSTOM) return;

    QString groupName = m_impl->taskGroupSelection->itemText(currentIndex);

    if (!m_impl->project || !m_impl->project->processData()) return;

    QStringList groupIds = m_impl->project->processData()->customGroupIds();
    groupIds.append(m_impl->project->processData()->userGroupIds());
    groupIds.sort(Qt::CaseInsensitive);

    GtTaskGroupRenameDialog dialog(groupName, groupIds, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        QString newName = dialog.newName();

        if (newName == groupName) return;

        if (!m_impl->project->processData()->renameTaskGroup(
                groupName, newName, GtTaskGroup::CUSTOM,
                m_impl->project->path()))
        {
            gtError() << tr("Failed to rename task group from '%1' to '%2'")
                             .arg(groupName)
                             .arg(newName);
            return;
        }

        if (!m_impl->project->processData()->save(m_impl->project->path()))
        {
            gtError() << tr("Failed to save project after renaming task group");
            return;
        }

        resetTaskGroupModel();

        QModelIndex newIndex = m_impl->taskGroupModel->indexByGroupName(
            newName, GtTaskGroup::CUSTOM);

        if (newIndex.isValid())
        {
            m_impl->taskGroupSelection->setCurrentIndex(newIndex.row());
        }
    }
}

void
GtProcessDock::addNewCustomTaskGroup()
{
    if (!m_impl->project || !m_impl->project->processData()) return;

    QStringList customGroupIds =
        m_impl->project->processData()->customGroupIds();
    customGroupIds.sort(Qt::CaseInsensitive);

    bool ok = false;
    QString groupNameRaw =
        QInputDialog::getText(this, tr("Add New Custom Task Group"),
                              tr("Enter name for new custom task group:"),
                              QLineEdit::Normal, QString(), &ok);

    if (!ok) return;

    const QString groupName = groupNameRaw.trimmed();
    if (groupName.isEmpty()) return;

    if (customGroupIds.contains(groupName, Qt::CaseInsensitive))
    {
        QMessageBox::warning(this, tr("Invalid Group Name"),
                             tr("A task group with this name already exists."));
        return;
    }

    const GtTaskGroup* newGroup =
        m_impl->project->processData()->createNewTaskGroup(groupName,
                                                           GtTaskGroup::CUSTOM);

    if (!newGroup)
    {
        gtError() << tr("Failed to create new task group: %1").arg(groupName);
        return;
    }

    if (!m_impl->project->processData()->save(m_impl->project->path()))
    {
        gtError() << tr("Failed to save project after creating task group: %1")
                         .arg(groupName);
        return;
    }

    resetTaskGroupModel();

    QModelIndex newIndex = m_impl->taskGroupModel->indexByGroupName(
        groupName, GtTaskGroup::CUSTOM);

    if (newIndex.isValid())
    {
        m_impl->taskGroupSelection->setCurrentIndex(newIndex.row());
    }
}

void
GtProcessDock::endResetView()
{
    updateProcessViewRootIndex();
    resetTaskGroupModel();

    // update selection after reset
    if (m_impl->view->selectionModel()->selection().size() > 0) return;

    GtObject* selectedObj = gtApp->selectedObject();
    if (!selectedObj) return;

    auto index = gtDataModel->indexFromObject(selectedObj);

    m_impl->view->setCurrentIndex(mapFromSource(index));
}

void
GtProcessDock::itemCollapsed(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    auto uuids = expandedItemUuids();

    uuids.removeOne(index.data(GtCoreDatamodel::UuidRole).toString());

    setExpandedItemUuids(uuids);
}

void
GtProcessDock::itemExpanded(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    auto uuid = index.data(GtCoreDatamodel::UuidRole).toString();

    auto uuids = expandedItemUuids();

    if (!uuids.contains(uuid))
    {
        uuids.append(uuid);
    }

    setExpandedItemUuids(uuids);
}

void
GtProcessDock::updateCustomTaskGroupsButtons()
{
    bool customScope = false;

    int currentIndex = m_impl->taskGroupSelection->currentIndex();
    if (currentIndex >= 0)
    {
        GtTaskGroup::SCOPE scope =
            m_impl->taskGroupModel->rowScope(currentIndex);

        customScope = (scope == GtTaskGroup::CUSTOM);
    }

    if (auto* deleteButton =
            findChild<QPushButton*>(QStringLiteral("deleteTaskGroupButton")))
    {
        deleteButton->setEnabled(customScope);
    }

    if (auto* renameButton =
            findChild<QPushButton*>(QStringLiteral("renameTaskGroupButton")))
    {
        renameButton->setEnabled(customScope);
    }
}

bool
GtProcessDock::mapPropertyConnections(GtTask* orig, GtTask* copy)
{
    QList<GtPropertyConnection*> origCons =
        orig->findChildren<GtPropertyConnection*>();

    QList<GtPropertyConnection*> newCons =
        copy->findChildren<GtPropertyConnection*>();

    if (origCons.size() != newCons.size())
    {
        return false;
    }

    foreach (GtPropertyConnection* origCon, origCons)
    {
        GtPropertyConnection* newCon =
            detail::findConnectionCopy(origCon, newCons);

        if (!newCon)
        {
            gtDebug() << "Could not find copied connection...";
            continue;
        }

        if (!detail::updateConnectionProperties(origCon, newCon, orig, copy))
        {
            gtInfo() << "Could not update property connection!";
            continue;
        }
    }

    return true;
}


void
GtProcessDock::updateLastUsedElementList(const QString& str)
{
    if (str.isEmpty())
    {
        return;
    }

    QStringList list = gtApp->settings()->lastProcessElements();

    if (list.contains(str))
    {
        int idx = list.indexOf(str);

        if (idx > 0)
        {
            list.move(idx, 0);
        }
    }
    else
    {
        list.prepend(str);
    }

    if (list.size() > 10)
    {
        list.removeLast();
    }

    gtApp->settings()->setLastProcessElements(list);
}

void
GtProcessDock::generateLastUsedElementMenu(QMenu* menu,
                                           GtObject* insertionTarget)
{
    if (!menu)
    {
        return;
    }

    QStringList list = gtApp->settings()->lastProcessElements();

    int count = 0;

    foreach (const QString& entry, list)
    {
        if (count >= 5)
        {
            break;
        }

        if (gtTaskFactory->taskDataExists(entry))
        {
            // entry is a task
            GtTaskData taskData = gtTaskFactory->taskData(entry);

            if (taskData->isValid())
            {
                if (gtApp->devMode() ||
                    taskData->status == GtTaskDataImpl::RELEASE)
                {
                    QAction* act = menu->addAction(taskData->id);
                    act->setData(entry);
                    if (insertionTarget)
                    {
                        QObject::connect(insertionTarget, &QObject::destroyed,
                                         act,
                                         [act]() { act->setEnabled(false); });
                    }
                    else
                    {
                        act->setEnabled(false);
                    }

                    disconnect(act, &QAction::triggered, nullptr, nullptr);
                    connect(
                        act, &QAction::triggered, this,
                        [this, act,
                         target = QPointer<GtObject>{insertionTarget}](bool) {
                            if (target)
                            {
                                actionTriggered(act, target.data());
                            }
                        });

                    count++;

                    auto extendedData =
                        dynamic_cast<GtExtendedTaskDataImpl*>(taskData.get());

                    if (!extendedData || extendedData->icon.isNull())
                    {
                        QIcon icn;

                        GtObjectUI* oui = gtApp->defaultObjectUI(
                            taskData->metaData().className());

                        if (oui)
                        {
                            icn = oui->icon(nullptr);
                        }

                        if (icn.isNull())
                        {
                            icn = gt::gui::icon::process();
                        }

                        act->setIcon(icn);
                    }
                    else
                    {
                        act->setIcon(extendedData->icon);
                    }
                }
            }
        }
        else if (gtCalculatorFactory->calculatorDataExists(entry))
        {
            // entry is a calculator
            GtCalculatorData calcData =
                gtCalculatorFactory->calculatorData(entry);

            if (calcData->isValid())
            {
                if (gtApp->devMode() ||
                    calcData->status == GtCalculatorDataImpl::RELEASE)
                {
                    QAction* act = menu->addAction(calcData->id);
                    act->setData(entry);
                    if (insertionTarget)
                    {
                        QObject::connect(insertionTarget, &QObject::destroyed,
                                         act,
                                         [act]() { act->setEnabled(false); });
                    }
                    else
                    {
                        act->setEnabled(false);
                    }

                    disconnect(act, &QAction::triggered, nullptr, nullptr);
                    connect(
                        act, &QAction::triggered, this,
                        [this, act,
                         target = QPointer<GtObject>{insertionTarget}](bool) {
                            if (target)
                            {
                                actionTriggered(act, target.data());
                            }
                        });

                    count++;

                    auto extendedData =
                        dynamic_cast<GtExtendedCalculatorDataImpl*>(
                            calcData.get());

                    if (!extendedData || extendedData->icon.isNull())
                    {
                        act->setIcon(gt::gui::icon::calculator());
                    }
                    else
                    {
                        act->setIcon(extendedData->icon);
                    }

                    if (!qobject_cast<GtTask*>(insertionTarget))
                    {
                        act->setEnabled(false);
                    }
                }
            }
        }
    }
}

void
GtProcessDock::deleteProcessComponent(GtObject* obj)
{
    auto pComp = qobject_cast<GtProcessComponent*>(obj);

    if (!pComp)
    {
        return;
    }

    if (!pComp->isReady())
    {
        return;
    }

    QList<GtObject*> toDelete;

    QList<GtPropertyConnection*> relatedCons =
        detail::relatedPropertyConnections(pComp);

    foreach (GtPropertyConnection* propCon, relatedCons)
    {
        toDelete.append(propCon);
    }

    detail::setOffLostConnectionWarnings(detail::lostPropertyConnections(pComp),
                                         detail::highestParentTask(pComp));

    toDelete.append(pComp);

    gtDataModel->deleteFromModel(toDelete);
}

void
GtProcessDock::keyPressEvent(QKeyEvent* event)
{
    if (!m_impl->view)
    {
        return;
    }

    if (!m_impl->view->selectionModel())
    {
        return;
    }

    if (m_impl->view->selectionModel()->selectedIndexes().isEmpty())
    {
        return;
    }

    QModelIndex index =
        m_impl->view->selectionModel()->selectedIndexes().first();

    QModelIndex srcIndex = mapToSource(index);

    if (!srcIndex.isValid())
    {
        return;
    }

    if (gtApp->compareKeyEvent(event, "openContextMenu"))
    {
        customContextMenu(srcIndex);
    }

    if (gtApp->compareKeyEvent(event, "rename"))
    {
        if (m_impl->taskGroupSelection->hasFocus())
        {
            onRenameTaskGroupButtonClicked();
        }
        else
        {
            emit m_impl->view->renameProcessElement(index);
        }
    }
}

void
GtProcessDock::deleteCustomTaskGroup(const QString& groupName)
{
    if (!m_impl->project || !m_impl->project->processData()) return;

    GtTaskGroup::SCOPE scope = GtTaskGroup::CUSTOM;

    if (!m_impl->project->processData()->deleteTaskGroup(groupName, scope))
    {
        gtError() << tr("Failed to delete task group: %1").arg(groupName);
        return;
    }

    if (!m_impl->project->processData()->save(m_impl->project->path()))
    {
        gtError() << tr("Failed to save project after deleting task group: %1")
                         .arg(groupName);
        return;
    }

    resetTaskGroupModel();
}

