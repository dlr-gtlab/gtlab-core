/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_gt_accessmanager.cpp
 */

#include "gtest/gtest.h"

#include <QNetworkAccessManager>

#include "gt_accessgroup.h"
#include "gt_accessmanager.h"

class TestGtAccessManager : public ::testing::Test
{
protected:
    void SetUp() override
    {
        manager = GtAccessManager::instance();
        ASSERT_NE(manager, nullptr);

        const int beforeCount = manager->numberOfDataGroups();
        firstGroupId = QString("network-test-group-%1").arg(beforeCount + 1);
        secondGroupId = QString("network-test-group-%1").arg(beforeCount + 2);
    }

    GtAccessManager* manager{nullptr};
    QString firstGroupId;
    QString secondGroupId;
};

TEST_F(TestGtAccessManager, instanceReturnsSingleton)
{
    EXPECT_EQ(GtAccessManager::instance(), manager);
}

TEST_F(TestGtAccessManager, defaultAccessGroupMakesManagerNonEmpty)
{
    EXPECT_FALSE(manager->isEmpty());
    EXPECT_GE(manager->numberOfDataGroups(), 1);
}

TEST_F(TestGtAccessManager, addAccessGroupRejectsInvalidAndDuplicateIds)
{
    EXPECT_EQ(manager->addAccessGroup(QString(), QObject::staticMetaObject), nullptr);

    GtAccessGroup* group = manager->addAccessGroup(firstGroupId, QObject::staticMetaObject);
    ASSERT_NE(group, nullptr);

    EXPECT_EQ(group->objectName(), firstGroupId);
    EXPECT_EQ(manager->addAccessGroup(firstGroupId, QObject::staticMetaObject), nullptr);
}

TEST_F(TestGtAccessManager, addingAccessGroupUpdatesLookupAndCount)
{
    const int beforeCount = manager->numberOfDataGroups();

    GtAccessGroup* group = manager->addAccessGroup(firstGroupId, QObject::staticMetaObject);
    ASSERT_NE(group, nullptr);

    EXPECT_EQ(manager->numberOfDataGroups(), beforeCount + 1);
    EXPECT_TRUE(manager->groupExists(firstGroupId));
    EXPECT_EQ(manager->accessGroup(firstGroupId), group);
    EXPECT_TRUE(manager->accessGroupIds().contains(firstGroupId));
    EXPECT_TRUE(manager->accessGroups().contains(group));
    EXPECT_EQ(manager->accessGroup(secondGroupId), nullptr);
}

TEST_F(TestGtAccessManager, qnamIsCreatedLazilyAndReused)
{
    QNetworkAccessManager* first = manager->qnam();
    ASSERT_NE(first, nullptr);

    EXPECT_EQ(manager->qnam(), first);
}

TEST_F(TestGtAccessManager, serializeRoundTripPreservesStringList)
{
    QStringList input;
    input << "host" << "22" << "user" << "password";

    QString serialized = GtAccessManager::serializeStringList(input);

    EXPECT_FALSE(serialized.isEmpty());
    EXPECT_EQ(GtAccessManager::deserializeStringList(serialized), input);
}
