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
#include "gt_externalizationmanager.h"
#include "gt_object.h"
#include "gt_objectfactory.h"
#include "gt_project.h"
#include "gt_projectprovider.h"
#include "gt_label.h"
#include "gt_labeldata.h"
#include "gt_processdata.h"
#include "gt_propertyconnection.h"
#include "gt_objectlinkproperty.h"
#include "gt_intproperty.h"
#include "gt_task.h"
#include "gt_taskgroup.h"
#include "gt_processfactory.h"
#include "gt_testhelper.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QUuid>

#include <memory>

class ProjectUuidLinkObject : public GtObject
{
    Q_OBJECT

public:
    Q_INVOKABLE ProjectUuidLinkObject() :
        m_link(QStringLiteral("link"), QStringLiteral("Link"),
               QStringLiteral("Object link"), this, {GT_CLASSNAME(GtObject)}),
        m_value(QStringLiteral("value"), QStringLiteral("Value"),
                QStringLiteral("Value for a property connection"), 0)
    {
        registerProperty(m_link);
        registerProperty(m_value);
    }

    GtObjectLinkProperty* link()
    {
        return &m_link;
    }
    GtIntProperty* value()
    {
        return &m_value;
    }

private:
    GtObjectLinkProperty m_link;
    GtIntProperty m_value;
};

class ProjectUuidTask : public GtTask
{
    Q_OBJECT

public:
    Q_INVOKABLE ProjectUuidTask() :
        m_link(QStringLiteral("link"), QStringLiteral("Link"),
               QStringLiteral("Object link"), this, {GT_CLASSNAME(GtObject)})
    {
        registerProperty(m_link);
    }

    GtObjectLinkProperty* link()
    {
        return &m_link;
    }

private:
    GtObjectLinkProperty m_link;
};

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
        m_previousExternalizationDir = gtExternalizationManager->projectDir();
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
        gtExternalizationManager->onProjectLoaded(
            m_previousExternalizationDir);

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
    QString m_previousExternalizationDir;
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
    EXPECT_TRUE(current->isOpen());
    EXPECT_EQ(gtDataModel->currentProject(), current);
    EXPECT_EQ(gtDataModel->findProject(rejected->objectName()), nullptr);
    EXPECT_EQ(rejected->parent(), nullptr);
    delete rejected;
}

TEST_F(ProjectLifecycleTest, newProjectCanBeAddedWhileAnotherProjectIsOpen)
{
    GtProject* current = createProject(QStringLiteral("Current Project"));
    GtProject* added = createProject(QStringLiteral("Added Project"));
    ASSERT_NE(current, nullptr);
    ASSERT_NE(added, nullptr);
    ASSERT_TRUE(gtDataModel->newProject(current, true));

    ASSERT_TRUE(gtDataModel->newProject(added, false));
    EXPECT_TRUE(current->isOpen());
    EXPECT_EQ(gtDataModel->currentProject(), current);
    EXPECT_FALSE(added->isOpen());
    EXPECT_EQ(gtDataModel->findProject(added->objectName()), added);

    EXPECT_TRUE(gtDataModel->switchProject(added));
    EXPECT_FALSE(current->isOpen());
    EXPECT_TRUE(added->isOpen());
    EXPECT_EQ(gtDataModel->currentProject(), added);
}

TEST_F(ProjectLifecycleTest, openProjectRejectsProjectOutsideSession)
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
    EXPECT_EQ(duplicate->objectName(), QStringLiteral("Duplicate Project"));
    EXPECT_EQ(duplicate->path(), duplicatePath);
    EXPECT_EQ(duplicate->childCount<GtObject*>(), 0);
    EXPECT_TRUE(QFileInfo::exists(duplicatePath + QDir::separator() +
                                  GtProject::mainFilename()));
}

