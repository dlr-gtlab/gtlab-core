/* GTlab - Gas Turbine laboratory
 * Source File: gt_processmonitoringview.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 17.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSMONITORINGVIEW_H
#define GTPROCESSMONITORINGVIEW_H

#include "gt_treeview.h"

/**
 * @brief The GtProcessMonitoringView class
 */
class GtProcessMonitoringView : public GtTreeView
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Parent wirget.
     */
    GtProcessMonitoringView(QWidget* parent = Q_NULLPTR);

    /**
     * @brief resizeColumns
     */
    void resizeColumns();

    /**
     * @brief mouseMoveEvent
     * @param event
     */
    virtual void mouseMoveEvent(QMouseEvent* event);

protected:
    /**
     * @brief drawRow
     * @param painter
     * @param option
     * @param index
     */
    void drawRow(QPainter *painter,
                 const QStyleOptionViewItem &option,
                 const QModelIndex &index) const Q_DECL_OVERRIDE;

    /**
     * @brief resizeEvent
     * @param event
     */
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

};

#endif // GTPROCESSMONITORINGVIEW_H
