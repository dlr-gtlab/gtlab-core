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
#include <utility>
#include <vector>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QStringList>
#include <QTemporaryDir>

#include "gt_abstractrunnable.h"
#include "gt_calculator.h"
#include "gt_coreapplication.h"
#include "gt_coredatamodel.h"
#include "gt_executioncontext.h"
#include "gt_externalizationmanager.h"
#include "gt_logging.h"
#include "gt_processcomponent.h"
#include "gt_project.h"
#include "gt_runnable.h"
#include "gt_session.h"
#include "gt_task.h"
#include "internal/gt_legacyprojectaccess.h"

TEST(LegacyProjectAccessRegistry, marksEachKeyOnlyOnce)
{
    gt::detail::LegacyProjectAccessRegistry registry;

    EXPECT_TRUE(registry.markIfNew(QStringLiteral("module/Component")));
    EXPECT_FALSE(registry.markIfNew(QStringLiteral("module/Component")));
    EXPECT_TRUE(registry.markIfNew(QStringLiteral("module/OtherComponent")));
}

TEST(LegacyProjectAccessRegistry, concurrentCallsMarkKeyOnlyOnce)
{
    gt::detail::LegacyProjectAccessRegistry registry;
    QString const key = QStringLiteral("module/Component");
    std::atomic<int> markedCount{0};
    std::vector<std::thread> workers;
    workers.reserve(16);

    for (int i = 0; i < 16; ++i)
    {
        workers.emplace_back([&registry, &key, &markedCount] {
            if (registry.markIfNew(key))
            {
                ++markedCount;
            }
        });
    }

    for (std::thread& worker : workers)
    {
        worker.join();
    }

    EXPECT_EQ(markedCount.load(), 1);
}

namespace
{

    const QString kWarningMarker =
        QStringLiteral("Legacy project access detected");

    class TestProject : public GtProject
    {
    public:
        explicit TestProject(QString path) : GtProject(std::move(path))
        {
        }
    };

    class TestSession : public GtSession
    {
    public:
        static bool createEmptySessionForTest(const QString& id)
        {
            return createEmptySession(id);
        }

        void addProjectForTest(GtProject* project)
        {
            addProject(project);
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
            constexpr auto sessionId = "legacyprojectaccess";
            ASSERT_FALSE(roamingPath().isEmpty());
            ASSERT_TRUE(QDir().mkpath(roamingPath()));
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
            gt::log::Logger::instance().removeDestination(
                "legacy-access-recorder");
        }

        QStringList warnings() const
        {
            QMutexLocker locker(&m_mutex);
            return m_warnings.filter(kWarningMarker);
        }

    private:
        mutable QMutex m_mutex;
        QStringList m_warnings;
    };

    QString writeProjectFile(const QString& path, const QString& name)
    {
        QDir().mkpath(path);
        QFile file(QDir(path).filePath(GtProject::mainFilename()));
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            return {};
        }
        file.write(
            QStringLiteral("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                           "<GTLAB projectname=\"%1\" version=\"1.7.0-rc1\">\n"
                           "    <env-footprint><core-ver>2.0.0</core-ver>"
                           "<modules/></env-footprint>\n"
                           "    <comment/><MODULES/><PROCESSES/><LABELS/>\n"
                           "</GTLAB>\n")
                .arg(name)
                .toUtf8());
        return path;
    }

    class RepeatedAccessCalculator : public GtCalculator
    {
        Q_OBJECT
    public:
        bool run() override
        {
            first = gtApp->currentProject();
            second = gtApp->currentProject();
            return true;
        }
        GtProject* first = nullptr;
        GtProject* second = nullptr;
    };

    class NestedAccessCalculator : public GtCalculator
    {
        Q_OBJECT
    public:
        bool run() override
        {
            observed = gtApp->currentProject();
            return true;
        }
        GtProject* observed = nullptr;
    };

    class ParentCalculator : public GtCalculator
    {
        Q_OBJECT
    public:
        bool run() override
        {
            if (!child || !child->exec())
            {
                return false;
            }
            observed = gtApp->currentProject();
            return true;
        }
        NestedAccessCalculator* child = nullptr;
        GtProject* observed = nullptr;
    };

    class PathOnlyCalculator : public GtCalculator
    {
        Q_OBJECT
    public:
        bool run() override
        {
            observed = gtApp->currentProject();
            return true;
        }
        GtProject* observed = nullptr;
    };

    class DisabledAccessCalculator : public GtCalculator
    {
        Q_OBJECT
    public:
        bool run() override
        {
            observed = gtApp->currentProject();
            return true;
        }
        GtProject* observed = nullptr;
    };

    class DirectAccessComponent : public GtProcessComponent
    {
        Q_OBJECT

    public:
        bool exec() override
        {
            observed = gtApp->currentProject();
            return true;
        }

        GtProject* observed = nullptr;
    };

    class LegacyProjectAccessTest : public ::testing::Test
    {
    protected:
        static void SetUpTestSuite()
        {
            qunsetenv("GTLAB_LEGACY_PROJECT_ACCESS_WARNING");
            tempDir = std::make_unique<QTemporaryDir>();
            ASSERT_TRUE(tempDir->isValid());
            application = std::make_unique<TestApplication>();

            auto session = std::make_unique<TestSession>();
            guiProject = new TestProject(
                writeProjectFile(tempDir->filePath(QStringLiteral("gui")),
                                 QStringLiteral("gui")));
            executionProject = new TestProject(
                writeProjectFile(tempDir->filePath(QStringLiteral("execution")),
                                 QStringLiteral("execution")));
            ASSERT_FALSE(guiProject->path().isEmpty());
            ASSERT_FALSE(executionProject->path().isEmpty());
            session->addProjectForTest(guiProject);
            session->addProjectForTest(executionProject);
            application->installSession(std::move(session));
            ASSERT_TRUE(gtDataModel->openProject(guiProject));
        }

        static void TearDownTestSuite()
        {
            application.reset();
            tempDir.reset();
        }

        static std::unique_ptr<QTemporaryDir> tempDir;
        static std::unique_ptr<TestApplication> application;
        static TestProject* guiProject;
        static TestProject* executionProject;
    };

    std::unique_ptr<QTemporaryDir> LegacyProjectAccessTest::tempDir;
    std::unique_ptr<TestApplication> LegacyProjectAccessTest::application;
    TestProject* LegacyProjectAccessTest::guiProject = nullptr;
    TestProject* LegacyProjectAccessTest::executionProject = nullptr;

} // namespace

