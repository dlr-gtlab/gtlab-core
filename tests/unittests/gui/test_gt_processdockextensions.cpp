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

#include <QAction>
#include <QComboBox>
#include <QDockWidget>
#include <QMainWindow>
#include <QMenu>
#include <QMetaObject>
#include <QStackedWidget>
#include <QVBoxLayout>

#include <utility>

namespace
{

class TestProcessDockPage : public GtProcessDockPage
{
public:
    explicit TestProcessDockPage(
        QString id = QStringLiteral("test-process-dock-page"),
        QString pageTitle = QStringLiteral("Test page")) :
        m_id(std::move(id)),
        m_title(std::move(pageTitle))
    { }

    QString pageId() const override
    {
        return m_id;
    }

    QString title() const override
    {
        return m_title;
    }

private:
    QString m_id;
    QString m_title;
};

GtObjectUIAction
makeInvokableAction(const QString& name,
                    GtObjectUIAction::InvokableActionMethod method)
{
    return GtObjectUIAction(name, std::move(method));
}

} // namespace

TEST(ProcessDockExtensions, createsAndActivatesRegisteredPage)
{
    QObject owner;
    gt::gui::registerProcessDockPage(&owner,
                                     [] { return new TestProcessDockPage; });

    QMainWindow window;
    auto* dock = new QDockWidget(&window);
    auto* otherDock = new QDockWidget(QStringLiteral("Other dock"), &window);
    window.addDockWidget(Qt::RightDockWidgetArea, dock);
    window.addDockWidget(Qt::RightDockWidgetArea, otherDock);
    window.tabifyDockWidget(dock, otherDock);

    auto* pageHost = new QWidget;
    auto* layout = new QVBoxLayout(pageHost);
    auto* selector = new QComboBox(pageHost);
    auto* stack = new QStackedWidget(pageHost);
    layout->addWidget(selector);
    layout->addWidget(stack);
    stack->addWidget(new QWidget);
    selector->addItem(QStringLiteral("Processes"), QStringLiteral("processes"));
    dock->setWidget(pageHost);

    window.show();
    otherDock->raise();
    dock->hide();

    ASSERT_TRUE(window.tabifiedDockWidgets(dock).contains(otherDock));
    gt::gui::detail::initializeProcessDockPages(stack, selector, dock);

    ASSERT_EQ(stack->count(), 2);
    ASSERT_EQ(selector->count(), 2);
    EXPECT_FALSE(selector->isHidden());
    EXPECT_EQ(selector->itemData(1).toString(),
              QStringLiteral("test-process-dock-page"));

    selector->setCurrentIndex(1);
    EXPECT_EQ(stack->currentIndex(), 1);

    auto* page = dynamic_cast<TestProcessDockPage*>(stack->widget(1));
    ASSERT_NE(page, nullptr);
    selector->setCurrentIndex(0);
    page->activationRequested();
    EXPECT_EQ(selector->currentIndex(), 1);
    EXPECT_TRUE(dock->isVisible());
}

TEST(ProcessDockExtensions, rejectsInvalidAndDuplicatePages)
{
    QObject owner;
    gt::gui::registerProcessDockPage(&owner, [] { return nullptr; });
    gt::gui::registerProcessDockPage(&owner, [] {
        return new TestProcessDockPage(QStringLiteral(" "),
                                       QStringLiteral("Blank ID"));
    });
    gt::gui::registerProcessDockPage(&owner, [] {
        return new TestProcessDockPage(QStringLiteral("no-title"),
                                       QStringLiteral(" "));
    });
    gt::gui::registerProcessDockPage(&owner, [] {
        return new TestProcessDockPage(QStringLiteral("processes"),
                                       QStringLiteral("Reserved"));
    });
    gt::gui::registerProcessDockPage(&owner, [] {
        return new TestProcessDockPage(QStringLiteral("unique-page"),
                                       QStringLiteral("First"));
    });
    gt::gui::registerProcessDockPage(&owner, [] {
        return new TestProcessDockPage(QStringLiteral("unique-page"),
                                       QStringLiteral("Duplicate"));
    });

    QStackedWidget stack;
    stack.addWidget(new QWidget);
    QComboBox selector;
    selector.addItem(QStringLiteral("Processes"), QStringLiteral("processes"));
    QDockWidget dock;

    gt::gui::detail::initializeProcessDockPages(&stack, &selector, &dock);

    EXPECT_EQ(stack.count(), 2);
    EXPECT_EQ(selector.count(), 2);
    EXPECT_EQ(selector.itemData(1).toString(), QStringLiteral("unique-page"));
}

