/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 02.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QPushButton>
#include <QFrame>
#include <QMenu>
#include <QMimeData>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QSignalMapper>
#include <QDebug>

#include "gt_processview.h"
#include "gt_datamodel.h"
#include "gt_application.h"
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
#include "gt_coreprocessexecutor.h"
#include "gt_processconnectioneditor.h"
#include "gt_objectmementodiff.h"
#include "gt_taskprovider.h"
#include "gt_propertyconnection.h"
#include "gt_command.h"
#include "gt_settings.h"
#include "gt_extendedtaskdata.h"
#include "gt_extendedcalculatordata.h"
#include "gt_taskfactory.h"
#include "gt_calculatorfactory.h"
#include "gt_textfilterdelegate.h"
#include "gt_deleteitemmessagebox.h"
#include "gt_objectui.h"
#include "gt_stylesheets.h"
#include "gt_importhandler.h"
#include "gt_importmenu.h"
#include "gt_exporthandler.h"
#include "gt_exportmenu.h"
#include "gt_propertyconnectionfunctions.h"
#include "gt_icons.h"

#include "gt_processdock.h"

using namespace GtPropertyConnectionFunctions;

GtProcessDock::GtProcessDock() :
    m_model(nullptr),
    m_filterModel(nullptr),
    m_processData(nullptr),
    m_currentProcess(nullptr),
    m_project(nullptr),
    m_actionMapper(new QSignalMapper(this))
{
    setObjectName(tr("Processes/Calculators"));

    auto widget = new QWidget(this);
    setWidget(widget);

    auto frame = new QFrame;
    auto frameLayout = new QVBoxLayout;

    frameLayout->setContentsMargins(0, 0, 0, 0);

    m_runButton = new QPushButton(tr("Run"));
    m_runButton->setIcon(GtGUI::Icon::runProcess16());
    m_runButton->setToolTip(tr("Run Selected Process"));
    m_runButton->setEnabled(false);

    m_runButton->setStyleSheet(
                GtStyleSheets::processRunButton(
                    GtStyleSheets::runningButtonState::notRunning));

    m_addElementButton = new QPushButton(tr("Add..."));
    m_addElementButton->setIcon(GtGUI::Icon::add16());
    m_addElementButton->setToolTip(tr("Add New Element"));
    m_addElementButton->setEnabled(false);
    m_addElementButton->setStyleSheet(GtStyleSheets::buttonStyleSheet());

    m_view = new GtProcessView(this);

    frame->setLayout(frameLayout);
    frame->setAutoFillBackground(true);
    frame->setFrameShape(m_view->frameShape());
    frame->setFrameStyle(m_view->frameStyle());
    frame->setFrameShadow(m_view->frameShadow());

    m_view->setFrameStyle(QTreeView::NoFrame);

    auto delegate = new GtTextFilterDelegate(this,
                                             GtTextFilterDelegate::allowSpaces);

    m_view->setItemDelegate(delegate);
    m_view->setEditTriggers(QTreeView::SelectedClicked);

    //    connect(m_dataView, SIGNAL(throwProperties(GtdObject*)),
    //            communicator, SIGNAL(throwProperties(GtdObject*)));

    auto btnLayout = new QHBoxLayout;
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(1);
    btnLayout->addWidget(m_runButton);
    btnLayout->addWidget(m_addElementButton);

    auto layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    layout->addLayout(btnLayout);
    //    layout->addWidget(m_view);
    frameLayout->addWidget(m_view);

    m_search = new GtSearchWidget;
    frameLayout->addWidget(m_search);

    layout->addWidget(frame);

    widget->setLayout(layout);

    //    connect(gtApp, SIGNAL(currentProjectChanged(GtProject*)),
    //            SLOT(currentProjectChanged(GtProject*)));
    connect(m_search, SIGNAL(textEdited(QString)),
            SLOT(filterData(QString)));
    connect(m_search, SIGNAL(textChanged(QString)),
            SLOT(filterData(QString)));
    connect(m_view, SIGNAL(searchRequest()), m_search,
            SLOT(enableSearch()));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(customContextMenu(QPoint)));
    connect(m_view, SIGNAL(clicked(QModelIndex)), SLOT(onClicked(QModelIndex)));
    connect(m_view, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(onDoubleClicked(QModelIndex)));
    connect(m_view, SIGNAL(copyProcessElement(QModelIndex)),
            SLOT(copyElement(QModelIndex)));
    connect(m_view, SIGNAL(cutProcessElement(QModelIndex)),
            SLOT(cutElement(QModelIndex)));
    connect(m_view, SIGNAL(deleteProcessElements(QList<QModelIndex>)),
            SLOT(deleteProcessElements(QList<QModelIndex>)));
    connect(m_view, SIGNAL(pasteProcessElement(QModelIndex)),
            SLOT(pasteElement(QModelIndex)));
    connect(m_view, SIGNAL(runTaskElement(QModelIndex)),
            SLOT(runProcess()));
    connect(m_view, SIGNAL(skipCalcultorElement(QModelIndex,bool)),
            SLOT(skipComponent(QModelIndex,bool)));
    connect(m_view, SIGNAL(renameProcessElement(QModelIndex)),
            SLOT(renameElement()));

    connect(m_runButton, SIGNAL(clicked(bool)), SLOT(runProcess()));
    connect(m_addElementButton, SIGNAL(clicked(bool)), SLOT(addElement()));
    connect(gtProcessExecutor, SIGNAL(queueChanged()),
            SLOT(onProcessQueueChange()));
    //    connect(m_view, SIGNAL(currentObjectChanged(GtObject*)),
    //            SLOT(updateButtons(GtObject*)));
    //    m_search->setVisible(false);

    connect(gtDataModel, SIGNAL(triggerEndResetDataModelView()),
            SLOT(resetModel()));
    connect(this, SIGNAL(selectedObjectChanged(GtObject*)),
            gtApp, SIGNAL(objectSelected(GtObject*)));
    connect(m_actionMapper, SIGNAL(mapped(QObject*)),
            SLOT(actionTriggered(QObject*)));

    registerShortCut("runProcess", QKeySequence(Qt::CTRL + Qt::Key_R));
    registerShortCut("unskipProcess", QKeySequence(Qt::CTRL + Qt::Key_T));
    registerShortCut("skipProcess", QKeySequence(Qt::CTRL + Qt::Key_G));
}

