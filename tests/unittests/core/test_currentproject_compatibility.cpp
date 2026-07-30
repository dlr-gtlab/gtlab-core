/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <memory>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QUuid>

#include "gt_abstractrunnable.h"
#include "gt_calculator.h"
#include "gt_coreapplication.h"
#include "gt_coredatamodel.h"
#include "gt_externalizationmanager.h"
#include "gt_executioncontext.h"
#include "gt_objectfactory.h"
#include "gt_project.h"
#include "gt_projectexecutionguard.h"
#include "gt_runnable.h"
#include "gt_session.h"
#include "gt_task.h"
#include "gt_taskrunner.h"

namespace {

class TestProject : public GtProject
{
public:
    explicit TestProject(const QString& path) : GtProject(path) {}
};

class TestSession : public GtSession
{
public:
    TestSession() : GtSession() {}

    static bool createEmptySessionForTest(const QString& id)
    {
        return createEmptySession(id);
    }

    void addProjectForTest(GtProject* project)
    {
        addProject(project);
    }

    bool selectProjectForTest(GtProject* project)
    {
        return setCurrentProject(project);
    }
};

class TestApplication : public GtCoreApplication
{
public:
    TestApplication() : GtCoreApplication(qApp, AppMode::Batch)
    {
        init();
    }

    ~TestApplication() override
    {
        gtExternalizationManager->onProjectLoaded(QDir::tempPath());
    }

    void installSession(std::unique_ptr<TestSession> session)
    {
        constexpr auto sessionId = "currentproject-compatibility";
        ASSERT_FALSE(gtApp->roamingPath().isEmpty());
        ASSERT_TRUE(QDir().mkpath(gtApp->roamingPath()));
        ASSERT_TRUE(TestSession::createEmptySessionForTest(
            QString::fromLatin1(sessionId)));
        m_session.reset();
        initSession(QString::fromLatin1(sessionId));

        for (auto* project : session->projects())
        {
            ASSERT_TRUE(gtDataModel->newProject(project, false));
        }
    }

protected:
    bool initFirstRun() override
    {
        return true;
    }
};

class TestRunnable : public GtAbstractRunnable
{
public:
    void run() override {}

    QDir tempDir() override { return {}; }

    bool clearTempDir(const QString&) override { return true; }

    QString projectPath() override
    {
        return gtApp && gtApp->currentProject() ?
                   gtApp->currentProject()->path() : QString{};
    }
};

class LegacyCurrentProjectCalculator : public GtCalculator
{
public:
    GtProject* observedProject = nullptr;

    bool run() override
    {
        observedProject = gtApp->currentProject();
        return observedProject != nullptr;
    }
};

class EndToEndLegacyCalculator : public GtCalculator
{
    Q_OBJECT

public:
    Q_INVOKABLE EndToEndLegacyCalculator() = default;

    static GtProject* projectToSelect;
    static GtProject* observedProject;

    bool run() override
    {
        if (projectToSelect)
        {
            gtApp->setCurrentProject(projectToSelect);
        }

        observedProject = gtApp->currentProject();
        return observedProject != nullptr;
    }
};

GtProject* EndToEndLegacyCalculator::projectToSelect = nullptr;
GtProject* EndToEndLegacyCalculator::observedProject = nullptr;

std::unique_ptr<TestSession> sessionWithProjects(TestProject*& first,
                                                 TestProject*& second)
{
    auto session = std::make_unique<TestSession>();
    const QString suffix = QUuid::createUuid().toString(QUuid::WithoutBraces);
    const QString firstPath = QDir::tempPath() +
                              QStringLiteral("/gtlab-project-a-1508-") + suffix;
    const QString secondPath = QDir::tempPath() +
                               QStringLiteral("/gtlab-project-b-1508-") + suffix;
    QDir().mkpath(firstPath);
    QDir().mkpath(secondPath);
    const auto writeProjectFile = [](const QString& path,
                                     const QString& name) {
        QFile file(QDir(path).filePath(GtProject::mainFilename()));
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            file.write(QStringLiteral(
                           "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                           "<GTLAB projectname=\"%1\" version=\"1.7.0-rc1\">\n"
                           "    <env-footprint>\n"
                           "        <core-ver>2.0.0</core-ver>\n"
                           "        <modules/>\n"
                           "    </env-footprint>\n"
                           "    <comment/>\n"
                           "    <MODULES/>\n"
                           "    <PROCESSES/>\n"
                           "    <LABELS/>\n"
                           "</GTLAB>\n")
                           .arg(name)
                           .toUtf8());
        }
    };
    writeProjectFile(firstPath, QStringLiteral("project-a-1508-") + suffix);
    writeProjectFile(secondPath, QStringLiteral("project-b-1508-") + suffix);
    first = new TestProject(firstPath);
    second = new TestProject(secondPath);
    session->addProjectForTest(first);
    session->addProjectForTest(second);
    return session;
}

} // namespace