TEST(ProcessDockExtensions, hidesSelectorWhenOnlyBuiltInPageExists)
{
    QStackedWidget stack;
    stack.addWidget(new QWidget);
    QComboBox selector;
    selector.addItem(QStringLiteral("Processes"), QStringLiteral("processes"));
    selector.show();
    QDockWidget dock;

    gt::gui::detail::initializeProcessDockPages(&stack, &selector, &dock);

    EXPECT_TRUE(selector.isHidden());
}

TEST(ProcessDockExtensions, addActionReceivesClickedTaskAndOwningModule)
{
    QObject owner;
    GtTask selectedTask;
    GtTask clickedTask;
    GtTaskGroup taskGroup;
    QObject* receivedOwner = nullptr;
    GtObject* receivedTarget = nullptr;
    int visibilityCalls = 0;

    auto action =
        makeInvokableAction(QStringLiteral("Add result"),
                            [&receivedOwner, &receivedTarget](
                                QObject* callbackOwner, GtObject* target) {
                                receivedOwner = callbackOwner;
                                receivedTarget = target;
                            });
    action.setVisibilityMethod([&visibilityCalls](QObject*, GtObject*) {
        ++visibilityCalls;
        return true;
    });

    gt::gui::registerProcessAddAction(&owner, action);

    GtObject* target = gt::gui::detail::processAddTarget(
        &clickedTask, &selectedTask, &taskGroup);
    EXPECT_EQ(target, &clickedTask);

    QMenu menu;
    const auto addedActions = gt::gui::detail::addProcessDockActionsToMenu(
        gt::gui::detail::processDockAddActions(), menu, target);
    ASSERT_EQ(addedActions.size(), 1);
    EXPECT_EQ(visibilityCalls, 1);
    addedActions.first()->trigger();

    EXPECT_EQ(receivedOwner, &owner);
    EXPECT_EQ(receivedTarget, &clickedTask);
}

TEST(ProcessDockExtensions, contextActionReceivesClickedComponent)
{
    QObject owner;
    GtTask selectedComponent;
    GtTask clickedComponent;
    GtObject* receivedTarget = nullptr;

    gt::gui::registerProcessContextAction(
        &owner,
        makeInvokableAction(QStringLiteral("Inspect process"),
                            [&receivedTarget](QObject*, GtObject* target) {
                                receivedTarget = target;
                            }));

    QMenu menu;
    const auto addedActions = gt::gui::detail::addProcessDockActionsToMenu(
        gt::gui::detail::processDockContextActions(), menu, &clickedComponent);
    ASSERT_EQ(addedActions.size(), 1);
    addedActions.first()->trigger();

    EXPECT_NE(receivedTarget, &selectedComponent);
    EXPECT_EQ(receivedTarget, &clickedComponent);
}

TEST(ProcessDockExtensions, taskGroupFallbackCanBeTriggered)
{
    QObject owner;
    GtTaskGroup taskGroup;
    GtObject* receivedTarget = nullptr;

    gt::gui::registerProcessAddAction(
        &owner,
        makeInvokableAction(QStringLiteral("Add result"),
                            [&receivedTarget](QObject*, GtObject* target) {
                                receivedTarget = target;
                            }));

    GtObject* target =
        gt::gui::detail::processAddTarget(nullptr, nullptr, &taskGroup);
    ASSERT_EQ(target, &taskGroup);

    QMenu menu;
    const auto addedActions = gt::gui::detail::addProcessDockActionsToMenu(
        gt::gui::detail::processDockAddActions(), menu, target);
    ASSERT_EQ(addedActions.size(), 1);
    addedActions.first()->trigger();

    EXPECT_EQ(receivedTarget, &taskGroup);
}