Qt::DockWidgetArea
GtProcessDock::getDockWidgetArea()
{
    return Qt::RightDockWidgetArea;
}

void
GtProcessDock::setCurrentProcess(GtTask* process)
{
    if (process == m_currentProcess)
    {
        return;
    }

    if (m_currentProcess)
    {
        disconnect(m_currentProcess, SIGNAL(destroyed()),
                   this, SLOT(onProcessDestroyed()));
        disconnect(m_currentProcess, SIGNAL(objectNameChanged(QString)),
                   this, SLOT(updateRunButton()));
        disconnect(m_currentProcess, SIGNAL(skipPropertyChanged()),
                   this, SLOT(updateRunButton()));
        disconnect(m_currentProcess, SIGNAL(dataChanged(GtObject*)),
                   this, SLOT(onTaskDataChanged(GtObject*)));
    }

    m_currentProcess = process;

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
        m_runButton->setEnabled(false);
    }
}

void
GtProcessDock::projectChangedEvent(GtProject* project)
{
    setCurrentProcess();

    delete m_model;

    m_model = new GtProcessComponentModel(this);
    m_filterModel = new GtProcessFilterModel(m_model);
    m_model->setSourceModel(gtDataModel);
    m_filterModel->setSourceModel(m_model);
    m_project = project;

    if (project)
    {
        m_processData = project->processData();

        if (m_processData)
        {
            filterData(m_search->text());
        }
    }

    updateButtons(m_processData);

    if (!project)
    {
        m_addElementButton->setEnabled(false);
    }

    m_view->expandAll();
    m_view->resizeColumns();

    connect(m_model,
            SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)),
            SLOT(onRowsAboutToBeMoved()));
    connect(m_model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
            SLOT(onRowsMoved()));
}

void
GtProcessDock::addEmptyTaskToRoot()
{
    if (!m_processData)
    {
        return;
    }

    QString taskId = gtDataModel->uniqueObjectName(tr("New Task"),
                     m_processData);

    if (taskId.isEmpty())
    {
        return;
    }

    m_view->setFocus();

    auto task = new GtTask;
    task->setObjectName(taskId);
    task->setFactory(gtProcessFactory);

    QModelIndex srcIndex = gtDataModel->appendChild(task, m_processData);

    QModelIndex index = mapFromSource(srcIndex);

    if (index.isValid())
    {
        QModelIndex tmpIndex = m_filterModel->index(index.row(), 1,
                               index.parent());
        QItemSelection itemSel(index, tmpIndex);

        m_view->scrollTo(index);
        m_view->selectionModel()->select(itemSel,
                                         QItemSelectionModel::ClearAndSelect);
        m_view->setCurrentIndex(index);
        m_view->edit(index);
    }
}

GtProcessComponent*
GtProcessDock::componentByModelIndex(const QModelIndex &index)
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
GtProcessDock::addCalculator()
{
    QModelIndex srcIndex = mapToSource(m_view->currentIndex());

    if (!srcIndex.isValid())
    {
        return;
    }

    GtObject* obj = gtDataModel->objectFromIndex(srcIndex);

    if (!obj)
    {
        return;
    }

    auto project = obj->findParent<GtProject*>();

    if (!project)
    {
        return;
    }

    GtCalculatorProvider provider;
    GtProcessWizard wizard(project, &provider, this);
    wizard.resize(560, 500);

    if (!wizard.exec())
    {
        return;
    }

    GtObjectMemento memento = provider.componentData();

    if (memento.isNull())
    {
        return;
    }

    auto newObj = memento.restore<GtProcessComponent*>(gtProcessFactory);

    if (!newObj)
    {
        return;
    }

    gtDebug() << "Calculator appended! (" << newObj->metaObject()->className()
              << ")";

    updateLastUsedElementList(newObj->metaObject()->className());

    QModelIndex newIndex = gtDataModel->appendChild(newObj, obj);

    QModelIndex index = mapFromSource(newIndex);

    if (!index.isValid())
    {
        return;
    }

    m_view->setFocus();

    QModelIndex tmpIndex = m_filterModel->index(index.row(), 1,
                           index.parent());
    QItemSelection itemSel(index, tmpIndex);

    m_view->expand(m_view->currentIndex());
    m_view->scrollTo(index);
    m_view->selectionModel()->select(itemSel,
                                     QItemSelectionModel::ClearAndSelect);
    m_view->setCurrentIndex(index);
}

