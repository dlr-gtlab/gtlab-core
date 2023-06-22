/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_objectuiactiongroup.cpp
 *
 *  Created on: 28.09.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#include "gt_objectuiactiongroup.h"

#include "gt_icons.h"

GtObjectUIActionGroup::GtObjectUIActionGroup() = default;

GtObjectUIActionGroup::GtObjectUIActionGroup(
        const QString& groupName,
        const QList<GtObjectUIAction>& actions,
        const QString& icon) :
   m_name(groupName),
   m_icon(gt::gui::getIcon(icon))
{
    for (GtObjectUIAction const& action : actions)
    {
        m_actions.append(action);
    }
}

const QList<GtObjectUIAction>&
GtObjectUIActionGroup::actions() const
{
    return m_actions;
}

const QString&
GtObjectUIActionGroup::name() const
{
    return m_name;
}

const QIcon&
GtObjectUIActionGroup::icon() const
{
    return m_icon;
}

void
GtObjectUIActionGroup::reserve(int size)
{
    m_actions.reserve(size);
}

GtObjectUIActionGroup&
GtObjectUIActionGroup::setIcon(const QIcon& icon)
{
    m_icon = icon;
    return *this;
}

GtObjectUIActionGroup&
GtObjectUIActionGroup::setIcon(const QString& icon)
{
    return setIcon(gt::gui::getIcon(icon));
}

GtObjectUIActionGroup&
GtObjectUIActionGroup::operator<<(const GtObjectUIAction& action)
{
    m_actions << action;
    return *this;
}
