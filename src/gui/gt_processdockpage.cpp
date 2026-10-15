/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_processdockpage.h"

#include <utility>

namespace
{

    QList<gt::gui::ProcessDockPageFactory>& pageFactories()
    {
        static QList<gt::gui::ProcessDockPageFactory> factories;
        return factories;
    }

    QList<GtObjectUIAction>& addActions()
    {
        static QList<GtObjectUIAction> actions;
        return actions;
    }

    QList<GtObjectUIAction>& contextActions()
    {
        static QList<GtObjectUIAction> actions;
        return actions;
    }

} // namespace

void
gt::gui::registerProcessDockPage(ProcessDockPageFactory factory)
{
    if (factory)
    {
        pageFactories().append(std::move(factory));
    }
}

void
gt::gui::registerProcessAddAction(GtObjectUIAction action)
{
    if (!action.isEmpty())
    {
        addActions().append(std::move(action));
    }
}

void
gt::gui::registerProcessContextAction(GtObjectUIAction action)
{
    if (!action.isEmpty())
    {
        contextActions().append(std::move(action));
    }
}

const QList<gt::gui::ProcessDockPageFactory>&
gt::gui::processDockPageFactories()
{
    return pageFactories();
}

const QList<GtObjectUIAction>&
gt::gui::processAddActions()
{
    return addActions();
}

const QList<GtObjectUIAction>&
gt::gui::processContextActions()
{
    return contextActions();
}
