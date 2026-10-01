/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <QStringList>
#include <QUuid>

#include "gt_abstractrunnable.h"
#include "gt_calculator.h"
#include "gt_coreapplication.h"
#include "gt_coredatamodel.h"
#include "gt_executioncontext.h"
#include "gt_externalizationmanager.h"
#include "gt_logging.h"
#include "gt_project.h"
#include "gt_runnable.h"
#include "gt_session.h"

#include "internal/gt_legacyprojectaccess.h"

namespace {

const QString kWarningMarker = QStringLiteral("Legacy project access detected");
const QString kFirstComponentClass = QStringLiteral("LegacyAccessCalculator");
const QString kSecondComponentClass = QStringLiteral("OtherAccessCalculator");

/// Project with accessible constructor
class TestProject : public GtProject
{
public:
    explicit TestProject(const QString& path) : GtProject(path) {}
};

/// Session exposing the protected project handling to the tests
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
};

/// Batch application with the standard initialization
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
        constexpr auto sessionId = "legacyprojectaccess";
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

/// Runnable that only provides the parent required by calculators
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

/// Calculator reading the current project the legacy way
class LegacyAccessCalculator : public GtCalculator
{
    Q_OBJECT

public:
    bool run() override
    {
        observedProject = gtApp->currentProject();
        ++accessCount;
        return true;
    }

    GtProject* observedProject = nullptr;
    int accessCount = 0;
};

/// Second calculator class used to verify per class deduplication
class OtherAccessCalculator : public GtCalculator
{
    Q_OBJECT

public:
    bool run() override
    {
        observedProject = gtApp->currentProject();
        return true;
    }

    GtProject* observedProject = nullptr;
};

/// Creates a project directory with a minimal project file on disk
QString createProjectPath(const QString& name)
{
    const QString suffix = QUuid::createUuid().toString(QUuid::WithoutBraces);
    const QString path = QDir::tempPath() + QStringLiteral("/gtlab-legacy-") +
                         name + QStringLiteral("-") + suffix;
    QDir().mkpath(path);

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
                       .arg(name + QStringLiteral("-") + suffix)
                       .toUtf8());
    }

    return path;
}

/// Collects the warnings that are logged while the recorder is alive
class WarningRecorder
{
public:
    WarningRecorder()
    {
        auto destination = gt::log::makeFunctorDestination(
            [this](std::string const& message, gt::log::Level level,
                   gt::log::Details const&) {
                if (level != gt::log::WarningLevel)
                {
                    return;
                }

                QMutexLocker locker(&m_mutex);
                m_warnings.append(QString::fromStdString(message));
            });

        gt::log::Logger::instance().addDestination("legacy-access-recorder",
                                                   std::move(destination));
    }

    ~WarningRecorder()
    {
        gt::log::Logger::instance().removeDestination("legacy-access-recorder");
    }

    WarningRecorder(WarningRecorder const&) = delete;
    WarningRecorder& operator=(WarningRecorder const&) = delete;

    /// Legacy access warnings collected so far
    QStringList legacyAccessWarnings() const
    {
        QMutexLocker locker(&m_mutex);
        return m_warnings.filter(kWarningMarker);
    }

    /// Number of legacy access warnings collected so far
    int count() const
    {
        return static_cast<int>(legacyAccessWarnings().size());
    }

private:
    mutable QMutex m_mutex;
    QStringList m_warnings;
};

