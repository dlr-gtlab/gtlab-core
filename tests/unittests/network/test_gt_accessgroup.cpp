/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_gt_accessgroup.cpp
 */

#include "gtest/gtest.h"

#include "gt_accessgroup.h"

/// This is a test fixture that does a init for each test
class TestGtAccessGroup : public ::testing::Test
{
protected:
    GtAccessGroup group{"group", QObject::staticMetaObject};
};

TEST_F(TestGtAccessGroup, addAccessData)
{
    EXPECT_FALSE(group.addAccessData("", 22, "user"));

    EXPECT_TRUE(group.addAccessData("host", 22, "user", "pw"));
    EXPECT_FALSE(group.addAccessData("host", 23, "other", "other"));

    EXPECT_EQ(group.numberOfAccessData(), 1);
    EXPECT_TRUE(group.hostExists("host"));
}

TEST_F(TestGtAccessGroup, removeAccessData)
{
    EXPECT_TRUE(group.addAccessData("host", 22, "user", "pw"));

    EXPECT_FALSE(group.removeAccessData(-1));
    EXPECT_FALSE(group.removeAccessData(1));
    EXPECT_TRUE(group.removeAccessData(0));

    EXPECT_EQ(group.numberOfAccessData(), 0);
    EXPECT_FALSE(group.hostExists("host"));
}

TEST_F(TestGtAccessGroup, setAccessData)
{
    EXPECT_TRUE(group.addAccessData("host", 22, "user", "pw"));

    GtAccessData newData("other", 42, "newuser", "newpw");

    EXPECT_FALSE(group.setAccessData(newData, -1));
    EXPECT_FALSE(group.setAccessData(newData, 1));
    EXPECT_TRUE(group.setAccessData(newData, 0));

    EXPECT_FALSE(group.hostExists("host"));
    EXPECT_TRUE(group.hostExists("other"));
    EXPECT_EQ(group.accessData().at(0).port(), 42);
}

TEST_F(TestGtAccessGroup, connectionMetaData)
{
    EXPECT_EQ(group.connectionMetaData().className(),
              QObject::staticMetaObject.className());
}