TEST_F(ProjectLifecycleTest, duplicatePreservesTaskUuids)
{
    GtProject* source = createProject(QStringLiteral("Task Source Project"));
    ASSERT_NE(source, nullptr);
    ASSERT_TRUE(gtDataModel->newProject(source, true));

    auto* taskFactory = gtProcessFactory->taskFactory();
    ASSERT_TRUE(taskFactory->registerClass(ProjectUuidTask::staticMetaObject,
                                           QStringLiteral("Core")));
    ASSERT_TRUE(gtObjectFactory->registerClass(
        ProjectUuidTask::staticMetaObject, QStringLiteral("Core")));
    const bool registeredLinkObject = !gtObjectFactory->knownClass(
        ProjectUuidLinkObject::staticMetaObject.className());
    if (registeredLinkObject)
    {
        ASSERT_TRUE(gtObjectFactory->registerClass(
            ProjectUuidLinkObject::staticMetaObject, QStringLiteral("Core")));
    }

    auto* processData = source->processData();
    ASSERT_NE(processData, nullptr);
    auto* projectTarget = new GtLabel(QStringLiteral("Project Target"));
    ASSERT_TRUE(source->labelData()->addLabel(projectTarget));
    const QString projectTargetUuid = projectTarget->uuid();

    auto* taskGroup = processData->createNewTaskGroup(
        QStringLiteral("Uuid Test Group"), GtTaskGroup::USER);
    ASSERT_NE(taskGroup, nullptr);
    auto* task = new ProjectUuidTask;
    task->setFactory(taskFactory);
    task->setObjectName(QStringLiteral("Uuid Test Task"));
    task->link()->setVal(projectTargetUuid);
    auto* target = new ProjectUuidLinkObject;
    target->setObjectName(QStringLiteral("Task Target"));
    auto* linkedObject = new ProjectUuidLinkObject;
    linkedObject->setObjectName(QStringLiteral("Task Linked Object"));
    linkedObject->link()->setVal(target->uuid());
    linkedObject->value()->setVal(42);
    ASSERT_TRUE(task->appendChild(target));
    ASSERT_TRUE(task->appendChild(linkedObject));
    auto* connection = new GtPropertyConnection;
    connection->setSourceUuid(linkedObject->uuid());
    connection->setTargetUuid(target->uuid());
    connection->setSourceProp(QStringLiteral("value"));
    connection->setTargetProp(QStringLiteral("value"));
    ASSERT_TRUE(task->appendChild(connection));
    connection->makeConnection();
    ASSERT_TRUE(connection->isConnected());
    const QString targetUuid = target->uuid();
    const QString linkedObjectUuid = linkedObject->uuid();
    const QString connectionUuid = connection->uuid();
    ASSERT_TRUE(taskGroup->appendChild(task));
    const QString taskUuid = task->uuid();
    ASSERT_EQ(task->link()->linkedObject(), projectTarget);

    ASSERT_TRUE(gtDataModel->saveProject(source));

    GtProjectProvider provider(source);
    const QString duplicatePath = gtTestHelper->newTempDir().absolutePath();
    std::unique_ptr<GtProject> duplicate(provider.duplicateProject(
        QStringLiteral("Task Duplicate Project"), duplicatePath));
    ASSERT_NE(duplicate, nullptr);
    EXPECT_EQ(task->uuid(), taskUuid);
    EXPECT_EQ(taskGroup->findDirectChild<ProjectUuidTask*>(
                  QStringLiteral("Uuid Test Task")),
              task);
    EXPECT_EQ(linkedObject->link()->linkedObject(task), target);

    ASSERT_TRUE(gtDataModel->closeProject(source));
    GtProject* duplicateProject = duplicate.release();
    ASSERT_TRUE(gtDataModel->newProject(duplicateProject, false));
    ASSERT_TRUE(gtDataModel->openProject(duplicateProject));
    EXPECT_EQ(duplicateProject->objectName(),
              QStringLiteral("Task Duplicate Project"));
    EXPECT_EQ(duplicateProject->path(), duplicatePath);

    auto* reopenedGroup =
        duplicateProject->processData()->findChild<GtTaskGroup*>(
            QStringLiteral("Uuid Test Group"));
    ASSERT_NE(reopenedGroup, nullptr);
    auto* reopenedTask = reopenedGroup->findDirectChild<ProjectUuidTask*>(
        QStringLiteral("Uuid Test Task"));
    ASSERT_NE(reopenedTask, nullptr);
    EXPECT_EQ(reopenedTask->uuid(), taskUuid);
    auto* reopenedTarget =
        reopenedTask->findDirectChild<ProjectUuidLinkObject*>(
            QStringLiteral("Task Target"));
    auto* reopenedLinkedObject =
        reopenedTask->findDirectChild<ProjectUuidLinkObject*>(
            QStringLiteral("Task Linked Object"));
    ASSERT_NE(reopenedTarget, nullptr);
    ASSERT_NE(reopenedLinkedObject, nullptr);
    EXPECT_EQ(reopenedTarget->uuid(), targetUuid);
    EXPECT_EQ(reopenedLinkedObject->uuid(), linkedObjectUuid);
    EXPECT_EQ(reopenedLinkedObject->link()->linkedObject(reopenedTask),
              reopenedTarget);
    auto* reopenedProjectTarget =
        duplicateProject->labelData()->findDirectChild<GtLabel*>(
            QStringLiteral("Project Target"));
    ASSERT_NE(reopenedProjectTarget, nullptr);
    EXPECT_EQ(reopenedProjectTarget->uuid(), projectTargetUuid);
    EXPECT_EQ(reopenedTask->link()->linkedObject(), reopenedProjectTarget);
    EXPECT_EQ(reopenedProjectTarget->parent(), duplicateProject->labelData());

    auto* reopenedConnection = qobject_cast<GtPropertyConnection*>(
        reopenedTask->getObjectByUuid(connectionUuid));
    ASSERT_NE(reopenedConnection, nullptr);
    EXPECT_EQ(reopenedConnection->sourceUuid(), linkedObjectUuid);
    EXPECT_EQ(reopenedConnection->targetUuid(), targetUuid);
    reopenedConnection->makeConnection();
    EXPECT_TRUE(reopenedConnection->isConnected());

    ASSERT_TRUE(gtDataModel->closeProject(duplicateProject));
    ASSERT_TRUE(gtDataModel->openProject(source));
    EXPECT_EQ(source->objectName(), QStringLiteral("Task Source Project"));
    auto* reopenedSourceTask =
        source->processData()->findChild<ProjectUuidTask*>(
            QStringLiteral("Uuid Test Task"));
    ASSERT_NE(reopenedSourceTask, nullptr);
    auto* reopenedSourceTarget = source->labelData()->findDirectChild<GtLabel*>(
        QStringLiteral("Project Target"));
    ASSERT_NE(reopenedSourceTarget, nullptr);
    EXPECT_EQ(reopenedSourceTask->link()->linkedObject(), reopenedSourceTarget);
    auto* reopenedSourceConnection = qobject_cast<GtPropertyConnection*>(
        reopenedSourceTask->getObjectByUuid(connectionUuid));
    ASSERT_NE(reopenedSourceConnection, nullptr);
    reopenedSourceConnection->makeConnection();
    EXPECT_TRUE(reopenedSourceConnection->isConnected());

    EXPECT_TRUE(
        gtObjectFactory->unregisterClass(ProjectUuidTask::staticMetaObject));
    EXPECT_TRUE(
        taskFactory->unregisterClass(ProjectUuidTask::staticMetaObject));
    if (registeredLinkObject)
    {
        EXPECT_TRUE(gtObjectFactory->unregisterClass(
            ProjectUuidLinkObject::staticMetaObject));
    }
}

