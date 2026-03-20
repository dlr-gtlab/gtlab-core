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
    ASSERT_FALSE(group.addAccessData("", 22, "user"));

    ASSERT_TRUE(group.addAccessData("host", 22, "user", "pw"));
    ASSERT_FALSE(group.addAccessData("host", 23, "other", "other"));

    ASSERT_EQ(group.numberOfAccessData(), 1);
    ASSERT_TRUE(group.hostExists("host"));
}

TEST_F(TestGtAccessGroup, removeAccessData)
{
    ASSERT_TRUE(group.addAccessData("host", 22, "user", "pw"));

    ASSERT_FALSE(group.removeAccessData(-1));
    ASSERT_FALSE(group.removeAccessData(1));
    ASSERT_TRUE(group.removeAccessData(0));

    ASSERT_EQ(group.numberOfAccessData(), 0);
    ASSERT_FALSE(group.hostExists("host"));
}

TEST_F(TestGtAccessGroup, setAccessData)
{
    ASSERT_TRUE(group.addAccessData("host", 22, "user", "pw"));

    GtAccessData newData("other", 42, "newuser", "newpw");

    ASSERT_FALSE(group.setAccessData(newData, -1));
    ASSERT_FALSE(group.setAccessData(newData, 1));
    ASSERT_TRUE(group.setAccessData(newData, 0));

    ASSERT_FALSE(group.hostExists("host"));
    ASSERT_TRUE(group.hostExists("other"));
    ASSERT_EQ(group.accessData().at(0).port(), 42);
}

TEST_F(TestGtAccessGroup, connectionMetaData)
{
    ASSERT_EQ(group.connectionMetaData().className(),
              QObject::staticMetaObject.className());
}
