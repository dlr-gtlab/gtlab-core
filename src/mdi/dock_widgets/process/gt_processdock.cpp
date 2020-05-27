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
#include "gt_explorermodel.h"
#include "gt_task.h"
#include "gt_calculator.h"
#include "gt_processfactory.h"
#include "gt_objectmemento.h"
#include "gt_mdilauncher.h"
#include "gt_processeditor.h"
#include "gt_processwizard.h"
#include "gt_calculatorprovider.h"
#include "gt_processfiltermodel.h"
#include "gt_coreprocessexecutor.h"
#include "gt_processmonitoring.h"
#include "gt_processconnectioneditor.h"
#include "gt_objectmementodiff.h"
#include "gt_loop.h"
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
#include "gt_relativeobjectlinkproperty.h"
#include "gt_objectui.h"
#include "gt_stylesheets.h"
#include "gt_importhandler.h"
#include "gt_importmenu.h"
#include "gt_exporthandler.h"
#include "gt_exportmenu.h"

#include "gt_processdock.h"

GtProcessDock::GtProcessDock() :
    m_model(Q_NULLPTR),
    m_filterModel(Q_NULLPTR),
    m_processData(Q_NULLPTR),
    m_currentProcess(Q_NULLPTR),
    m_project(Q_NULLPTR),
    m_actionMapper(new QSignalMapper(this))
{
    setObjectName(tr("Processes/Calculators"));

    QWidget* widget = new QWidget(this);
    setWidget(widget);

    QFrame* frame = new QFrame;
    QVBoxLayout* frameLayout = new QVBoxLayout;

    frameLayout->setContentsMargins(0, 0, 0, 0);

    m_runButton = new QPushButton(tr("Run"));
    m_runButton->setIcon(gtApp->icon("runProcessIcon_16.png"));
    m_runButton->setToolTip(tr("Run Selected Process"));
    m_runButton->setEnabled(false);

    m_runButton->setStyleSheet(
                GtStyleSheets::processRunButton("rgb(230,255,230)"));

    m_addElementButton = new QPushButton(tr("Add..."));
    m_addElementButton->setIcon(gtApp->icon("addIcon_16.png"));
    m_addElementButton->setToolTip(tr("Add New Element"));
    m_addElementButton->setEnabled(false);
    m_addElementButton->setStyleSheet(GtStyleSheets::buttonStyleSheet());

    m_view = new GtProcessView(this);

    frame->setLayout(frameLayout);
    QPalette palette = frame->palette();
    palette.setColor(backgroundRole(), QColor(255, 255, 255));
    frame->setPalette(palette);
    frame->setAutoFillBackground(true);
    frame->setFrameShape(m_view->frameShape());
    frame->setFrameStyle(m_view->frameStyle());
    frame->setFrameShadow(m_view->frameShadow());

    m_view->setFrameStyle(QTreeView::NoFrame);

    GtTextFilterDelegate* delegate = new GtTextFilterDelegate(
                this, GtTextFilterDelegate::allowSpaces);

    m_view->setItemDelegate(delegate);
    m_view->setEditTriggers(QTreeView::SelectedClicked);

    //    connect(m_dataView, SIGNAL(throwProperties(GtdObject*)),
    //            communicator, SIGNAL(throwProperties(GtdObject*)));

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(1);
    btnLayout->addWidget(m_runButton);
    btnLayout->addWidget(m_addElementButton);

    QVBoxLayout* layout = new QVBoxLayout;
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
        disconnect(m_currentProcess, SIGNAL(dataChanged(GtObject*)),
                   this, SLOT(onTaskDataChanged(GtObject*)));
    }

    m_currentProcess = process;

    if (process)
    {
        connect(process, SIGNAL(destroyed()), SLOT(onProcessDestroyed()));
        connect(process, SIGNAL(objectNameChanged(QString)),
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

    if (m_model)
    {
        delete m_model;
    }

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

    if (project == Q_NULLPTR)
    {
        m_addElementButton->setEnabled(false);
    }

    m_view->expandAll();
    m_view->resizeColumns();

    connect(m_model, SIGNAL(rowsAboutToBeMoved(QModelIndex, int,
                            int, QModelIndex,
                            int)),
            SLOT(onRowsAboutToBeMoved()));
    connect(m_model, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)),
            SLOT(onRowsMoved()));
}

void
GtProcessDock::addEmptyTaskToRoot()
{
    if (m_processData == Q_NULLPTR)
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

    GtTask* task = new GtTask;
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

GtCalculator*
GtProcessDock::calcByModelIndex(QModelIndex index)
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

    if (srcIndex.model() != gtDataModel)
    {
        return Q_NULLPTR;
    }

    GtObject* obj = gtDataModel->objectFromIndex(srcIndex);

    if (obj != Q_NULLPTR)
    {
        return qobject_cast<GtCalculator*>(obj);
    }

    return Q_NULLPTR;
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

    if (obj == Q_NULLPTR)
    {
        return;
    }

    GtProject* project = obj->findParent<GtProject*>();

    if (project == Q_NULLPTR)
    {
        return;
    }

    GtCalculatorProvider provider;
    GtProcessWizard wizard(project, &provider);
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

    GtProcessComponent* newObj = memento.restore<GtProcessComponent*>(
                                     gtProcessFactory);

    if (newObj == Q_NULLPTR)
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
    if (m_processData == Q_NULLPTR)
    {
        return;
    }

    GtProject* project = m_processData->findParent<GtProject*>();

    if (project == Q_NULLPTR)
    {
        gtError() << "project == Q_NULLPTR";
        return;
    }

    GtTaskProvider provider;
    GtProcessWizard wizard(project, &provider);
    wizard.resize(560, 500);

    if (!wizard.exec())
    {
        return;
    }

    GtObject* parentObj = Q_NULLPTR;

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

    if (parentObj == Q_NULLPTR)
    {
        return;
    }

    GtObjectMemento memento = provider.componentData();

    if (memento.isNull())
    {
        return;
    }

    GtProcessComponent* newObj = memento.restore<GtProcessComponent*>(
                                     gtProcessFactory);

    if (newObj == Q_NULLPTR)
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
    GtTask* retval = Q_NULLPTR;

    if (obj == Q_NULLPTR)
    {
        return retval;
    }

    retval = qobject_cast<GtTask*>(obj);

    if (retval == Q_NULLPTR)
    {
        // object not a task
        return findRootTaskHelper(obj->findParent<GtTask*>());
    }
    else
    {
        // object a task
        if (!qobject_cast<GtProcessData*>(obj->parent()))
        {
            return findRootTaskHelper(obj->findParent<GtTask*>());
        }
    }

    return retval;
}

