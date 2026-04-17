/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_project.h"
#include "gt_projectsettings.h"

#include <QDir>
#include <QFile>
#include <QJsonValue>
#include <QJsonArray>
#include <QIODevice>
#include <QTemporaryDir>

namespace
{

class TestProject : public GtProject
{
public:
    explicit TestProject(const QString& path) : GtProject(path) {}
};

static bool writeProjectFile(const QString& dirPath, const QString& projectName)
{
    QFile file(dirPath + QDir::separator() + GtProject::mainFilename());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        return false;
    }

    const QByteArray data = QString(
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<GTLAB projectname=\"%1\" version=\"1.7.0-rc1\">\n"
        "    <env-footprint>\n"
        "        <modules/>\n"
        "    </env-footprint>\n"
        "    <comment/>\n"
        "    <MODULES/>\n"
        "    <PROCESSES/>\n"
        "    <LABELS/>\n"
        "</GTLAB>\n")
        .arg(projectName)
        .toUtf8();

    return file.write(data) == data.size();
}

} // namespace

TEST(GtProjectSettings, basicApiRoundTrip)
{
    GtProjectSettings settings;

    EXPECT_FALSE(settings.ignoringIrregularities());
    EXPECT_TRUE(settings.ownObjectFileSerializationEnabled());
    EXPECT_TRUE(settings.contains(QStringLiteral("IgnoreIrregularities")));
    EXPECT_TRUE(settings.contains(QStringLiteral("AllowOwnObjectSerialization")));
    EXPECT_EQ(settings.size(), 2);
    EXPECT_FALSE(settings.isEmpty());

    int changedCount = 0;
    QObject::connect(&settings, &GtProjectSettings::changed,
                     [&changedCount]() { ++changedCount; });

    settings.setIgnoreIrregularities(true);
    EXPECT_EQ(changedCount, 1);
    settings.setIgnoreIrregularities(true);
    EXPECT_EQ(changedCount, 1);

    settings.setOwnObjectFileSerializationEnabled(false);
    EXPECT_EQ(changedCount, 2);
    EXPECT_TRUE(settings.ignoringIrregularities());
    EXPECT_FALSE(settings.ownObjectFileSerializationEnabled());

    settings[QStringLiteral("custom")] = 123;
    EXPECT_TRUE(settings.contains(QStringLiteral("custom")));
    EXPECT_EQ(settings.value(QStringLiteral("custom")).toInt(), 123);
    EXPECT_EQ(settings.size(), 3);

    const QJsonValue json = settings.toJson();
    ASSERT_TRUE(json.isObject());

    GtProjectSettings restored;
    restored.fromJson(json);

    EXPECT_TRUE(restored.ignoringIrregularities());
    EXPECT_FALSE(restored.ownObjectFileSerializationEnabled());
    EXPECT_EQ(restored.value(QStringLiteral("custom")).toInt(), 123);
    EXPECT_TRUE(restored.contains(QStringLiteral("custom")));
    EXPECT_EQ(restored.size(), 3);

    EXPECT_EQ(restored.remove(QStringLiteral("custom")), 1);
    EXPECT_FALSE(restored.contains(QStringLiteral("custom")));

    restored.clear();
    EXPECT_TRUE(restored.isEmpty());
    EXPECT_EQ(restored.size(), 0);
}

TEST(GtProjectSettings, legacyBoolPayloadIsAccepted)
{
    GtProjectSettings settings;

    settings.fromJson(QJsonValue(true));

    EXPECT_TRUE(settings.ignoringIrregularities());
    EXPECT_TRUE(settings.ownObjectFileSerializationEnabled());
}

TEST(GtProject, filenameHelpersReturnExpectedSuffixes)
{
    EXPECT_EQ(GtProject::mainFilename(), QStringLiteral("project.gtlab"));
    EXPECT_EQ(GtProject::mainFileExtension(), QStringLiteral("gtlab"));
    EXPECT_EQ(GtProject::moduleExtension(), QStringLiteral("gtmod"));
}

