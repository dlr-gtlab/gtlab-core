/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOUTPUTDOCK_H
#define GTOUTPUTDOCK_H

#include <QModelIndexList>

#include "gt_dockwidget.h"

class GtListView;
class GtTreeView;
class GtFilteredLogModel;
class GtTaskHistoryModel;
//class GtPythonScriptingConsole;

/**
 * @brief The GtOutputDock class
 */
class GtOutputDock : public GtDockWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtOutputDock
     */
    Q_INVOKABLE GtOutputDock();

    /** Returns the recommended dock widget area in main windows.
        @return dock widget area */
    virtual Qt::DockWidgetArea getDockWidgetArea();

protected:
    /**
     * @brief Event called after current project has changed.
     * @param project New current project.
     */
    void projectChangedEvent(GtProject* project) Q_DECL_OVERRIDE;

private:
    GtListView* m_listView;

    GtFilteredLogModel* m_model;

    /// Task historty view
    GtTreeView* m_taskPageView;

    /// Task history model
    GtTaskHistoryModel* m_historyModel;

//    /// Python Console
//    GtPythonScriptingConsole* m_pythonConsole;

    /**
     * @brief copyToClipboard
     * @param indexes
     */
    void copyToClipboard(const QModelIndexList& indexes);

    /**
     * @brief removeItems - removes items from model and moves the scrollbar
     * to ensure visibility of the element above the deleted ones
     * @param indexes - list of modelIndices to be deleted
     */
    void removeItems(const QModelIndexList& indexes);

private slots:
    /**
     * @brief Just a test output method.
     */
    void testOutput();

    /**
     * @brief scrollToBottom - scroll to bottom of the view
     */
    void scrollToBottom();

    /**
     * @brief exportLog
     */
    void exportLog();

    /**
     * @brief openContextMenu
     */
    void openContextMenu();

    /**
     * @brief onCopyRequest
     */
    void onCopyRequest();

    /**
     * @brief onDeleteRequest -  slot as reaction on a delete request:
     * removes selected items from model by using the removeItems function
     */
    void onDeleteRequest();
};

#endif // GTOUTPUTDOCK_H