bool
GtProcessDock::componentIsReady(GtProcessComponent* comp)
{
    if (comp == Q_NULLPTR)
    {
        return false;
    }

    return comp->isReady();
}

void
GtProcessDock::filterData(const QString& val)
{
    m_filterModel->setFilterRegExp(val);

    if (m_rootIndex.isValid())
    {
        return;
    }

    m_view->setModel(NULL);

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
                    SIGNAL(currentChanged(QModelIndex, QModelIndex)),
                    SLOT(onCurrentChanged(QModelIndex, QModelIndex)),
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

    if (qobject_cast<GtCalculator*>(obj))
    {
        m_addElementButton->setEnabled(false);
    }

    updateRunButton();
}

void
GtProcessDock::updateRunButton()
{
    if (m_currentProcess == Q_NULLPTR || m_currentProcess->hasDummyChildren())
    {
        m_runButton->setEnabled(false);
        m_runButton->setText(tr("Run"));
        return;
    }

    const QString& str = m_currentProcess->objectName();

    GtTask* current = gtProcessExecutor->currentRunningTask();

    if (current != Q_NULLPTR && current != m_currentProcess)
    {
        m_runButton->setIcon(gtApp->icon("queueIcon_16.png"));

        m_runButton->setStyleSheet(
                    GtStyleSheets::processRunButton("rgb(236,219,184)"));

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
    else
    {
        m_runButton->setIcon(gtApp->icon("runProcessIcon_16.png"));

        m_runButton->setStyleSheet(
                    GtStyleSheets::processRunButton("rgb(230,255,230)"));

        if (str.isEmpty())
        {
            m_runButton->setEnabled(false);
            m_runButton->setToolTip(tr("No Process selected"));
            m_runButton->setText(tr("Run"));
        }
        else
        {
            if (m_currentProcess == current)
            {

                m_runButton->setStyleSheet(GtStyleSheets::processRunButton(
                                               "rgb(255,230,230)"));

                if (m_currentProcess->currentState() ==
                        GtProcessComponent::TERMINATION_REQUESTED)
                {
                    m_runButton->setIcon(gtApp->icon("stopRequestIcon_16.png"));
                    m_runButton->setEnabled(false);
                    m_runButton->setToolTip(tr("Terminating process..."));
                    m_runButton->setText(str + tr(" terminating..."));
                }
                else
                {
                    m_runButton->setIcon(gtApp->icon("stopIcon_16.png"));
                    m_runButton->setEnabled(true);
                    m_runButton->setToolTip(tr("Stop Selected Process"));
                    m_runButton->setText(tr("Stop") + " (" + str + ")");
                }
            }
            else
            {
                m_runButton->setEnabled(true);
                m_runButton->setToolTip(tr("Run Selected Process"));
                m_runButton->setText(tr("Run") + " (" + str + ")");
            }
        }
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

    if (GtTask* task = qobject_cast<GtTask*>(obj))
    {
        //        gtMdiLauncher->open(GtProcessEditor::staticMetaObject.className(),
        //                            task);
        if (!task->isReady())
        {
            return;
        }

        configTask(task);
    }
    else if (GtCalculator* calc = qobject_cast<GtCalculator*>(obj))
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

        if (obj != Q_NULLPTR)
        {
            GtTask* task = findRootTaskHelper(obj);

            if (task != Q_NULLPTR)
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
    if (m_currentProcess == Q_NULLPTR)
    {
        return;
    }

    QMessageBox mb;
    mb.setIcon(QMessageBox::Question);
    mb.setWindowTitle(tr("Stop Running Task"));
    mb.setWindowIcon(gtApp->icon("stopIcon_16.png"));
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
    addemptytask->setIcon(gtApp->icon(QStringLiteral("addProcessIcon_16.png")));

    if (!srcIndex.isValid())
    {
        menu.addSeparator();
    }

    QAction* addtask = menu.addAction(tr("New Task..."));
    addtask->setIcon(gtApp->icon(QStringLiteral("addProcessIcon_16.png")));

    QAction* addcalc = menu.addAction(tr("New Calculator..."));
    addcalc->setIcon(gtApp->icon(QStringLiteral("calculatorIcon_16.png")));

    if (!srcIndex.isValid())
    {
        addcalc->setVisible(false);
    }
    else
    {
        addemptytask->setVisible(false);

        GtObject* obj = gtDataModel->objectFromIndex(srcIndex);

        if (obj == Q_NULLPTR)
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
GtProcessDock::customContextMenu(const QPoint& pos)
{
    if (m_view->model() == Q_NULLPTR)
    {
        return;
    }

    QModelIndex index = m_view->indexAt(pos);
    QModelIndex srcIndex = mapToSource(index);

    if (srcIndex.isValid())
    {
        if (m_view->selectionModel()->selectedIndexes().size() < 3)
        {
            GtObject* obj = gtDataModel->objectFromIndex(srcIndex);

            if (obj == Q_NULLPTR)
            {
                return;
            }

            if (GtTask* p = qobject_cast<GtTask*>(obj))
            {
                processContextMenu(p, srcIndex);
            }
            else if (GtCalculator* c = qobject_cast<GtCalculator*>(obj))
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

        QMenu* addMenu = menu.addMenu(gtApp->icon("addIcon_16.png"),
                                      tr("Add..."));

        QAction* addemptytask = addMenu->addAction(tr("Empty Task"));
        addemptytask->setIcon(
                    gtApp->icon(QStringLiteral("addProcessIcon_16.png")));

        addMenu->addSeparator();

        QAction* addtask = addMenu->addAction(tr("New Task..."));
        addtask->setIcon(gtApp->icon(QStringLiteral("addProcessIcon_16.png")));

        if (!gtApp->settings()->lastProcessElements().isEmpty())
        {
            addMenu->addSeparator();
            generateLastUsedElementMenu(addMenu, true);
        }

        menu.addSeparator();

        GtImportMenu* imenu = new GtImportMenu(m_processData, &menu);

        menu.addMenu(imenu);

        menu.addSeparator();

        QAction* actpaste = menu.addAction("Paste");
        actpaste->setIcon(gtApp->icon("pasteIcon_16.png"));
        actpaste->setEnabled(false);

        QClipboard* clipboard = QApplication::clipboard();
        const QMimeData* mimeData = clipboard->mimeData();

        if ((mimeData != Q_NULLPTR) &&
                (mimeData->hasFormat(QStringLiteral("GtObject"))))
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
GtProcessDock::processContextMenu(GtTask* obj, const QModelIndex& index)
{
    QMenu menu(this);
    QAction* actrun = menu.addAction(tr("Run Task"));
    QAction* actstop = menu.addAction(tr("Stop Task"));

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

    QAction* actmon = menu.addAction(tr("Open Monitoring Table"));
    actmon->setIcon(gtApp->icon(QStringLiteral("monitoringIcon.png")));

    if (!gtApp->devMode())
    {
        actmon->setVisible(false);
    }

    menu.addSeparator();

    QAction* actconfig = menu.addAction("Config...");
    actconfig->setIcon(gtApp->icon("configIcon_16.png"));

    QAction* actconnect = menu.addAction(tr("Connection Editor"));
    actconnect->setIcon(gtApp->icon(QStringLiteral("connectionIcon_16.png")));

    menu.addSeparator();

    QMenu* addMenu = menu.addMenu(gtApp->icon("addIcon_16.png"), tr("Add..."));

    QAction* addtask = addMenu->addAction(tr("New Task..."));
    addtask->setIcon(gtApp->icon(QStringLiteral("addElementIcon_16.png")));

    QAction* addcalc = addMenu->addAction(tr("New Calculator..."));
    addcalc->setIcon(gtApp->icon(QStringLiteral("calculatorIcon_16.png")));

    if (!gtApp->settings()->lastProcessElements().isEmpty())
    {
        addMenu->addSeparator();
        generateLastUsedElementMenu(addMenu, false);
    }

    menu.addSeparator();

    actrun->setIcon(gtApp->icon("runProcessIcon_16.png"));
    actstop->setIcon(gtApp->icon("stopIcon_16.png"));
    //    QAction* actglobals = menu.addAction("Globals");
    //    actglobals->setIcon(gtApp->icon("globalsIcon_16.png"));
    QAction* actrename = menu.addAction("Rename");
    actrename->setIcon(gtApp->icon("inputIcon_16.png"));

    menu.addSeparator();

    // importer menu
    QList<GtImporterMetaData> importerList =
            gtImportHandler->importerMetaData(obj->metaObject()->className());

    if (!importerList.isEmpty())
    {
        GtImportMenu* imenu = new GtImportMenu(obj, &menu);

        menu.addMenu(imenu);

        menu.addSeparator();
    }

    // exporter menu
    QList<GtExporterMetaData> exporterList =
            gtExportHandler->exporterMetaData(obj->metaObject()->className());

    if (!exporterList.isEmpty())
    {
        GtExportMenu* emenu = new GtExportMenu(obj, &menu);

        menu.addMenu(emenu);

        menu.addSeparator();
    }

    QAction* actclone = menu.addAction("Clone");
    actclone->setIcon(gtApp->icon("cloneIcon_16.png"));

    QAction* actcut = menu.addAction("Cut");
    actcut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
    actcut->setIcon(gtApp->icon("cutIcon_16.png"));

    QAction* actcopy = menu.addAction("Copy");
    actcopy->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    actcopy->setIcon(gtApp->icon("copyIcon_16.png"));
    QAction* actpaste = menu.addAction("Paste");
    actpaste->setIcon(gtApp->icon("pasteIcon_16.png"));
    actpaste->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
    actpaste->setEnabled(false);
    menu.addSeparator();
    QAction* actdelete = menu.addAction("Delete");
    actdelete->setIcon(gtApp->icon("closeIcon_16.png"));
    actdelete->setShortcut(QKeySequence(Qt::Key_Delete));

    QClipboard* clipboard = QApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();

    if ((mimeData != Q_NULLPTR) &&
            (mimeData->hasFormat(QStringLiteral("GtObject"))))
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

    if (!componentIsReady(obj))
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
        //        deleteElement(mapFromSource(index));
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
    else if (a == actmon)
    {
        gtMdiLauncher->open(GT_CLASSNAME(GtProcessMonitoring), obj);
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
    QMenu menu(this);
    QAction* actconfig = menu.addAction("Config...");
    actconfig->setIcon(gtApp->icon("configIcon_16.png"));

    QAction* actconnect = menu.addAction(tr("Connection Editor"));
    actconnect->setIcon(gtApp->icon(QStringLiteral("connectionIcon_16.png")));

    menu.addSeparator();

    QAction* actskip = menu.addAction("Skip");
    actskip->setIcon(gtApp->icon("skipIcon_16.png"));

    QAction* actunskip = menu.addAction("Unskip");
    actunskip->setIcon(gtApp->icon("arrowrightIcon.png"));

    if (!obj->isSkipped())
    {
        actunskip->setVisible(false);
    }
    else
    {
        actskip->setVisible(false);
    }

    QAction* actrename = menu.addAction("Rename");
    actrename->setIcon(gtApp->icon("inputIcon_16.png"));

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
        GtImportMenu* imenu = new GtImportMenu(obj, &menu);

        menu.addMenu(imenu);

        menu.addSeparator();
    }

    // exporter menu
    QList<GtExporterMetaData> exporterList =
            gtExportHandler->exporterMetaData(obj->metaObject()->className());

    if (!exporterList.isEmpty())
    {
        GtExportMenu* emenu = new GtExportMenu(obj, &menu);

        menu.addMenu(emenu);

        menu.addSeparator();
    }

    QAction* actclone = menu.addAction("Clone");
    actclone->setIcon(gtApp->icon("cloneIcon_16.png"));

    QAction* actcut = menu.addAction("Cut");
    actcut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
    actcut->setIcon(gtApp->icon("cutIcon_16.png"));

    QAction* actcopy = menu.addAction("Copy");
    actcopy->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    actcopy->setIcon(gtApp->icon("copyIcon_16.png"));

    QAction* actpaste = menu.addAction("Paste");
    actpaste->setIcon(gtApp->icon("pasteIcon_16.png"));
    actpaste->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
    actpaste->setEnabled(false);

    menu.addSeparator();

    QAction* actdelete = menu.addAction("Delete");
    actdelete->setIcon(gtApp->icon("closeIcon_16.png"));
    actdelete->setShortcut(QKeySequence(Qt::Key_Delete));

    if (!componentIsReady(obj))
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
        //        deleteElement(mapFromSource(index));
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
        skipCalculator(mapFromSource(index));
    }
    else if (a == actunskip)
    {
        skipCalculator(mapFromSource(index), false);
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
GtProcessDock::multiSelectionContextMenu(QList<QModelIndex> indexList)
{
    QMenu menu(this);

    QAction* skipCalcs = menu.addAction("Skip Selected Calculators");
    skipCalcs->setIcon(gtApp->icon("skipIcon_16.png"));

    QAction* unskipCalcs = menu.addAction("Unskip Selected Calculators");
    unskipCalcs->setIcon(gtApp->icon("arrowrightIcon.png"));

    QAction* deleteElements = menu.addAction("Delete Process Elements");
    deleteElements->setIcon(gtApp->icon("closeIcon_16.png"));

    bool allSkipped = true;
    bool allUnskipped = true;

    foreach (QModelIndex index, indexList)
    {
        GtCalculator* calc = calcByModelIndex(index);

        if (calc != Q_NULLPTR)
        {
            if (!calc->isSkipped())
            {
                allSkipped = false;
            }

            if (calc->isSkipped())
            {
                allUnskipped = false;
            }
        }
    }

    if (allSkipped)
    {
        skipCalcs->setVisible(false);
    }
    else if (allUnskipped)
    {
        unskipCalcs->setVisible(false);
    }

    QAction* a = menu.exec(QCursor::pos());

    if (a == skipCalcs)
    {
        skipCalculator(indexList);
    }
    else if (a == unskipCalcs)
    {
        skipCalculator(indexList, false);
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

    if (obj == Q_NULLPTR)
    {
        return;
    }

    GtProcessComponent* pComp = qobject_cast<GtProcessComponent*>(obj);

    if (pComp == Q_NULLPTR)
    {
        return;
    }

    if (!pComp->isReady())
    {
        return;
    }

    GtObject* copy = obj->copy();

    GtTask* origTask = qobject_cast<GtTask*>(pComp);
    GtTask* newTask = qobject_cast<GtTask*>(copy);

    if (newTask != Q_NULLPTR && origTask != Q_NULLPTR)
    {
        if (qobject_cast<GtProcessData*>(origTask->parent()) != Q_NULLPTR)
        {
            mapPropertyConnections(origTask, newTask);
        }
        else
        {
            GtTask* origTaskClone = qobject_cast<GtTask*>(origTask->clone());

            if (origTaskClone == Q_NULLPTR)
            {
                return;
            }

            QList<GtPropertyConnection*> validPropCons =
                internalPropertyConnections(origTask);

            foreach (GtPropertyConnection* propCon, validPropCons)
            {
                GtObject* conClone = propCon->clone();
                GtObject* conCopy = propCon->copy();

                if (conClone != Q_NULLPTR && conCopy != Q_NULLPTR)
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

    if (obj == Q_NULLPTR)
    {
        return;
    }

    GtProcessComponent* pComp = qobject_cast<GtProcessComponent*>(obj);

    if (pComp == Q_NULLPTR)
    {
        return;
    }

    if (!pComp->isReady())
    {
        return;
    }

    GtObject* parent = obj->parentObject();

    if (parent == Q_NULLPTR)
    {
        return;
    }

    GtObject* cloned = obj->copy();

    // check cloned object
    if (cloned == Q_NULLPTR)
    {
        return;
    }

    GtTask* origTask = qobject_cast<GtTask*>(obj);
    GtTask* newTask = qobject_cast<GtTask*>(cloned);

    if (newTask != Q_NULLPTR && origTask != Q_NULLPTR)
    {
        if (qobject_cast<GtProcessData*>(origTask->parent()) != Q_NULLPTR)
        {
            mapPropertyConnections(origTask, newTask);
        }
        else
        {
            GtTask* origTaskClone =
                qobject_cast<GtTask*>(origTask->clone());

            if (origTaskClone == Q_NULLPTR)
            {
                return;
            }

            QList<GtPropertyConnection*> validPropCons =
                internalPropertyConnections(origTask);

            foreach (GtPropertyConnection* propCon, validPropCons)
            {
                GtObject* conClone = propCon->clone();
                GtObject* conCopy = propCon->copy();

                if (conClone != Q_NULLPTR && conCopy != Q_NULLPTR)
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

    if (obj == Q_NULLPTR)
    {
        return;
    }

    GtProcessComponent* pComp = qobject_cast<GtProcessComponent*>(obj);

    if (pComp == Q_NULLPTR)
    {
        return;
    }

    if (!pComp->isReady())
    {
        return;
    }

    if (m_processData == Q_NULLPTR)
    {
        return;
    }

    copyElement(index);

    QList<GtObject*> toDelete;

    if (qobject_cast<GtTask*>(pComp) != Q_NULLPTR)
    {
        if (qobject_cast<GtProcessData*>(pComp->parent()) == Q_NULLPTR)
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

        if (obj == Q_NULLPTR)
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

    if (obj == Q_NULLPTR)
    {
        gtError() << tr("Could not restore object from clipboard!");
        return;
    }

    if (!qobject_cast<GtProcessData*>(parent))
    {
        GtProcessComponent* pComp = qobject_cast<GtProcessComponent*>(parent);

        if (pComp == Q_NULLPTR)
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
    if (obj == Q_NULLPTR || parent == Q_NULLPTR)
    {
        return;
    }

    if (qobject_cast<GtCalculator*>(parent))
    {
        return;
    }

    if (!qobject_cast<GtProcessData*>(parent))
    {
        GtProcessComponent* pComp = qobject_cast<GtProcessComponent*>(parent);

        if (pComp == Q_NULLPTR)
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
        propCon->setParent(Q_NULLPTR);
    }

    QModelIndex srcIndex = gtDataModel->appendChild(obj, parent);

    GtTask* task = qobject_cast<GtTask*>(obj);

    if (task != Q_NULLPTR)
    {
        GtTask* highestParent = highestParentTask(task);

        if (highestParent != Q_NULLPTR)
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

    if (parent == Q_NULLPTR)
    {
        return;
    }

    pasteElement(parent);
}

void
GtProcessDock::skipCalculator(const QModelIndex& index, bool skip)
{
    QString msg;

    if (skip == true)
    {
        msg = "Skip";
    }
    else
    {
        msg = "Unskip";
    }

    msg.append(" Selected Calculator");

    GtCommand cmd = gtApp->startCommand(gtApp->currentProject(), msg);
    skipCalculator(calcByModelIndex(index), skip);

    gtApp->endCommand(cmd);
}

void
GtProcessDock::skipCalculator(const QList<QModelIndex>& indexList, bool skip)
{
    QList<GtCalculator*> calcs;

    foreach (QModelIndex index, indexList)
    {
        GtCalculator* calc = calcByModelIndex(index);

        if (calc == Q_NULLPTR)
        {
            continue;
        }

        if (!calcs.contains(calc))
        {
            calcs.append(calc);
        }
    }

    QString msg;

    if (skip == true)
    {
        msg = "Skip";
    }
    else
    {
        msg = "Unskip";
    }

    msg.append(" Selected Calculators");

    GtCommand cmd = gtApp->startCommand(gtApp->currentProject(), msg);
    foreach (GtCalculator* calc, calcs)
    {
        skipCalculator(calc, skip);
    }

    gtApp->endCommand(cmd);
}

void
GtProcessDock::skipCalculator(GtCalculator* calc, bool skip)
{
    if (calc == Q_NULLPTR)
    {
        return;
    }

    calc->setSkipped(skip);
}

void
GtProcessDock::configCalculator(GtCalculator* calc)
{
    if (calc == Q_NULLPTR)
    {
        return;
    }

    GtProject* project = calc->findParent<GtProject*>();

    if (project == Q_NULLPTR)
    {
        return;
    }

    //    gtDebug() << "config calculator " << calc->metaObject()->className() <<
    //                 "...";

    GtCalculatorProvider provider(calc);
    GtProcessWizard wizard(project, &provider);

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
    if (task == Q_NULLPTR)
    {
        return;
    }

    if (!gtTaskFactory->taskDataExists(task->metaObject()->className()))
    {
        return;
    }

    GtProject* project = gtApp->currentProject();

    if (project == Q_NULLPTR)
    {
        return;
    }

    GtTaskProvider provider(task);
    GtProcessWizard wizard(project, &provider);

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
        return QModelIndex();
    }

    QModelIndex tmp1 = m_filterModel->mapToSource(index);

    return m_model->mapToSource(tmp1);
}

QModelIndex
GtProcessDock::mapFromSource(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return QModelIndex();
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
    if (obj == Q_NULLPTR)
    {
        return;
    }

    // cast object to process component
    GtProcessComponent* comp = qobject_cast<GtProcessComponent*>(obj);

    // check casted process component
    if (comp == Q_NULLPTR)
    {
        return;
    }

    // get root task
    GtTask* rootTask = comp->rootTask();

    // check root task
    if (rootTask == Q_NULLPTR)
    {
        return;
    }

    GtProject* project = rootTask->findParent<GtProject*>();

    if (project == Q_NULLPTR)
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
    if (gtApp->currentProject() == Q_NULLPTR)
    {
        return;
    }

    QAction* act = qobject_cast<QAction*>(obj);

    if (act == Q_NULLPTR)
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

        if (newObj == Q_NULLPTR)
        {
            return;
        }

        GtTask* task = qobject_cast<GtTask*>(newObj);

        if (task == Q_NULLPTR)
        {
            delete newObj;
            return;
        }

        task->setFactory(gtProcessFactory);

        GtTaskProvider provider(task);
        GtProcessWizard wizard(gtApp->currentProject(), &provider);
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

        GtObject* parentObj = Q_NULLPTR;

        if (srcIndex.isValid())
        {
            parentObj = gtDataModel->objectFromIndex(srcIndex);
        }
        else
        {
            parentObj = m_processData;
        }

        if (parentObj == Q_NULLPTR)
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

        if (currentObj == Q_NULLPTR)
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

        if (newObj == Q_NULLPTR)
        {
            return;
        }

        GtCalculator* calc = qobject_cast<GtCalculator*>(newObj);

        if (calc == Q_NULLPTR)
        {
            delete newObj;
            return;
        }

        calc->setFactory(gtProcessFactory);

        GtCalculatorProvider provider(calc);
        GtProcessWizard wizard(gtApp->currentProject(), &provider);
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

        if (newCon == Q_NULLPTR)
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

bool
GtProcessDock::mapRelativeObjectLink(GtObject* orig, GtObject* copy,
                                     GtObject* copyClone, GtObject* obj,
                                     GtRelativeObjectLinkProperty* relLink)
{
    if (orig == Q_NULLPTR || copy == Q_NULLPTR || copyClone == Q_NULLPTR)
    {
        return false;
    }

    if (copy->uuid() != copyClone->uuid())
    {
        return false;
    }

    if (orig->uuid() == copy->uuid())
    {
        return false;
    }

    QString origTargetUUID = relLink->getVal();
    if (origTargetUUID.isEmpty())
    {
        return true;
    }

    GtObject* origTarget = orig->getObjectByUuid(origTargetUUID);

    if (origTarget == Q_NULLPTR)
    {
        gtDebug() << "Could not identify target object...";
        return false;
    }

    GtObject* targetCopyClone =
            findEquivalentObject(copyClone, origTarget);

    if (targetCopyClone == Q_NULLPTR)
    {
        gtDebug() << "Could not find equivalent target object...";
        return false;
    }

    GtObject* objCopyClone =  findEquivalentObject(copyClone, obj);

    if (objCopyClone == Q_NULLPTR)
    {
        gtDebug() << "Could not find equivalent object...";
        return false;
    }

    GtObject* objCopy = copy->getObjectByUuid(objCopyClone->uuid());

    GtObject* targetCopy = copy->getObjectByUuid(
                targetCopyClone->uuid());

    if (objCopy == Q_NULLPTR || targetCopy == Q_NULLPTR)
    {
        return false;
    }

    GtAbstractProperty* prop =
            objCopy->findProperty(relLink->ident());

    if (prop == Q_NULLPTR)
    {
        gtDebug() << "Could not find property";
        return false;
    }

    GtRelativeObjectLinkProperty*  newRelLink =
            qobject_cast<GtRelativeObjectLinkProperty*>(prop);

    if (newRelLink == Q_NULLPTR)
    {
        gtDebug() << "Is no relative obj Link!";
        return false;
    }

    newRelLink->setVal(targetCopy->uuid());

    return true;
}

GtPropertyConnection*
GtProcessDock::findConnectionCopy(GtPropertyConnection* origCon,
                                  QList<GtPropertyConnection*> newCons)
{
    if (origCon == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    if (newCons.isEmpty())
    {
        return Q_NULLPTR;
    }

    foreach (GtPropertyConnection* newCon, newCons)
    {
        bool check = true;

        if (newCon->sourceUuid() != origCon->sourceUuid())
        {
            check = false;
        }
        else if (newCon->targetUuid() != origCon->targetUuid())
        {
            check = false;
        }
        else if (newCon->sourceProp() != origCon->sourceProp())
        {
            check = false;
        }
        else if (newCon->targetProp() != origCon->targetProp())
        {
            check = false;
        }

        if (check)
        {
            return newCon;
        }
    }

    return Q_NULLPTR;
}

bool
GtProcessDock::updateConnectionProperties(GtPropertyConnection* origCon,
        GtPropertyConnection* copyCon,
        GtTask* orig, GtTask* copy)
{
    if (origCon == Q_NULLPTR || copyCon == Q_NULLPTR)
    {
        return false;
    }

    if (orig == Q_NULLPTR || copy == Q_NULLPTR)
    {
        return false;
    }

    GtObject* origSourceObj = orig->getObjectByUuid(origCon->sourceUuid());
    GtObject* origTargetObj = orig->getObjectByUuid(origCon->targetUuid());

    if (origSourceObj == Q_NULLPTR || origTargetObj == Q_NULLPTR)
    {
        gtError() << "Could not find original objects...";
        return false;
    }

    GtObject* newSourceObj = findEquivalentObject(copy, origSourceObj);
    GtObject* newTargetObj = findEquivalentObject(copy, origTargetObj);

    if (newSourceObj == Q_NULLPTR || newTargetObj == Q_NULLPTR)
    {
        gtError() << "Could not find new objects...";
        return false;
    }

    copyCon->setSourceUuid(newSourceObj->uuid());
    copyCon->setTargetUuid(newTargetObj->uuid());

    gtDebug() << "Connection updated successfully!";

    return true;
}

bool
GtProcessDock::updateRelativeObjectLinks(GtObject* orig, GtObject* copy)
{
    if (orig == Q_NULLPTR || copy == Q_NULLPTR)
    {
        return false;
    }

    QList<GtObject*> origObjects = orig->findChildren<GtObject*>();
    origObjects.append(orig);

    GtObject* copyClone = copy->clone();

    if (copyClone == Q_NULLPTR)
    {
        return false;
    }

    if (copyClone->uuid() != copy->uuid())
    {
        return false;
    }

    foreach (GtObject* obj, origObjects)
    {
        QList<GtRelativeObjectLinkProperty*> relLinks =
            obj->propertiesByType<GtRelativeObjectLinkProperty*>();

        foreach (GtRelativeObjectLinkProperty* relLink, relLinks)
        {
            if (!mapRelativeObjectLink(orig, copy, copyClone, obj, relLink))
            {
                gtDebug() << "Failed to update relative object link!";
                continue;
            }
            gtDebug() << "Successfully Updated Relative Object Link!";
        }
    }

    delete copyClone;

    return true;
}

GtObject*
GtProcessDock::findEquivalentObject(GtObject* parent, GtObject* origObj)
{
    if (parent == Q_NULLPTR || origObj == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    QString name = origObj->objectName();
    QString className = origObj->toMemento().className();

    QList<GtObject*> objects = parent->findChildren<GtObject*>();
    objects.append(parent);

    foreach (GtObject* obj, objects)
    {
        QString nameTmp = obj->objectName();
        QString classNameTmp = obj->toMemento().className();

        if (nameTmp == name && classNameTmp == className)
        {
            QList<GtAbstractProperty*> props = origObj->properties();

            bool cont = false;

            foreach (GtAbstractProperty* absProp, props)
            {
                GtAbstractProperty* newProp =
                    obj->findProperty(absProp->ident());

                if (newProp == Q_NULLPTR)
                {
                    continue;
                }

                if (absProp->valueToVariant() != newProp->valueToVariant())
                {
                    cont = true;
                    break;
                }
            }

            if (cont)
            {
                continue;
            }

            GtObject* origPar = qobject_cast<GtObject*>(origObj->parent());
            GtObject* parent = qobject_cast<GtObject*>(obj->parent());

            if (origPar == Q_NULLPTR && parent == Q_NULLPTR)
            {
                return obj;
            }

            if (origPar->toMemento().className() !=
                    parent->toMemento().className())
            {
                gtDebug() << "Parent class names are different!";
                continue;
            }

            int children =  origPar->childCount<GtObject*>();
            int childrenTmp = parent->childCount<GtObject*>();

            if (children != childrenTmp)
            {
                gtDebug() << "Number of direct children are different!";
                continue;
            }

            children = origPar->findChildren<GtObject*>().size();
            childrenTmp = parent->findChildren<GtObject*>().size();

            if (children != childrenTmp)
            {
                gtDebug() << "Number of children different!";
                continue;
            }

            return obj;
        }
    }

    return Q_NULLPTR;
}

GtTask*
GtProcessDock::highestParentTask(GtTask* childTask)
{
    if (childTask == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    if (childTask->parent() == Q_NULLPTR)
    {
        return childTask;
    }

    if (qobject_cast<GtProcessData*>(childTask->parent()) != Q_NULLPTR)
    {
        return childTask;
    }
    else
    {
        GtTask* parent = qobject_cast<GtTask*>(childTask->parent());

        if (parent != Q_NULLPTR)
        {
            return highestParentTask(parent);
        }
    }

    return Q_NULLPTR;
}

GtTask*
GtProcessDock::highestParentTask(GtCalculator* childCalc)
{
    if (childCalc == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    if (childCalc->parent() == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    GtTask* task = qobject_cast<GtTask*>(childCalc->parent());

    if (task != Q_NULLPTR)
    {
        return highestParentTask(task);
    }

    return Q_NULLPTR;
}

GtTask*
GtProcessDock::highestParentTask(GtProcessComponent* childComp)
{
    if (childComp == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    if (GtTask* task = qobject_cast<GtTask*>(childComp))
    {
        return highestParentTask(task);
    }

    if (GtCalculator* calc = qobject_cast<GtCalculator*>(childComp))
    {
        return highestParentTask(calc);
    }

    return Q_NULLPTR;
}

QList<GtPropertyConnection*>
GtProcessDock::internalPropertyConnections(GtTask* task)
{
    QList<GtPropertyConnection*> retVal;

    if (task == Q_NULLPTR)
    {
        return retVal;
    }

    GtTask* highestParent = highestParentTask(task);

    if (highestParent == Q_NULLPTR)
    {
        return retVal;
    }

    QList<GtPropertyConnection*> allPropCons =
        highestParent->findChildren<GtPropertyConnection*>();

    foreach (GtPropertyConnection* propCon, allPropCons)
    {
        GtObject* sourceObj =
            highestParent->getObjectByUuid(propCon->sourceUuid());
        GtObject* targetObj =
            highestParent->getObjectByUuid(propCon->targetUuid());

        if (sourceObj == Q_NULLPTR || targetObj == Q_NULLPTR)
        {
            continue;
        }

        QList<GtObject*> allChildren = task->findChildren<GtObject*>();

        if (allChildren.contains(sourceObj) && allChildren.contains(targetObj))
        {
            retVal.append(propCon);
        }
        else if (sourceObj == qobject_cast<GtObject*>(task) &&
                 allChildren.contains(targetObj))
        {
            retVal.append(propCon);
        }
        else if (targetObj == qobject_cast<GtObject*>(task) &&
                 allChildren.contains(sourceObj))
        {
            retVal.append(propCon);
        }
    }

    return retVal;
}

QList<GtPropertyConnection*>
GtProcessDock::internalPropertyConnections(GtProcessComponent* pComp)
{
    QList<GtPropertyConnection*> retVal;

    if (qobject_cast<GtCalculator*>(pComp))
    {
        return retVal;
    }
    else if (GtTask* task = qobject_cast<GtTask*>(pComp))
    {
        retVal = internalPropertyConnections(task);
    }

    return retVal;
}

QList<GtPropertyConnection*>
GtProcessDock::lostPropertyConnections(GtTask* task)
{
    QList<GtPropertyConnection*> retVal;

    if (task == Q_NULLPTR)
    {
        return retVal;
    }

    GtTask* highestParent = highestParentTask(task);

    if (highestParent == Q_NULLPTR)
    {
        return retVal;
    }

    QList<GtPropertyConnection*> allPropCons =
        highestParent->findChildren<GtPropertyConnection*>();

    foreach (GtPropertyConnection* propCon, allPropCons)
    {
        GtObject* sourceObj =
            highestParent->getObjectByUuid(propCon->sourceUuid());
        GtObject* targetObj =
            highestParent->getObjectByUuid(propCon->targetUuid());

        if (sourceObj == Q_NULLPTR || targetObj == Q_NULLPTR)
        {
            continue;
        }

        QList<GtObject*> allChildren = task->findChildren<GtObject*>();

        if (allChildren.contains(sourceObj) &&
                !allChildren.contains(targetObj))
        {
            if (qobject_cast<GtObject*>(task) != targetObj)
            {
                retVal.append(propCon);
            }
        }
        else if (!allChildren.contains(sourceObj) &&
                 allChildren.contains(targetObj))
        {
            if (qobject_cast<GtObject*>(task) != sourceObj)
            {
                retVal.append(propCon);
            }
        }
    }

    return retVal;
}

QList<GtPropertyConnection*>
GtProcessDock::lostPropertyConnections(GtCalculator* calc)
{
    return relatedPropertyConnections(calc);
}

QList<GtPropertyConnection*>
GtProcessDock::lostPropertyConnections(GtProcessComponent* pComp)
{
    if (GtTask* task = qobject_cast<GtTask*>(pComp))
    {
        return lostPropertyConnections(task);
    }
    else if (GtCalculator* calc = qobject_cast<GtCalculator*>(pComp))
    {
        return lostPropertyConnections(calc);
    }

    return QList<GtPropertyConnection*>();
}

QList<GtPropertyConnection*>
GtProcessDock::relatedPropertyConnections(GtTask* task)
{
    QList<GtPropertyConnection*> retVal;

    foreach (GtPropertyConnection* propCon,
             internalPropertyConnections(task))
    {
        retVal.append(propCon);
    }

    foreach (GtPropertyConnection* propCon,
             lostPropertyConnections(task))
    {
        retVal.append(propCon);
    }

    return retVal;
}

QList<GtPropertyConnection*>
GtProcessDock::relatedPropertyConnections(GtCalculator* calc)
{
    QList<GtPropertyConnection*> retVal;

    if (calc == Q_NULLPTR)
    {
        return retVal;
    }

    GtTask* highestParent = highestParentTask(calc);

    if (highestParent == Q_NULLPTR)
    {
        return retVal;
    }

    QList<GtPropertyConnection*> allPropCons =
        highestParent->findChildren<GtPropertyConnection*>();

    foreach (GtPropertyConnection* propCon, allPropCons)
    {
        GtObject* sourceObj =
            highestParent->getObjectByUuid(propCon->sourceUuid());

        GtObject* targetObj =
            highestParent->getObjectByUuid(propCon->targetUuid());

        if (sourceObj == Q_NULLPTR || targetObj == Q_NULLPTR)
        {
            continue;
        }

        if (calc == sourceObj || calc == targetObj)
        {
            retVal.append(propCon);
        }
    }

    return retVal;
}

QList<GtPropertyConnection*>
GtProcessDock::relatedPropertyConnections(GtProcessComponent* comp)
{
    if (GtTask* task = qobject_cast<GtTask*>(comp))
    {
        return relatedPropertyConnections(task);
    }
    else if (GtCalculator* calc = qobject_cast<GtCalculator*>(comp))
    {
        return relatedPropertyConnections(calc);
    }

    return QList<GtPropertyConnection*>();
}

void
GtProcessDock::setOffLostConnectionWarnings(
    QList<GtPropertyConnection*> lostCons, GtTask* highestParent)
{
    foreach (GtPropertyConnection* propCon, lostCons)
    {
        GtObject* source = highestParent->getObjectByUuid(
                               propCon->sourceUuid());

        GtObject* target = highestParent->getObjectByUuid(
                               propCon->targetUuid());

        gtWarning() << "WARNING: Losing property connection!";

        GtTask* sourceParent = Q_NULLPTR;
        GtTask* targetParent = Q_NULLPTR;

        if (qobject_cast<GtCalculator*>(source) != Q_NULLPTR)
        {
            sourceParent = qobject_cast<GtTask*>(source->parentObject());
        }
        else
        {
            sourceParent = qobject_cast<GtTask*>(source);
        }

        if (qobject_cast<GtCalculator*>(target) != Q_NULLPTR)
        {
            targetParent = qobject_cast<GtTask*>(target->parentObject());
        }
        else
        {
            targetParent = qobject_cast<GtTask*>(target);
        }

        if (source != Q_NULLPTR && target != Q_NULLPTR)
        {
            if (sourceParent != Q_NULLPTR &&  targetParent != Q_NULLPTR)
            {
                gtWarning() << "Source:" << source->objectName()
                            << "in Parent Task:" << sourceParent->objectName();
                gtWarning() << "Target:" << target->objectName()
                            << "in Parent Task:" << targetParent->objectName();
            }
        }
    }
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
    if (menu == Q_NULLPTR)
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

                    GtExtendedTaskDataImpl* extendedData =
                            dynamic_cast<GtExtendedTaskDataImpl*>(
                                taskData.get());

                    if (extendedData == Q_NULLPTR ||
                        extendedData->icon.isNull())
                    {
                        QIcon icn;

                        GtObjectUI* oui = gtApp->objectUI(
                                    taskData->metaData().className());

                        if (oui != Q_NULLPTR)
                        {
                            icn = oui->icon(Q_NULLPTR);
                        }

                        if (icn.isNull())
                        {
                            icn = gtApp->icon(
                                        QStringLiteral("processIcon_16.png"));
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

                    GtExtendedCalculatorDataImpl* extendedData =
                            dynamic_cast<GtExtendedCalculatorDataImpl*>(
                                calcData.get());

                    if (extendedData == Q_NULLPTR ||
                        extendedData->icon.isNull())
                    {
                        act->setIcon(gtApp->icon(QStringLiteral(
                                                     "calculatorIcon_16.png")));
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
    GtProcessComponent* pComp = qobject_cast<GtProcessComponent*>(obj);

    if (pComp == Q_NULLPTR)
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
