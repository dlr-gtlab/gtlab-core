/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef GT_GUILAYOUTADAPTER_H
#define GT_GUILAYOUTADAPTER_H

#include <gt_gui_exports.h>
#include <QObject>

class GtMdiItem;
class GT_GUI_EXPORT GtGuiLayoutAdapter : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Requests to focus the item with the given id
     * @param itemId Item's id to foucs
     */
    virtual void focus(const QString& itemId) = 0;

    /**
     * @brief Adds the item's widget to the adapter
     * @param item Item to add to the adapter
     * @return Whether appending was successful.
     */
    virtual bool addItem(GtMdiItem& item) = 0;

    /**
     * @brief Returns wheher any open item with the given id exists
     * @param itemId Item id
     * @return Whether an item with the given id is open
     */
    bool hasItem(const QString& itemId);

    /**
     * @brief Returns the item for the given item id or null if no open item
     * was found
     * @param itemId Item identification
     * @return Item belonging to the given itemId (may be null)
     */
    virtual GtMdiItem* item(const QString& itemId) = 0;

    /**
     * @brief Returns the item from the widget (containing widget)
     * @param widget Widget to retrieve mdi item from
     * @return Item belonging to `widget` (may be null)
     */
    virtual GtMdiItem* itemFromWidget(QWidget& widget) = 0;

    /**
     * @brief Returns the currently active gui item
     * @return Active item (may be null)
     */
    virtual GtMdiItem* currentItem() = 0;

    /**
     * @brief Closes all open items
     */
    virtual void closeAll() = 0;

protected:
    GtGuiLayoutAdapter();

    /// sets the containing widget
    void setContainingWidget(GtMdiItem& item, QWidget* containingWidget);

    /// calls the `initialized` method of `item`
    void triggerInitialization(GtMdiItem& item);

    /// calls the `showEvent` method of `item`
    void triggerShowEvent(GtMdiItem& item);
};

#endif // GT_GUILAYOUTADAPTER_H