void
GtProcessDock::addTask()
{
    if (!m_processData)
    {
        return;
    }

    auto project = m_processData->findParent<GtProject*>();

    if (!project)
    {
        gtError() << "project == nullptr";
        return;
    }

    GtTaskProvider provider;
    GtProcessWizard wizard(project, &provider, this);
    wizard.resize(560, 500);

    if (!wizard.exec())
    {
        return;
    }

    GtObject* parentObj = nullptr;

    if (!m_view->currentIndex().isValid())
    {
        parentObj = m_processData;
    }
    else
    {
        // add task to other task
        QModelIndex srcIndex = mapToSource(m_view->currentIndex());

        if (!srcIndex.isValid())
        {
            return;
        }

        parentObj = gtDataModel->objectFromIndex(srcIndex);
    }

    if (!parentObj)
    {
        return;
    }

    GtObjectMemento memento = provider.componentData();

    if (memento.isNull())
    {
        return;
    }

    auto newObj = memento.restore<GtProcessComponent*>(gtProcessFactory);

    if (!newObj)
    {
        return;
    }

    QString taskId = gtDataModel->uniqueObjectName(newObj->objectName(),
                     parentObj);
    newObj->setObjectName(taskId);

    gtDebug() << "Task appended! (" << newObj->metaObject()->className()
              << ")";

    updateLastUsedElementList(newObj->metaObject()->className());

    QModelIndex newIndex = gtDataModel->appendChild(newObj, parentObj);

    QModelIndex index = mapFromSource(newIndex);

    if (!index.isValid())
    {
        return;
    }

    m_view->setFocus();

    QModelIndex tmpIndex = m_filterModel->index(index.row(), 1,
                           index.parent());
    QItemSelection itemSel(index, tmpIndex);

    m_view->expand(m_view->currentIndex());
    m_view->scrollTo(index);
    m_view->selectionModel()->select(itemSel,
                                     QItemSelectionModel::ClearAndSelect);
    m_view->setCurrentIndex(index);
    m_view->edit(index);
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
    if (!qobject_cast<GtProcessData*>(obj->parent()))
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
    if (!m_filterModel)
    {
        return;
    }

    m_filterModel->setFilterRegExp(val);

    if (m_rootIndex.isValid())
    {
        return;
    }

    m_view->setModel(nullptr);

    if (m_processData)
    {
        QModelIndex srcIndex = gtDataModel->indexFromObject(m_processData);
        QModelIndex index = mapFromSource(srcIndex);

        m_rootIndex = QPersistentModelIndex(index);

        if (m_rootIndex.isValid())
        {
            m_view->setModel(m_filterModel);
            m_view->setRootIndex(m_rootIndex);
            connect(m_view->selectionModel(),
                    SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                    SLOT(onCurrentChanged(QModelIndex,QModelIndex)),
                    Qt::UniqueConnection);
            m_view->setCurrentIndex(QModelIndex());
        }
    }

    m_view->resizeColumns();
}

void
GtProcessDock::updateButtons(GtObject* obj)
{
    m_addElementButton->setEnabled(true);

    setCurrentProcess(findRootTaskHelper(obj));

    if (obj && (qobject_cast<GtCalculator*>(obj) || obj->isDummy() ||
                obj->hasDummyParents()))
    {
        m_addElementButton->setEnabled(false);
    }

    updateRunButton();
}

