/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 31.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTEXPLORERVIEW_H
#define GTEXPLORERVIEW_H

#include "gt_treeview.h"

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
    GtExplorerView(QWidget* parent = 0);

private:
    /**
     * @brief Overloaded resize event.
     * @param event Resize event.
     */
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

    /**
     * @brief Overloaded mouse double click event.
     * @param event Mouse event.
     */
    void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    
    /**
     * @brief keyPressEvent
     * @param event
     */
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

    /**
     * @brief dragEnterEvent
     * @param event
     */
    void dragEnterEvent(QDragEnterEvent* event) Q_DECL_OVERRIDE;

    /**
     * @brief dropEvent
     * @param event
     */
    void dropEvent(QDropEvent* event) Q_DECL_OVERRIDE;

    /**
     * @brief dragMoveEvent
     * @param event
     */
    void dragMoveEvent(QDragMoveEvent* event) Q_DECL_OVERRIDE;

    /**
     * @brief Checks if mime data is acceptable or not.
     * @param mimeData MimeData to check
     * @return true if mime data is acceptable, otherwise false is returned.
     */
    bool checkMimeData(const QMimeData* mimeData);

signals:
    /**
     * @brief mdiItemRequested
     * @param index
     */
    void mdiItemRequested(const QModelIndex& index);

    /**
     * @brief mdiItemRequested
     * @param index
     */
    void deleteElementsRequest(const QList<QModelIndex>& indexList);

};

#endif // GTEXPLORERVIEW_H