TEST(GtProject, projectSettingsAccessorsExposeTheSameState)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("ProjectSettingsTest")));

    TestProject project(tempDir.path());

    EXPECT_EQ(&project.getProjectSettings(), &project.projectSettings());
    EXPECT_TRUE(project.getProjectSettings().ownObjectFileSerializationEnabled());
    EXPECT_FALSE(project.getProjectSettings().ignoringIrregularities());

    project.projectSettings().setIgnoreIrregularities(true);
    project.projectSettings().setOwnObjectFileSerializationEnabled(false);

    EXPECT_TRUE(project.getProjectSettings().ignoringIrregularities());
    EXPECT_FALSE(project.getProjectSettings().ownObjectFileSerializationEnabled());
}

TEST(GtProjectSettings, insertingNewKeyTriggersChangedSignal)
{
    GtProjectSettings settings;
    ASSERT_FALSE(settings.contains(QStringLiteral("NewKey")));

    int changedCount = 0;
    QObject::connect(&settings, &GtProjectSettings::changed,
                     [&changedCount]() { ++changedCount; });

    // Inserting a completely new key hits lines 63-64
    settings.setValue(QStringLiteral("NewKey"), QStringLiteral("value"));

    EXPECT_TRUE(settings.contains(QStringLiteral("NewKey")));
    EXPECT_EQ(settings.value(QStringLiteral("NewKey")).toString(), QStringLiteral("value"));
    EXPECT_EQ(changedCount, 1);
}

TEST(GtProjectSettings, fromJsonWithNonObjectDoesNothing)
{
    GtProjectSettings settings;

    // fromJson with non-object calls initDefaultValues() first (clears + sets defaults)
    // Null value - line 135 early return
    settings.fromJson(QJsonValue::Null);
    EXPECT_TRUE(settings.contains(QStringLiteral("IgnoreIrregularities")));
    EXPECT_TRUE(settings.contains(QStringLiteral("AllowOwnObjectSerialization")));

    // Array value - line 135 early return
    settings.fromJson(QJsonValue(QJsonArray{}));
    EXPECT_TRUE(settings.contains(QStringLiteral("IgnoreIrregularities")));

    // Number value - line 135 early return
    settings.fromJson(QJsonValue(123));
    EXPECT_TRUE(settings.contains(QStringLiteral("AllowOwnObjectSerialization")));
}

TEST(GtProjectSettings, iteratorMethods)
{
    GtProjectSettings settings;
    settings.setValue(QStringLiteral("key1"), 1);
    settings.setValue(QStringLiteral("key2"), 2);

    // Default settings add 2 keys, plus our 2 = 4 total
    int count = 0;
    for (auto it = settings.begin(); it != settings.end(); ++it)
    {
        Q_UNUSED(it);
        ++count;
    }
    EXPECT_EQ(count, 4);

    // Test cbegin/cend iterators
    count = 0;
    for (auto it = settings.cbegin(); it != settings.cend(); ++it)
    {
        Q_UNUSED(it);
        ++count;
    }
    EXPECT_EQ(count, 4);
}

TEST(GtProjectSettings, operatorBracketAccess)
{
    GtProjectSettings settings;
    settings.setValue(QStringLiteral("existing"), 42);
    ASSERT_TRUE(settings.contains(QStringLiteral("existing")));

    // Update via operator[]
    settings[QStringLiteral("existing")] = 100;
    EXPECT_EQ(settings.value(QStringLiteral("existing")).toInt(), 100);

    // Insert new via operator[]
    settings[QStringLiteral("new")] = QStringLiteral("test");
    EXPECT_TRUE(settings.contains(QStringLiteral("new")));
    EXPECT_EQ(settings.value(QStringLiteral("new")).toString(), QStringLiteral("test"));
}