/// Fixture with a session containing a GUI and an execution project
class LegacyProjectAccessTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        clearDiagnosticState();

        application = std::make_unique<TestApplication>();

        auto session = std::make_unique<TestSession>();

        guiProject = new TestProject(createProjectPath(QStringLiteral("gui")));
        execProject = new TestProject(
                createProjectPath(QStringLiteral("exec")));

        session->addProjectForTest(guiProject);
        session->addProjectForTest(execProject);

        application->installSession(std::move(session));

        ASSERT_TRUE(gtDataModel->openProject(guiProject));

        // the session project is the resolution result without context
        ASSERT_EQ(gtApp->currentProject(), guiProject);
    }

    void TearDown() override
    {
        application.reset();
        clearDiagnosticState();
    }

    static void clearDiagnosticState()
    {
        GtLegacyProjectAccess::clearRegistry();
        GtLegacyProjectAccess::setDeveloperModePolicy(
            GtLegacyProjectAccess::DeveloperModePolicy{});
    }

    /// Enables or disables GTlab developer mode
    void setDeveloperMode(bool value)
    {
        ASSERT_NE(gtApp, nullptr);
        gtApp->setDevMode(value);
    }

    /// Executes a calculator of type T with an execution context installed
    template <typename T>
    GtProject* runLegacyAccess(GtProject* contextProject)
    {
        TestRunnable runnable;
        T calculator;
        calculator.setParent(&runnable);

        GtExecutionContext context(contextProject);
        GtExecutionContextScope contextScope(context);
        GtProcessComponentExecutionScope componentScope(&calculator);

        EXPECT_TRUE(calculator.exec());

        return calculator.observedProject;
    }

    std::unique_ptr<TestApplication> application;
    TestProject* guiProject = nullptr;
    TestProject* execProject = nullptr;
};

} // namespace

TEST_F(LegacyProjectAccessTest, developerModeWarnsOnceForLegacyAccess)
{
    setDeveloperMode(true);

    WarningRecorder recorder;

    EXPECT_EQ(runLegacyAccess<LegacyAccessCalculator>(execProject),
              execProject);

    ASSERT_EQ(recorder.count(), 1);

    QString const warning = recorder.legacyAccessWarnings().front();

    // identifies the affected component
    EXPECT_TRUE(warning.contains(kFirstComponentClass));

    // names the legacy access and the recommended alternatives
    EXPECT_TRUE(warning.contains(QStringLiteral("gtApp->currentProject()")));
    EXPECT_TRUE(warning.contains(QStringLiteral("GtExecutionContext")));

    // no global deprecation is claimed
    EXPECT_TRUE(warning.contains(QStringLiteral("not deprecated globally")));

    // points to the developer documentation
    EXPECT_TRUE(warning.contains(
        QStringLiteral("Project context and currentProject()")));
}

TEST_F(LegacyProjectAccessTest, repeatedAccessWarnsOnlyOnce)
{
    setDeveloperMode(true);

    WarningRecorder recorder;

    TestRunnable runnable;
    LegacyAccessCalculator calculator;
    calculator.setParent(&runnable);

    GtExecutionContext context(execProject);
    GtExecutionContextScope contextScope(context);
    GtProcessComponentExecutionScope componentScope(&calculator);

    for (int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(gtApp->currentProject(), execProject);
        EXPECT_EQ(gtDataModel->currentProject(), execProject);
    }

    EXPECT_EQ(recorder.count(), 1);
}

TEST_F(LegacyProjectAccessTest, repeatedExecutionsStayDeduplicated)
{
    setDeveloperMode(true);

    WarningRecorder recorder;

    for (int i = 0; i < 3; ++i)
    {
        EXPECT_EQ(runLegacyAccess<LegacyAccessCalculator>(execProject),
                  execProject);
    }

    EXPECT_EQ(recorder.count(), 1);
}

TEST_F(LegacyProjectAccessTest, differentComponentClassesWarnIndividually)
{
    setDeveloperMode(true);

    WarningRecorder recorder;

    EXPECT_EQ(runLegacyAccess<LegacyAccessCalculator>(execProject),
              execProject);
    EXPECT_EQ(runLegacyAccess<OtherAccessCalculator>(execProject), execProject);
    EXPECT_EQ(runLegacyAccess<LegacyAccessCalculator>(execProject),
              execProject);

    QStringList const warnings = recorder.legacyAccessWarnings();

    ASSERT_EQ(warnings.size(), 2);
    EXPECT_EQ(static_cast<int>(warnings.filter(kFirstComponentClass).size()), 1);
    EXPECT_EQ(static_cast<int>(warnings.filter(kSecondComponentClass).size()), 1);
}