void
GtProcessDock::updateRunButton()
{
    // no process selected
    if (!m_currentProcess)
    {
        m_runButton->setIcon(GtGUI::Icon::runProcess16());

        m_runButton->setStyleSheet(
                    GtStyleSheets::processRunButton(
                        GtStyleSheets::runningButtonState::notRunning));

        m_runButton->setEnabled(false);
        m_runButton->setText(tr("Run"));
        m_runButton->setToolTip(tr("No Process selected"));
        return;
    }

    const QString& str = m_currentProcess->objectName();

    GtTask* current = gtProcessExecutor->currentRunningTask();

    // no task is running
    if (!current)
    {
        m_runButton->setIcon(GtGUI::Icon::runProcess16());

        m_runButton->setStyleSheet(
                    GtStyleSheets::processRunButton(
                        GtStyleSheets::runningButtonState::notRunning));

        if (str.isEmpty())
        {
            m_runButton->setEnabled(false);
            m_runButton->setToolTip(tr("No Process selected"));
            m_runButton->setText(tr("Run"));
        }
        else
        {
            m_runButton->setEnabled(true);
            m_runButton->setToolTip(tr("Run Selected Process"));
            m_runButton->setText(tr("Run") + " (" + str + ")");
        }
    }
    // a task is running
    else if (current != m_currentProcess)
    {
        // queue
        m_runButton->setIcon(GtGUI::Icon::queue16());

        m_runButton->setStyleSheet(
                    GtStyleSheets::processRunButton(
                        GtStyleSheets::runningButtonState::otherProcessRunning));

        if (gtProcessExecutor->taskQueued(m_currentProcess))
        {
            m_runButton->setEnabled(false);
            m_runButton->setText(str + tr(" already queued"));
            m_runButton->setToolTip(tr("Process already queued"));
        }
        else
        {
            m_runButton->setEnabled(true);
            m_runButton->setText(tr("Add (") + str + tr(") to queue"));
            m_runButton->setToolTip(tr("Add Selected Process to Queue"));
        }
    }
    // selected task is running
    else
    {
        m_runButton->setIcon(GtGUI::Icon::runProcess16());

        m_runButton->setStyleSheet(
                    GtStyleSheets::processRunButton(
                        GtStyleSheets::runningButtonState::thisProcessRunning));

        // stopping
        if (m_currentProcess->currentState() ==
                GtProcessComponent::TERMINATION_REQUESTED)
        {
            m_runButton->setIcon(GtGUI::Icon::stopRequest16());
            m_runButton->setEnabled(false);
            m_runButton->setToolTip(tr("Terminating process..."));
            m_runButton->setText(str + tr(" terminating..."));
        }
        else
        {
            m_runButton->setIcon(GtGUI::Icon::stop16());
            m_runButton->setEnabled(true);
            m_runButton->setToolTip(tr("Stop Selected Process"));
            m_runButton->setText(tr("Stop") + " (" + str + ")");
        }
    }

    // disable run button if process is not ready, skip running task
    if (current != m_currentProcess && (m_currentProcess->hasDummyChildren() ||
                                        m_currentProcess->isSkipped()))
    {
        // text, style and icon are already set
        m_runButton->setEnabled(false);
        m_runButton->setToolTip(tr("Invalid Process selected"));
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
    emit selectedObjectChanged(obj);
}

void
GtProcessDock::onClicked(const QModelIndex& index)
{
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
        //        gtMdiLauncher->open(GtProcessEditor::staticMetaObject.className(),
        //                            task);
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
    QModelIndex srcIndex = mapToSource(m_view->currentIndex());

    if (srcIndex.isValid())
    {
        GtObject* obj = gtDataModel->objectFromIndex(srcIndex);

        if (obj)
        {
            GtTask* task = findRootTaskHelper(obj);

            if (task)
            {
                if (task->currentState() ==
                        GtProcessComponent::RUNNING)
                {
                    terminateProcess();
                }
                else
                {
                    gtProcessExecutor->runTask(task);
                }
            }
        }
    }
}

void
GtProcessDock::terminateProcess()
{
    if (!m_currentProcess)
    {
        return;
    }

    QMessageBox mb;
    mb.setIcon(QMessageBox::Question);
    mb.setWindowTitle(tr("Stop Running Task"));
    mb.setWindowIcon(GtGUI::Icon::stop16());
    mb.setText(tr("Stop the execution of") + " " +
               m_currentProcess->objectName() + "?\n\n" +
               tr("Note: The process cannot be aborted while a "
                  "calculator is running. \n"
                  "The process will be marked for termination "
                  "and will be stopped after completion of the "
                  "calculator."));
    mb.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    mb.setDefaultButton(QMessageBox::Cancel);
    int ret = mb.exec();

    switch (ret)
    {
        case QMessageBox::Yes:
        {
            gtProcessExecutor->terminateTask(m_currentProcess);
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

void
GtProcessDock::addElement()
{
    QMenu menu(this);

    QModelIndex srcIndex = mapToSource(m_view->currentIndex());

    QAction* addemptytask = menu.addAction(tr("Empty Task"));
    addemptytask->setIcon(GtGUI::Icon::addProcess16());

    if (!srcIndex.isValid())
    {
        menu.addSeparator();
    }

    QAction* addtask = menu.addAction(tr("New Task..."));
    addtask->setIcon(GtGUI::Icon::addProcess16());

    QAction* addcalc = menu.addAction(tr("New Calculator..."));
    addcalc->setIcon(GtGUI::Icon::calculator16());

    if (!srcIndex.isValid())
    {
        addcalc->setVisible(false);
    }
    else
    {
        addemptytask->setVisible(false);

        GtObject* obj = gtDataModel->objectFromIndex(srcIndex);

        if (!obj)
        {
            return;
        }

        if (!qobject_cast<GtTask*>(obj))
        {
            addcalc->setEnabled(false);
            addtask->setEnabled(false);
        }
    }

    if (!gtApp->settings()->lastProcessElements().isEmpty())
    {
        menu.addSeparator();
        generateLastUsedElementMenu(&menu, !srcIndex.isValid());
    }

    QAction* a = menu.exec(QCursor::pos());

    if (a == addemptytask)
    {
        addEmptyTaskToRoot();
    }
    else if (a == addtask)
    {
        addTask();
    }
    else if (a == addcalc)
    {
        addCalculator();
    }
}

void
GtProcessDock::customContextMenu(const QModelIndex& srcIndex)
{
    if (srcIndex.isValid())
    {
        if (m_view->selectionModel()->selectedIndexes().size() < 3)
        {
            GtObject* obj = gtDataModel->objectFromIndex(srcIndex);

            if (!obj)
            {
                return;
            }

            if (auto p = qobject_cast<GtTask*>(obj))
            {
                processContextMenu(p, srcIndex);
            }
            else if (auto c = qobject_cast<GtCalculator*>(obj))
            {
                calculatorContextMenu(c, srcIndex);
            }
        }
        else
        {
            multiSelectionContextMenu(
                        m_view->selectionModel()->selectedIndexes());
        }
    }
    else
    {
        QMenu menu(this);

        QMenu* addMenu = menu.addMenu(GtGUI::Icon::add16(), tr("Add..."));

        QAction* addemptytask = addMenu->addAction(tr("Empty Task"));
        addemptytask->setIcon(GtGUI::Icon::addProcess16());

        addMenu->addSeparator();

        QAction* addtask = addMenu->addAction(tr("New Task..."));
        addtask->setIcon(GtGUI::Icon::addProcess16());

        if (!gtApp->settings()->lastProcessElements().isEmpty())
        {
            addMenu->addSeparator();
            generateLastUsedElementMenu(addMenu, true);
        }

        menu.addSeparator();

        auto imenu = new GtImportMenu(m_processData, &menu);

        menu.addMenu(imenu);

        menu.addSeparator();

        QAction* actpaste = menu.addAction("Paste");
        actpaste->setIcon(GtGUI::Icon::paste16());
        actpaste->setEnabled(false);

        QClipboard* clipboard = QApplication::clipboard();
        const QMimeData* mimeData = clipboard->mimeData();

        if (mimeData &&
                mimeData->hasFormat(QStringLiteral("GtObject")))
        {
            GtObjectMemento memento(mimeData->data(QStringLiteral("GtObject")));

            if (memento.canCastTo(GtTask::staticMetaObject.className(),
                                  gtProcessFactory))
            {
                actpaste->setEnabled(true);
            }
        }

        QAction* a = menu.exec(QCursor::pos());

        if (a == actpaste)
        {
            pasteElement(m_processData);
            //            pasteProcessComponent(QModelIndex());
            //            pasteProcessElement(index);
        }
        else if (a == addemptytask)
        {
            addEmptyTaskToRoot();
        }
        else if (a == addtask)
        {
            addTask();
        }
    }
}


void
GtProcessDock::customContextMenu(const QPoint& pos)
{
    if (!m_view->model())
    {
        return;
    }

    QModelIndex index = m_view->indexAt(pos);
    QModelIndex srcIndex = mapToSource(index);

    customContextMenu(srcIndex);
}

void
GtProcessDock::processContextMenu(GtTask* obj, const QModelIndex& index)
{
    // incomplete processelements should not be modified
    bool hasInvalidParents = obj->hasDummyParents();

    QMenu menu(this);

    QAction* actrun = menu.addAction(tr("Run Task"));
    actrun->setIcon(GtGUI::Icon::runProcess16());
    QAction* actstop = menu.addAction(tr("Stop Task"));
    actstop->setIcon(GtGUI::Icon::stop16());

    /// This line is only for the entry in the context menu and
    /// does not trigger the action
    actrun->setShortcut(getShortCut("runProcess"));

    if (qobject_cast<GtProcessData*>(obj->parent()))
    {
        actrun->setVisible(true);
        actstop->setVisible(true);
    }
    else
    {
        actrun->setVisible(false);
        actstop->setVisible(false);
    }

    menu.addSeparator();

    QAction* actconfig = menu.addAction("Config...");
    actconfig->setIcon(GtGUI::Icon::config16());

    QAction* actconnect = menu.addAction(tr("Connection Editor"));
    actconnect->setIcon(GtGUI::Icon::connection16());

    menu.addSeparator();

    QMenu* addMenu = menu.addMenu(GtGUI::Icon::add16(), tr("Add..."));

    QAction* addtask = addMenu->addAction(tr("New Task..."));
    addtask->setIcon(GtGUI::Icon::addElement16());

    QAction* addcalc = addMenu->addAction(tr("New Calculator..."));
    addcalc->setIcon(GtGUI::Icon::calculator16());

    if (!gtApp->settings()->lastProcessElements().isEmpty())
    {
        addMenu->addSeparator();
        generateLastUsedElementMenu(addMenu, false);
    }

    menu.addSeparator();

    QAction* actskip = menu.addAction("Skip");
    actskip->setIcon(GtGUI::Icon::skip16());
    actskip->setShortcut(getShortCut("skipProcess"));

    QAction* actunskip = menu.addAction("Unskip");
    actunskip->setIcon(GtGUI::Icon::arrowRight());
    actunskip->setShortcut(getShortCut("unskipProcess"));

    if (!obj->isSkipped())
    {
        actunskip->setVisible(false);
    }
    else
    {
        actskip->setVisible(false);
    }

    QAction* actrename = menu.addAction("Rename");
    actrename->setIcon(GtGUI::Icon::input16());
    actrename->setShortcut(gtApp->getShortCutSequence("rename"));

    menu.addSeparator();

    // importer menu
    QList<GtImporterMetaData> importerList =
            gtImportHandler->importerMetaData(obj->metaObject()->className());

    if (!importerList.isEmpty())
    {
        auto imenu = new GtImportMenu(obj, &menu);
        imenu->setEnabled(!hasInvalidParents);

        menu.addMenu(imenu);
        menu.addSeparator();
    }

    // exporter menu
    QList<GtExporterMetaData> exporterList =
            gtExportHandler->exporterMetaData(obj->metaObject()->className());

    if (!exporterList.isEmpty())
    {
        auto emenu = new GtExportMenu(obj, &menu);
        emenu->setEnabled(!obj->hasDummyChildren());

        menu.addMenu(emenu);
        menu.addSeparator();
    }

    QAction* actclone = menu.addAction("Clone");
    actclone->setIcon(GtGUI::Icon::clone16());

    QAction* actcut = menu.addAction("Cut");
    actcut->setShortcut(gtApp->getShortCutSequence("cut"));
    actcut->setIcon(GtGUI::Icon::cut16());

    QAction* actcopy = menu.addAction("Copy");
    actcopy->setShortcut(gtApp->getShortCutSequence("copy"));
    actcopy->setIcon(GtGUI::Icon::copy16());
    QAction* actpaste = menu.addAction("Paste");
    actpaste->setIcon(GtGUI::Icon::paste16());
    actpaste->setShortcut(gtApp->getShortCutSequence("paste"));
    actpaste->setEnabled(false);
    menu.addSeparator();
    QAction* actdelete = menu.addAction("Delete");
    actdelete->setIcon(GtGUI::Icon::delete16());
    actdelete->setShortcut(gtApp->getShortCutSequence("delete"));

    if (!hasInvalidParents)
    {
        QClipboard* clipboard = QApplication::clipboard();
        const QMimeData* mimeData = clipboard->mimeData();

        if (mimeData &&
                mimeData->hasFormat(QStringLiteral("GtObject")))
        {
            GtObjectMemento memento(mimeData->data(QStringLiteral("GtObject")));

            if (memento.canCastTo(GtCalculator::staticMetaObject.className(),
                                  gtProcessFactory))
            {
                actpaste->setEnabled(true);
            }
            else if (memento.canCastTo(GtTask::staticMetaObject.className(),
                                       gtProcessFactory))
            {
                actpaste->setEnabled(true);
            }
        }
    }

    if (!componentIsReady(obj) || hasInvalidParents)
    {
        actrun->setEnabled(false);
        actconnect->setEnabled(false);
        actrename->setEnabled(false);
        actclone->setEnabled(false);
        actcut->setEnabled(false);
        actcopy->setEnabled(false);
        actdelete->setEnabled(false);
        actconfig->setEnabled(false);
        addMenu->setEnabled(false);
    }
    else
    {
        actstop->setVisible(false);
    }

    QAction* a = menu.exec(QCursor::pos());

    if (a == actdelete)
    {
        deleteProcessElements(m_view->selectionModel()->selectedIndexes());
    }
    else if (a == actrename)
    {
        QModelIndex idx = mapFromSource(index);

        if (idx.isValid())
        {
            m_view->edit(idx);
        }
    }
    else if (a == actskip)
    {
        skipComponent(mapFromSource(index));
    }
    else if (a == actunskip)
    {
        skipComponent(mapFromSource(index), false);
    }
    else if (a == actcopy)
    {
        copyElement(mapFromSource(index));
    }
    else if (a == actcut)
    {
        cutElement(mapFromSource(index));
    }
    else if (a == actclone)
    {
        cloneElement(mapFromSource(index));
    }
    else if (a == actpaste)
    {
        pasteElement(obj);
    }
    else if (a == actrun)
    {
        runProcess();
    }
    else if (a == actstop)
    {
        terminateProcess();
    }
    else if (a == actconnect)
    {
        openConnectionEditor(index);
    }
    else if (a == addtask)
    {
        addTask();
    }
    else if (a == addcalc)
    {
        addCalculator();
    }
    else if (a == actconfig)
    {
        configTask(obj);
    }
}

void
GtProcessDock::calculatorContextMenu(GtCalculator* obj,
                                     const QModelIndex& index)
{
    // incomplete processelements should not be modified
    bool hasInvalidParents = obj->hasDummyParents();

    QMenu menu(this);
    QAction* actconfig = menu.addAction("Config...");
    actconfig->setIcon(GtGUI::Icon::config16());

    QAction* actconnect = menu.addAction(tr("Connection Editor"));
    actconnect->setIcon(GtGUI::Icon::connection16());

    menu.addSeparator();

    QAction* actskip = menu.addAction("Skip");
    actskip->setIcon(GtGUI::Icon::skip16());
    actskip->setShortcut(getShortCut("skipProcess"));

    QAction* actunskip = menu.addAction("Unskip");
    actunskip->setIcon(GtGUI::Icon::arrowRight());
    actunskip->setShortcut(getShortCut("unskipProcess"));

    if (!obj->isSkipped())
    {
        actunskip->setVisible(false);
    }
    else
    {
        actskip->setVisible(false);
    }

    QAction* actrename = menu.addAction("Rename");
    actrename->setIcon(GtGUI::Icon::input16());
    actrename->setShortcut(gtApp->getShortCutSequence("rename"));

    if (!obj->isRenamable())
    {
        actrename->setEnabled(false);
    }

    menu.addSeparator();

    // importer menu
    QList<GtImporterMetaData> importerList =
            gtImportHandler->importerMetaData(obj->metaObject()->className());

    if (!importerList.isEmpty())
    {
        auto imenu = new GtImportMenu(obj, &menu);
        imenu->setEnabled(!hasInvalidParents);

        menu.addMenu(imenu);
        menu.addSeparator();
    }

    // exporter menu
    QList<GtExporterMetaData> exporterList =
            gtExportHandler->exporterMetaData(obj->metaObject()->className());

    if (!exporterList.isEmpty())
    {
        auto emenu = new GtExportMenu(obj, &menu);

        menu.addMenu(emenu);
        menu.addSeparator();
    }

    QAction* actclone = menu.addAction("Clone");
    actclone->setIcon(GtGUI::Icon::clone16());

    QAction* actcut = menu.addAction("Cut");
    actcut->setShortcut(gtApp->getShortCutSequence("cut"));
    actcut->setIcon(GtGUI::Icon::cut16());

    QAction* actcopy = menu.addAction("Copy");
    actcopy->setShortcut(gtApp->getShortCutSequence("copy"));
    actcopy->setIcon(GtGUI::Icon::copy16());

    QAction* actpaste = menu.addAction("Paste");
    actpaste->setIcon(GtGUI::Icon::paste16());
    actpaste->setShortcut(gtApp->getShortCutSequence("paste"));
    actpaste->setEnabled(false);

    menu.addSeparator();

    QAction* actdelete = menu.addAction("Delete");
    actdelete->setIcon(GtGUI::Icon::delete16());
    actdelete->setShortcut(gtApp->getShortCutSequence("delete"));

    if (!componentIsReady(obj) || hasInvalidParents)
    {
        actconfig->setEnabled(false);
        actconnect->setEnabled(false);
        actrename->setEnabled(false);
        actclone->setEnabled(false);
        actcut->setEnabled(false);
        actcopy->setEnabled(false);
        actdelete->setEnabled(false);
        actskip->setEnabled(false);
    }

    QAction* a = menu.exec(QCursor::pos());

    if (a == actdelete)
    {
        deleteProcessElements(m_view->selectionModel()->selectedIndexes());
    }
    else if (a == actrename)
    {
        QModelIndex idx = mapFromSource(index);

        if (idx.isValid())
        {
            m_view->edit(idx);
        }
    }
    else if (a == actskip)
    {
        skipComponent(mapFromSource(index));
    }
    else if (a == actunskip)
    {
        skipComponent(mapFromSource(index), false);
    }
    else if (a == actcopy)
    {
        copyElement(mapFromSource(index));
    }
    else if (a == actcut)
    {
        cutElement(mapFromSource(index));
    }
    else if (a == actclone)
    {
        cloneElement(mapFromSource(index));
    }
    else if (a == actpaste)
    {
        pasteElement(obj);
    }
    else if (a == actconfig)
    {
        configCalculator(obj);
    }
    else if (a == actconnect)
    {
        openConnectionEditor(index);
    }
}

void
GtProcessDock::multiSelectionContextMenu(QList<QModelIndex> const& indexList)
{
    QMenu menu(this);

    QAction* skipCalcs = menu.addAction("Skip Selected Calculators");
    skipCalcs->setIcon(GtGUI::Icon::skip16());

    QAction* unskipCalcs = menu.addAction("Unskip Selected Calculators");
    unskipCalcs->setIcon(GtGUI::Icon::arrowRight());

    QAction* deleteElements = menu.addAction("Delete Process Elements");
    deleteElements->setIcon(GtGUI::Icon::delete16());

    bool allSkipped = true;
    bool allUnskipped = true;
    // counter for dummy objects
    int dummyObjects = 0;

    for (const QModelIndex& index : indexList)
    {
        GtProcessComponent* pc = componentByModelIndex(index);

        if (!pc || pc->isDummy() || pc->hasDummyParents())
        {
            dummyObjects += 1;
            continue;
        }

        dummyObjects += (pc->hasDummyParents());

        if (!pc->isSkipped())
        {
            allSkipped = false;
        }
        else
        {
            allUnskipped = false;
        }
    }

    if (allUnskipped)
    {
        unskipCalcs->setVisible(false);
    }
    else if (allSkipped)
    {
        skipCalcs->setVisible(false);
    }

    // dummy objects cannot be deleted
    if (dummyObjects > 0)
    {
        deleteElements->setEnabled(false);
    }
    // dummy objects cannot be skipped
    if (dummyObjects == indexList.length())
    {
        skipCalcs->setEnabled(false);
        unskipCalcs->setEnabled(false);
    }

    QAction* a = menu.exec(QCursor::pos());

    if (a == skipCalcs)
    {
        skipComponent(indexList);
    }
    else if (a == unskipCalcs)
    {
        skipComponent(indexList, false);
    }
    else if (a == deleteElements)
    {
        deleteProcessElements(indexList);
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
        if (qobject_cast<GtProcessData*>(origTask->parent()))
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
                internalPropertyConnections(origTask);

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

            setOffLostConnectionWarnings(lostPropertyConnections(origTask),
                                         highestParentTask(origTask));

            mapPropertyConnections(origTaskClone, newTask);

            delete origTaskClone;
        }

        updateRelativeObjectLinks(origTask, newTask);
    }

    QMimeData* mimeData = gtDataModel->mimeDataFromObject(copy, false);
    QApplication::clipboard()->setMimeData(mimeData);

    delete copy;
}

void
GtProcessDock::renameElement()
{
    QModelIndex srcIndex = m_view->currentIndex();

    if (srcIndex.isValid())
    {
        m_view->edit(m_view->currentIndex());
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
        if (qobject_cast<GtProcessData*>(origTask->parent()))
        {
            mapPropertyConnections(origTask, newTask);
        }
        else
        {
            GtTask* origTaskClone =
                qobject_cast<GtTask*>(origTask->clone());

            if (!origTaskClone)
            {
                return;
            }

            QList<GtPropertyConnection*> validPropCons =
                internalPropertyConnections(origTask);

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

            setOffLostConnectionWarnings(lostPropertyConnections(origTask),
                                         highestParentTask(origTask));

            mapPropertyConnections(origTaskClone, newTask);

            delete origTaskClone;
        }

        updateRelativeObjectLinks(origTask, newTask);
    }

    // paste new element
    pasteElement(cloned, parent);
}

void
GtProcessDock::cutElement(const QModelIndex& index)
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

    if (!m_processData)
    {
        return;
    }

    copyElement(index);

    QList<GtObject*> toDelete;

    if (qobject_cast<GtTask*>(pComp))
    {
        if (!qobject_cast<GtProcessData*>(pComp->parent()))
        {
            GtTask* child = qobject_cast<GtTask*>(pComp);

            QList<GtPropertyConnection*> relatedCons =
                relatedPropertyConnections(child);

            foreach (GtPropertyConnection* propCon, relatedCons)
            {
                toDelete.append(propCon);
            }

            setOffLostConnectionWarnings(relatedCons, highestParentTask(child));
        }
    }

    toDelete.append(pComp);

    GtCommand command =
        gtApp->startCommand(gtApp->currentProject(),
                            tr("Cut Process Element") +
                            QStringLiteral(" (") + obj->objectName() +
                            QStringLiteral(")"));

    gtDataModel->deleteFromModel(toDelete);

    gtApp->endCommand(command);
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
        case QMessageBox::Yes:
        {
            gtDataModel->reduceToParents(objects);

            GtCommand cmmd = gtApp->startCommand(gtApp->currentProject(),
                                                 "Delete Process Elements");

            foreach (GtObject* obj, objects)
            {
                deleteProcessComponent(obj);
            }

            gtApp->endCommand(cmmd);

        }

        case QMessageBox::Cancel:
        {
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

    GtObject* obj = gtDataModel->objectFromMimeData(mimeData, true,
                    gtProcessFactory);

    if (!obj)
    {
        gtError() << tr("Could not restore object from clipboard!");
        return;
    }

    if (!qobject_cast<GtProcessData*>(parent))
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
    if (!obj || !parent)
    {
        return;
    }

    if (qobject_cast<GtCalculator*>(parent))
    {
        return;
    }

    if (!qobject_cast<GtProcessData*>(parent))
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

    // create new unique identification string
    QString newId = gtDataModel->uniqueObjectName(obj->objectName(),
                    parent);

    // set new identification string
    obj->setObjectName(newId);

    m_view->setFocus();

    GtCommand paste = gtApp->startCommand(gtApp->currentProject(),
                                          "paste process element");

    QList<GtPropertyConnection*> propCons =
        obj->findChildren<GtPropertyConnection*>();

    foreach (GtPropertyConnection* propCon, propCons)
    {
        propCon->setParent(nullptr);
    }

    QModelIndex srcIndex = gtDataModel->appendChild(obj, parent);

    auto task = qobject_cast<GtTask*>(obj);

    if (task)
    {
        GtTask* highestParent = highestParentTask(task);

        if (highestParent)
        {
            foreach (GtPropertyConnection* propCon, propCons)
            {
                gtDataModel->appendChild(propCon, highestParent);
            }
        }
    }

    gtApp->endCommand(paste);

    QModelIndex index = mapFromSource(srcIndex);

    if (index.isValid())
    {
        QModelIndex tmpIndex = m_filterModel->index(index.row(), 1,
                               index.parent());
        QItemSelection itemSel(index, tmpIndex);

        m_view->scrollTo(index);
        m_view->selectionModel()->select(itemSel,
                                         QItemSelectionModel::ClearAndSelect);
        m_view->setCurrentIndex(index);
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
    QString msg;

    if (skip)
    {
        msg = "Skip";
    }
    else
    {
        msg = "Unskip";
    }

    msg.append(" Selected Process Elements");

    GtCommand cmd = gtApp->startCommand(gtApp->currentProject(), msg);
    skipComponent(componentByModelIndex(index), skip);

    gtApp->endCommand(cmd);
}

void
GtProcessDock::skipComponent(const QList<QModelIndex>& indexList, bool skip)
{
    QList<GtProcessComponent*> pcs;

    foreach (QModelIndex index, indexList)
    {
        GtProcessComponent* pc = componentByModelIndex(index);

        if (!pc)
        {
            continue;
        }

        if (!pcs.contains(pc))
        {
            pcs.append(pc);
        }
    }

    QString msg;

    if (skip)
    {
        msg = "Skip";
    }
    else
    {
        msg = "Unskip";
    }

    msg.append(" Selected Process Elements");

    GtCommand cmd = gtApp->startCommand(gtApp->currentProject(), msg);
    foreach (GtProcessComponent* pc, pcs)
    {
        skipComponent(pc, skip);
    }

    gtApp->endCommand(cmd);
}

void
GtProcessDock::skipComponent(GtProcessComponent* comp, bool skip)
{
    if (!comp)
    {
        return;
    }

    comp->setSkipped(skip);
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

    GtCommand command =
        gtApp->startCommand(gtApp->currentProject(),
                            calc->objectName() +
                            tr(" configuration changed"));
    calc->fromMemento(memento);
    gtApp->endCommand(command);
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

    GtCommand command =
        gtApp->startCommand(project,
                            task->objectName() +
                            tr(" configuration changed"));
    task->fromMemento(memento);
    gtApp->endCommand(command);
}

QModelIndex
GtProcessDock::mapToSource(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return {};
    }

    QModelIndex tmp1 = m_filterModel->mapToSource(index);

    return m_model->mapToSource(tmp1);
}

QModelIndex
GtProcessDock::mapFromSource(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return {};
    }

    QModelIndex tmp1 = m_model->mapFromSource(index);

    return  m_filterModel->mapFromSource(tmp1);
}

GtProcessData*
GtProcessDock::processData()
{
    return m_processData;
}

void
GtProcessDock::resetModel()
{
    projectChangedEvent(m_project);
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
                                 QStringLiteral(" - ") +
                                 rootTask->objectName();

                GtCommand command = gtApp->startCommand(project, delMsg);

                rootTask->applyDiff(diff);

                gtApp->endCommand(command);
            }
        }
    }
}

