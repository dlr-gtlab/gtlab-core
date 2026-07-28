/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_gt_session
 *
 *  Created on: 06.09.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDir>
#include <QDebug>

#include "gtest/gtest.h"

#include "gt_testhelper.h"

#include "gt_projectsettings.h"
#include "gt_project.h"
#include "gt_session.h"

#include <memory>

class TestSession : public GtSession
{
public:
    explicit TestSession(const QString& sessionPath) :
        GtSession("testsession", sessionPath),
        m_sessionPath(sessionPath)

    {}

    QString sessionFilePath() const
    {
        return m_sessionPath;
    }

    void _toJsonObject()
    {
        toJsonObject();
    }

    bool _fromJsonObject()
    {
        return fromJsonObject(sessionFilePath());
    }

    void addProjectForTest(GtProject* project)
    {
        addProject(project);
    }

    bool setCurrentProjectForTest(GtProject* project)
    {
        return setCurrentProject(project);
    }

private:
    QString m_sessionPath;

};

class TestSessionProject : public GtProject
{
public:
    explicit TestSessionProject(const QString& path) : GtProject(path)
    {}
};

/// This is a test fixture that does a init for each test
class TestGtSession : public ::testing::Test
{
public:
    static char* argv;

protected:
    void SetUp() override
    {
        qDebug() << "TestGtSession::SetUp";
        auto dir  = gtTestHelper->newTempDir();
        QString sessionPath = dir.absoluteFilePath("testsession.json");
        m_session = std::make_unique<TestSession>(sessionPath);

    }
    std::unique_ptr<TestSession> m_session;

};

TEST_F(TestGtSession, instance)
{
    ASSERT_FALSE(m_session->isValid());
//    ASSERT_TRUE(m_session->model() == nullptr);
}

TEST_F(TestGtSession, roamingPath)
{
    ASSERT_FALSE(GtSession::roamingPath().isEmpty());
}

TEST_F(TestGtSession, sessionFilePath)
{
    ASSERT_FALSE(m_session->sessionFilePath().isEmpty());
}

TEST_F(TestGtSession, fromJsonObject)
{
    ASSERT_FALSE(m_session->_fromJsonObject());
}

TEST_F(TestGtSession, closedProjectCannotBecomeCurrent)
{
    auto* project = new TestSessionProject(gtTestHelper->newTempDir().path());
    project->setObjectName(QStringLiteral("Closed Project"));
    m_session->addProjectForTest(project);

    EXPECT_FALSE(project->isOpen());
    EXPECT_FALSE(m_session->setCurrentProjectForTest(project));
    EXPECT_EQ(m_session->currentProject(), nullptr);
}

TEST(GtProjectSettings, fromJsonAcceptsLegacyBoolFormat)
{
    GtProjectSettings settings;

    settings.fromJson(QJsonValue(true));

    EXPECT_TRUE(settings.ignoringIrregularities());
    EXPECT_TRUE(settings.ownObjectFileSerializationEnabled());
}
