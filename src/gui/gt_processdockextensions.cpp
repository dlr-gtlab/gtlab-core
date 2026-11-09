/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_processdockextensions.h"

#include "internal/gt_processdockextensions_p.h"
#include "gt_guiutilities.h"
#include "gt_logging.h"
#include "gt_task.h"
#include "gt_taskgroup.h"

#include <QComboBox>
#include <QMenu>
#include <QSet>
#include <QStackedWidget>

#include <algorithm>
#include <utility>

namespace
{

    QList<gt::gui::detail::ProcessDockPage>& pageRegistrations()
    {
        static QList<gt::gui::detail::ProcessDockPage> registrations;
        return registrations;
    }

    QList<gt::gui::detail::ProcessDockAction>& addActionRegistrations()
    {
        static QList<gt::gui::detail::ProcessDockAction> registrations;
        return registrations;
    }

    QList<gt::gui::detail::ProcessDockAction>& contextActionRegistrations()
    {
        static QList<gt::gui::detail::ProcessDockAction> registrations;
        return registrations;
    }

} // namespace

void
gt::gui::registerProcessDockPage(QObject* owner, ProcessDockPageFactory factory)
{
    if (owner && factory)
    {
        pageRegistrations().append({owner, std::move(factory)});
    }
}

void
gt::gui::registerProcessAddAction(QObject* owner, GtObjectUIAction action)
{
    if (owner && !action.isEmpty())
    {
        addActionRegistrations().append({owner, std::move(action)});
    }
}

void
gt::gui::registerProcessContextAction(QObject* owner, GtObjectUIAction action)
{
    if (owner && !action.isEmpty())
    {
        contextActionRegistrations().append({owner, std::move(action)});
    }
}

QList<gt::gui::detail::ProcessDockPage>
gt::gui::detail::processDockPages()
{
    auto& registrations = pageRegistrations();
    registrations.erase(std::remove_if(registrations.begin(),
                                       registrations.end(),
                                       [](const auto& registration) {
                                           return registration.owner.isNull();
                                       }),
                        registrations.end());
    return registrations;
}

QList<gt::gui::detail::ProcessDockAction>
gt::gui::detail::processDockAddActions()
{
    auto& registrations = addActionRegistrations();
    registrations.erase(std::remove_if(registrations.begin(),
                                       registrations.end(),
                                       [](const auto& registration) {
                                           return registration.owner.isNull();
                                       }),
                        registrations.end());
    return registrations;
}

QList<gt::gui::detail::ProcessDockAction>
gt::gui::detail::processDockContextActions()
{
    auto& registrations = contextActionRegistrations();
    registrations.erase(std::remove_if(registrations.begin(),
                                       registrations.end(),
                                       [](const auto& registration) {
                                           return registration.owner.isNull();
                                       }),
                        registrations.end());
    return registrations;
}

void
gt::gui::detail::initializeProcessDockPages(QStackedWidget* stack,
                                            QComboBox* selector)
{
    if (!stack || !selector)
    {
        return;
    }

    QObject::connect(selector,
                     QOverload<int>::of(&QComboBox::currentIndexChanged), stack,
                     &QStackedWidget::setCurrentIndex, Qt::UniqueConnection);

    QSet<QString> pageIds{QStringLiteral("processes")};
    for (const auto& registration : processDockPages())
    {
        GtProcessDockPage* page = registration.factory();
        if (!page)
        {
            gtWarning()
                << "Ignoring a Process Dock page factory that returned null.";
            continue;
        }

        const QString id = page->pageId().trimmed();
        const QString title = page->title().trimmed();
        if (id.isEmpty() || title.isEmpty())
        {
            gtWarning()
                << "Ignoring a Process Dock page with an empty ID or title.";
            delete page;
            continue;
        }

        if (pageIds.contains(id))
        {
            gtWarning() << "Ignoring duplicate Process Dock page ID:" << id;
            delete page;
            continue;
        }
        pageIds.insert(id);

        const int pageIndex = stack->addWidget(page);
        selector->addItem(page->icon(), page->title(), id);
        QObject::connect(
            page, &GtProcessDockPage::activationRequested, selector,
            [selector, pageIndex]() { selector->setCurrentIndex(pageIndex); });
    }

    selector->setVisible(stack->count() > 1);
}

GtObject*
gt::gui::detail::processAddTarget(GtObject* selectedComponent,
                                  GtObject* selectedObject,
                                  GtTaskGroup* taskGroup)
{
    GtObject* context = selectedComponent ? selectedComponent : selectedObject;
    return qobject_cast<GtTask*>(context) ? context : taskGroup;
}

bool
gt::gui::detail::hasVisibleProcessDockActions(
    const QList<ProcessDockAction>& actions, GtObject* target)
{
    for (const auto& entry : actions)
    {
        const auto& action = entry.action;
        if (entry.owner && !action.isEmpty() && action.method() &&
            (!action.visibilityMethod() ||
             action.visibilityMethod()(entry.owner, target)))
        {
            return true;
        }
    }

    return false;
}

void
gt::gui::detail::addProcessDockActionsToMenu(
    const QList<ProcessDockAction>& actions, QMenu& menu, GtObject* target)
{
    for (const auto& entry : actions)
    {
        if (entry.owner)
        {
            gt::gui::addToMenu(entry.action, menu, target, entry.owner);
        }
    }
}