void
GtProcessDock::onProcessQueueChange()
{
    updateRunButton();
}

void
GtProcessDock::onRowsAboutToBeMoved()
{
    m_command = gtApp->startCommand(gtApp->currentProject(),
                                    tr("Process elements moved"));
}

void
GtProcessDock::onRowsMoved()
{
    gtApp->endCommand(m_command);
}

void
GtProcessDock::onTaskDataChanged(GtObject* obj)
{
    if (obj == m_currentProcess)
    {
        updateRunButton();
    }
}

void
GtProcessDock::actionTriggered(QObject* obj)
{
    if (!gtApp->currentProject())
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
        GtProcessWizard wizard(gtApp->currentProject(), &provider, this);
        wizard.setWindowTitle(tr("Config ") + taskData->id);

        if (!wizard.exec())
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

        QModelIndex srcIndex = mapToSource(m_view->currentIndex());

        GtObject* parentObj = nullptr;

        if (srcIndex.isValid())
        {
            parentObj = gtDataModel->objectFromIndex(srcIndex);
        }
        else
        {
            parentObj = m_processData;
        }

        if (!parentObj)
        {
            delete task;
            return;
        }

        updateLastUsedElementList(task->metaObject()->className());

        QModelIndex newIndex = gtDataModel->appendChild(task, parentObj);

        QModelIndex index = mapFromSource(newIndex);

        if (!index.isValid())
        {
            return;
        }

        m_view->setFocus();

        QModelIndex tmpIndex = m_filterModel->index(index.row(), 1,
                               index.parent());
        QItemSelection itemSel(index, tmpIndex);

        m_view->expand(m_view->currentIndex());
        m_view->scrollTo(index);
        m_view->selectionModel()->select(itemSel,
                                         QItemSelectionModel::ClearAndSelect);
        m_view->setCurrentIndex(index);
        m_view->edit(index);
    }
    else if (gtCalculatorFactory->calculatorDataExists(className))
    {
        QModelIndex srcIndex = mapToSource(m_view->currentIndex());

        if (!srcIndex.isValid())
        {
            return;
        }

        GtObject* currentObj = gtDataModel->objectFromIndex(srcIndex);

        if (!currentObj)
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
        GtProcessWizard wizard(gtApp->currentProject(), &provider, this);
        wizard.setWindowTitle(tr("Config ") + calcData->id);

        if (!wizard.exec())
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

        QModelIndex newIndex = gtDataModel->appendChild(calc, currentObj);

        QModelIndex index = mapFromSource(newIndex);

        if (!index.isValid())
        {
            return;
        }

        m_view->setFocus();

        QModelIndex tmpIndex = m_filterModel->index(index.row(), 1,
                               index.parent());
        QItemSelection itemSel(index, tmpIndex);

        m_view->expand(m_view->currentIndex());
        m_view->scrollTo(index);
        m_view->selectionModel()->select(itemSel,
                                         QItemSelectionModel::ClearAndSelect);
        m_view->setCurrentIndex(index);
    }
}

