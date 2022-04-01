/* GTlab - Gas Turbine laboratory
 * Source File: gt_processconnectionview.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSCONNECTIONVIEW_H
#define GTPROCESSCONNECTIONVIEW_H

#include "gt_treeview.h"
#include <QPointer>

class GtProcessConnectionGraphicsView;
class GtProcessConnectionModel;
class GtProcessConnectionItem;

/**
 * @brief The GtProcessConnectionView class
 */
class GtProcessConnectionView : public GtTreeView
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent - Parent widget.
     */
    explicit GtProcessConnectionView(QWidget* parent = nullptr);

    /**
     * @brief Sets given graphics view to current graphics view.
     * @param graphicsView - New graphics view widget.
     */
    void setGraphicsView(GtProcessConnectionGraphicsView* graphicsView);

    /**
     * @brief Returns the last visible item in the tree view or invalid model
     * index if not found any.
     * @param index - Starting index
     * @return Last visible index.
     */
    QModelIndex lastVisibleIndex(const QModelIndex& index = QModelIndex());

    /**
     * @brief Returns last visible parent index for given process connection
     * item.
     * @param item - Starting index.
     * @return Last visible parent index.
     */
    QModelIndex lastVisibleParentIndex(GtProcessConnectionItem* item);

    /**
     * @brief Returns last visible parent index for given process connection
     * item.
     * @param index - Starting index.
     * @return Last visible parent index.
     */
    QModelIndex lastVisibleParentIndex(const QModelIndex& index);

    /**
     * @brief Returns pointer to process connection model.
     * @return Pointer to process connection model.
     */
    GtProcessConnectionModel* connectionModel();

    /**
     * @brief Returns process connection item corresponding to given
     * identification strings.
     * @param uuid - UUID of component.
     * @param propId - Property identification string.
     * @return Process connection item.
     */
    GtProcessConnectionItem* itemById(const QString& uuid,
                                      const QString& propId);

protected:
    /**
     * @brief drawRow
     * @param painter
     * @param option
     * @param index
     */
    void drawRow(QPainter *painter,
                 const QStyleOptionViewItem &option,
                 const QModelIndex &index) const override;

    /**
     * @brief paintEvent
     * @param event
     */
    void paintEvent(QPaintEvent* event) override;

private:
    /// Pointer to graphics view widget.
    QPointer<GtProcessConnectionGraphicsView> m_graphicsView;

};

#endif // GTPROCESSCONNECTIONVIEW_H
