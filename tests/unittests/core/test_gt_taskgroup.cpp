/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 * Source File: test_gt_taskgroup.cpp
 */

#include "gtest/gtest.h"

#include <QDomDocument>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>

#include "gt_calculator.h"
#include "gt_objectfactory.h"
#include "gt_processfactory.h"
#include "gt_task.h"
#include "gt_taskgroup.h"
#include "gt_xmlutilities.h"

class MetadataTestTask : public GtTask
{
    Q_OBJECT

public:
    Q_INVOKABLE MetadataTestTask() = default;
};

class MetadataTestCalculator : public GtCalculator
{
    Q_OBJECT

public:
    Q_INVOKABLE MetadataTestCalculator() = default;

    bool run() override
    {
        return true;
    }
};

class MetadataTestObject : public GtObject
{
    Q_OBJECT

public:
    Q_INVOKABLE MetadataTestObject() = default;
};

class GtTaskGroupMetadataTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_TRUE(gtProcessFactory->taskFactory()->registerClass(
            MetadataTestTask::staticMetaObject, QStringLiteral("TaskModule")));
        ASSERT_TRUE(gtProcessFactory->calculatorFactory()->registerClass(
            MetadataTestCalculator::staticMetaObject,
            QStringLiteral("CalculatorModule")));
        ASSERT_TRUE(gtObjectFactory->registerClass(
            MetadataTestObject::staticMetaObject,
            QStringLiteral("DatamodelModule")));
    }

    void TearDown() override
    {
        gtProcessFactory->taskFactory()->unregisterClass(
            MetadataTestTask::staticMetaObject);
        gtProcessFactory->calculatorFactory()->unregisterClass(
            MetadataTestCalculator::staticMetaObject);
        gtObjectFactory->unregisterClass(MetadataTestObject::staticMetaObject);
    }
};

TEST_F(GtTaskGroupMetadataTest, SavesClassProvidersAndLoadsTask)
{
    QTemporaryDir projectDir;
    ASSERT_TRUE(projectDir.isValid());

    GtTaskGroup group(QStringLiteral("group"), true);
    auto* task = new MetadataTestTask;
    auto* calculator = new MetadataTestCalculator;
    ASSERT_TRUE(task->appendChild(calculator));
    ASSERT_TRUE(group.appendChild(task));
    ASSERT_TRUE(group.save(projectDir.path(), GtTaskGroup::CUSTOM));

    const QString taskFileName =
        GtTaskGroup::groupPath(projectDir.path(), GtTaskGroup::CUSTOM,
                               QStringLiteral("group")) +
        QDir::separator() + task->uuid() + QStringLiteral(".gttask");
    QFile taskFile(taskFileName);
    ASSERT_TRUE(taskFile.open(QIODevice::ReadOnly | QIODevice::Text));
    QDomDocument doc;
    ASSERT_TRUE(doc.setContent(&taskFile));

    const QDomElement metadata = doc.documentElement().firstChildElement(
        QStringLiteral("METADATA"));
    ASSERT_FALSE(metadata.isNull());
    const auto mappings =
        gt::xml::readClassModuleMap(doc.documentElement());
    EXPECT_EQ(mappings.value(QStringLiteral("MetadataTestTask")),
              QStringLiteral("TaskModule"));
    EXPECT_EQ(mappings.value(QStringLiteral("MetadataTestCalculator")),
              QStringLiteral("CalculatorModule"));

    GtTaskGroup loadedGroup(QStringLiteral("group"));
    ASSERT_TRUE(loadedGroup.read(projectDir.path(), GtTaskGroup::CUSTOM));
    EXPECT_EQ(loadedGroup.findDirectChildren<MetadataTestTask*>().size(), 1);
}

