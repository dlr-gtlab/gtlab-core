/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_gt_statehandler.cpp
 */

#include "gtest/gtest.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QTemporaryDir>

#include "gt_project.h"
#include "gt_state.h"
#include "gt_statecontainer.h"
#include "gt_statehandler.h"

namespace {

class TestProject : public GtProject
{
public:
    explicit TestProject(const QString& path) : GtProject(path) {}
};

class TestGtStateHandler : public ::testing::Test
{
protected:
    void SetUp() override
    {
        QCoreApplication::setOrganizationName("DLR");
        QCoreApplication::setOrganizationDomain("www.dlr.de");
        QCoreApplication::setApplicationName("StateUnitTests");
        QSettings settings(QSettings::IniFormat,
                           QSettings::UserScope,
                           QCoreApplication::organizationName(),
                           QCoreApplication::applicationName());
        settings.clear();
        settings.sync();
    }

    void TearDown() override
    {
        QSettings settings(QSettings::IniFormat,
                           QSettings::UserScope,
                           QCoreApplication::organizationName(),
                           QCoreApplication::applicationName());
        settings.clear();
        settings.sync();
    }

    static void writeProjectFile(const QString& dirPath, const QString& projectName)
    {
        QFile file(dirPath + QDir::separator() + GtProject::mainFilename());
        ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
        const QByteArray data = QString(
            "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
            "<GTLAB projectname=\"%1\">\n"
            "    <env-footprint>\n"
            "        <core-ver>2.0.0</core-ver>\n"
            "        <modules/>\n"
            "    </env-footprint>\n"
            "    <MODULES/>\n"
            "    <PROCESSES/>\n"
            "</GTLAB>\n")
            .arg(projectName)
            .toUtf8();
        ASSERT_EQ(file.write(data), data.size());
    }
};

} // namespace

TEST_F(TestGtStateHandler, stateContainerRejectsInvalidArguments)
{
    GtStateContainer container(nullptr);
    GtObject guardian;

    EXPECT_EQ(container.initializeState("group", "id", "path", 1, nullptr), nullptr);
    EXPECT_EQ(container.initializeState("", "id", "path", 1, &guardian), nullptr);
    EXPECT_EQ(container.initializeState("group", "", "path", 1, &guardian), nullptr);
    EXPECT_EQ(container.initializeState("group", "id", "", 1, &guardian), nullptr);
}

TEST_F(TestGtStateHandler, stateContainerReusesStateAndCanForceInitialization)
{
    GtStateContainer container(nullptr);
    GtObject guardian;

    GtState* state = container.initializeState("container-group", "state-id", "state-path", 1, &guardian);
    ASSERT_NE(state, nullptr);
    EXPECT_EQ(state->getValue().toInt(), 1);

    state->setValue(11, false);

    GtState* reused = container.initializeState("container-group", "other-id", "state-path", 5, &guardian);
    EXPECT_EQ(reused, state);
    EXPECT_EQ(reused->getValue().toInt(), 11);

    GtState* forced = container.initializeState("container-group", "other-id", "state-path", 7, &guardian, true);
    EXPECT_EQ(forced, state);
    EXPECT_EQ(forced->getValue().toInt(), 7);
}

TEST_F(TestGtStateHandler, stateContainerLoadsGlobalStateFromSettings)
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
    settings.beginGroup("settings-group");
    settings.setValue("settings-path", 42);

    GtStateContainer container(nullptr);
    GtObject guardian;

    GtState* state = container.initializeState("settings-group", "state-id", "settings-path", 1, &guardian);
    ASSERT_NE(state, nullptr);
    EXPECT_EQ(state->getValue().toInt(), 42);
}

TEST_F(TestGtStateHandler, stateHandlerFallsBackToGlobalContainer)
{
    GtObject guardian;
    GtStateHandler* handler = GtStateHandler::instance();

    GtState* direct = handler->initializeState("global-fallback-group", "state-id", "global-fallback-path", 3, &guardian);
    ASSERT_NE(direct, nullptr);

    GtState* fallback = handler->initializeState(nullptr, "global-fallback-group", "other-id", "global-fallback-path", 9, &guardian);
    EXPECT_EQ(fallback, direct);
    EXPECT_EQ(fallback->getValue().toInt(), 3);
}

TEST_F(TestGtStateHandler, stateHandlerRejectsInvalidProjectSpecificArguments)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    writeProjectFile(tempDir.path(), "state-project-invalid");

    TestProject project(tempDir.path());
    GtObject guardian;
    GtStateHandler* handler = GtStateHandler::instance();

    EXPECT_EQ(handler->initializeState(&project, "group", "id", "path", 1, nullptr), nullptr);
    EXPECT_EQ(handler->initializeState(&project, "", "id", "path", 1, &guardian), nullptr);
    EXPECT_EQ(handler->initializeState(&project, "group", "", "path", 1, &guardian), nullptr);
    EXPECT_EQ(handler->initializeState(&project, "group", "id", "", 1, &guardian), nullptr);
}

TEST_F(TestGtStateHandler, stateHandlerLoadsAndSavesProjectSpecificState)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    writeProjectFile(tempDir.path(), "state-project");

    QSettings projectSettings(tempDir.path() + QDir::separator() + "project.ini",
                              QSettings::IniFormat);
    projectSettings.beginGroup("project-group");
    projectSettings.setValue("project-path", 99);

    TestProject project(tempDir.path());
    GtObject guardian;
    GtStateHandler* handler = GtStateHandler::instance();

    GtState* state = handler->initializeState(&project, "project-group", "state-id", "project-path", 1, &guardian);
    ASSERT_NE(state, nullptr);
    EXPECT_EQ(state->getValue().toInt(), 99);

    state->setValue(77, false);

    QSettings stored(tempDir.path() + QDir::separator() + "project.ini",
                     QSettings::IniFormat);
    stored.beginGroup("project-group");
    EXPECT_EQ(stored.value("project-path"), QVariant(77));

    GtState* reused = handler->initializeState(&project, "project-group", "other-id", "project-path", 5, &guardian);
    EXPECT_EQ(reused, state);
}

TEST_F(TestGtStateHandler, stateContainerUsesPathAsIdentityKey)
{
    GtStateContainer container(nullptr);
    GtObject guardian;

    GtState* first = container.initializeState("identity-group", "first-id", "shared-path", 1, &guardian);
    ASSERT_NE(first, nullptr);
    EXPECT_EQ(first->id(), QString("first-id"));

    GtState* second = container.initializeState("identity-group", "second-id", "shared-path", 2, &guardian);
    ASSERT_NE(second, nullptr);

    EXPECT_EQ(second, first);
    EXPECT_EQ(second->id(), QString("first-id"));
    EXPECT_EQ(second->getValue().toInt(), 1);
}

TEST_F(TestGtStateHandler, stateContainerSeparatesStatesByGroup)
{
    GtStateContainer container(nullptr);
    GtObject guardian;

    GtState* first = container.initializeState("group-a", "state-id", "shared-path", 1, &guardian);
    GtState* second = container.initializeState("group-b", "state-id", "shared-path", 2, &guardian);

    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);
    EXPECT_NE(first, second);
    EXPECT_EQ(first->getValue().toInt(), 1);
    EXPECT_EQ(second->getValue().toInt(), 2);
}
