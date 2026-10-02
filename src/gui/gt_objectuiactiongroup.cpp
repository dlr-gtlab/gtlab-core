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

struct GtObjectUIActionGroup::Impl
{
    explicit
    Impl(QString name_, QIcon icon_ = {}, GtActionList actions = {}) :
        actions(std::move(actions)),
        name(std::move(name_)),
        icon(std::move(icon_))
    { }

    /// List of actions
    QList<GtObjectUIAction> actions;

    /// Group action text
    QString name;

    /// Group action icon
    QIcon icon;

    /// order priority
    int priority{gt::gui::OrderPriority::Default};
};

GtObjectUIActionGroup::GtObjectUIActionGroup() :
    GtObjectUIActionGroup(QString{})
{ }

GtObjectUIActionGroup::GtObjectUIActionGroup(QString groupName) :
    GtObjectUIActionGroup(std::move(groupName), GtActionList{})
{ }

GtObjectUIActionGroup::GtObjectUIActionGroup(
        QString groupName,
        QList<GtObjectUIAction> actions) :
    pimpl(std::make_unique<Impl>(std::move(groupName), QIcon{}, std::move(actions)))
{ }

GtObjectUIActionGroup::GtObjectUIActionGroup(
        QString groupName,
        QList<GtObjectUIAction> actions,
        const QString& icon) :
    pimpl(std::make_unique<Impl>(std::move(groupName), gt::gui::getIcon(icon), std::move(actions)))
{ }

GtObjectUIActionGroup::GtObjectUIActionGroup(GtObjectUIActionGroup const& o) noexcept :
    pimpl(std::make_unique<Impl>(*o.pimpl))
{ }

GtObjectUIActionGroup::GtObjectUIActionGroup(GtObjectUIActionGroup&& o) noexcept :
    pimpl(std::make_unique<Impl>(std::move(*o.pimpl)))
{ }

GtObjectUIActionGroup&
GtObjectUIActionGroup::operator=(GtObjectUIActionGroup const& o) noexcept
{
    GtObjectUIActionGroup tmp{o};
    swap(tmp);
    return *this;
}

GtObjectUIActionGroup&
GtObjectUIActionGroup::operator=(GtObjectUIActionGroup&& o) noexcept
{
    GtObjectUIActionGroup tmp{std::move(o)};
    swap(tmp);
    return *this;
}

GtObjectUIActionGroup::~GtObjectUIActionGroup() noexcept = default;

const QList<GtObjectUIAction>&
GtObjectUIActionGroup::actions() const
{
    return pimpl->actions;
}

const QString&
GtObjectUIActionGroup::name() const
{
    return pimpl->name;
}

const QIcon&
GtObjectUIActionGroup::icon() const
{
    return pimpl->icon;
}

void
GtObjectUIActionGroup::reserve(int size)
{
    if (size > 0) pimpl->actions.reserve(size);
}

int
GtObjectUIActionGroup::orderPriority() const
{
    return pimpl->priority;
}

GtObjectUIActionGroup&
GtObjectUIActionGroup::setIcon(const QIcon& icon)
{
    pimpl->icon = icon;
    return *this;
}

GtObjectUIActionGroup&
GtObjectUIActionGroup::setIcon(const QString& icon)
{
    return setIcon(gt::gui::getIcon(icon));
}

GtObjectUIActionGroup&
GtObjectUIActionGroup::setOrderPriority(int priority)
{
    pimpl->priority = priority;
    return *this;
}

GtObjectUIActionGroup&
GtObjectUIActionGroup::operator<<(const GtObjectUIAction& action)
{
    return addAction(action);
}

GtObjectUIActionGroup&
GtObjectUIActionGroup::addAction(const GtObjectUIAction& action)
{
    pimpl->actions << action;
    return *this;
}

void
GtObjectUIActionGroup::swap(GtObjectUIActionGroup& other) noexcept
{
    std::swap(pimpl->name, other.pimpl->name);
    std::swap(pimpl->icon, other.pimpl->icon);
    std::swap(pimpl->actions, other.pimpl->actions);
    std::swap(pimpl->priority, other.pimpl->priority);
}
