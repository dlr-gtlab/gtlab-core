/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 13.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOUTPUTDOCK_H
#define GTOUTPUTDOCK_H

#include <QModelIndexList>

#include "gt_dockwidget.h"

class GtTableView;
class GtFilteredLogModel;
class QPushButton;

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
    Qt::DockWidgetArea getDockWidgetArea() override;

protected:
    /**
     * @brief Event called after current project has changed.
     * @param project New current project.
     */
    void projectChangedEvent(GtProject* project) override;

    /**
     * @brief keyPressEvent
     * @param event - key press event to handle
     */
    void keyPressEvent(QKeyEvent* event) override;

private:
    /// Log view
    GtTableView* m_logView{};

    /// Filter model
    GtFilteredLogModel* m_model{};

    /// Toggle trace button (hide/show)
    QPushButton* m_traceButton{};

    /// Toggle debug button (hide/show)
    QPushButton* m_debugButton{};

    /// Toggle info button (hide/show)
    QPushButton* m_infoButton{};

    /// Toggle warning button (hide/show)
    QPushButton* m_warningButton{};

    /// Toggle error button (hide/show)
    QPushButton* m_errorButton{};

    /// Flag, whether the log should be automatically scrolled to the bottom or not
    bool m_autoScrollToBottom{true};

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
     * @param Test cae
     */
    void testOutput(int testCase = 1);

    /**
     * @brief scrollToBottom - scroll to bottom of the view
     */
    void onRowsInserted();

    /**
     * @brief exportLog
     */
    void exportLog();

    /**
     * @brief openContextMenu
     */
    void openContextMenu();

    /**
     * @brief openContextMenu
     */
    void openTestCaseContextMenu();

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
