/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_localcollectionwidget.h
 *
 *  Created on: 07.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_LOCALCOLLECTIONWIDGET_H
#define GT_LOCALCOLLECTIONWIDGET_H

#include "gt_gui_exports.h"

#include <QWidget>
#include <QTreeView>

class GtCollectionItem;
class GtLocalCollectionModel;

/**
 * @brief The GtLocalCollectionWidget class
 */
class GT_GUI_EXPORT GtLocalCollectionWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtLocalCollectionWidget
     */
    explicit GtLocalCollectionWidget(QWidget* parent = nullptr);

    /**
     * @brief setItems
     * @param items
     */
    void setItems(const QList<GtCollectionItem>& items);

    /**
     * @brief getItem
     * @param index
     * @return
     */
    GtCollectionItem getItem(const QModelIndex& index);

    /**
     * @brief setCustomContextMenuEnabled
     * @param val
     */
    void setCustomContextMenuEnabled(bool val);

    /**
     * @brief setShowItemInfo
     * @param val
     */
    void setShowItemInfo(bool val);

    /**
     * @brief Returns the model index of the item at the given row.
     * @param row The row of the list you want to get the model index for.
     * @return the model index of the item at the given row
     */
    QModelIndex getModelIndex(int row);

    /**
     * @brief Sets the current index to the list view.
     * @param index The index you want to set.
     */
    void setCurrentIndex(const QModelIndex& index);

private:
    /// Tree view.
    QTreeView* m_view;

    /// Local collection model.
    GtLocalCollectionModel* m_model;

    /// Enables custom context menu
    bool m_contextMenuEnabled;

private slots:
    /**
     * @brief onItemClicked
     * @param index
     */
    void onItemClicked(const QModelIndex& index);

    /**
     * @brief onCustomContextMenuRequested
     * @param pos
     */
    void onCustomContextMenuRequested(const QPoint& pos);

    /**
     * @brief onCurrentChanged
     * @param current
     * @param previous
     */
    void onCurrentChanged(const QModelIndex& current,
                          const QModelIndex& previous = QModelIndex());

signals:
    /**
     * @brief Emitted after item was selected. Returns selected item.
     * @param Selected item.
     */
    void itemSelected(const QModelIndex& index);

    /**
     * @brief itemDoubleClicked
     * @param index
     */
    void itemDoubleClicked(const QModelIndex& index);

};

#endif // GT_LOCALCOLLECTIONWIDGET_H
