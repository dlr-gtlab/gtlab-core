/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_postdock.h
 *
 *  Created on: 26.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPOSTDOCK_H
#define GTPOSTDOCK_H

#include <QPointer>
#include <QModelIndex>
#include "gt_dockwidget.h"

class QPushButton;
class QSortFilterProxyModel;
class QDir;
class GtListView;
class GtSearchWidget;
class GtPostModel;

/**
 * @brief The GtPostDock class
 */
class GtPostDock : public GtDockWidget
{
    Q_OBJECT

public:
    /**
      * @brief Constructor
      */
    Q_INVOKABLE GtPostDock();

    /** Returns the recommended dock widget area in main windows.
        @return dock widget area */
    Qt::DockWidgetArea getDockWidgetArea() override;

    /**
     * @brief renameFile
     * @param oldName
     * @param newName
     * @return
     */
    bool renameFile(const QString& oldName, const QString& newName);

    /**
     * @brief entries
     * @return
     */
    const QStringList& entries();

protected:
    /** Event called after current project has changed.
        @param new current project */
    void projectChangedEvent(GtProject* project) override;

    /**
     * @brief Event filter to catch key press events of given object.
     * @param obj Object pointer.
     * @param event Current event of given object.
     * @return Returns event filter acceptance.
     */
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    /// List view
    GtListView* m_listView;

    /// Search widget
    GtSearchWidget* m_search;

    /// Export labels to file button
    QPushButton* m_exportButton;

    /// Import labels from file button
    QPushButton* m_importButton;

    /// Add new label button
    QPushButton* m_addButton;

    /// Template data model
    GtPostModel* m_datamodel;

    /// Sort filter model
    QSortFilterProxyModel* m_model;

    /// Project
    QPointer<GtProject> m_project;

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
     * @brief openTemplateViewer
     * @param index
     */
    void openTemplateViewer(const QModelIndex& index);

    /**
     * @brief deleteEntry
     * @param index
     */
    void deleteEntry(const QModelIndex& index);

    /**
     * @brief keyPressEvent
     * @param event - key press event to handle
     */
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    /**
     * @brief newPostTemplate
     */
    void newPostTemplate();

    /**
     * @brief customContextMenu
     * @param pos
     */
    void customContextMenu(QPoint const& pos);

    /**
     * @brief customContextMenu
     * @param index
     */
    void customContextMenu(QModelIndex const& index);

    /**
     * @brief filterData
     * @param val
     */
    void filterData(const QString& val);

    /**
     * @brief onDoubleClicked
     * @param index
     */
    void onDoubleClicked(const QModelIndex& index);

signals:
    /**
     * @brief contextMenuKeyPressSignal
     * Signal to emit if the context menu is requested by the key press
     * of shortcut
     */
    void contextMenuKeyPressSignal(QModelIndex);
};

#endif // GTPOSTDOCK_H