TEST_F(LegacyProjectAccessTest, guiAccessOutsideComponentExecutionStaysSilent)
{
    setDeveloperMode(true);

    WarningRecorder recorder;

    EXPECT_EQ(gtApp->currentProject(), guiProject);
    EXPECT_EQ(gtDataModel->currentProject(), guiProject);

    EXPECT_EQ(recorder.count(), 0);
}

TEST_F(LegacyProjectAccessTest, componentAccessWithoutContextStaysSilent)
{
    setDeveloperMode(true);

    WarningRecorder recorder;

    TestRunnable runnable;
    LegacyAccessCalculator calculator;
    calculator.setParent(&runnable);

    GtProcessComponentExecutionScope componentScope(&calculator);

    // without execution context the session project is still the result, and
    // no compatibility fallback of #1514 was used
    EXPECT_EQ(gtApp->currentProject(), guiProject);
    EXPECT_EQ(recorder.count(), 0);
}

TEST_F(LegacyProjectAccessTest, contextWithoutComponentExecutionStaysSilent)
{
    setDeveloperMode(true);

    WarningRecorder recorder;

    GtExecutionContext context(execProject);
    GtExecutionContextScope contextScope(context);

    // no component is executing, e.g. GUI code reacting to a signal that was
    // emitted while an execution context was installed
    EXPECT_EQ(gtApp->currentProject(), execProject);
    EXPECT_EQ(recorder.count(), 0);
}

TEST_F(LegacyProjectAccessTest, warningsAreSuppressedOutsideDeveloperMode)
{
    setDeveloperMode(false);

    WarningRecorder recorder;

    // resolution is unchanged, only the diagnostic is suppressed
    EXPECT_EQ(runLegacyAccess<LegacyAccessCalculator>(execProject),
              execProject);

    EXPECT_EQ(recorder.count(), 0);
}

TEST_F(LegacyProjectAccessTest, developerModePolicyControlsDiagnostic)
{
    setDeveloperMode(false);

    {
        WarningRecorder recorder;
        EXPECT_EQ(runLegacyAccess<LegacyAccessCalculator>(execProject),
                  execProject);
        EXPECT_EQ(recorder.count(), 0);
    }

    // an active policy replaces the developer mode check
    GtLegacyProjectAccess::clearRegistry();
    GtLegacyProjectAccess::setDeveloperModePolicy([]() { return true; });

    {
        WarningRecorder recorder;
        EXPECT_EQ(runLegacyAccess<LegacyAccessCalculator>(execProject),
                  execProject);
        EXPECT_EQ(recorder.count(), 1);
    }

    GtLegacyProjectAccess::clearRegistry();
    GtLegacyProjectAccess::setDeveloperModePolicy([]() { return false; });

    setDeveloperMode(true);

    {
        WarningRecorder recorder;
        EXPECT_EQ(runLegacyAccess<LegacyAccessCalculator>(execProject),
                  execProject);
        EXPECT_EQ(recorder.count(), 0);
    }
}

TEST_F(LegacyProjectAccessTest, pathOnlyContextKeepsResolutionSemantics)
{
    setDeveloperMode(true);

    WarningRecorder recorder;

    TestRunnable runnable;
    LegacyAccessCalculator calculator;
    calculator.setParent(&runnable);

    GtExecutionContext context(nullptr, QStringLiteral("/path-only-project"));
    GtExecutionContextScope contextScope(context);
    GtProcessComponentExecutionScope componentScope(&calculator);

    // a path only context still does not fall back to the session project
    EXPECT_EQ(gtApp->currentProject(), nullptr);
    EXPECT_EQ(recorder.count(), 1);
}

