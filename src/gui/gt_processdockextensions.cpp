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

#include <QAction>
#include <QComboBox>
#include <QDockWidget>
#include <QMenuBar>
#include <QPointer>
#include <QMenu>
#include <QSet>
#include <QStackedWidget>
#include <QWidget>

#include <algorithm>
#include <utility>

namespace
{

QAction*
actionBefore(QMenu& menu, int priority)
{
    if (priority == gt::gui::OrderPriority::Last)
    {
        return nullptr;
    }

    const auto actions = menu.actions();
    if (actions.isEmpty())
    {
        return nullptr;
    }
    if (priority == gt::gui::OrderPriority::First)
    {
        return actions.first();
    }

    const auto iter = std::find_if(
        actions.cbegin(), actions.cend(), [priority](const QAction* action) {
            bool ok = false;
            const int existingPriority =
                action->property("orderPriority").toInt(&ok);
            return (ok ? existingPriority : 0) > priority;
        });
    return iter == actions.cend() ? nullptr : *iter;
}

QWidget*
findParentWidget(QWidget& menu)
{
    auto* parent = qobject_cast<QWidget*>(menu.parent());
    if (auto* parentMenu = qobject_cast<QMenu*>(parent))
    {
        return findParentWidget(*parentMenu);
    }
    if (auto* parentMenuBar = qobject_cast<QMenuBar*>(parent))
    {
        return findParentWidget(*parentMenuBar);
    }
    return parent;
}

QList<gt::gui::detail::ProcessDockPage>&
pageRegistrations()
{
    static QList<gt::gui::detail::ProcessDockPage> registrations;
    return registrations;
}

QList<gt::gui::detail::ProcessDockAction>&
addActionRegistrations()
{
    static QList<gt::gui::detail::ProcessDockAction> registrations;
    return registrations;
}

QList<gt::gui::detail::ProcessDockAction>&
contextActionRegistrations()
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
                                            QComboBox* selector,
                                            QDockWidget* dock)
{
    if (!stack || !selector || !dock)
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
        QObject::connect(page, &GtProcessDockPage::activationRequested, dock,
                         [selector, dock, pageIndex]() {
                             selector->setCurrentIndex(pageIndex);
                             dock->show();
                             dock->raise();
                         });
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

QList<QAction*>
gt::gui::detail::addProcessDockActionsToMenu(
    const QList<ProcessDockAction>& actions, QMenu& menu, GtObject* target)
{
    QList<QAction*> addedActions;

    for (const auto& entry : actions)
    {
        const QPointer<QObject> owner = entry.owner;
        if (!owner)
        {
            continue;
        }

        const auto& uiAction = entry.action;
        if (uiAction.isSeparator())
        {
            QAction* separator = menu.insertSeparator(
                actionBefore(menu, uiAction.orderPriority()));
            gt::gui::setOrderPriority(*separator, uiAction.orderPriority());
            continue;
        }

        const auto method = uiAction.method();
        if (!method)
        {
            continue;
        }

        auto* action = new QAction(uiAction.name(), &menu);
        gt::gui::setOrderPriority(*action, uiAction.orderPriority());
        menu.insertAction(actionBefore(menu, uiAction.orderPriority()), action);
        addedActions.append(action);

        if (!uiAction.icon().isNull())
        {
            action->setIcon(uiAction.icon());
        }

        if (!uiAction.shortCut().isEmpty())
        {
            action->setShortcut(uiAction.shortCut());
            action->setShortcutContext(Qt::ApplicationShortcut);
            if (QWidget* parent = findParentWidget(menu))
            {
                parent->addAction(action);
            }
        }

        const QPointer<GtObject> guardedTarget{target};
        const bool hasTarget = target != nullptr;
        const auto visibility = uiAction.visibilityMethod();
        const auto verification = uiAction.verificationMethod();

        const auto isAlive = [owner, guardedTarget, hasTarget]() {
            return owner && (!hasTarget || !guardedTarget.isNull());
        };
        const auto updateState = [owner, guardedTarget, hasTarget, visibility,
                                  verification, isAlive, action]() {
            if (!isAlive())
            {
                action->setEnabled(false);
                action->setVisible(false);
                return;
            }

            GtObject* liveTarget = hasTarget ? guardedTarget.data() : nullptr;
            if (visibility)
            {
                const bool isVisible = visibility(owner.data(), liveTarget);
                if (!isAlive())
                {
                    action->setEnabled(false);
                    action->setVisible(false);
                    return;
                }
                action->setVisible(isVisible);
            }
            if (verification)
            {
                const bool isEnabled = verification(
                    owner.data(), hasTarget ? guardedTarget.data() : nullptr);
                if (!isAlive())
                {
                    action->setEnabled(false);
                    action->setVisible(false);
                    return;
                }
                action->setEnabled(isEnabled);
            }
        };

        updateState();
        if (visibility || verification)
        {
            QObject::connect(&menu, &QMenu::aboutToShow, action, updateState);
        }

        QObject::connect(action, &QAction::triggered, &menu,
                         [owner, guardedTarget, hasTarget, isAlive, method]() {
                             if (!isAlive())
                             {
                                 return;
                             }
                             method(owner.data(),
                                    hasTarget ? guardedTarget.data() : nullptr);
                         });

        QObject::connect(owner.data(), &QObject::destroyed, action, [action]() {
            action->setEnabled(false);
            action->setVisible(false);
        });
        if (hasTarget)
        {
            QObject::connect(target, &QObject::destroyed, action, [action]() {
                action->setEnabled(false);
                action->setVisible(false);
            });
        }
    }

    if (!addedActions.isEmpty())
    {
        const QList<QAction*> menuActions = menu.actions();
        const auto firstAddedAction = std::find_if(
            menuActions.cbegin(), menuActions.cend(),
            [&addedActions](const QAction* menuAction) {
                return std::find(addedActions.cbegin(), addedActions.cend(),
                                 menuAction) != addedActions.cend();
            });

        if (firstAddedAction != menuActions.cend())
        {
            QAction* sectionSeparator = menu.insertSeparator(*firstAddedAction);
            const auto updateSeparator = [addedActions, sectionSeparator]() {
                const bool anyVisible = std::any_of(
                    addedActions.cbegin(), addedActions.cend(),
                    [](const QAction* action) { return action->isVisible(); });
                sectionSeparator->setVisible(anyVisible);
            };
            updateSeparator();
            QObject::connect(&menu, &QMenu::aboutToShow, sectionSeparator,
                             updateSeparator);
            for (const QAction* action : addedActions)
            {
                QObject::connect(action, &QAction::changed, sectionSeparator,
                                 updateSeparator);
            }
        }
    }

    return addedActions;
}
