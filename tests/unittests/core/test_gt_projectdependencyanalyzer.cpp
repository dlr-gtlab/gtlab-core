/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_projectdependencyanalyzer.h"

#include "gt_abstractobjectfactory.h"
#include "gt_calculator.h"
#include "gt_coreapplication.h"
#include "gt_coredatamodel.h"
#include "gt_externalizationmanager.h"
#include "gt_footprint.h"
#include "gt_label.h"
#include "gt_modules.h"
#include "gt_object.h"
#include "gt_objectfactory.h"
#include "gt_objectgroup.h"
#include "gt_objectmemento.h"
#include "gt_processfactory.h"
#include "gt_project.h"
#include "gt_projectprovider.h"
#include "gt_task.h"
#include "gt_testhelper.h"
#include "gt_versionnumber.h"

#include <QCoreApplication>
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QUuid>

#include <initializer_list>
#include <memory>

namespace
{

    class TestProject : public GtProject
    {
    public:
        explicit TestProject(const QString& path) : GtProject(path)
        {
        }
    };

    class TestProjectObjectFactory : public GtAbstractObjectFactory
    {
    };

    bool writeFile(const QString& path, const QByteArray& data)
    {
        if (!QFileInfo(path).absoluteDir().mkpath(
                QFileInfo(path).absolutePath()))
        {
            return false;
        }

        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate |
                       QIODevice::Text))
        {
            return false;
        }

        return file.write(data) == data.size();
    }

    /*
 * @brief Replaces the stored environment footprint of a project file
 * @param projectPath Project directory
 * @param footprintXml Replacement footprint XML ("<env-footprint>" element)
 */
    bool replaceStoredFootprint(const QString& projectPath,
                                const QString& footprintXml)
    {
        const QString filename =
            projectPath + QDir::separator() + GtProject::mainFilename();

        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            return false;
        }
        const QString content = QString::fromUtf8(file.readAll());
        file.close();

        const QRegularExpression rx(
            QStringLiteral("<env-footprint>.*</env-footprint>"),
            QRegularExpression::DotMatchesEverythingOption);

        const auto match = rx.match(content);
        if (!match.hasMatch())
        {
            return false;
        }

        QString updated = content;
        updated.replace(match.capturedStart(0), match.capturedLength(0),
                        footprintXml);

        return writeFile(filename, updated.toUtf8());
    }

    QStringList footprintModuleIds(GtProject* project)
    {
        return GtFootprint{project->readFootprint()}.modules().keys();
    }

} // namespace

/*
 * Classes that are provided by modules of the current environment. They are
 * registered at the object factory or at the process factories during tests.
 */
class FootprintUnusedClass : public GtObject
{
    Q_OBJECT

public:
    Q_INVOKABLE FootprintUnusedClass() = default;
};

class FootprintModuleTask : public GtTask
{
    Q_OBJECT

public:
    Q_INVOKABLE FootprintModuleTask() = default;
};

class FootprintModuleCalculator : public GtCalculator
{
    Q_OBJECT

public:
    Q_INVOKABLE FootprintModuleCalculator() = default;

    bool run() override
    {
        return true;
    }
};

/*
 * Fixture: batch application with session, projects are created and opened
 * through the data model. Modules are loaded through the public application API.
 *
 * Note: the object factory is declared before the application so that it
 * outlives the project object tree.
 */
class ProjectFootprintTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_previousExternalizationDir = gtExternalizationManager->projectDir();
        m_previousModuleDirs = qgetenv("GTLAB_MODULE_DIRS");
        qputenv("GTLAB_MODULE_DIRS",
                QFileInfo(QString::fromUtf8(FOOTPRINT_ModuleB_PATH))
                    .absolutePath()
                    .toUtf8());
        m_previousConfigHome = qgetenv("XDG_CONFIG_HOME");
        qputenv("XDG_CONFIG_HOME",
                gtTestHelper->newTempDir().absolutePath().toUtf8());

        ASSERT_TRUE(m_moduleFactory.registerClass(GT_METADATA(GtObject),
                                                  QStringLiteral("ModuleA")));

        m_app = std::make_unique<GtCoreApplication>(
            QCoreApplication::instance(), GtCoreApplication::AppMode::Batch);
        m_app->init();

        QDir roamingPath(GtCoreApplication::roamingPath());
        ASSERT_TRUE(roamingPath.mkpath(QStringLiteral("session")));

        m_sessionId = QStringLiteral("project-footprint-") +
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
        if (m_previousModuleDirs.isNull())
            qunsetenv("GTLAB_MODULE_DIRS");
        else
            qputenv("GTLAB_MODULE_DIRS", m_previousModuleDirs);
        gtExternalizationManager->onProjectLoaded(m_previousExternalizationDir);

        if (m_previousConfigHome.isNull())
        {
            qunsetenv("XDG_CONFIG_HOME");
        }
        else
        {
            qputenv("XDG_CONFIG_HOME", m_previousConfigHome);
        }
    }

    GtProject* createProject(const QString& name,
                             const QStringList& moduleIds = {})
    {
        GtProjectProvider provider;
        provider.setProjectName(name);
        provider.setProjectModules(moduleIds);
        provider.setProjectPath(gtTestHelper->newTempDir().absolutePath());
        auto* project = provider.project();
        if (project)
        {
            EXPECT_TRUE(gtDataModel->newProject(project, true));
        }
        return project;
    }

    /// @brief Creates a dummy object for a class that is not registered
    GtObject* createDummy(const QString& className)
    {
        QDomDocument doc;
        if (!doc.setContent(
                QStringLiteral(
                    "<object class=\"%1\" name=\"dummy\""
                    " uuid=\"{00000000-0000-0000-0000-000000000001}\"/>")
                    .arg(className)))
        {
            return nullptr;
        }

        GtObjectMemento memento(doc.documentElement());
        auto* dummy = memento.restore<GtObject*>(gtObjectFactory);
        if (dummy)
        {
            EXPECT_TRUE(dummy->isDummy());
        }
        return dummy;
    }

    TestProjectObjectFactory m_moduleFactory;
    std::unique_ptr<GtCoreApplication> m_app;
    QString m_sessionId;
    QByteArray m_previousConfigHome;
    QByteArray m_previousModuleDirs;
    QString m_previousExternalizationDir;
};

TEST_F(ProjectFootprintTest, unusedEnvironmentModulesAreExcluded)
{
    GtProject* project = createProject(QStringLiteral("FootprintUnusedEnv"));
    ASSERT_NE(project, nullptr);

    // "ModuleB" is available in the current environment but none of its
    // classes is used by the project data
    TestProjectObjectFactory unusedFactory;
    ASSERT_TRUE(unusedFactory.registerClass(GT_METADATA(FootprintUnusedClass),
                                            QStringLiteral("ModuleB")));

    ASSERT_TRUE(
        m_app->loadSingleModule(QString::fromUtf8(FOOTPRINT_ModuleA_PATH)));
    ASSERT_TRUE(
        m_app->loadSingleModule(QString::fromUtf8(FOOTPRINT_ModuleB_PATH)));

    auto* object = new GtObject;
    object->setFactory(&m_moduleFactory);
    ASSERT_TRUE(project->appendChild(object));

    ASSERT_TRUE(gtDataModel->saveProject(project));

    // only the module that provides a used class is stored
    EXPECT_EQ(footprintModuleIds(project),
              QStringList{QStringLiteral("ModuleA")});

    delete object;
}

TEST_F(ProjectFootprintTest, footprintSurvivesSaveReloadRoundTrip)
{
    GtProject* project = createProject(QStringLiteral("FootprintRoundTrip"));
    ASSERT_NE(project, nullptr);

    auto* object = new GtObject;
    object->setFactory(&m_moduleFactory);
    ASSERT_TRUE(project->appendChild(object));

    // Real ModuleA metadata includes the transitive requirement ModuleB.
    ASSERT_TRUE(m_app->loadSingleModule(
        QString::fromUtf8(FOOTPRINT_SuccessfulOverride_PATH)));

    ASSERT_TRUE(gtDataModel->saveProject(project));

    const QStringList expected{QStringLiteral("ModuleA"),
                               QStringLiteral("ModuleB")};
    EXPECT_EQ(footprintModuleIds(project), expected);

    delete object;

    // reload the project from its saved files. The in-memory factory based
    // provenance is gone, so the dependency information can only survive when
    // it was persisted during the save above (#1171 class provider metadata).
    TestProject reloaded(project->path());
    ASSERT_TRUE(reloaded.isValid());

    // the stored footprint of the project still contains the required module
    // and its transitive dependency, with no unrelated module
    EXPECT_EQ(footprintModuleIds(&reloaded), expected);

    // the persistent class provider metadata still attributes the used class to
    // its providing module, i.e. the dependency can be resolved again after the
    // save/reload round trip.
    EXPECT_EQ(reloaded.classModuleId(QStringLiteral("GtObject")),
              QStringLiteral("ModuleA"));
}

