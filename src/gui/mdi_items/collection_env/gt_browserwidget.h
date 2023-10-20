/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_browserwidget.h
 *
 *  Created on: 07.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_BROWSERWIDGET_H
#define GT_BROWSERWIDGET_H

#include "gt_abstractbrowserwidget.h"

class QTreeView;
class GtCollectionNetworkItem;
class GtCollectionBrowserModel;

/**
 * @brief The GtBrowserWidget class
 */
class GtBrowserWidget : public GtAbstractBrowserWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtBrowserWidget
     * @param parent
     */
    explicit GtBrowserWidget(QWidget* parent = nullptr);

    /**
     * @brief setItems
     * @param installedItems
     * @param availableItems
     * @param updateAvailableItems
     */
    void setItems(const QList<GtCollectionNetworkItem>& installedItems,
                  const QList<GtCollectionNetworkItem>& availableItems,
                  const QList<GtCollectionNetworkItem>& updateAvailableItems);

    /**
     * @brief itemsToDownload
     * @return
     */
    QList<GtCollectionNetworkItem> itemsToDownload() override;

    /**
     * @brief itemsToUpdate
     * @return
     */
    QList<GtCollectionNetworkItem> itemsToUpdate() override;

    /**
     * @brief selectAllItems
     */
    void selectAllItems() override;

    /**
     * @brief unselectAllItems
     */
    void unselectAllItems() override;

private:
    /// collection browser model.
    GtCollectionBrowserModel* m_model;

    /// Tree view
    QTreeView* m_view;

private slots:
    /**
     * @brief onItemClicked
     * @param index
     */
    void onItemClicked(const QModelIndex& index);

    /**
     * @brief onItemSelectionChange
     */
    void onItemSelectionChange();

};

#endif // GT_BROWSERWIDGET_H