TEST_F(LegacyProjectAccessTest, nestedScopesRestorePreviousComponent)
{
    setDeveloperMode(true);

    WarningRecorder recorder;

    TestRunnable runnable;
    LegacyAccessCalculator outer;
    OtherAccessCalculator inner;
    outer.setParent(&runnable);
    inner.setParent(&runnable);

    EXPECT_EQ(GtProcessComponentExecutionScope::current(), nullptr);

    GtExecutionContext context(execProject);
    GtExecutionContextScope contextScope(context);

    GtProcessComponentExecutionScope outerScope(&outer);
    EXPECT_EQ(GtProcessComponentExecutionScope::current(), &outer);

    {
        GtProcessComponentExecutionScope innerScope(&inner);
        EXPECT_EQ(GtProcessComponentExecutionScope::current(), &inner);

        // attributed to the innermost executing component
        EXPECT_EQ(gtApp->currentProject(), execProject);
    }

    EXPECT_EQ(GtProcessComponentExecutionScope::current(), &outer);

    EXPECT_EQ(gtApp->currentProject(), execProject);

    EXPECT_EQ(GtProcessComponentExecutionScope::current(), &outer);

    QStringList const warnings = recorder.legacyAccessWarnings();

    ASSERT_EQ(warnings.size(), 2);
    EXPECT_TRUE(warnings.at(0).contains(kSecondComponentClass));
    EXPECT_TRUE(warnings.at(1).contains(kFirstComponentClass));
}

TEST_F(LegacyProjectAccessTest, runnableExecutionInstallsComponentScope)
{
    setDeveloperMode(true);

    WarningRecorder recorder;

    GtRunnable runnable({}, GtExecutionContext(execProject));

    auto* first = new LegacyAccessCalculator;
    auto* second = new OtherAccessCalculator;
    auto* repeated = new LegacyAccessCalculator;

    ASSERT_TRUE(runnable.appendProcessComponent(first));
    ASSERT_TRUE(runnable.appendProcessComponent(second));
    ASSERT_TRUE(runnable.appendProcessComponent(repeated));

    runnable.run();

    // resolution of #1514 is unchanged for all queued components
    EXPECT_EQ(first->observedProject, execProject);
    EXPECT_EQ(second->observedProject, execProject);
    EXPECT_EQ(repeated->observedProject, execProject);
    EXPECT_TRUE(runnable.successful());

    // the runnable marks each queued component as executing, and the warning
    // is deduplicated per component class
    EXPECT_EQ(recorder.count(), 2);
}

TEST_F(LegacyProjectAccessTest, concurrentExecutionsWarnOncePerClass)
{
    setDeveloperMode(true);

    WarningRecorder recorder;

    std::atomic<int> mismatches{0};
    std::vector<std::thread> threads;
    threads.reserve(6);

    for (int i = 0; i < 6; ++i)
    {
        threads.emplace_back([this, i, &mismatches] {
            GtExecutionContext context(execProject);
            GtExecutionContextScope contextScope(context);

            if (i % 2 == 0)
            {
                LegacyAccessCalculator calculator;
                GtProcessComponentExecutionScope scope(&calculator);

                for (int k = 0; k < 25; ++k)
                {
                    if (gtApp->currentProject() != execProject)
                    {
                        ++mismatches;
                    }
                }
            }
            else
            {
                OtherAccessCalculator calculator;
                GtProcessComponentExecutionScope scope(&calculator);

                for (int k = 0; k < 25; ++k)
                {
                    if (gtDataModel->currentProject() != execProject)
                    {
                        ++mismatches;
                    }
                }
            }
        });
    }

    for (std::thread& thread : threads)
    {
        thread.join();
    }

    EXPECT_EQ(mismatches.load(), 0);
    EXPECT_EQ(recorder.count(), 2);
}

TEST_F(LegacyProjectAccessTest, clearedRegistryAllowsRepeatWarning)
{
    setDeveloperMode(true);

    WarningRecorder recorder;

    EXPECT_EQ(runLegacyAccess<LegacyAccessCalculator>(execProject),
              execProject);
    EXPECT_EQ(recorder.count(), 1);

    GtLegacyProjectAccess::clearRegistry();

    EXPECT_EQ(runLegacyAccess<LegacyAccessCalculator>(execProject),
              execProject);
    EXPECT_EQ(recorder.count(), 2);
}

#include "test_gt_legacyprojectaccess.moc"