TEST_F(ProjectFootprintTest, taskAndCalculatorModulesAreStored)
{
    GtProject* project = createProject(QStringLiteral("FootprintTask"));
    ASSERT_NE(project, nullptr);

    // release the global process factories also when a check below fails
    struct FactoryCleanup
    {
        ~FactoryCleanup()
        {
            gtProcessFactory->taskFactory()->unregisterClass(
                GT_METADATA(FootprintModuleTask));
            gtProcessFactory->calculatorFactory()->unregisterClass(
                GT_METADATA(FootprintModuleCalculator));
        }
    } factoryCleanup;

    // task and calculator classes are provided by the process factories
    ASSERT_TRUE(gtProcessFactory->taskFactory()->registerClass(
        GT_METADATA(FootprintModuleTask), QStringLiteral("TaskModule")));
    ASSERT_TRUE(gtProcessFactory->calculatorFactory()->registerClass(
        GT_METADATA(FootprintModuleCalculator),
        QStringLiteral("CalculatorModule")));

    auto* task = new FootprintModuleTask;
    auto* calculator = new FootprintModuleCalculator;
    ASSERT_TRUE(task->appendChild(calculator));
    ASSERT_TRUE(project->appendChild(task));

    EXPECT_TRUE(gtDataModel->saveProject(project));

    EXPECT_EQ(footprintModuleIds(project),
              (QStringList{QStringLiteral("CalculatorModule"),
                           QStringLiteral("TaskModule")}));

    // deleting the task also deletes its calculator. Both modules are no
    // longer required, although their classes are still listed in the
    // class provider manifest of the project.
    delete task;

    EXPECT_TRUE(gtDataModel->saveProject(project));
    EXPECT_TRUE(footprintModuleIds(project).isEmpty());
}

TEST_F(ProjectFootprintTest, unusedModulesArePrunedWhenMetadataIsKnown)
{
    GtProject* project = createProject(QStringLiteral("FootprintPrune"));
    ASSERT_NE(project, nullptr);

    ASSERT_TRUE(
        m_app->loadSingleModule(QString::fromUtf8(FOOTPRINT_ModuleA_PATH)));

    auto* object = new GtObject;
    object->setFactory(&m_moduleFactory);
    ASSERT_TRUE(project->appendChild(object));

    ASSERT_TRUE(gtDataModel->saveProject(project));
    EXPECT_EQ(footprintModuleIds(project),
              QStringList{QStringLiteral("ModuleA")});

    // the object using a class of ModuleA is removed; as the dependency
    // metadata of ModuleA is available, the module is no longer required
    delete object;

    ASSERT_TRUE(gtDataModel->saveProject(project));
    EXPECT_TRUE(footprintModuleIds(project).isEmpty());

    // the stale provider manifest must not create a module dependency but
    // it is still stored for currently unavailable modules
    QFile file(project->path() + QDir::separator() + GtProject::mainFilename());
    ASSERT_TRUE(file.open(QIODevice::ReadOnly | QIODevice::Text));
    EXPECT_TRUE(
        QString::fromUtf8(file.readAll()).contains(QStringLiteral("ModuleA")));
}

TEST_F(ProjectFootprintTest, knownClassesWithoutProviderAreIgnored)
{
    GtProject* project = createProject(QStringLiteral("CoreClasses"));
    ASSERT_NE(project, nullptr);

    auto* group = new GtObjectGroup;
    ASSERT_TRUE(project->appendChild(group));
    auto* label = new GtLabel;
    ASSERT_TRUE(group->appendChild(label));

    GtProjectDependencyAnalyzer analyzer(project);

    EXPECT_TRUE(
        analyzer.usedClassNames().contains(QStringLiteral("GtObjectGroup")));

    // module ownership is never guessed: known classes without a module
    // provider (e.g. core classes) do not create module dependencies and
    // do not invalidate the footprint
    EXPECT_TRUE(analyzer.directlyRequiredModuleIds().isEmpty());
    EXPECT_TRUE(analyzer.unknownUsedClassNames().isEmpty());

    ASSERT_TRUE(gtDataModel->saveProject(project));

    EXPECT_TRUE(footprintModuleIds(project).isEmpty());

    delete group;
}

