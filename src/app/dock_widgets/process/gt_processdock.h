/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 02.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSDOCK_H
#define GTPROCESSDOCK_H

#include "gt_dockwidget.h"

#include <QPointer>
#include <QPersistentModelIndex>

#include "gt_command.h"

class QPushButton;
class QMenu;
class QSignalMapper;
class GtProcessView;
class GtProcessComponentModel;
class GtTreeFilterModel;
class GtSearchWidget;
class GtProcessData;
class GtTask;
class GtCalculator;
class GtObject;
class GtProcessComponent;
class GtPropertyConnection;
class GtRelativeObjectLinkProperty;
class GtCoreProcessExecutor;

/**
 * @brief The GtProcessDock class
 */
class GtProcessDock : public GtDockWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtProcessDock
     */
    Q_INVOKABLE GtProcessDock();

    /** Returns the recommended dock widget area in main windows.
        @return dock widget area */
    Qt::DockWidgetArea getDockWidgetArea() override;

    /**
     * @brief mapToSource
     * @param index
     * @return
     */
    QModelIndex mapToSource(const QModelIndex& index);

    /**
     * @brief mapFromSource
     * @param index
     * @return
     */
    QModelIndex mapFromSource(const QModelIndex& index);

    /**
     * @brief Returns pointer to process data object.
     * @return Process data object
     */
    GtProcessData* processData();

public slots:
    /**
     * @brief copies process element at index
     * @param index
     */
    void copyElement(const QModelIndex& index);

    /**
     * @brief renameElement open texteditor to rename the object which is
     * selected in the process view
     */
    void renameElement();

    /**
     * @brief clones process element at index
     * @param index
     */
    void cloneElement(const QModelIndex& index);

    /**
     * @brief cuts process element at index
     * @param index
     */
    void cutElement(const QModelIndex& index);

    /**
     * @brief deletes process elements at indexes
     * @param indexes
     */
    void deleteProcessElements(const QList<QModelIndex>& indexList);

    /**
     * @brief pastes process element at index
     * @param index
     */
    void pasteElement(GtObject* parent);

    /**
     * @brief pasteElement
     * @param obj
     * @param parent
     */
    void pasteElement(GtObject* obj, GtObject* parent);

    /**
     * @brief pasteElement
     * @param parentIndex
     */
    void pasteElement(const QModelIndex& parentIndex);

    /**
     * @brief sets skip status to component at index
     * @param index
     */
    void skipComponent(const QModelIndex& index, bool skip = true);

    /**
     * @brief sets skip status to components at indexes
     * @param indexlist
     */
    void skipComponent(const QList<QModelIndex>& indexList, bool skip = true);

    /**
     * @brief sets skip status to component
     * @param component
     */
    void skipComponent(GtProcessComponent* comp, bool skip = true);

protected:
    /** Event called after current project has changed.
        @param new current project */
    void projectChangedEvent(GtProject* project) override;

