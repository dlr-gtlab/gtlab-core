/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
    explicit GtProcessView(QWidget* parent = nullptr);

    /**
     * @brief resizeColumns
     */
    void resizeColumns();

protected:
    /**
     * @brief resizeEvent
     * @param event
     */
    void resizeEvent(QResizeEvent* event) override;

    /**
     * @brief keyPressEvent
     * @param event
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief mousePressEvent
     * @param event
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief dropEvent
     * @param event
     */
    void dropEvent(QDropEvent* event) override;
signals:
    /**
     * @brief pasteProcessElement
     * @param index - modelindex for which the signal is emited
     */
    void pasteProcessElement(const QModelIndex& index);

    /**
     * @brief copyProcessElement
     * @param index - modelindex for which the signal is emited
     */
    void copyProcessElement(const QModelIndex& index);

    /**
     * @brief cutProcessElement
     * @param index - modelindex for which the signal is emited
     */
    void cutProcessElement(const QModelIndex& index);

    /**
     * @brief cloneProcessElement
     * @param index - modelindex for which the signal is emited
     */
    void cloneProcessElement(const QModelIndex& index);

    /**
     * @brief deleteProcessElements
     * @param indexList
     */
    void deleteProcessElements(const QList<QModelIndex>& indexList);

    /**
     * @brief runTaskElement
     * @param index - modelindex for which the signal is emited
     */
    void runTaskElement(const QModelIndex& index);

    /**
     * @brief skipCalculatorElements
     * @param index - modelindex for which the signal is emited
     * @param skip - bool as flag to skip or unskip
     */
    void skipCalculatorElements(const QList<QModelIndex>& index, bool skip);

    /**
     * @brief renameProcessElement
     * @param index - modelindex for which the signal is emited
     */
    void renameProcessElement(const QModelIndex& index);

    void moveProcessElements(const QList<QModelIndex>& source,
                             const QModelIndex& target);
};

#endif // GTPROCESSVIEW_H