bool
GtProcessDock::mapPropertyConnections(GtTask* orig,
                                      GtTask* copy)
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
        GtPropertyConnection* newCon = findConnectionCopy(origCon, newCons);

        if (!newCon)
        {
            gtDebug() << "Could not find copied connection...";
            continue;
        }

        if (!updateConnectionProperties(origCon, newCon, orig, copy))
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
GtProcessDock::generateLastUsedElementMenu(QMenu* menu, bool isRoot)
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

                    connect(act, SIGNAL(triggered(bool)),
                            m_actionMapper, SLOT(map()),
                            Qt::UniqueConnection);
                    m_actionMapper->setMapping(act, act);

                    count++;

                    auto extendedData =
                            dynamic_cast<GtExtendedTaskDataImpl*>(
                                taskData.get());

                    if (!extendedData ||
                        extendedData->icon.isNull())
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
                            icn = GtGUI::Icon::process16();
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

                    connect(act, SIGNAL(triggered(bool)),
                            m_actionMapper, SLOT(map()),
                            Qt::UniqueConnection);
                    m_actionMapper->setMapping(act, act);

                    count++;

                    auto extendedData =
                            dynamic_cast<GtExtendedCalculatorDataImpl*>(
                                calcData.get());

                    if (!extendedData ||
                        extendedData->icon.isNull())
                    {
                        act->setIcon(GtGUI::Icon::calculator16());
                    }
                    else
                    {
                        act->setIcon(extendedData->icon);
                    }

                    if (isRoot)
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
        relatedPropertyConnections(pComp);

    foreach (GtPropertyConnection* propCon, relatedCons)
    {
        toDelete.append(propCon);
    }

    setOffLostConnectionWarnings(lostPropertyConnections(pComp),
                                 highestParentTask(pComp));

    toDelete.append(pComp);

    gtDataModel->deleteFromModel(toDelete);
}

void
GtProcessDock::keyPressEvent(QKeyEvent* event)
{  
    QModelIndex index = m_view->selectionModel()->selectedIndexes().first();

    QModelIndex srcIndex = mapToSource(index);

    if (!srcIndex.isValid())
    {
        return;
    }

    if (gtApp->compareKeyEvent(event, "OpenContextMenu"))
    {
        customContextMenu(srcIndex);
    }
}