TEST(ProcessDockExtensions, updatesVisibilityAndVerificationPredicatesOnce)
{
    QObject owner;
    GtTask target;
    bool visible = false;
    bool enabled = false;
    int visibilityCalls = 0;

    auto action = makeInvokableAction(QStringLiteral("Conditional action"),
                                      [](QObject*, GtObject*) { });
    action.setVisibilityMethod([&](QObject*, GtObject*) {
        ++visibilityCalls;
        return visible;
    });
    action.setVerificationMethod([&](QObject*, GtObject*) { return enabled; });
    gt::gui::registerProcessContextAction(&owner, action);

    QMenu menu;
    const auto addedActions = gt::gui::detail::addProcessDockActionsToMenu(
        gt::gui::detail::processDockContextActions(), menu, &target);
    ASSERT_EQ(addedActions.size(), 1);
    QAction* contributedAction = addedActions.first();

    EXPECT_EQ(visibilityCalls, 1);
    EXPECT_FALSE(contributedAction->isVisible());
    EXPECT_FALSE(contributedAction->isEnabled());

    visible = true;
    enabled = true;
    ASSERT_TRUE(
        QMetaObject::invokeMethod(&menu, "aboutToShow", Qt::DirectConnection));
    EXPECT_EQ(visibilityCalls, 2);
    EXPECT_TRUE(contributedAction->isVisible());
    EXPECT_TRUE(contributedAction->isEnabled());

    visible = false;
    ASSERT_TRUE(
        QMetaObject::invokeMethod(&menu, "aboutToShow", Qt::DirectConnection));
    EXPECT_EQ(visibilityCalls, 3);
    EXPECT_FALSE(contributedAction->isVisible());
}

TEST(ProcessDockExtensions, disablesAndGuardsActionsAfterOwnerDestruction)
{
    int calls = 0;
    auto* owner = new QObject;
    auto* target = new GtTask;
    int predicateCalls = 0;
    auto registeredAction =
        makeInvokableAction(QStringLiteral("Temporary"),
                            [&calls](QObject*, GtObject*) { ++calls; });
    registeredAction.setVisibilityMethod(
        [&predicateCalls](QObject*, GtObject*) {
            ++predicateCalls;
            return true;
        });
    gt::gui::registerProcessAddAction(owner, registeredAction);

    QMenu menu;
    const auto addedActions = gt::gui::detail::addProcessDockActionsToMenu(
        gt::gui::detail::processDockAddActions(), menu, target);
    ASSERT_EQ(addedActions.size(), 1);
    QAction* action = addedActions.first();

    delete owner;
    EXPECT_FALSE(action->isEnabled());
    EXPECT_FALSE(action->isVisible());
    ASSERT_TRUE(
        QMetaObject::invokeMethod(&menu, "aboutToShow", Qt::DirectConnection));
    EXPECT_EQ(predicateCalls, 1);

    // QAction::trigger() normally ignores disabled actions. Re-enable it to
    // verify that the callback itself also checks the lifetime guard.
    action->setEnabled(true);
    action->setVisible(true);
    action->trigger();
    EXPECT_EQ(calls, 0);

    delete target;
}

TEST(ProcessDockExtensions, disablesAndGuardsActionsAfterTargetDestruction)
{
    QObject owner;
    int calls = 0;
    auto* target = new GtTask;
    gt::gui::registerProcessContextAction(
        &owner,
        makeInvokableAction(QStringLiteral("Inspect"),
                            [&calls](QObject*, GtObject*) { ++calls; }));

    QMenu menu;
    const auto addedActions = gt::gui::detail::addProcessDockActionsToMenu(
        gt::gui::detail::processDockContextActions(), menu, target);
    ASSERT_EQ(addedActions.size(), 1);
    QAction* action = addedActions.first();

    delete target;
    EXPECT_FALSE(action->isEnabled());
    EXPECT_FALSE(action->isVisible());

    action->setEnabled(true);
    action->setVisible(true);
    action->trigger();
    EXPECT_EQ(calls, 0);
}

TEST(ProcessDockExtensions, allowsContributedActionsWithNullTarget)
{
    QObject owner;
    int calls = 0;
    bool receivedNullTarget = false;
    gt::gui::registerProcessAddAction(
        &owner, makeInvokableAction(
                    QStringLiteral("Global action"),
                    [&calls, &receivedNullTarget](QObject*, GtObject* target) {
                        ++calls;
                        receivedNullTarget = target == nullptr;
                    }));

    QMenu menu;
    const auto addedActions = gt::gui::detail::addProcessDockActionsToMenu(
        gt::gui::detail::processDockAddActions(), menu, nullptr);
    ASSERT_EQ(addedActions.size(), 1);
    EXPECT_TRUE(addedActions.first()->isEnabled());
    EXPECT_TRUE(addedActions.first()->isVisible());
    addedActions.first()->trigger();

    EXPECT_EQ(calls, 1);
    EXPECT_TRUE(receivedNullTarget);
}

} // namespace
