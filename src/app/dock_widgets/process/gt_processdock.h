/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 02.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSDOCK_H
#define GTPROCESSDOCK_H

#include "gt_dockwidget.h"
#include "gt_command.h"
#include "gt_taskgroup.h"

#include <QPointer>
#include <QPersistentModelIndex>

class QPushButton;
class QMenu;
class QComboBox;
class GtProcessView;
class GtProcessComponentModel;
class GtTreeFilterModel;
class GtSearchWidget;
class GtTaskGroupModel;
class GtTask;
class GtCalculator;
class GtObject;
class GtProcessComponent;
class GtPropertyConnection;
class GtRelativeObjectLinkProperty;
class GtCoreProcessExecutor;
class GtState;

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
    void skipComponents(const QList<QModelIndex>& indexList, bool skip = true);

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
    /// Combobox for task group selection
    QComboBox* m_taskGroupSelection;

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
    GtTaskGroupModel* m_taskGroupModel;

    /// model
    QPointer<GtTreeFilterModel> m_filterModel;

    /// Search widget
    GtSearchWidget* m_search;

    /// Pointer to selected task group of current project
    QPointer<GtTaskGroup> m_taskGroup;

    /// Pointer to current process
    QPointer<GtTask> m_currentProcess;

    /// Pointer to current project
    QPointer<GtProject> m_project;

    /// Command
    GtCommand m_command{};

    /// Expanded item UUIDs
    GtState* m_expandedItemUuidsState;

    /// Scope of the last selected task group
    GtState* m_lastTaskGroupScopeState;

    /// ID of the last selected task group
    GtState* m_lastTaskGroupIdState;

    /**
     * @brief Determines the current Task Group from the process data and
     * updates the corresponding GUI components in the process dock.
     */
    void updateCurrentTaskGroup();

    /**
     * @brief setCurrentProcess
     * @param process
     */
    void setCurrentProcess(GtTask* process = nullptr);

    /**
     * @brief addEmptyTaskToRoot
     */
    void addEmptyTaskToRoot();

    void addEmptyTask(GtObject* root);

    /**
     * @brief addCalculator
     */
    void addCalculator();

    /**
     * @brief Opens task wizard to select new task.
     */
    void addTask();

    /**
     * @brief Opens task wizard to select new task to be added as root.
     */
    void addRootTask();

    /**
     * @brief addTaskToParent
     * @param parent
     */
    void addTaskToParent(GtObject* parentObj);

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
     * @brief Helper method for creating the add element menu of most context
     * menus
     * @param menu Menu to fill
     */
    void makeAddMenu(QMenu& menu);

    /**
     * @brief Creates the context menu based on the current selection and
     * the index provided.
     * @param srcIndex Index to create context menu on
     */
    void customContextMenu(QModelIndex const& srcIndex);

    /**
     * @brief Creates the default context menu (i.e. when no valid index was
     * selected)
     */
    void defaultContextMenu();

    /**
     * @brief processContextMenu
     * @param obj
     */
    void processContextMenu(GtProcessComponent& obj, const QModelIndex& index, bool isTask);

    /**
     * @brief multiSelectionContextMenu
     * @param indexList
     */
    void multiSelectionContextMenu(const QList<QModelIndex>& indexList);

    /**
     * @brief Takes the UUIDs of the expanded items from the project states and
     * expands the corresponding items in the process view.
     */
    void restoreExpandStates();

    /**
     * @brief Helper method that recursively expands the items corresponding to
     * the given list of UUIDs, starting from the specified start index.
     * @param expandedUuids A list of UUIDs identifying the items to be
     * expanded.
     * @param startIndex The index from which to start.
     */
    void restoreExpandStatesHelper(const QStringList& expandedUuids,
                                   const QModelIndex& startIndex);

    /**
     * @brief Determines the model index of the currently selected task group
     * and sets it as the root index for the process view.
     */
    void updateProcessViewRootIndex();

    /**
     * @brief Retrieves the UUIDs of expanded process items from the project
     * states and returns them as a list of strings.
     * @return A list of UUIDs representing the expanded process items.
     */
    QStringList expandedItemUuids() const;

    /**
     * @brief Stores the given list of UUIDs in the project states. These UUIDs
     * represent the process items that are currently expanded.
     * @param uuids A list of UUIDs representing the expanded process items.
     */
    void setExpandedItemUuids(const QStringList& uuids);

    /**
     * @brief Retrieves and returns the scope of the last selected Task Group
     * from the project states.
     * @return The scope of the last selected Task Group.
     */
    GtTaskGroup::SCOPE lastTaskGroupScope() const;

    /**
     * @brief Stores the given Task Group scope in the project states. This
     * represents the scope of the last selected Task Group.
     * @param scope The scope of the last selected Task Group to be stored.
     */
    void setLastTaskGroupScope(GtTaskGroup::SCOPE scope);

    /**
    * @brief Retrieves and returns the ID of the last selected Task Group
    * from the project states.
    * @return The ID of the last selected Task Group.
    */
    QString lastTaskGroupId() const;

    /**
     * @brief Stores the given Task Group ID in the project states.
     * This ID represents the last selected Task Group.
     * @param groupId The ID of the last selected Task Group to be stored.
     */
    void setLastTaskGroupId(const QString& groupId);

    /**
     * @brief Resets the Task Group Model using the list of currently existing
     * Task Groups from the process data.
     */
    void resetTaskGroupModel();

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
     * @brief Creates a context menu at the screen position depending on the
     * current selection etc.
     * @param pos Cursor position
     */
    void customContextMenu(const QPoint& pos);

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
    void onExecutorChanged(GtCoreProcessExecutor* exec);

    void currentTaskGroupIndexChanged(int index);

    /**
     * @brief Called after resetting the gtDataModel. It updates the root
     * index of the process view.
     */
    void endResetView();

    /**
     * @brief Called after an item is collapsed. It removes the UUID of the
     * collapsed item from the list of expanded process items.
     * @param index The index of the item that was collapsed.
     */
    void itemCollapsed(const QModelIndex& index);

    /**
     * @brief Called after an item is expanded. It adds the UUID of the
     * expanded item to the list of expanded process items.
     * @param index The index of the item that was expanded.
     */
    void itemExpanded(const QModelIndex& index);

signals:
    /**
     * @brief selectedObjectChanged
     */
    void selectedObjectChanged(GtObject*);
};

#endif // GTPROCESSDOCK_H
