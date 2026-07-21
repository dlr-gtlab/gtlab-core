/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_gt_coredatamodel
 *
 *  Created on: 02.09.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"

#include "gt_coredatamodel.h"
#include "gt_coreapplication.h"
#include "gt_object.h"
#include "gt_project.h"
#include "gt_projectprovider.h"
#include "gt_testhelper.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QUuid>

#include <memory>

class TestCoreDataModel : public GtCoreDatamodel
{
public:
    TestCoreDataModel() : GtCoreDatamodel()
    {

    }

    bool appendProjectDataForTest(GtProject* project,
                                  const QList<GtObject*>& projectData)
    {
        return appendProjectData(project, projectData);
    }

};

/// This is a test fixture that does a init for each test
class TestGtCoreDataModel : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_model = new TestCoreDataModel;
    }

    void TearDown() override
    {
        delete m_model;
    }

    GtCoreDatamodel* m_model;

};

TEST_F(TestGtCoreDataModel, instance)
{
    ASSERT_TRUE(m_model != nullptr);

    GtCoreDatamodel* model = gtDataModel;

    ASSERT_TRUE(m_model == model);

    GtCoreDatamodel* model2 = GtCoreDatamodel::instance();

    ASSERT_TRUE(m_model == model2);
}

TEST_F(TestGtCoreDataModel, getUniqueName)
{
    using s = std::string;
    std::vector<s> l = {"aa", "bb", "bb[2]", "cc", "cc[2]", "cc[3]"};

    auto f = [](const s& str) { return QString(str.c_str()); };

    EXPECT_EQ(s("unique"), gt::makeUniqueName("unique", l, f).toStdString());
    EXPECT_EQ(s("aa[2]"), gt::makeUniqueName("aa", l, f).toStdString());
    EXPECT_EQ(s("bb[3]"), gt::makeUniqueName("bb", l, f).toStdString());
    EXPECT_EQ(s("cc[4]"), gt::makeUniqueName("cc", l, f).toStdString());

    std::vector<QString> l2 = {"aa", "bb", "bb[2]", "cc", "cc[2]", "cc[3]"};

    EXPECT_EQ(s("unique"), gt::makeUniqueName("unique", l2).toStdString());
    EXPECT_EQ(s("aa[2]"), gt::makeUniqueName("aa", l2).toStdString());
    EXPECT_EQ(s("bb[3]"), gt::makeUniqueName("bb", l2).toStdString());
    EXPECT_EQ(s("cc[4]"), gt::makeUniqueName("cc", l2).toStdString());
}

TEST_F(TestGtCoreDataModel, makeUnqiueName)
{
    GtObject parent;
    GtObject* child1 = new GtObject;
    child1->setObjectName("Test");
    parent.appendChild(child1);
    GtObject* child2 = new GtObject;
    child2->setObjectName("Test[3]");
    parent.appendChild(child2);

    auto newName = gt::makeUniqueName("Test", parent);
    EXPECT_EQ(newName.toStdString(), "Test[2]");

    GtObject* child3 = new GtObject;
    child3->setObjectName(newName);
    parent.appendChild(child3);

    EXPECT_EQ(gt::makeUniqueName("Test", parent).toStdString(), "Test[4]");

    // this overload should only check its siblings
    EXPECT_EQ(gt::makeUniqueName(*child3).toStdString(), "Test[2]");
    // check if custom name is provided
    EXPECT_EQ(gt::makeUniqueName(*child3, "Test[3]").toStdString(), "Test[3][2]");
}

TEST_F(TestGtCoreDataModel, makeUnqiueName_null_parent)
{
    auto newName = gt::makeUniqueName("Test", nullptr);
    EXPECT_EQ(newName.toStdString(), "Test");
}

TEST_F(TestGtCoreDataModel, appendProjectDataRejectsEmptyData)
{
    EXPECT_FALSE(static_cast<TestCoreDataModel*>(m_model)
                 ->appendProjectDataForTest(nullptr, {}));
}

class ProjectLifecycleTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_previousConfigHome = qgetenv("XDG_CONFIG_HOME");
        qputenv("XDG_CONFIG_HOME",
                gtTestHelper->newTempDir().absolutePath().toUtf8());
        m_app = std::make_unique<GtCoreApplication>(
                    QCoreApplication::instance(),
                    GtCoreApplication::AppMode::Batch);
        m_app->init();

        QDir roamingPath(GtCoreApplication::roamingPath());
        ASSERT_TRUE(roamingPath.mkpath(QStringLiteral("session")));

        m_sessionId = QStringLiteral("project-lifecycle-") +
                      QUuid::createUuid().toString(QUuid::WithoutBraces);
        ASSERT_TRUE(m_app->newSession(m_sessionId));
        m_app->switchSession(m_sessionId);
        ASSERT_NE(m_app->session(), nullptr);
    }

    void TearDown() override
    {
        m_app->switchSession(QStringLiteral("default"));
        m_app->deleteSession(m_sessionId);
        m_app.reset();

        if (m_previousConfigHome.isNull())
        {
            qunsetenv("XDG_CONFIG_HOME");
        }
        else
        {
            qputenv("XDG_CONFIG_HOME", m_previousConfigHome);
        }
    }

    GtProject* createProject(const QString& name)
    {
        GtProjectProvider provider;
        provider.setProjectName(name);
        provider.setProjectPath(gtTestHelper->newTempDir().absolutePath());
        return provider.project();
    }

    std::unique_ptr<GtCoreApplication> m_app;
    QString m_sessionId;
    QByteArray m_previousConfigHome;
};

TEST_F(ProjectLifecycleTest, opensClosesAndSwitchesProjects)
{
    GtProject* first = createProject(QStringLiteral("First Project"));
    GtProject* second = createProject(QStringLiteral("Second Project"));
    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);
    ASSERT_TRUE(gtDataModel->newProject(first, false));
    ASSERT_TRUE(gtDataModel->newProject(second, false));

    EXPECT_TRUE(gtDataModel->openProject(first));
    EXPECT_TRUE(first->isOpen());
    EXPECT_EQ(gtDataModel->currentProject(), first);

    EXPECT_TRUE(gtDataModel->switchProject(second));
    EXPECT_FALSE(first->isOpen());
    EXPECT_EQ(first->childCount<GtObject*>(), 0);
    EXPECT_TRUE(second->isOpen());
    EXPECT_EQ(gtDataModel->currentProject(), second);

    EXPECT_TRUE(gtDataModel->switchProject(second));
    EXPECT_FALSE(gtDataModel->switchProject(nullptr));
    EXPECT_TRUE(gtDataModel->closeProject(second));
    EXPECT_FALSE(second->isOpen());
    EXPECT_EQ(gtDataModel->currentProject(), nullptr);
}

TEST_F(ProjectLifecycleTest, newProjectRollsBackWhenOpeningFails)
{
    GtProject* current = createProject(QStringLiteral("Current Project"));
    ASSERT_NE(current, nullptr);
    ASSERT_TRUE(gtDataModel->newProject(current, true));

    GtProject* rejected = createProject(QStringLiteral("Rejected Project"));
    ASSERT_NE(rejected, nullptr);
    EXPECT_FALSE(gtDataModel->newProject(rejected, true));
    EXPECT_EQ(gtDataModel->findProject(rejected->objectName()), nullptr);
    EXPECT_EQ(rejected->parent(), nullptr);
    delete rejected;
}

TEST_F(ProjectLifecycleTest, openProjectRollsBackProjectOutsideSession)
{
    std::unique_ptr<GtProject> detached(
        createProject(QStringLiteral("Detached Project")));
    ASSERT_NE(detached, nullptr);

    EXPECT_FALSE(gtDataModel->openProject(detached.get()));
    EXPECT_FALSE(detached->isOpen());
    EXPECT_EQ(detached->childCount<GtObject*>(), 0);
    EXPECT_EQ(gtDataModel->currentProject(), nullptr);
}

TEST_F(ProjectLifecycleTest, duplicateIsPersistedAndReturnedClosed)
{
    GtProject* source = createProject(QStringLiteral("Source Project"));
    ASSERT_NE(source, nullptr);
    ASSERT_TRUE(gtDataModel->newProject(source, true));

    GtProjectProvider provider(source);
    const QString duplicatePath = gtTestHelper->newTempDir().absolutePath();
    std::unique_ptr<GtProject> duplicate(provider.duplicateProject(
        QStringLiteral("Duplicate Project"), duplicatePath));

    ASSERT_NE(duplicate, nullptr);
    EXPECT_FALSE(duplicate->isOpen());
    EXPECT_EQ(duplicate->childCount<GtObject*>(), 0);
    EXPECT_TRUE(QFileInfo::exists(duplicatePath + QDir::separator() +
                                  GtProject::mainFilename()));
}
