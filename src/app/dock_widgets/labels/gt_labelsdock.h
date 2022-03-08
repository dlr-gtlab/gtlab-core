/* GTlab - Gas Turbine laboratory
 * Source File: gt_labelsdock.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 18.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLABELSDOCK_H
#define GTLABELSDOCK_H

#include <QPersistentModelIndex>
#include <QPointer>

#include "gt_dockwidget.h"

class QSortFilterProxyModel;
class QPushButton;
class GtListView;
class GtStyledModel;
class GtSearchWidget;
class GtLabelData;
class GtLabel;

/**
 * @brief The GtLabelsDock class
 */
class GtLabelsDock : public GtDockWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtLabelsDock
     */
    Q_INVOKABLE GtLabelsDock();

    /** Returns the recommended dock widget area in main windows.
        @return dock widget area */
    Qt::DockWidgetArea getDockWidgetArea() override;

    /**
     * @brief Returns label object from given index.
     * @param Label index.
     * @return Label object.
     */
    GtLabel* labelFromIndex(const QModelIndex& index);

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
     * @brief Returns current project. Returns NULL if no project is set.
     * @return Current project.
     */
    GtProject* project();

protected:
    /** Event called after current project has changed.
        @param new current project */
    void projectChangedEvent(GtProject* project) override;

private:
    /// Model
    QSortFilterProxyModel* m_model;

    /// Styled model
    GtStyledModel* m_styledModel;

    /// Export labels to file button
    QPushButton* m_exportButton;

    /// Import labels from file button
    QPushButton* m_importButton;

    /// Add new label button
    QPushButton* m_addButton;

    /// Labels list view
    GtListView* m_listView;

    /// Root index
    QPersistentModelIndex m_rootIndex;

    /// Search widget
    GtSearchWidget* m_search;

    /// Pointer to label data of current project
    QPointer<GtLabelData> m_labelData;

    /// Pointer to current project
    QPointer<GtProject> m_project;

    /**
     * @brief Searches for usages of given label index inside current project.
     * @param Label index.
     */
    void findUsages(const QModelIndex& index);

    /**
     * @brief Deletes all usages of given label inside current project.
     * @param Label index.
     * @param Delete label indicator.
     */
    void deleteUsages(const QModelIndex& index, bool deleteLabel = true);

    /**
     * @brief Deletes all usages of given labels inside current project.
     * @param Label indexes.
     * @param Delete label indicator.
     */
    void deleteMultipleUsages(const QModelIndexList& indexes,
                              bool deleteLabels = true);

private slots:
    /**
     * @brief filterData
     * @param val
     */
    void filterData(const QString& val);

    /**
     * @brief Adds new label to current project.
     */
    void newLabel();

    /**
     * @brief customContextMenu
     * @param pos
     */
    void customContextMenu(const QPoint& pos);

    /**
     * @brief resetModel
     */
    void resetModel();

};

#endif // GTLABELSDOCK_H
