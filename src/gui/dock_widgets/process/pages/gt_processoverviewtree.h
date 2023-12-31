/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processoverviewtree.h
 *
 *  Created on: 03.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_PROCESSOVERVIEWTREE_H
#define GT_PROCESSOVERVIEWTREE_H

#include "gt_treeview.h"

class GtTreeFilterModel;
class GtProcessOverviewModel;

/**
 * @brief The GtProcessOverviewTree class
 */
class GtProcessOverviewTree : public GtTreeView
{
    Q_OBJECT

public:
    /**
     * @brief GtProcessOverviewTree
     * @param parent
     */
    explicit GtProcessOverviewTree(GtProcessOverviewModel *model, QWidget *parent = nullptr);

    /**
     * @brief resizeColumns
     */
    void resizeColumns();

    /**
     * @brief Returns filter model.
     * @return Filter model.
     */
    GtTreeFilterModel* filterModel();

    /**
     * @brief Returns source model.
     * @return Source model.
     */
    GtProcessOverviewModel* sourceModel();

    /**
     * @brief sortAlphabetically
     *  Sort Elements by first column in Alphabetical-Order
     */
    void sortAlphabetically();

protected:
    /**
     * @brief resizeEvent
     * @param event
     */
    void resizeEvent(QResizeEvent* event) override;

private:
    /// Source model.
    GtProcessOverviewModel* m_model;

    /// Filter model.
    GtTreeFilterModel* m_filterModel;

    /**
     * @brief setCollapsed
     * @param val
     */
    void setCollapsed(const QModelIndex& index, bool val);

private slots:
    /**
     * @brief onCollapsed
     * @param index
     */
    void onCollapsed(const QModelIndex& index);

    /**
     * @brief onExpanded
     * @param index
     */
    void onExpanded(const QModelIndex& index);

};

#endif // GT_PROCESSOVERVIEWTREE_H
