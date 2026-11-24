/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef GT_GUITABLAYOUTADAPTER_H
#define GT_GUITABLAYOUTADAPTER_H

#include <gt_guilayoutadapter.h>
#include <gt_tabwidget.h>
#include <gt_mdiitem.h>

#include <QPointer>

class GT_GUI_EXPORT GtGuiTabLayoutAdapter final : public GtGuiLayoutAdapter
{
    Q_OBJECT

public:
    GtGuiTabLayoutAdapter(GtTabWidget& tabWidget);

    /**
     * @brief Whether the adapter is null and invalid.
     * @return Is null
     */
    bool isNull() const noexcept;

    /**
     * @brief Requests to focus the item with the given id
     * @param itemId Item's id to foucs
     */
    void focus(const QString& itemId) override;

    /**
     * @brief Adds the item's widget to the adapter
     * @param item Item to add to the adapter
     * @return Whether appending was successful.
     */
    bool addItem(GtMdiItem& item) override;

    /**
     * @brief Returns the item for the given item id or null if no open item
     * was found
     * @param itemId Item identification
     * @return Item belonging to the given itemId (may be null)
     */
    GtMdiItem* item(const QString& itemId) override;

    /**
     * @brief Returns the item from the widget (containing widget)
     * @param widget Widget to retrieve mdi item from
     * @return Item belonging to `widget` (may be null)
     */
    GtMdiItem* itemFromWidget(QWidget& widget) override;

    /**
     * @brief Returns the currently active gui item
     * @return Active item (may be null)
     */
    GtMdiItem* currentItem() override;

    /**
     * @brief Closes all open items
     */
    void closeAll() override;

private:
    QPointer<GtTabWidget> m_tab;
    QHash<QWidget*, QPointer<GtMdiItem>> m_openItems;

    void onTabCloseRequested(int idx);

    void onObjectNameChanged(GtMdiItem* item);
};

#endif // GT_GUITABLAYOUTADAPTER_H
