/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <memory>

#include <QCoreApplication>
#include <QDir>

#include "gt_abstractrunnable.h"
#include "gt_calculator.h"
#include "gt_coreapplication.h"
#include "gt_project.h"
#include "gt_session.h"

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

    void installSession(std::unique_ptr<TestSession> session)
    {
        m_session = std::move(session);
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

std::unique_ptr<TestSession> sessionWithProjects(TestProject*& first,
                                                 TestProject*& second)
{
    auto session = std::make_unique<TestSession>();
    first = new TestProject(QStringLiteral("/test/project-a"));
    second = new TestProject(QStringLiteral("/test/project-b"));
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
    ASSERT_TRUE(session->selectProjectForTest(first));
    application.installSession(std::move(session));

    EXPECT_EQ(gtApp->currentProject(), first);

    ASSERT_TRUE(gtApp->setCurrentProject(second));
    EXPECT_EQ(gtApp->currentProject(), second);
}

TEST(CurrentProjectCompatibility, legacyCalculatorReadsSelectedProject)
{
    TestApplication application;
    TestProject* first = nullptr;
    TestProject* second = nullptr;
    auto session = sessionWithProjects(first, second);
    ASSERT_TRUE(session->selectProjectForTest(second));
    application.installSession(std::move(session));

    TestRunnable runnable;
    LegacyCurrentProjectCalculator calculator;
    calculator.setParent(&runnable);

    ASSERT_TRUE(calculator.exec());
    EXPECT_EQ(calculator.observedProject, second);
}
