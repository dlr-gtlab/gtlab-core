/* GTlab - Gas Turbine laboratory
 * Source File: gt_objectuiactiongroup.h
 * copyright 2009-2017 by DLR
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
     * @brief GtObjectUIActionGroup
     */
    GtObjectUIActionGroup();

    /**
     * @brief GtObjectUIActionGroup
     */
    GtObjectUIActionGroup(const QString& groupName,
                          const QList<GtObjectUIAction>& actions,
                          const QString& icon = {});
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
     * @brief Appends the action to the group
     * @param action Action to append
     * @return This
     */
    GtObjectUIActionGroup& operator<<(GtObjectUIAction const& action);

private:
    /// List of actions
    QList<GtObjectUIAction> m_actions;

    /// Group action text
    QString m_name;

    /// Group action icon
    QIcon m_icon;
};

namespace gt
{
namespace gui
{

inline GtObjectUIActionGroup
makeActionGroup(const QString& groupName, int sizeHint = -1)
{
    auto tmp = GtObjectUIActionGroup(groupName, {});
    tmp.reserve(sizeHint);
    return tmp;
}

} // namespace gui

} // namespace gt

#endif // GTOBJECTUIACTIONGROUP_H
