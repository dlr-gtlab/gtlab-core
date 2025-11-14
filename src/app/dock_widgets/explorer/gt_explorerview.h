/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 31.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTEXPLORERVIEW_H
#define GTEXPLORERVIEW_H

#include "gt_treeview.h"

#include <QPropertyAnimation>

/**
 * @brief The GtExplorerView class
 */
class GtExplorerView : public GtTreeView
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent Parent widget.
     */
    explicit GtExplorerView(QWidget* parent = nullptr);

    /**
     * @brief We override the default behavior to improve handling
     * of deep object hierarchies
     */
    void scrollTo(const QModelIndex &index,
                  ScrollHint hint = EnsureVisible) override;

private:
    /**
     * @brief Overloaded resize event.
     * @param event Resize event.
     */
    void resizeEvent(QResizeEvent* event) override;

    /**
     * @brief Overloaded mouse double click event.
     * @param event Mouse event.
     */
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    
    /**
     * @brief keyPressEvent
     * @param event
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief dragEnterEvent
     * @param event
     */
    void dragEnterEvent(QDragEnterEvent* event) override;

    /**
     * @brief dropEvent
     * @param event
     */
    void dropEvent(QDropEvent* event) override;

    /**
     * @brief dragMoveEvent
     * @param event
     */
    void dragMoveEvent(QDragMoveEvent* event) override;

    /**
     * @brief Checks if mime data is acceptable or not.
     * @param mimeData MimeData to check
     * @return true if mime data is acceptable, otherwise false is returned.
     */
    bool checkMimeData(const QMimeData* mimeData);

    // To animate the horizontal scroll bar when selecting objects
    QPropertyAnimation* m_hScrollAnim = nullptr;

signals:
    /**
     * @brief mdiItemRequested
     * @param index
     */
    void mdiItemRequested(const QModelIndex& index);

};

#endif // GTEXPLORERVIEW_H
