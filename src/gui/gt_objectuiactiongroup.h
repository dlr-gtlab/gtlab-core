/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_objectuiactiongroup.h
 *
 *  Created on: 28.09.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef GTOBJECTUIACTIONGROUP_H
#define GTOBJECTUIACTIONGROUP_H

#include "gt_gui_exports.h"

#include "gt_objectuiaction.h"

#include <QString>
#include <QList>

/**
 * @brief The GtObjectUIActionGroup class
 */
class GT_GUI_EXPORT GtObjectUIActionGroup
{
public:

    /**
     * @brief Constructor
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Use non-default constructor.")
    GtObjectUIActionGroup();

    explicit
    GtObjectUIActionGroup(QString groupName,
                          QIcon icon = {});

    GtObjectUIActionGroup(QString groupName,
                          QList<GtObjectUIAction> actions,
                          QIcon icon = {});

    GtObjectUIActionGroup(QString groupName,
                          QList<GtObjectUIAction> actions,
                          const QString& icon);

    GtObjectUIActionGroup(GtObjectUIActionGroup const&) noexcept;
    GtObjectUIActionGroup(GtObjectUIActionGroup&&) noexcept;
    GtObjectUIActionGroup& operator=(GtObjectUIActionGroup const&) noexcept;
    GtObjectUIActionGroup& operator=(GtObjectUIActionGroup&&) noexcept;
    ~GtObjectUIActionGroup() noexcept;

    /**
     * @brief actions
     * @return list of actions
     */
    const QList<GtObjectUIAction>& actions() const;

    /**
     * @brief name
     * @return group name
     */
    const QString& name() const;

    /**
     * @brief icon
     * @return icon
     */
    const QIcon& icon() const;

    /**
     * @brief Reserves space for size actions
     * @param size
     */
    void reserve(int size);

    /**
     * @brief Returns the priority according to which this action is sorted
     * in a menu. An action with a lower priority 'x' prepends all actions
     * with a higher prority > x.
     * @return Order priority
     */
    int orderPriority() const;

    /**
     * @brief Dedicated setter for the UI icon
     * @param icon Icon
     * @return This
     */
    GtObjectUIActionGroup& setIcon(const QIcon& icon);

    /**
     * @brief Overload. Accepts a string instead
     * @param icon Icon name or path
     * @return This
     */
    GtObjectUIActionGroup& setIcon(const QString& icon);

    /**
     * @brief Sets the order priority according to which the action is sorted
     * in the menu. An action with a lower priority 'x' prepends all actions
     * with a higher prority > x.
     * @param priority Order priority
     * @return This
     */
    GtObjectUIActionGroup& setOrderPriority(int priority);

    /**
     * @brief Adds the action to the group. Depending on the order priority of
     * `action` it may be displayed at a different position in the menu.
     * @param action Action to append
     * @return This
     */
    GtObjectUIActionGroup& operator<<(GtObjectUIAction const& action);

    /**
     * @brief Adds the action to the group Depending on the order priority of
     * `action` it may be displayed at a different position in the menu.
     * @param action Action to append
     * @return This
     */
    GtObjectUIActionGroup& addAction(GtObjectUIAction const& action);

    /**
     * @brief Swaps this action group with `other`
     * @param other Other
     */
    void swap(GtObjectUIActionGroup& other) noexcept;

private:

    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

inline void swap(GtObjectUIActionGroup& a, GtObjectUIActionGroup& b) noexcept { a.swap(b); }

namespace gt
{
namespace gui
{

inline GtObjectUIActionGroup
makeActionGroup(const QString& groupName, int sizeHint = -1)
{
    auto tmp = GtObjectUIActionGroup(groupName);
    tmp.reserve(sizeHint);
    return tmp;
}

} // namespace gui

} // namespace gt

#endif // GTOBJECTUIACTIONGROUP_H
