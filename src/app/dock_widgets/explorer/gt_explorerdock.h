/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 31.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTEXPLORERDOCK_H
#define GTEXPLORERDOCK_H

#include <QPointer>
#include <QModelIndex>

#include "gt_dockwidget.h"

class GtExplorerView;
class GtResultsView;
class GtExplorerModel;
class GtStyledModel;
class GtObject;
class GtSearchWidget;

/**
 * @brief The GtExplorerDock class
 */
class GtExplorerDock : public GtDockWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    Q_INVOKABLE GtExplorerDock();

    /**
     * @brief Destructor.
     */
    ~GtExplorerDock() override;

    /** Returns the recommended dock widget area in main windows.
        @return dock widget area */
    Qt::DockWidgetArea getDockWidgetArea() override;

    /**
     * @brief Virtual function to add functionality after application
     * startup was successful
     */
    void initAfterStartup() override;

protected:
    void projectChangedEvent(GtProject* project) override;

private:
    /// Tree view
    GtExplorerView* m_view;

    /// Explorer model
    QPointer<GtExplorerModel> m_model;

    /// Styled model
    QPointer<GtStyledModel> m_styledModel;

    /// Results model
    //QPointer<GtResultsModel> m_resultsModel;

    /// Expand states
    QStringList m_expandStates;

    /// Search widget
    GtSearchWidget* m_searchWidget;

    /**
     * @brief Open the context menu of obj
     * @param obj Object
     * @param index View index
     */
    void objectContextMenu(GtObject* obj, const QModelIndex& index);

    /**
     * @brief objectContextMenu
     * @param objs
     */
    void objectContextMenu(const QList<GtObject*>& objs);

    /**
     * @brief mapToSource
     * @param index
     * @return
     */
    QModelIndex mapToSource(const QModelIndex& index);

    /**
     * @brief Maps given model index to given model. Returns true if mapping
     * was successful otherwise returns false.
     * @tparam T Model
     * @param index Model index
     * @param model Model
     * @return whether model index was successfully mapped to given model.
     */
    template <class T>
    bool mapToSourceHelper(QModelIndex& index, T model)
    {
        if (!model)
        {
            return false;
        }

        if (index.model() != model)
        {
            return false;
        }

        index = model->mapToSource(index);

        return true;
    }

    /**
     * @brief maps the view index to a GtObject* in the source model
     * @param index
     * @return
     */
    GtObject* mapToObject(const QModelIndex& index);

    /**
     * @brief mapFromSource
     * @param index
     * @return
     */
    QModelIndex mapFromSource(const QModelIndex& index);

    /**
     * @brief Maps given model index from given source model to given target
     * model. Returns true if mapping as successful otherwise returns false.
     * @param index Model index
     * @tparam T1 Source model
     * @tparam T2 Target model
     * @return whether model index was successfully mapped from source model.
     */
    template <class T1, class T2>
    bool mapFromSourceHelper(QModelIndex& index, T1 sourceModel, T2 model)
    {
        if (index.model() != sourceModel)
        {
            return false;
        }

        if (!model)
        {
            return false;
        }

        index = model->mapFromSource(index);

        return true;
    }

    /**
     * @brief saveExpandStates
     */
    void saveExpandStates();

    /**
     * @brief saveExpandStatesToSettings
     */
    void saveExpandStatesToSettings();

    /**
     * @brief restoreExpandStates
     * @param list
     */
    void restoreExpandStates(const QStringList& list);

    /**
     * @brief restoreExpandStatesHelper
     * @param expandedItems
     * @param model
     * @param startIndex
     */
    void restoreExpandStatesHelper(const QStringList& expandedItems,
                                   QAbstractItemModel* model,
                                   QModelIndex startIndex);

    /**
     * @brief keyPressEvent
     * @param event - key press event to handle
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief firstSelectedIndex
     * @return
     */
    QModelIndex firstSelectedIndex() const;

    /**
     * @brief listElements
     * @param parent - parent index
     * @return list of all child elements of the given index parent in the model
     */
    QModelIndexList listElements(const QModelIndex& parent) const;

public slots:

    /**
     * @brief selectObjectByUuid
     * Find a object in the model by uuid and
     * select the index of this object in the view
     * @param uuid - uuid to match
     */
    void selectObjectByUuid(QString const& uuid);

private slots:

    /**
     * @brief onSessionChanged
     */
    void onSessionChanged();

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
    void onMdiItemRequested(const QModelIndex& index);

    /**
     * @brief filterData
     * @param val
     */
    void filterData(const QString& val);

    /**
     * @brief onSearchEnabled
     */
    void onSearchEnabled();

    /**
     * @brief customContextMenuDataView
     * @param pos of the mouse while the request of the context menu
     */
    void customContextMenuDataView(const QPoint& pos);

    /**
     * @brief customContextMenuDataView
     * @param index of first selected item
     */
    void customContextMenuDataView(const QModelIndex &index);

    /**
     * @brief beginResetView
     */
    void beginResetView();

    /**
     * @brief endResetView
     */
    void endResetView();

    /**
     * @brief deleteElements
     * @param indexList
     */
    void deleteElements(const QList<QModelIndex>& indexList);

    /**
     * @brief A slot than handles selection changes only by keyboard
     * events. Mouse events are actively suppressed, to avoid
     * drag/drop problems.
     *
     * Mouse events are handled by another slot
     */
    void handleKeyboardSelectionChange(const QModelIndex &current,
                                       const QModelIndex &);

signals:
    /**
     * @brief selectedObjectChanged
     */
    void selectedObjectChanged(GtObject*);

};

#endif // GTEXPLORERDOCK_H
