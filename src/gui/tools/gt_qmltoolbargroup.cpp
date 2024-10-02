/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * Author: Martin Siggel (AT-TWK)
 */

#include "gt_qmltoolbargroup.h"

#include <QIcon>

GtQmlToolbarGroup::GtQmlToolbarGroup() : GtQmlToolbarGroup("")
{
}

GtQmlToolbarGroup::GtQmlToolbarGroup(QString name, QObject* parent) :
    GtListModel(parent), m_groupName(std::move(name))
{
}

GtQmlToolbarGroup::GtQmlToolbarGroup(QString name,
                                 const std::vector<GtQmlAction*>& actions,
                                 QObject* parent) :
    GtListModel(parent),
    m_groupName(std::move(name))
{
    setListData(actions);
}

QString
GtQmlToolbarGroup::name() const
{
    return m_groupName;
}

void
GtQmlToolbarGroup::setName(QString name)
{
    if (name != m_groupName) return;

    m_groupName = std::move(name);
    emit nameChanged();
}

bool
GtQmlToolbarGroup::isVisible() const
{
    // if all items are visible, it is also not visible
    bool oneIsVisible =
        std::any_of(std::begin(m_data), std::end(m_data),
                    [](GtQmlAction* action)
    {
        if (!action) return false;
        return action->isVisible();
    });

    return m_visible && oneIsVisible;
}

void
GtQmlToolbarGroup::setVisible(bool visible)
{
    if (m_visible == visible) return;

    m_visible = visible;
    emit visibleChanged();
}

bool
GtQmlToolbarGroup::append(GtQmlAction* action)
{
    if (!action) return false;

    bool success = GtListModel::append(action);

    if (success)
    {
        // if all actions of a group is invisible,
        // the whole group shall be invisible
        connect(action, &GtQmlAction::visibleChanged, this,
                &GtQmlToolbarGroup::visibleChanged);
    }

    // update visibility
    emit visibleChanged();

    return success;
}

void
GtQmlToolbarGroup::setListData(const std::vector<GtQmlAction *> &data)
{
    Base::setListData(data);

    for (auto* action : data)
    {
        if (!action) continue;
        connect(action, &GtQmlAction::visibleChanged, this,
                &GtQmlToolbarGroup::visibleChanged);
    }

    emit visibleChanged();
}
