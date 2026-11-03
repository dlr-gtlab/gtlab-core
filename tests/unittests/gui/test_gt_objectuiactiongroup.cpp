/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_objectuiactiongroup.h"
#include "gt_objectuiaction.h"

#include <QIcon>

TEST(GtObjectUIActionGroup, copy_constructor)
{
    GtObjectUIAction action1;
    action1.setOrderPriority(gt::gui::OrderPriority::ExportAction);
    GtObjectUIAction action2;
    action2.setOrderPriority(gt::gui::OrderPriority::ImportAction);

    GtObjectUIActionGroup group{"TestGroup"};
    group.setOrderPriority(gt::gui::OrderPriority::First);
    group << action1 << action2;

    GtObjectUIActionGroup copy(group);

    EXPECT_EQ(copy.name(), group.name());
    EXPECT_EQ(copy.orderPriority(), group.orderPriority());
    EXPECT_EQ(copy.orderPriority(), gt::gui::OrderPriority::First);
    EXPECT_EQ(copy.actions().size(), group.actions().size());
    EXPECT_EQ(copy.actions().size(), 2);
}

TEST(GtObjectUIActionGroup, move_constructor)
{
    GtObjectUIAction action;
    action.setOrderPriority(gt::gui::OrderPriority::DeleteAction);

    GtObjectUIActionGroup group{"TestGroup"};
    group.setOrderPriority(gt::gui::OrderPriority::First);
    group << action;

    GtObjectUIActionGroup moved(std::move(group));

    EXPECT_EQ(moved.name(), "TestGroup");
    EXPECT_EQ(moved.actions().size(), 1);
    EXPECT_EQ(moved.orderPriority(), gt::gui::OrderPriority::First);

    EXPECT_TRUE(group.name().isEmpty());
    EXPECT_TRUE(group.actions().isEmpty());
}

TEST(GtObjectUIActionGroup, copy_assignment)
{
    GtObjectUIAction action1;
    action1.setOrderPriority(gt::gui::OrderPriority::ExportAction);
    GtObjectUIAction action2;
    action2.setOrderPriority(gt::gui::OrderPriority::ImportAction);

    GtObjectUIActionGroup group("TestGroup");
    group.setOrderPriority(gt::gui::OrderPriority::First);
    group << action1 << action2;

    GtObjectUIActionGroup copy("TestGroup2");
    copy.setOrderPriority(gt::gui::OrderPriority::Last);
    copy = group;

    EXPECT_EQ(copy.name(), group.name());
    EXPECT_EQ(copy.orderPriority(), group.orderPriority());
    EXPECT_EQ(copy.orderPriority(), gt::gui::OrderPriority::First);
    EXPECT_EQ(copy.actions().size(), group.actions().size());
    EXPECT_EQ(copy.actions().size(), 2);
}

TEST(GtObjectUIActionGroup, move_assignment)
{
    GtObjectUIAction action;
    action.setOrderPriority(gt::gui::OrderPriority::DeleteAction);

    GtObjectUIActionGroup group{"TestGroup"};
    group.setOrderPriority(gt::gui::OrderPriority::First);
    group << action;

    GtObjectUIActionGroup moved("TestGroup2");
    moved.setOrderPriority(gt::gui::OrderPriority::Last);
    moved = std::move(group);

    EXPECT_EQ(moved.name(), "TestGroup");
    EXPECT_EQ(moved.actions().size(), 1);
    EXPECT_EQ(moved.orderPriority(), gt::gui::OrderPriority::First);

    EXPECT_TRUE(group.name().isEmpty());
    EXPECT_TRUE(group.actions().isEmpty());
}

TEST(GtObjectUIActionGroup, add_action)
{
    GtObjectUIActionGroup group("TestGroup");

    GtObjectUIAction action1;
    action1.setOrderPriority(gt::gui::OrderPriority::ExportAction);
    GtObjectUIAction action2;
    action2.setOrderPriority(gt::gui::OrderPriority::ImportAction);

    group.addAction(action1);
    group.addAction(action2);

    EXPECT_EQ(group.actions().size(), 2);
}

TEST(GtObjectUIActionGroup, add_action_perator)
{
    GtObjectUIActionGroup group("TestGroup");

    GtObjectUIAction action1;
    action1.setOrderPriority(gt::gui::OrderPriority::DeleteAction);
    GtObjectUIAction action2;
    action2.setOrderPriority(gt::gui::OrderPriority::RenameAction);

    group << action1 << action2;

    EXPECT_EQ(group.actions().size(), 2);
}
