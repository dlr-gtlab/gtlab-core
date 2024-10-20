/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 */

#include <gtest/gtest.h>
#include <gt_qmltoolbargroup.h>

struct TestToolbarGroup : public testing::Test
{
    TestToolbarGroup()
    {
        // a1 | a2 | a3
        actions.push_back(new GtQmlAction("a1", QUrl(""), &group));
        actions.push_back(GtQmlAction::makeSeparator(&group));
        actions.push_back(new GtQmlAction("a2", QUrl(""), &group));
        actions.push_back(GtQmlAction::makeSeparator(&group));
        actions.push_back(new GtQmlAction("a3", QUrl(""), &group));

        for (auto& a : actions) group.append(a);
    }

    GtQmlToolbarGroup group;
    std::vector<GtQmlAction*> actions;
};

TEST_F(TestToolbarGroup, toolbarvisible)
{
    EXPECT_EQ(true, group.isVisible());
}

TEST_F(TestToolbarGroup, toolbarInvisible)
{
    // all invisible, hence no toolbar

    actions[0]->setVisible(false);
    actions[2]->setVisible(false);
    actions[4]->setVisible(false);

    EXPECT_EQ(true, !group.isVisible());
}

TEST_F(TestToolbarGroup, firstSeparatorInv)
{
    // a2 | a3
    actions[0]->setVisible(false);

    EXPECT_EQ(false, actions[1]->isVisible());
    EXPECT_EQ(true, actions[3]->isVisible());
    EXPECT_EQ(true, group.isVisible());
}

TEST_F(TestToolbarGroup, lastSeparatorInv)
{
    // a1 | a2
    actions[4]->setVisible(false);

    EXPECT_EQ(true, actions[1]->isVisible());
    EXPECT_EQ(false, actions[3]->isVisible());
    EXPECT_EQ(true, group.isVisible());
}

TEST_F(TestToolbarGroup, aggregateSeparators)
{
    // a1 | a3
    actions[2]->setVisible(false);

    EXPECT_EQ(true, actions[1]->isVisible());
    EXPECT_EQ(false, actions[3]->isVisible());
    EXPECT_EQ(true, group.isVisible());
}