TEST_F(ProjectFootprintTest, storedVersionsArePreservedWithoutMetadata)
{
    GtProject* project = createProject(QStringLiteral("FootprintFallback"));
    ASSERT_NE(project, nullptr);

    auto* object = new GtObject;
    object->setFactory(&m_moduleFactory);
    ASSERT_TRUE(project->appendChild(object));

    // no module metadata available: the required module is unresolved
    ASSERT_TRUE(replaceStoredFootprint(
        project->path(),
        QStringLiteral("<env-footprint>"
                       "<core-ver>2.0.0</core-ver>"
                       "<modules>"
                       "<module><id>ModuleA</id><ver>4.5.6</ver></module>"
                       "<module><id>OldUnused</id><ver>7.7.0</ver></module>"
                       "</modules>"
                       "</env-footprint>")));

    ASSERT_TRUE(gtDataModel->saveProject(project));

    const auto modules = GtFootprint{project->readFootprint()}.modules();

    // the previously stored version of the required but unavailable module
    // is kept; a module version is never invented
    ASSERT_TRUE(modules.contains(QStringLiteral("ModuleA")));
    EXPECT_EQ(modules.value(QStringLiteral("ModuleA")).toString(),
              QStringLiteral("4.5.6"));

    // entries that cannot be proven unused are preserved as long as the
    // dependency metadata of a required module is unavailable
    ASSERT_TRUE(modules.contains(QStringLiteral("OldUnused")));
    EXPECT_EQ(modules.value(QStringLiteral("OldUnused")).toString(),
              QStringLiteral("7.7.0"));

    delete object;
}

TEST_F(ProjectFootprintTest, staleEntriesArePrunedWhenMetadataIsAvailable)
{
    GtProject* project = createProject(QStringLiteral("FootprintRecompute"));
    ASSERT_NE(project, nullptr);

    auto* object = new GtObject;
    object->setFactory(&m_moduleFactory);
    ASSERT_TRUE(project->appendChild(object));

    // module metadata is available again
    ASSERT_TRUE(
        m_app->loadSingleModule(QString::fromUtf8(FOOTPRINT_ModuleA_PATH)));

    ASSERT_TRUE(replaceStoredFootprint(
        project->path(),
        QStringLiteral("<env-footprint>"
                       "<core-ver>2.0.0</core-ver>"
                       "<modules>"
                       "<module><id>ModuleA</id><ver>4.5.6</ver></module>"
                       "<module><id>OldUnused</id><ver>7.7.0</ver></module>"
                       "</modules>"
                       "</env-footprint>")));

    ASSERT_TRUE(gtDataModel->saveProject(project));

    const auto modules = GtFootprint{project->readFootprint()}.modules();

    // the requirements can be recomputed: the stale entry is removed while the
    // required module uses the loaded version
    EXPECT_EQ(modules.keys(), QStringList{QStringLiteral("ModuleA")});
    EXPECT_EQ(modules.value(QStringLiteral("ModuleA")).toString(),
              QStringLiteral("1.2.3"));

    delete object;
}

TEST_F(ProjectFootprintTest, unloadedModuleDataPreservesFootprintEntries)
{
    GtProject* project = createProject(QStringLiteral("FootprintUnloaded"),
                                       {QStringLiteral("Ghost")});
    ASSERT_NE(project, nullptr);

    auto* object = new GtObject;
    object->setFactory(&m_moduleFactory);
    ASSERT_TRUE(project->appendChild(object));

    ASSERT_TRUE(
        m_app->loadSingleModule(QString::fromUtf8(FOOTPRINT_ModuleA_PATH)));

    // the project selects module "Ghost" whose data file exists but whose
    // package cannot be loaded because the module is not available
    ASSERT_TRUE(
        writeFile(project->path() + QDir::separator() +
                      QStringLiteral("ghost.") + GtProject::moduleExtension(),
                  QByteArrayLiteral("<GTLABMODULE uuid=\"{ghost}\"/>")));

    ASSERT_TRUE(replaceStoredFootprint(
        project->path(),
        QStringLiteral("<env-footprint>"
                       "<core-ver>2.0.0</core-ver>"
                       "<modules>"
                       "<module><id>ModuleA</id><ver>1.0.0</ver></module>"
                       "<module><id>Ghost</id><ver>3.2.1</ver></module>"
                       "<module><id>OldUnused</id><ver>7.7.0</ver></module>"
                       "</modules>"
                       "</env-footprint>")));

    ASSERT_TRUE(gtDataModel->saveProject(project));

    auto modules = GtFootprint{project->readFootprint()}.modules();

    // the module with unavailable data is kept with its stored version
    ASSERT_TRUE(modules.contains(QStringLiteral("Ghost")));
    EXPECT_EQ(modules.value(QStringLiteral("Ghost")).toString(),
              QStringLiteral("3.2.1"));
    ASSERT_TRUE(modules.contains(QStringLiteral("ModuleA")));

    delete object;

    // once the data is gone and the metadata of all required modules is
    // available, all entries are recomputed and stale entries are removed
    ASSERT_TRUE(QFile::remove(project->path() + QDir::separator() +
                              QStringLiteral("ghost.") +
                              GtProject::moduleExtension()));

    ASSERT_TRUE(gtDataModel->saveProject(project));

    modules = GtFootprint{project->readFootprint()}.modules();
    EXPECT_TRUE(modules.isEmpty());
}