private:
    /// Button to run selected process
    QPushButton* m_runButton;

    /// Button to add a new element to process control environment
    QPushButton* m_addElementButton;

    /// Button to open the process queue
    QPushButton* m_processQueueButton;

    /// Tree view
    GtProcessView* m_view;

    /// model
    GtProcessComponentModel* m_model;

    /// model
    QPointer<GtTreeFilterModel> m_filterModel;

    /// Search widget
    GtSearchWidget* m_search;

    /// Root index
    QPersistentModelIndex m_rootIndex;

    /// Pointer to process data of current project
    QPointer<GtProcessData> m_processData;

    /// Pointer to current process
    QPointer<GtTask> m_currentProcess;

    /// Pointer to current project
    QPointer<GtProject> m_project;

    /// Command
    GtCommand m_command;

    /// mapper for action signals
    QSignalMapper* m_actionMapper;

    /**
     * @brief setCurrentProcess
     * @param process
     */
    void setCurrentProcess(GtTask* process = nullptr);

    /**
     * @brief addTask
     */
    void addEmptyTaskToRoot();

    /**
     * @brief addCalculator
     */
    void addCalculator();

    /**
     * @brief Opens task wizard to select new task.
     */
    void addTask();

    /**
     * @brief findRootTaskHelper
     * @param obj
     * @return
     */
    GtTask* findRootTaskHelper(GtObject* obj);

    /**
     * @brief isReady
     * @param comp
     * @return
     */
    bool componentIsReady(GtProcessComponent* comp);

    /**
     * @brief componentByModelIndex
     * @param index
     * @return
     */
    GtProcessComponent* componentByModelIndex(const QModelIndex& index);

    /**
     * @brief Updates property connections in a copied object based on the
     *  information contained in the original object
     * @param orig
     * @param copy
     * @return
     */
    bool mapPropertyConnections(GtTask* orig,
                                GtTask* copy);

    /**
     * @brief Updates list of last used process elements.
     * @param str Last used process element class name.
     */
    void updateLastUsedElementList(const QString& str);

    /**
     * @brief Generates menu based on list of last used process elements.
     * @param menu Parent menu.
     * @param isRoot Identifier whether element menu should be generated for a
     * root component.
     */
    void generateLastUsedElementMenu(QMenu* menu, bool isRoot);

    /**
     * @brief deleteProcessComponent
     * @param pComp
     */
    void deleteProcessComponent(GtObject* obj);

    /**
     * @brief keyPressEvent
     * @param event - key press event to handle
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief customContextMenu
     * @param srcIndex
     */
    void customContextMenu(QModelIndex const& srcIndex);

private slots:
    /**
     * @brief filterData
     * @param val
     */
    void filterData(const QString& val);

    /**
     * @brief updateButtons
     * @param obj
     */
    void updateButtons(GtObject* obj = nullptr);

    /**
     * @brief updateRunButton
     * @param str
     * @param process
     */
    void updateRunButton();

    /**
     * @brief onProcessDestroyed
     */
    void onProcessDestroyed();

    /**
     * @brief onCurrentChanged
     * @param current
     * @param previous
     */
    void onCurrentChanged(const QModelIndex& current,
                          const QModelIndex& previous = QModelIndex());

    /**
     * @brief onClicked
     * @param index
     */
    void onClicked(const QModelIndex& index);

    /**
     * @brief onDoubleClicked
     * @param index
     */
    void onDoubleClicked(const QModelIndex& index);

    /**
     * @brief runProcess
     */
    void runProcess();

    /**
     * @brief Terminates current task.
     */
    void terminateProcess();

    /**
     * @brief addElement
     */
    void addElement();

    /**
     * @brief customContextMenu
     * @param pos
     */
    void customContextMenu(const QPoint& pos);

    /**
     * @brief processContextMenu
     * @param obj
     */
    void processContextMenu(GtTask* obj, const QModelIndex& index);

    /**
     * @brief calculatorContextMenu
     * @param obj
     */
    void calculatorContextMenu(GtCalculator* obj, const QModelIndex& index);

    /**
     * @brief multiSelectionContextMenu
     * @param indexList
     */
    void multiSelectionContextMenu(const QList<QModelIndex>& indexList);

    /**
     * @brief Opens the calcualtor configuration wizard.
     * @param calc Calculator for configuration.
     */
    void configCalculator(GtCalculator* calc);

    /**
     * @brief Opens the task configuration wizard.
     * @param task Task for configuration.
     */
    void configTask(GtTask* task);

    /**
     * @brief resetModel
     */
    void resetModel();

    /**
     * @brief Opens a connection editor.
     * @param Model index.
     */
    void openConnectionEditor(const QModelIndex& index);

    /**
     * @brief onRowsAboutToBeMoved
     */
    void onRowsAboutToBeMoved();

    /**
     * @brief onRowsMoved
     */
    void onRowsMoved();

    /**
     * @brief Called after current task object has changes.
     * @param Changed object pointer.
     */
    void onTaskDataChanged(GtObject* obj);

    /**
     * @brief Called after action was triggered.
     * @param obj Triggered aciton
     */
    void actionTriggered(QObject* obj);

    /**
     * @brief Handles executor change
     * @param exec New Executor
     */
    void onExecutoChanged(GtCoreProcessExecutor* exec);

signals:
    /**
     * @brief selectedObjectChanged
     */
    void selectedObjectChanged(GtObject*);
};

#endif // GTPROCESSDOCK_H
