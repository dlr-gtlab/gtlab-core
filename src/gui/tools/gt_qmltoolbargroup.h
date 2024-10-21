/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * Author: Martin Siggel (AT-TWK)
 */

#ifndef GTQMLTOOLBARGROUP_H
#define GTQMLTOOLBARGROUP_H

#include <gt_qmlaction.h>

#include <gt_listmodel.h>

#include <gt_gui_exports.h>

template <>
inline QVariant
gt::asVariant(GtQmlAction* const& action)
{
    return QVariant::fromValue(action);
}

/**
 * @brief A logical group of actions that can be added
 *        and removed from the toolbar
 */
class GT_GUI_EXPORT GtQmlToolbarGroup : public GtListModel<GtQmlAction*>
{
    Q_OBJECT

    /**
     * @brief This property holds the name of the group
     *
     * The name is currently not rendered. It is a placeholder, if we decide
     * to show the group name together with the icons.
     *
     * See also `GtQmlToolbarGroup::name` and `GtQmlToolbarGroup::setName`
     */
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)

    /**
     * @brief This property holds whether the toolbar group can be seen
     *
     * Note, the group is only visible, if the user requires visibility and
     * if there is at least one visible action.
     *
     * See also `GtQmlToolbarGroup::isVisible` and `GtQmlToolbarGroup::setVisible`
     */
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged FINAL)

public:
    GtQmlToolbarGroup();

    explicit GtQmlToolbarGroup(QString name, QObject* parent = nullptr);
    GtQmlToolbarGroup(QString name, const std::vector<GtQmlAction*>& actions,
                    QObject* parent = nullptr);

    QString name() const;
    void setName(QString name);

    bool isVisible() const;
    void setVisible(bool visible);

    /**
     * @brief Adds a action to the group
     *
     * Note: this will emit visibleChanged, as the visibility
     * of the groupd needs to be evaluated again
     *
     * @return true on success
     */
    bool append(GtQmlAction* action);

    /**
     * @brief Adds a separator into the group at the current position
     */
    bool addSeparator();

    /**
     * @brief Overloaded function to set all actions at once
     * @param data List of actions
     */
    void setListData(const std::vector<GtQmlAction*>& data);

signals:
    void nameChanged();
    void visibleChanged();

private:
    /**
     * @brief Updates the visibility of the toolbar and its separators
     * depending on the actions visibility
     *
     * - If no action is visible, the whole group gets invisible
     *   since there is nothing to show
     *
     * - If no action is in front of any separator, the separator gets invisible
     * - If no action is visible after a separator, it gets invisible as well
     */
    void updateVisibility();


    using Base = GtListModel<GtQmlAction*>;

    QString m_groupName;
    bool m_visible{true};
};

#endif // GTQMLTOOLBARGROUP_H
