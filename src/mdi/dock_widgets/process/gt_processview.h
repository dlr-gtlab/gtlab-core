/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 02.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSVIEW_H
#define GTPROCESSVIEW_H

#include "gt_treeview.h"

class GtObject;

/**
 * @brief The GtProcessView class
 */
class GtProcessView : public GtTreeView
{
    Q_OBJECT

public:
    /**
     * @brief GtProcessView
     * @param parent
     */
    GtProcessView(QWidget* parent = 0);

    /**
     * @brief resizeColumns
     */
    void resizeColumns();

protected:
    /**
     * @brief resizeEvent
     * @param event
     */
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

    /**
     * @brief keyPressEvent
     * @param event
     */
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

    /**
     * @brief mousePressEvent
     * @param event
     */
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

signals:
    /**
     * @brief pasteProcessElement
     * @param index
     */
    void pasteProcessElement(const QModelIndex& index);

    /**
     * @brief copyProcessElement
     * @param index
     */
    void copyProcessElement(const QModelIndex& index);

    /**
     * @brief cutProcessElement
     * @param index
     */
    void cutProcessElement(const QModelIndex& index);

    /**
     * @brief deleteProcessElements
     * @param indexList
     */
    void deleteProcessElements(const QList<QModelIndex>& indexList);

};

#endif // GTPROCESSVIEW_H