TEST(CurrentProjectCompatibility, guiFallbackTracksSelectedProject)
{
    TestApplication application;
    TestProject* first = nullptr;
    TestProject* second = nullptr;
    auto session = sessionWithProjects(first, second);
    application.installSession(std::move(session));
    ASSERT_TRUE(gtDataModel->openProject(first));

    EXPECT_EQ(gtApp->currentProject(), first);
}

TEST(CurrentProjectCompatibility, legacyCalculatorReadsSelectedProject)
{
    TestApplication application;
    TestProject* first = nullptr;
    TestProject* second = nullptr;
    auto session = sessionWithProjects(first, second);
    application.installSession(std::move(session));
    ASSERT_TRUE(gtDataModel->openProject(first));

    TestRunnable runnable;
    LegacyCurrentProjectCalculator calculator;
    calculator.setParent(&runnable);

    ASSERT_TRUE(calculator.exec());
    EXPECT_EQ(calculator.observedProject, first);
}

TEST(CurrentProjectCompatibility, executionContextOverridesGuiFallback)
{
    TestApplication application;
    TestProject* first = nullptr;
    TestProject* second = nullptr;
    auto session = sessionWithProjects(first, second);
    application.installSession(std::move(session));
    ASSERT_TRUE(gtDataModel->openProject(first));

    GtExecutionContext context(second);
    GtExecutionContextScope scope(context);

    EXPECT_EQ(gtApp->currentProject(), second);
    EXPECT_EQ(gtDataModel->currentProject(), second);
}

TEST(CurrentProjectCompatibility, leavingExecutionContextRestoresGuiFallback)
{
    TestApplication application;
    TestProject* first = nullptr;
    TestProject* second = nullptr;
    auto session = sessionWithProjects(first, second);
    application.installSession(std::move(session));
    ASSERT_TRUE(gtDataModel->openProject(first));

    {
        GtExecutionContext context(second);
        GtExecutionContextScope scope(context);
        EXPECT_EQ(gtApp->currentProject(), second);
    }

    EXPECT_EQ(gtApp->currentProject(), first);
}

TEST(CurrentProjectCompatibility, saveAndCloseRejectBusyProject)
{
    TestApplication application;
    TestProject* first = nullptr;
    TestProject* second = nullptr;
    auto session = sessionWithProjects(first, second);
    application.installSession(std::move(session));
    ASSERT_TRUE(gtDataModel->openProject(first));

    GtProjectExecutionGuard guard;
    ASSERT_EQ(guard.tryAcquire(first),
              GtProjectExecutionGuard::Result::Acquired);

    EXPECT_FALSE(gtDataModel->saveProject(first));
    EXPECT_FALSE(gtDataModel->closeProject(first));
}

TEST(CurrentProjectCompatibility, nestedExecutionContextsRestorePreviousProject)
{
    TestApplication application;
    TestProject* first = nullptr;
    TestProject* second = nullptr;
    auto session = sessionWithProjects(first, second);
    application.installSession(std::move(session));
    ASSERT_TRUE(gtDataModel->openProject(first));

    GtExecutionContext outer(second);
    GtExecutionContext inner(first);
    GtExecutionContextScope outerScope(outer);
    EXPECT_EQ(gtApp->currentProject(), second);
    {
        GtExecutionContextScope innerScope(inner);
        EXPECT_EQ(gtApp->currentProject(), first);
    }
    EXPECT_EQ(gtApp->currentProject(), second);
}

TEST(CurrentProjectCompatibility,
     endToEndLegacyExecutionKeepsProjectContextWhenGuiSelectionChanges)
{
    TestApplication application;
    TestProject* first = nullptr;
    TestProject* second = nullptr;
    auto session = sessionWithProjects(first, second);
    application.installSession(std::move(session));
    ASSERT_TRUE(gtDataModel->openProject(first));

    gtObjectFactory->registerClass(
        EndToEndLegacyCalculator::staticMetaObject);
    gtObjectFactory->registerClass(GtTask::staticMetaObject);

    auto task = std::make_unique<GtTask>();
    task->setFactory(gtObjectFactory);
    auto calculator = std::make_unique<EndToEndLegacyCalculator>();
    calculator->setFactory(gtObjectFactory);
    ASSERT_TRUE(task->appendChild(calculator.get()));
    calculator.release();
    ASSERT_TRUE(second->appendChild(task.get()));
    task.release();

    EndToEndLegacyCalculator::projectToSelect = first;
    EndToEndLegacyCalculator::observedProject = nullptr;

    auto* runnable = new GtRunnable({}, GtExecutionContext(second));
    GtTaskRunner runner(second->findDirectChildren<GtTask*>().front());
    ASSERT_TRUE(runner.setUp(runnable, second));

    runnable->run();

    EXPECT_EQ(EndToEndLegacyCalculator::observedProject, second);
    EXPECT_EQ(gtApp->currentProject(), first);

    EndToEndLegacyCalculator::projectToSelect = nullptr;
    EndToEndLegacyCalculator::observedProject = nullptr;
}

#include "test_currentproject_compatibility.moc"
