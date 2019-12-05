/* GTlab - Gas Turbine laboratory
 * Source File: gt_processoverviewtree.h
 * copyright 2009-2017 by DLR
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
    GtProcessOverviewTree(GtProcessOverviewModel* model,
                          QWidget* parent = Q_NULLPTR);

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
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

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