TEST_F(ProjectFootprintTest, dummyObjectsKeepTheirProviderModuleRequired)
{
    const QString path = gtTestHelper->newTempDir().absolutePath();
    ASSERT_TRUE(
        writeFile(path + QDir::separator() + GtProject::mainFilename(),
                  QByteArrayLiteral(
                      "<GTLAB projectname=\"FootprintDummy\" version=\"2.1.0\">"
                      "<env-footprint><core-ver>2.1.0</core-ver><modules/></"
                      "env-footprint><MODULES/>"
                      "<METADATA><CLASS-PROVIDERS><MODULE name=\"ModuleX\">"
                      "<CLASS name=\"UnknownModuleClass\"/>"
                      "</MODULE></CLASS-PROVIDERS></METADATA></GTLAB>")));
    auto* project = new TestProject(path);
    ASSERT_TRUE(gtDataModel->newProject(project, true));

    auto* dummy = createDummy(QStringLiteral("UnknownModuleClass"));
    ASSERT_NE(dummy, nullptr);

    ASSERT_TRUE(project->appendChild(dummy));

    ASSERT_TRUE(gtDataModel->saveProject(project));

    const auto modules = GtFootprint{project->readFootprint()}.modules();

    // the dummy object still requires its providing module ...
    EXPECT_EQ(modules.keys(), QStringList{QStringLiteral("ModuleX")});

    // ... but the version of the unavailable module is not invented
    EXPECT_TRUE(modules.value(QStringLiteral("ModuleX")).isNull());
}

TEST_F(ProjectFootprintTest, legacyDummyWithoutProviderKeepsOldFootprint)
{
    GtProject* project = createProject(QStringLiteral("FootprintLegacy"));
    ASSERT_NE(project, nullptr);

    // a legacy project contains a dummy object of a class that has never
    // been stored with class provider metadata
    auto* dummy = createDummy(QStringLiteral("LegacyUnknownClass"));
    ASSERT_NE(dummy, nullptr);
    ASSERT_TRUE(project->appendChild(dummy));

    // the provider of the unknown class cannot be determined
    GtProjectDependencyAnalyzer analyzer(project);
    EXPECT_TRUE(analyzer.directlyRequiredModuleIds().isEmpty());
    EXPECT_EQ(analyzer.unknownUsedClassNames(),
              QSet<QString>{QStringLiteral("LegacyUnknownClass")});

    ASSERT_TRUE(replaceStoredFootprint(
        project->path(),
        QStringLiteral("<env-footprint>"
                       "<core-ver>2.0.0</core-ver>"
                       "<modules>"
                       "<module><id>LegacyModule</id><ver>1.2.3</ver></module>"
                       "<module><id>OtherModule</id><ver>2.0.0</ver></module>"
                       "</modules>"
                       "</env-footprint>")));

    ASSERT_TRUE(gtDataModel->saveProject(project));

    const auto modules = GtFootprint{project->readFootprint()}.modules();

    // no entry of the previously stored footprint is proven to be unused
    EXPECT_EQ(modules.keys(), (QStringList{QStringLiteral("LegacyModule"),
                                           QStringLiteral("OtherModule")}));
    EXPECT_EQ(modules.value(QStringLiteral("LegacyModule")).toString(),
              QStringLiteral("1.2.3"));
    EXPECT_EQ(modules.value(QStringLiteral("OtherModule")).toString(),
              QStringLiteral("2.0.0"));

    // the footprint can be recalculated after the unknown object is removed
    delete dummy;

    ASSERT_TRUE(gtDataModel->saveProject(project));

    EXPECT_TRUE(GtFootprint{project->readFootprint()}.modules().isEmpty());
}

TEST(GtProjectDependencyAnalyzer, analyzerAcceptsNullProject)
{
    GtProjectDependencyAnalyzer analyzer(nullptr);

    EXPECT_TRUE(analyzer.usedClassNames().isEmpty());
    EXPECT_TRUE(analyzer.directlyRequiredModuleIds().isEmpty());
    EXPECT_TRUE(analyzer.unknownUsedClassNames().isEmpty());
}

#include "test_gt_projectdependencyanalyzer.moc"
