/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_processdockextensions.h"
#include "internal/gt_processdockextensions_p.h"
#include "gt_task.h"
#include "gt_taskgroup.h"

#include <QComboBox>
#include <QMenu>
#include <QStackedWidget>

namespace
{

    class TestProcessDockPage : public GtProcessDockPage
    {
    public:
        QString pageId() const override
        {
            return QStringLiteral("test-process-dock-page");
        }

        QString title() const override
        {
            return QStringLiteral("Test page");
        }
    };

} // namespace

TEST(ProcessDockExtensions, createsAndActivatesRegisteredPage)
{
    QObject owner;
    gt::gui::registerProcessDockPage(&owner,
                                     [] { return new TestProcessDockPage; });

    QStackedWidget stack;
    stack.addWidget(new QWidget);
    QComboBox selector;
    selector.addItem(QStringLiteral("Processes"), QStringLiteral("processes"));

    gt::gui::detail::initializeProcessDockPages(&stack, &selector);

    ASSERT_EQ(stack.count(), 2);
    ASSERT_EQ(selector.count(), 2);
    EXPECT_FALSE(selector.isHidden());
    EXPECT_EQ(selector.itemData(1).toString(),
              QStringLiteral("test-process-dock-page"));

    selector.setCurrentIndex(1);
    EXPECT_EQ(stack.currentIndex(), 1);

    auto* page = dynamic_cast<TestProcessDockPage*>(stack.widget(1));
    ASSERT_NE(page, nullptr);
    selector.setCurrentIndex(0);
    page->activationRequested();
    EXPECT_EQ(selector.currentIndex(), 1);
}

TEST(ProcessDockExtensions, addActionReceivesOwningModuleAndTaskTarget)
{
    QObject owner;
    GtTask task;
    GtTaskGroup taskGroup;
    QObject* receivedOwner = nullptr;
    GtObject* receivedTarget = nullptr;

    GtObjectUIAction action(QStringLiteral("Add result"),
                            GtObjectUIAction::InvokableActionMethod(
                                [&receivedOwner, &receivedTarget](
                                    QObject* callbackOwner, GtObject* target) {
                                    receivedOwner = callbackOwner;
                                    receivedTarget = target;
                                }));
    action.setVisibilityMethod(GtObjectUIAction::InvokableVisibilityMethod(
        [&receivedOwner](QObject* callbackOwner, GtObject*) {
            receivedOwner = callbackOwner;
            return true;
        }));

    gt::gui::registerProcessAddAction(&owner, action);
    GtObject* selectedTask =
        gt::gui::detail::processAddTarget(&task, nullptr, &taskGroup);
    GtObject* selectedGroup =
        gt::gui::detail::processAddTarget(nullptr, nullptr, &taskGroup);
    EXPECT_EQ(selectedTask, &task);
    EXPECT_EQ(selectedGroup, &taskGroup);

    const auto actions = gt::gui::detail::processDockAddActions();
    ASSERT_TRUE(
        gt::gui::detail::hasVisibleProcessDockActions(actions, selectedTask));
    EXPECT_EQ(receivedOwner, &owner);

    QMenu menu;
    gt::gui::detail::addProcessDockActionsToMenu(actions, menu, selectedTask);
    ASSERT_EQ(menu.actions().size(), 1);
    menu.actions().first()->trigger();
    EXPECT_EQ(receivedOwner, &owner);
    EXPECT_EQ(receivedTarget, &task);
}

TEST(ProcessDockExtensions, contextActionReceivesClickedComponent)
{
    QObject owner;
    GtTask component;
    QObject* receivedOwner = nullptr;
    GtObject* receivedTarget = nullptr;

    gt::gui::registerProcessContextAction(
        &owner,
        GtObjectUIAction(QStringLiteral("Inspect process"),
                         GtObjectUIAction::InvokableActionMethod(
                             [&receivedOwner, &receivedTarget](
                                 QObject* callbackOwner, GtObject* target) {
                                 receivedOwner = callbackOwner;
                                 receivedTarget = target;
                             })));

    const auto actions = gt::gui::detail::processDockContextActions();
    QMenu menu;
    gt::gui::detail::addProcessDockActionsToMenu(actions, menu, &component);
    ASSERT_EQ(menu.actions().size(), 1);
    menu.actions().first()->trigger();
    EXPECT_EQ(receivedOwner, &owner);
    EXPECT_EQ(receivedTarget, &component);
}

TEST(ProcessDockExtensions, removesContributionsWhenOwnerIsDestroyed)
{
    {
        QObject owner;
        gt::gui::registerProcessAddAction(
            &owner,
            GtObjectUIAction(QStringLiteral("Temporary"), [](GtObject*) {}));
        EXPECT_FALSE(gt::gui::detail::processDockAddActions().isEmpty());
    }

    EXPECT_TRUE(gt::gui::detail::processDockAddActions().isEmpty());
}