TEST_F(GtTaskGroupMetadataTest, PreservesMetadataForMissingNestedClass)
{
    QTemporaryDir projectDir;
    ASSERT_TRUE(projectDir.isValid());

    GtTaskGroup group(QStringLiteral("group"), true);
    auto* task = new MetadataTestTask;
    ASSERT_TRUE(task->appendChild(new MetadataTestCalculator));
    ASSERT_TRUE(group.appendChild(task));
    ASSERT_TRUE(group.save(projectDir.path(), GtTaskGroup::CUSTOM));
    const QString taskFileName =
        GtTaskGroup::groupPath(projectDir.path(), GtTaskGroup::CUSTOM,
                               QStringLiteral("group")) +
        QDir::separator() + task->uuid() + QStringLiteral(".gttask");

    ASSERT_TRUE(gtProcessFactory->calculatorFactory()->unregisterClass(
        MetadataTestCalculator::staticMetaObject));
    GtTaskGroup loadedGroup(QStringLiteral("group"));
    ASSERT_TRUE(loadedGroup.read(projectDir.path(), GtTaskGroup::CUSTOM));
    ASSERT_TRUE(loadedGroup.save(projectDir.path(), GtTaskGroup::CUSTOM));

    QFile taskFile(taskFileName);
    ASSERT_TRUE(taskFile.open(QIODevice::ReadOnly | QIODevice::Text));
    QDomDocument doc;
    ASSERT_TRUE(doc.setContent(&taskFile));
    const auto mappings =
        gt::xml::readClassModuleMap(doc.documentElement());
    EXPECT_EQ(mappings.value(QStringLiteral("MetadataTestCalculator")),
              QStringLiteral("CalculatorModule"));
}

TEST_F(GtTaskGroupMetadataTest, SavesNonProcessClassProvider)
{
    QTemporaryDir projectDir;
    ASSERT_TRUE(projectDir.isValid());

    GtTaskGroup group(QStringLiteral("group"), true);
    auto* task = new MetadataTestTask;
    ASSERT_TRUE(task->appendChild(new MetadataTestObject));
    ASSERT_TRUE(group.appendChild(task));
    ASSERT_TRUE(group.save(projectDir.path(), GtTaskGroup::CUSTOM));

    const QString taskFileName =
        GtTaskGroup::groupPath(projectDir.path(), GtTaskGroup::CUSTOM,
                               QStringLiteral("group")) +
        QDir::separator() + task->uuid() + QStringLiteral(".gttask");
    QFile taskFile(taskFileName);
    ASSERT_TRUE(taskFile.open(QIODevice::ReadOnly | QIODevice::Text));
    QDomDocument doc;
    ASSERT_TRUE(doc.setContent(&taskFile));

    const auto mappings =
        gt::xml::readClassModuleMap(doc.documentElement());
    EXPECT_EQ(mappings.value(QStringLiteral("MetadataTestObject")),
              QStringLiteral("DatamodelModule"));
}

TEST_F(GtTaskGroupMetadataTest, KeepsFileForMissingTaskClass)
{
    QTemporaryDir projectDir;
    ASSERT_TRUE(projectDir.isValid());

    GtTaskGroup group(QStringLiteral("group"), true);
    auto* task = new MetadataTestTask;
    const QString taskUuid = task->uuid();
    ASSERT_TRUE(group.appendChild(task));
    ASSERT_TRUE(group.save(projectDir.path(), GtTaskGroup::CUSTOM));
    const QString taskFileName =
        GtTaskGroup::groupPath(projectDir.path(), GtTaskGroup::CUSTOM,
                               QStringLiteral("group")) +
        QDir::separator() + task->uuid() + QStringLiteral(".gttask");

    ASSERT_TRUE(gtProcessFactory->taskFactory()->unregisterClass(
        MetadataTestTask::staticMetaObject));
    GtTaskGroup loadedGroup(QStringLiteral("group"));
    ASSERT_TRUE(loadedGroup.read(projectDir.path(), GtTaskGroup::CUSTOM));
    ASSERT_TRUE(loadedGroup.save(projectDir.path(), GtTaskGroup::CUSTOM));

    QFile taskFile(taskFileName);
    ASSERT_TRUE(taskFile.open(QIODevice::ReadOnly | QIODevice::Text));
    QDomDocument doc;
    ASSERT_TRUE(doc.setContent(&taskFile));
    const auto mappings =
        gt::xml::readClassModuleMap(doc.documentElement());
    EXPECT_EQ(mappings.value(QStringLiteral("MetadataTestTask")),
              QStringLiteral("TaskModule"));

    QFile indexFile(GtTaskGroup::groupPath(
                        projectDir.path(), GtTaskGroup::CUSTOM,
                        QStringLiteral("group")) +
                    QDir::separator() + QStringLiteral("index.json"));
    ASSERT_TRUE(indexFile.open(QIODevice::ReadOnly | QIODevice::Text));
    const QJsonDocument index = QJsonDocument::fromJson(indexFile.readAll());
    ASSERT_TRUE(index.isObject());
    const QJsonArray active =
        index.object().value(QStringLiteral("active")).toArray();
    EXPECT_TRUE(active.contains(taskUuid));
}

#include "test_gt_taskgroup.moc"
