/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_gt_accessmanager.cpp
 */

#include "gtest/gtest.h"

#include "gt_accessgroup.h"
#include "gt_accessmanager.h"

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QTemporaryDir>
#include <QUuid>

#include <memory>

class TestGtAccessManager : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        configDir = std::make_unique<QTemporaryDir>();
        ASSERT_TRUE(configDir->isValid());

        previousConfigHome = qgetenv("XDG_CONFIG_HOME");
        qputenv("XDG_CONFIG_HOME", configDir->path().toUtf8());
    }

    static void TearDownTestSuite()
    {
        if (previousConfigHome.isEmpty())
        {
            qunsetenv("XDG_CONFIG_HOME");
        }
        else
        {
            qputenv("XDG_CONFIG_HOME", previousConfigHome);
        }

        configDir.reset();
    }

    void SetUp() override
    {
        manager = GtAccessManager::instance();
        ASSERT_NE(manager, nullptr);

        firstGroupId = uniqueGroupId("network-test-group");
        secondGroupId = uniqueGroupId("network-test-group");
    }

    static QString uniqueGroupId(const QString& prefix)
    {
        return QString("%1-%2").arg(
            prefix, QUuid::createUuid().toString(QUuid::WithoutBraces));
    }

    static QString accessFileName(QString groupId)
    {
        groupId.replace(QStringLiteral(" "), QStringLiteral("_"));
        return groupId.toLower() + QStringLiteral(".gtacc");
    }

    static QString accessDirPath()
    {
        return configDir->path() + QDir::separator() + QStringLiteral("access");
    }

    static void writeAccessFile(const QString& groupId,
                                const QStringList& entries)
    {
        QDir dir;
        ASSERT_TRUE(dir.mkpath(accessDirPath()));

        QFile file(accessDirPath() + QDir::separator() +
                   accessFileName(groupId));
        ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));

        QDataStream out(&file);
        out << GtAccessManager::serializeStringList(entries);
    }

    GtAccessManager* manager{nullptr};
    QString firstGroupId;
    QString secondGroupId;

    static std::unique_ptr<QTemporaryDir> configDir;
    static QByteArray previousConfigHome;
};

std::unique_ptr<QTemporaryDir> TestGtAccessManager::configDir;
QByteArray TestGtAccessManager::previousConfigHome;

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
    EXPECT_EQ(manager->addAccessGroup(QString(), QObject::staticMetaObject),
              nullptr);

    GtAccessGroup* group =
        manager->addAccessGroup(firstGroupId, QObject::staticMetaObject);
    ASSERT_NE(group, nullptr);

    EXPECT_EQ(group->objectName(), firstGroupId);
    EXPECT_EQ(manager->addAccessGroup(firstGroupId, QObject::staticMetaObject),
              nullptr);
}

TEST_F(TestGtAccessManager, addingAccessGroupUpdatesLookupAndCount)
{
    const int beforeCount = manager->numberOfDataGroups();

    GtAccessGroup* group =
        manager->addAccessGroup(firstGroupId, QObject::staticMetaObject);
    ASSERT_NE(group, nullptr);

    EXPECT_EQ(manager->numberOfDataGroups(), beforeCount + 1);
    EXPECT_TRUE(manager->groupExists(firstGroupId));
    EXPECT_EQ(manager->accessGroup(firstGroupId), group);
    EXPECT_TRUE(manager->accessGroupIds().contains(firstGroupId));
    EXPECT_TRUE(manager->accessGroups().contains(group));
    EXPECT_EQ(manager->accessGroup(secondGroupId), nullptr);
}

TEST_F(TestGtAccessManager, addAccessGroupLoadsPersistedEntries)
{
    const QString persistedGroupId = uniqueGroupId("Network Test Group");
    writeAccessFile(persistedGroupId,
                    {persistedGroupId, QStringLiteral("host-a"),
                     QStringLiteral("22"), QStringLiteral("user-a"),
                     QStringLiteral("pw-a"), QStringLiteral("host-b"),
                     QStringLiteral("2022"), QStringLiteral("user-b"),
                     QStringLiteral("pw-b")});

    GtAccessGroup* group =
        manager->addAccessGroup(persistedGroupId, QObject::staticMetaObject);
    ASSERT_NE(group, nullptr);
    ASSERT_EQ(group->numberOfAccessData(), 2);

    const auto& data = group->accessData();
    EXPECT_EQ(data.at(0).host(), QStringLiteral("host-a"));
    EXPECT_EQ(data.at(0).port(), 22);
    EXPECT_EQ(data.at(0).user(), QStringLiteral("user-a"));
    EXPECT_EQ(data.at(0).password(), QStringLiteral("pw-a"));
    EXPECT_EQ(data.at(1).host(), QStringLiteral("host-b"));
    EXPECT_EQ(data.at(1).port(), 2022);
    EXPECT_EQ(data.at(1).user(), QStringLiteral("user-b"));
    EXPECT_EQ(data.at(1).password(), QStringLiteral("pw-b"));
}

TEST_F(TestGtAccessManager, addAccessGroupIgnoresInvalidPersistedEntries)
{
    const QString persistedGroupId = uniqueGroupId("invalid-group");
    writeAccessFile(
        persistedGroupId,
        {persistedGroupId, QStringLiteral("host-only"), QStringLiteral("22")});

    GtAccessGroup* group =
        manager->addAccessGroup(persistedGroupId, QObject::staticMetaObject);
    ASSERT_NE(group, nullptr);
    EXPECT_EQ(group->numberOfAccessData(), 0);
}

TEST_F(TestGtAccessManager, saveAccessDataWritesGroupFiles)
{
    const QString persistedGroupId = uniqueGroupId("Save Group With Spaces");
    GtAccessGroup* group =
        manager->addAccessGroup(persistedGroupId, QObject::staticMetaObject);
    ASSERT_NE(group, nullptr);
    ASSERT_TRUE(group->addAccessData(QStringLiteral("save-host"), 4242,
                                     QStringLiteral("save-user"),
                                     QStringLiteral("save-pw")));

    EXPECT_TRUE(manager->saveAccessData());

    QFile file(accessDirPath() + QDir::separator() +
               accessFileName(persistedGroupId));
    ASSERT_TRUE(file.exists());
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));

    QDataStream in(&file);
    QString serialized;
    in >> serialized;

    EXPECT_EQ(GtAccessManager::deserializeStringList(serialized),
              QStringList({persistedGroupId, QStringLiteral("save-host"),
                           QStringLiteral("4242"), QStringLiteral("save-user"),
                           QStringLiteral("save-pw")}));
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
    input << "host"
          << "22"
          << "user"
          << "password";

    QString serialized = GtAccessManager::serializeStringList(input);

    EXPECT_FALSE(serialized.isEmpty());
    EXPECT_EQ(GtAccessManager::deserializeStringList(serialized), input);
}

TEST_F(TestGtAccessManager, macAddressReturnsAStableValueOrEmptyString)
{
    const QString first = manager->macAddress();
    const QString second = manager->macAddress();

    EXPECT_EQ(first, second);
}