TEST_F(ProjectLifecycleTest, duplicateFailsWhenTaskGroupsCannotBeLoaded)
{
    GtProject* source = createProject(QStringLiteral("Task Source Project"));
    ASSERT_NE(source, nullptr);
    ASSERT_TRUE(gtDataModel->newProject(source, true));

    auto* taskFactory = gtProcessFactory->taskFactory();
    ASSERT_TRUE(taskFactory->registerClass(ProjectUuidTask::staticMetaObject,
                                           QStringLiteral("Core")));
    ASSERT_TRUE(gtObjectFactory->registerClass(
        ProjectUuidTask::staticMetaObject, QStringLiteral("Core")));

    auto* processData = source->processData();
    ASSERT_NE(processData, nullptr);
    auto* group = processData->createNewTaskGroup(
        QStringLiteral("Unreadable Group"), GtTaskGroup::USER);
    ASSERT_NE(group, nullptr);
    auto* task = new ProjectUuidTask;
    task->setFactory(taskFactory);
    ASSERT_TRUE(group->appendChild(task));
    const QString taskUuid = task->uuid();
    ASSERT_TRUE(gtDataModel->saveProject(source));
    const QString groupPath = GtTaskGroup::groupPath(
        source->path(), GtTaskGroup::USER, QStringLiteral("Unreadable Group"));
    const QString taskFile =
        groupPath + QDir::separator() + taskUuid + QStringLiteral(".gttask");

    ASSERT_TRUE(gtDataModel->closeProject(source));
    ASSERT_TRUE(QFile::remove(taskFile));
    ASSERT_TRUE(gtDataModel->openProject(source));
    ASSERT_NE(source->processData(), nullptr);
    EXPECT_FALSE(source->processData()->initAllTaskGroups(source->path()));

    GtProjectProvider provider(source);
    const QString duplicatePath = gtTestHelper->newTempDir().absolutePath();
    EXPECT_EQ(provider.duplicateProject(QStringLiteral("Failed Duplicate"),
                                        duplicatePath),
              nullptr);
    EXPECT_EQ(provider.project(), source);
    EXPECT_TRUE(source->isOpen());
    EXPECT_EQ(gtDataModel->currentProject(), source);

    EXPECT_TRUE(
        gtObjectFactory->unregisterClass(ProjectUuidTask::staticMetaObject));
    EXPECT_TRUE(
        taskFactory->unregisterClass(ProjectUuidTask::staticMetaObject));
}

#include "test_gt_coredatamodel.moc"