TEST_F(LegacyProjectAccessTest, doesNotWarnOutsideDeveloperMode)
{
    gtApp->setDevMode(false);
    WarningRecorder recorder;

    GtRunnable runnable({}, GtExecutionContext(executionProject));
    auto* calculator = new DisabledAccessCalculator;
    ASSERT_TRUE(runnable.appendProcessComponent(calculator));
    runnable.run();

    EXPECT_TRUE(runnable.successful());
    EXPECT_EQ(calculator->observed, executionProject);
    EXPECT_TRUE(recorder.warnings().isEmpty());
}

TEST_F(LegacyProjectAccessTest, attributesActualExecutionAndPreservesResolution)
{
    gtApp->setDevMode(true);
    WarningRecorder recorder;

    GtRunnable runnable({}, GtExecutionContext(executionProject));
    auto* repeated = new RepeatedAccessCalculator;
    auto* parent = new ParentCalculator;
    auto* child = new NestedAccessCalculator;
    parent->child = child;
    ASSERT_TRUE(runnable.appendProcessComponent(repeated));
    ASSERT_TRUE(runnable.appendProcessComponent(parent));
    ASSERT_TRUE(runnable.appendProcessComponent(child));

    runnable.run();

    EXPECT_TRUE(runnable.successful());
    EXPECT_EQ(repeated->first, executionProject);
    EXPECT_EQ(repeated->second, executionProject);
    EXPECT_EQ(parent->observed, executionProject);
    EXPECT_EQ(child->observed, executionProject);
    const QStringList warnings = recorder.warnings();
    ASSERT_EQ(warnings.size(), 3);
    EXPECT_TRUE(
        warnings.at(0).contains(QStringLiteral("RepeatedAccessCalculator")));
    EXPECT_TRUE(
        warnings.at(1).contains(QStringLiteral("NestedAccessCalculator")));
    EXPECT_TRUE(warnings.at(2).contains(QStringLiteral("ParentCalculator")));

    // GUI selection outside execution remains a supported silent use.
    EXPECT_EQ(gtApp->currentProject(), guiProject);
    EXPECT_EQ(recorder.warnings().size(), 3);
}

TEST_F(LegacyProjectAccessTest, pathOnlyContextWarnsWithoutClaimingResolution)
{
    gtApp->setDevMode(true);
    WarningRecorder recorder;

    GtRunnable runnable(
        {}, GtExecutionContext(nullptr, QStringLiteral("/path-only-project")));
    auto* calculator = new PathOnlyCalculator;
    ASSERT_TRUE(runnable.appendProcessComponent(calculator));
    runnable.run();

    EXPECT_TRUE(runnable.successful());
    EXPECT_EQ(calculator->observed, nullptr);
    const QStringList warnings = recorder.warnings();
    ASSERT_EQ(warnings.size(), 1);
    EXPECT_TRUE(
        warnings.front().contains(QStringLiteral("result may be null")));
}

TEST_F(LegacyProjectAccessTest, attributesDirectProcessComponentChild)
{
    gtApp->setDevMode(true);
    WarningRecorder recorder;

    GtRunnable runnable({}, GtExecutionContext(executionProject));
    auto* task = new GtTask;
    auto* component = new DirectAccessComponent;
    ASSERT_TRUE(task->appendChild(component));
    ASSERT_TRUE(runnable.appendProcessComponent(task));

    runnable.run();

    EXPECT_TRUE(runnable.successful());
    EXPECT_EQ(component->observed, executionProject);
    const QStringList warnings = recorder.warnings();
    ASSERT_EQ(warnings.size(), 1);
    EXPECT_TRUE(
        warnings.front().contains(QStringLiteral("DirectAccessComponent")));
    EXPECT_FALSE(warnings.front().contains(QStringLiteral("'Task'")));
}

#include "test_gt_legacyprojectaccess.moc"
