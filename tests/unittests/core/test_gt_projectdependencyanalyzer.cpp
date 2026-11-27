/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_projectdependencyanalyzer.h"
#include "gt_package.h"
#include "gt_objectmementodiff.h"
#include "gt_projectexecutionguard.h"

#include "gt_abstractobjectfactory.h"
#include "gt_calculator.h"
#include "gt_coreapplication.h"
#include "gt_coredatamodel.h"
#include "gt_externalizationmanager.h"
#include "gt_externalizedobject.h"
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
#include "gt_xmlutilities.h"

#include <QCoreApplication>
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QScopeGuard>
#include <QUuid>

#include <initializer_list>
#include <memory>

namespace
{

class TestProject : public GtProject
{
public:
    explicit TestProject(const QString& path) :
        GtProject(path)
    { }
};

class TestProjectObjectFactory : public GtAbstractObjectFactory
{
};

bool
writeFile(const QString& path, const QByteArray& data)
{
    if (!QFileInfo(path).absoluteDir().mkpath(QFileInfo(path).absolutePath()))
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
bool
replaceStoredFootprint(const QString& projectPath, const QString& footprintXml)
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

QStringList
footprintModuleIds(GtProject* project)
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
    std::unique_ptr<GtObject> createDummy(const QString& className)
    {
        QDomDocument doc;
        if (!doc.setContent(
                QStringLiteral(
                    "<object class=\"%1\" name=\"dummy\""
                    " uuid=\"{00000000-0000-0000-0000-000000000001}\"/>")
                    .arg(className)))
        {
            return {};
        }

        GtObjectMemento memento(doc.documentElement());
        auto dummy = memento.restore_unique(gtObjectFactory);
        if (dummy)
        {
            EXPECT_TRUE(dummy->isDummy());
        }
        return dummy;
    }

    TestProjectObjectFactory m_moduleFactory;
    TestProjectObjectFactory m_secondModuleFactory;
    std::unique_ptr<GtCoreApplication> m_app;
    QString m_sessionId;
    QByteArray m_previousConfigHome;
    QByteArray m_previousModuleDirs;
    QString m_previousExternalizationDir;
};

class ProjectPackagesTest : public ProjectFootprintTest
{
protected:
    void SetUp() override
    {
        ProjectFootprintTest::SetUp();
        ASSERT_FALSE(HasFatalFailure());
        ASSERT_TRUE(m_app->loadSingleModule(
            QString::fromUtf8(FOOTPRINT_ProjectPackages_PATH)));
    }

    void TearDown() override
    {
        if (auto* project = m_app->currentProject())
            gtDataModel->closeProject(project);
        const auto* type = gtObjectFactory->GtAbstractObjectFactory::metaObject(
            QStringLiteral("ProjectPackageFixture"));
        if (type)
        {
            const auto metaObject = *type;
            gtObjectFactory->unregisterClass(metaObject);
        }
        ProjectFootprintTest::TearDown();
    }

    const QString moduleId = QStringLiteral("ProjectPackages");
};

TEST_F(ProjectPackagesTest, packageMembershipTracksMementosAndSuccessfulSaves)
{
    auto* project = createProject(QStringLiteral("PackageMembership"));
    ASSERT_NE(project, nullptr);
    const auto file = project->moduleDataPath(moduleId);
    QString error;
    auto package = project->createPackage(moduleId, error);
    ASSERT_TRUE(package) << qPrintable(error);
    auto duplicate = project->createPackage(moduleId, error);
    ASSERT_TRUE(duplicate) << qPrintable(error);
    EXPECT_TRUE(
        gtDataModel->appendChildren({package.get(), duplicate.get()}, project)
            .isEmpty());
    EXPECT_TRUE(package->isDeletable());
    const auto packageId = package->uuid();
    const auto beforeAdd = project->toMemento();
    ASSERT_TRUE(
        gtDataModel->appendChild(std::move(package), project).isValid());
    GtObjectMementoDiff add(beforeAdd, project->toMemento());
    EXPECT_TRUE(project->hasChanges());
    EXPECT_EQ(project->moduleIds(), QStringList{moduleId});
    EXPECT_FALSE(QFile::exists(file));
    EXPECT_FALSE(project->createPackage(moduleId, error));
    EXPECT_FALSE(project->canAppendChild(duplicate.get()));
    ASSERT_TRUE(project->revertDiff(add));
    EXPECT_TRUE(project->moduleIds().isEmpty());
    ASSERT_TRUE(project->applyDiff(add));
    ASSERT_NE(project->findPackage(moduleId), nullptr);
    EXPECT_EQ(project->findPackage(moduleId)->uuid(), packageId);

    auto child = std::make_unique<GtObjectGroup>();
    child->setObjectName(QStringLiteral("Saved Data"));
    const auto childId = child->uuid();
    ASSERT_TRUE(project->findPackage(moduleId)->appendChild(std::move(child)));
    ASSERT_TRUE(gtDataModel->saveProject(project));
    project->createBackup(QStringLiteral("Before package deletion"));
    const QDir backups(gt::project::backupDirPath(*project));
    const auto originalBackups =
        backups.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    ASSERT_EQ(originalBackups.size(), 1);
    project->findPackage(moduleId)
        ->findDirectChildren<GtObject*>()
        .first()
        ->setObjectName(QStringLiteral("Unsaved Data"));

    auto* current = project->findPackage(moduleId);
    const auto edited = project->toMemento();
    {
        GtProjectExecutionGuard guard;
        ASSERT_EQ(guard.tryAcquire(project),
                  GtProjectExecutionGuard::Result::Acquired);
        EXPECT_FALSE(gtDataModel->deleteFromModel(current));
    }
    ASSERT_TRUE(gtDataModel->deleteFromModel(current));
    GtObjectMementoDiff removal(edited, project->toMemento());
    EXPECT_TRUE(project->moduleIds().isEmpty());
    EXPECT_TRUE(QFile::exists(file));
    ASSERT_TRUE(gtDataModel->saveProject(project));
    EXPECT_TRUE(TestProject(project->path()).moduleIds().isEmpty());
    ASSERT_TRUE(project->revertDiff(removal));
    ASSERT_NE(project->getObjectByUuid(childId), nullptr);
    EXPECT_EQ(project->getObjectByUuid(childId)->objectName(),
              QStringLiteral("Unsaved Data"));
    ASSERT_TRUE(gtDataModel->saveProject(project));
    ASSERT_TRUE(project->applyDiff(removal));
    EXPECT_EQ(project->getObjectByUuid(childId), nullptr);
    ASSERT_TRUE(project->revertDiff(removal));
    ASSERT_TRUE(gtDataModel->closeProject(project));
    ASSERT_TRUE(gtDataModel->openProject(project));
    ASSERT_NE(project->findPackage(moduleId), nullptr);
    EXPECT_EQ(project->findPackage(moduleId)->uuid(), packageId);
    ASSERT_NE(project->getObjectByUuid(childId), nullptr);
    EXPECT_EQ(project->getObjectByUuid(childId)->objectName(),
              QStringLiteral("Unsaved Data"));
    ASSERT_TRUE(gtDataModel->closeProject(project));
    ASSERT_EQ(project->restoreBackupFiles(originalBackups.first()),
              GtProject::RestoreStatus::Success);
    ASSERT_TRUE(gtDataModel->openProject(project));
    ASSERT_NE(project->getObjectByUuid(childId), nullptr);
    EXPECT_EQ(project->getObjectByUuid(childId)->objectName(),
              QStringLiteral("Saved Data"));
}

TEST_F(ProjectPackagesTest, saveNewPackageReplacesRetainedFile)
{
    auto* project = createProject(QStringLiteral("StalePackage"), {moduleId});
    ASSERT_NE(project, nullptr);
    auto* oldPackage = project->findPackage(moduleId);
    ASSERT_NE(oldPackage, nullptr);
    auto child = std::make_unique<GtObjectGroup>();
    ASSERT_TRUE(oldPackage->appendChild(std::move(child)));
    ASSERT_TRUE(gtDataModel->saveProject(project));
    const auto oldId = oldPackage->uuid();
    QFile retainedFile(project->moduleDataPath(moduleId));
    ASSERT_TRUE(retainedFile.open(QIODevice::ReadOnly));
    const auto oldData = retainedFile.readAll();
    retainedFile.close();
    ASSERT_TRUE(gtDataModel->deleteFromModel(oldPackage));
    ASSERT_TRUE(gtDataModel->saveProject(project));
    ASSERT_TRUE(gtDataModel->closeProject(project));
    ASSERT_TRUE(gtDataModel->openProject(project));
    EXPECT_TRUE(project->moduleIds().isEmpty());

    QString error;
    auto package = project->createPackage(moduleId, error);
    ASSERT_TRUE(package) << qPrintable(error);
    const auto newId = package->uuid();
    EXPECT_NE(newId, oldId);
    EXPECT_TRUE(package->findChildren<GtObject*>().isEmpty());
    ASSERT_TRUE(
        gtDataModel->appendChild(std::move(package), project).isValid());
    ASSERT_TRUE(retainedFile.open(QIODevice::ReadOnly));
    EXPECT_EQ(retainedFile.readAll(), oldData);
    retainedFile.close();
    ASSERT_TRUE(gtDataModel->saveProject(project));
    ASSERT_TRUE(gtDataModel->closeProject(project));
    ASSERT_TRUE(gtDataModel->openProject(project));
    const auto* reopened = project->findPackage(moduleId);
    ASSERT_NE(reopened, nullptr);
    EXPECT_EQ(reopened->uuid(), newId);
    EXPECT_TRUE(reopened->findChildren<GtObject*>().isEmpty());
}

TEST_F(ProjectPackagesTest, restoreRetainedPackageWithLinkedObjects)
{
    auto* project =
        createProject(QStringLiteral("RetainedPackage"), {moduleId});
    ASSERT_NE(project, nullptr);
    auto* package = project->findPackage(moduleId);
    ASSERT_NE(package, nullptr);
    const auto packageId = package->uuid();
    auto child = std::make_unique<GtObjectGroup>();
    child->setObjectName(QStringLiteral("Linked Data"));
    child->setSaveAsOwnFile(true);
    const auto childId = child->uuid();
    ASSERT_TRUE(package->appendChild(std::move(child)));
    ASSERT_TRUE(gtDataModel->saveProject(project));
    ASSERT_TRUE(gtDataModel->deleteFromModel(package));
    ASSERT_TRUE(gtDataModel->saveProject(project));
    ASSERT_TRUE(gtDataModel->closeProject(project));
    ASSERT_TRUE(gtDataModel->openProject(project));

    auto liveObject = std::make_unique<GtObjectGroup>();
    liveObject->setUuid(childId);
    ASSERT_TRUE(project->appendChild(liveObject.get()));
    QString error;
    ASSERT_EQ(project->restorePackage(moduleId, error),
              GtProject::PackageRestoreStatus::Success)
        << qPrintable(error);
    const auto* restored = project->findPackage(moduleId);
    ASSERT_NE(restored, nullptr);
    EXPECT_EQ(restored->uuid(), packageId);
    ASSERT_NE(restored->getObjectByUuid(childId), nullptr);
    EXPECT_EQ(restored->getObjectByUuid(childId)->objectName(),
              QStringLiteral("Linked Data"));
    EXPECT_EQ(liveObject->uuid(), childId);
    liveObject.reset();
    ASSERT_TRUE(gtDataModel->saveProject(project));
    EXPECT_EQ(TestProject(project->path()).moduleIds(), QStringList{moduleId});
}

TEST_F(ProjectPackagesTest, restoresDuplicateUuidsLikeProjectOpen)
{
    auto* project = createProject(QStringLiteral("DuplicateUuids"), {moduleId});
    ASSERT_NE(project, nullptr);
    ASSERT_TRUE(gtDataModel->closeProject(project));
    const QByteArray id = "{2c5231fd-cb20-44b6-8da3-f87f550d9492}";
    ASSERT_TRUE(writeFile(
        project->moduleDataPath(moduleId),
        "<GTLABMODULE uuid=\"{a611ef6b-227d-4a03-bbe9-7392dc6080df}\" "
        "version=\"1.2.3\">"
        "<object class=\"GtObjectGroup\" name=\"First\" uuid=\"" +
            id + "\"/><object class=\"GtObjectGroup\" name=\"Second\" uuid=\"" +
            id + "\"/></GTLABMODULE>"));

    ASSERT_TRUE(gtDataModel->openProject(project));
    auto* package = project->findPackage(moduleId);
    ASSERT_NE(package, nullptr);
    ASSERT_EQ(package->childCount<GtObjectGroup*>(), 2);
    ASSERT_TRUE(gtDataModel->deleteFromModel(package));

    QString error;
    ASSERT_EQ(project->restorePackage(moduleId, error),
              GtProject::PackageRestoreStatus::Success)
        << qPrintable(error);
    package = project->findPackage(moduleId);
    ASSERT_NE(package, nullptr);
    const auto objects = package->findDirectChildren<GtObjectGroup*>();
    ASSERT_EQ(objects.size(), 2);
    EXPECT_EQ(objects.at(0)->uuid(), QString::fromUtf8(id));
    EXPECT_EQ(objects.at(1)->uuid(), QString::fromUtf8(id));
}

TEST_F(ProjectPackagesTest, failedBackupValidationRestoresOriginalFiles)
{
    auto* project = createProject(QStringLiteral("Original"), {moduleId});
    ASSERT_NE(project, nullptr);
    ASSERT_TRUE(gtDataModel->closeProject(project));

    const QDir dir(project->path());
    const auto manifest = dir.filePath(GtProject::mainFilename());
    QFile originalManifest(manifest);
    ASSERT_TRUE(originalManifest.open(QIODevice::ReadOnly));
    const auto originalData = originalManifest.readAll();
    originalManifest.close();
    ASSERT_TRUE(
        writeFile(dir.filePath("linked.gtobj.xml"), "original linked data"));
    ASSERT_TRUE(
        writeFile(dir.filePath("backup/invalid/project.gtlab"), "<GTLAB/>"));
    ASSERT_TRUE(writeFile(dir.filePath("backup/invalid/linked.gtobj.xml"),
                          "replacement linked data"));
    ASSERT_TRUE(writeFile(dir.filePath("backup/invalid/extra/new.gtobj.xml"),
                          "backup-only data"));
    const bool upgradesAvailable = project->upgradesAvailable();

    EXPECT_EQ(project->restoreBackupFiles(QStringLiteral("invalid")),
              GtProject::RestoreStatus::ErrorCopyFailed);
    ASSERT_TRUE(originalManifest.open(QIODevice::ReadOnly));
    EXPECT_EQ(originalManifest.readAll(), originalData);
    QFile linked(dir.filePath("linked.gtobj.xml"));
    ASSERT_TRUE(linked.open(QIODevice::ReadOnly));
    EXPECT_EQ(linked.readAll(), QByteArrayLiteral("original linked data"));
    EXPECT_FALSE(dir.exists("extra/new.gtobj.xml"));
    EXPECT_TRUE(project->isValid());
    EXPECT_EQ(project->objectName(), QStringLiteral("Original"));
    EXPECT_EQ(project->moduleIds(), QStringList{moduleId});
    EXPECT_EQ(project->upgradesAvailable(), upgradesAvailable);
    ASSERT_TRUE(gtDataModel->openProject(project));
    EXPECT_NE(project->findPackage(moduleId), nullptr);
}

TEST_F(ProjectPackagesTest, ignoresUnsafeModulePathsWhenUpgrading)
{
    const QDir parent = gtTestHelper->newTempDir();
    ASSERT_TRUE(parent.mkpath("project"));
    const QDir dir(parent.filePath("project"));
    ASSERT_TRUE(writeFile(dir.filePath(GtProject::mainFilename()),
                          R"(<GTLAB projectname="Upgrade" version="2.0.0">
            <env-footprint><core-ver>2.0.0</core-ver><modules>
              <module><id>ProjectPackages</id><ver>1.0.0</ver></module>
            </modules></env-footprint>
            <MODULES><MODULE name="../outside"/><MODULE name="ProjectPackages"/>
                     <MODULE name="Unavailable Module"/><MODULE name="project"/>
                     <MODULE name="linked"/></MODULES>
           </GTLAB>)"));
    const auto outside = parent.filePath("outside.gtlab");
    ASSERT_TRUE(writeFile(outside, "untouched"));
    ASSERT_TRUE(
        writeFile(dir.filePath("unavailable module.gtlab"), "<GTLABMODULE/>"));
#ifndef Q_OS_WIN
    ASSERT_TRUE(QFile::link(outside, dir.filePath("linked.gtmod")));
#endif
    TestProject project(dir.path());
    ASSERT_TRUE(project.isValid());
    EXPECT_TRUE(project.moduleDataPath(QStringLiteral("../outside")).isEmpty());
    EXPECT_TRUE(
        project.moduleDataPath(QStringLiteral("..\\outside")).isEmpty());
    EXPECT_FALSE(project.moduleIds().contains(QStringLiteral("../outside")));
#ifndef Q_OS_WIN
    EXPECT_TRUE(project.moduleDataPath(QStringLiteral("linked")).isEmpty());
#endif
    EXPECT_FALSE(project.availableModuleUpgrades().isEmpty());
    ASSERT_TRUE(project.upgradeProject({}));
    QFile outsideFile(outside);
    ASSERT_TRUE(outsideFile.open(QIODevice::ReadOnly));
    EXPECT_EQ(outsideFile.readAll(), QByteArrayLiteral("untouched"));
    EXPECT_FALSE(parent.exists("outside.gtmod"));
    EXPECT_TRUE(dir.exists(GtProject::mainFilename()));
    EXPECT_TRUE(dir.exists("unavailable module.gtmod"));
}

TEST_F(ProjectPackagesTest, caseVariantDeclarationsShareSlotAndKeepMetadata)
{
    auto* initial = createProject(QStringLiteral("MixedCase"), {moduleId});
    ASSERT_NE(initial, nullptr);
    ASSERT_TRUE(gtDataModel->closeProject(initial));
    const QDir dir(initial->path());
    const auto manifest = dir.filePath(GtProject::mainFilename());
    ASSERT_TRUE(writeFile(manifest,
                          R"(<GTLAB projectname="MixedCase" version="2.0.0">
            <env-footprint><core-ver>2.0.0</core-ver><modules>
              <module><id>ProjectPackages</id><ver>1.2.3</ver></module>
            </modules></env-footprint>
            <MODULES>
              <MODULE name="projectpackages" legacy="keep"/>
              <MODULE name="ProjectPackages" version="1.2.3"/>
              <MODULE name="Ghost" version="1.0.0"/>
              <MODULE name="ghost"><extension/></MODULE>
            </MODULES></GTLAB>)"));

    ASSERT_TRUE(gtDataModel->deleteProject(initial));
    GtProjectProvider provider(manifest);
    auto* project = provider.project();
    ASSERT_NE(project, nullptr);
    ASSERT_TRUE(gtDataModel->newProject(project, true));
    EXPECT_EQ(project->moduleIds(),
              (QStringList{QStringLiteral("Ghost"), moduleId}));
    EXPECT_NE(project->findPackage(moduleId), nullptr);
    ASSERT_TRUE(gtDataModel->saveProject(project));

    QDomDocument saved;
    QFile savedManifest(manifest);
    ASSERT_TRUE(gt::xml::readDomDocumentFromFile(savedManifest, saved, true));
    const auto declarations =
        saved.documentElement().firstChildElement("MODULES");
    QStringList names;
    for (auto entry = declarations.firstChildElement("MODULE"); !entry.isNull();
         entry = entry.nextSiblingElement("MODULE"))
    {
        const auto name = entry.attribute("name");
        names.append(name);
        if (name == QStringLiteral("projectpackages"))
            EXPECT_EQ(entry.attribute("legacy"), QStringLiteral("keep"));
        if (name == QStringLiteral("Ghost"))
            EXPECT_EQ(entry.attribute("version"), QStringLiteral("1.0.0"));
        if (name == QStringLiteral("ghost"))
            EXPECT_FALSE(entry.firstChildElement("extension").isNull());
    }
    EXPECT_EQ(names,
              (QStringList{QStringLiteral("Ghost"), QStringLiteral("ghost"),
                           moduleId, QStringLiteral("projectpackages")}));
}

TEST_F(ProjectPackagesTest, restoresProviderMetadataOnlyOnSuccess)
{
    auto* project = createProject(QStringLiteral("RetainedClassProvider"));
    ASSERT_NE(project, nullptr);
    const auto filename = project->moduleDataPath(moduleId);
    const QByteArray package =
        "<GTLABMODULE uuid=\"{a611ef6b-227d-4a03-bbe9-7392dc6080df}\" "
        "version=\"1.2.3\">"
        "<METADATA><CLASS-PROVIDERS><MODULE name=\"UnavailableProvider\">"
        "<CLASS name=\"UnavailablePackageData\"/>"
        "</MODULE></CLASS-PROVIDERS></METADATA>"
        "<object class=\"UnavailablePackageData\" name=\"Saved Data\""
        " uuid=\"{2c5231fd-cb20-44b6-8da3-f87f550d9492}\"/>"
        "</GTLABMODULE>";
    const auto className = QStringLiteral("UnavailablePackageData");
    const auto provider = QStringLiteral("UnavailableProvider");
    const auto childId =
        QStringLiteral("{2c5231fd-cb20-44b6-8da3-f87f550d9492}");

    auto mismatchedPackage = package;
    mismatchedPackage.replace("<GTLABMODULE ",
                              "<GTLABMODULE module=\"Other\" ");
    ASSERT_TRUE(writeFile(filename, mismatchedPackage));
    QString error;
    EXPECT_EQ(project->restorePackage(moduleId, error),
              GtProject::PackageRestoreStatus::Failed);
    EXPECT_FALSE(error.isEmpty());
    EXPECT_TRUE(project->classModuleId(className).isEmpty());

    ASSERT_TRUE(writeFile(filename, package));
    ASSERT_EQ(project->restorePackage(moduleId, error),
              GtProject::PackageRestoreStatus::Success)
        << qPrintable(error);
    const auto* restored = project->getObjectByUuid(childId);
    ASSERT_NE(restored, nullptr);
    EXPECT_TRUE(restored->isDummy());
    EXPECT_EQ(project->classModuleId(className), provider);
    ASSERT_TRUE(gtDataModel->saveProject(project));
    EXPECT_EQ(TestProject(project->path()).classModuleId(className), provider);
}

TEST_F(ProjectPackagesTest, checksRetainedPackageVersion)
{
    auto* project = createProject(QStringLiteral("PackageVersions"));
    ASSERT_NE(project, nullptr);
    const QString file = project->moduleDataPath(moduleId);
    const QByteArray root =
        "<GTLABMODULE uuid=\"{d17e8426-532f-43c1-ba02-80a3f548259a}\"";
    QString error;
    ASSERT_TRUE(writeFile(file, root + " version=\"1.0.0\"/>"));
    using Status = GtProject::PackageRestoreStatus;
    EXPECT_EQ(project->restorePackage(moduleId, error), Status::Failed);
    EXPECT_TRUE(error.contains(QStringLiteral("upgrade")));
    ASSERT_TRUE(writeFile(file, root + " version=\"9.0.0\"/>"));
    EXPECT_EQ(project->restorePackage(moduleId, error), Status::Failed);
    EXPECT_TRUE(error.contains(QStringLiteral("newer")));
    ASSERT_TRUE(writeFile(file, root + " version=\"invalid\"/>"));
    EXPECT_EQ(project->restorePackage(moduleId, error), Status::Failed);
    EXPECT_TRUE(error.contains(QStringLiteral("invalid module version")));
    ASSERT_TRUE(writeFile(
        file, root + "><objectref href=\"missing.gtobj.xml\"/></GTLABMODULE>"));
    EXPECT_EQ(project->restorePackage(moduleId, error),
              Status::NeedsVersionConfirmation);
    EXPECT_TRUE(error.isEmpty());
    EXPECT_EQ(project->restorePackage(moduleId, error, true), Status::Failed);
    EXPECT_FALSE(error.isEmpty());

    ASSERT_TRUE(writeFile(file, root + "/>"));
    EXPECT_EQ(project->restorePackage(moduleId, error, true), Status::Success)
        << qPrintable(error);
}

TEST_F(ProjectPackagesTest, retainsUnreadableDeclaration)
{
    auto* project = createProject(QStringLiteral("BrokenPackage"), {moduleId});
    ASSERT_NE(project, nullptr);
    ASSERT_TRUE(gtDataModel->closeProject(project));
    const auto brokenFile = project->moduleDataPath(moduleId);
    ASSERT_TRUE(writeFile(brokenFile, "<GTLABMODULE"));
    ASSERT_TRUE(gtDataModel->openProject(project));
    EXPECT_FALSE(project->findPackage(moduleId));
    QString error;
    EXPECT_EQ(project->restorePackage(moduleId, error),
              GtProject::PackageRestoreStatus::Failed);
    EXPECT_FALSE(project->createPackage(moduleId, error));
    ASSERT_TRUE(gtDataModel->saveProject(project));
    EXPECT_EQ(TestProject(project->path()).moduleIds(), QStringList{moduleId});
    QFile unreadable(brokenFile);
    ASSERT_TRUE(unreadable.open(QIODevice::ReadOnly));
    EXPECT_EQ(unreadable.readAll(), QByteArray("<GTLABMODULE"));
}

TEST_F(ProjectPackagesTest, failedPackageSaveKeepsOldSelectionAndDirtyState)
{
    auto* project = createProject(QStringLiteral("FailingPackageSave"));
    ASSERT_NE(project, nullptr);
    QString error;
    {
        GtProjectExecutionGuard guard;
        ASSERT_EQ(guard.tryAcquire(project),
                  GtProjectExecutionGuard::Result::Acquired);
        EXPECT_FALSE(project->createPackage(moduleId, error));
    }
    auto package = project->createPackage(moduleId, error);
    ASSERT_TRUE(package);
    package->setObjectName(QStringLiteral("Fail Save"));
    ASSERT_TRUE(
        gtDataModel->appendChild(std::move(package), project).isValid());
    EXPECT_FALSE(gtDataModel->saveProject(project));
    EXPECT_TRUE(project->hasChanges());
    EXPECT_EQ(project->moduleIds(), QStringList{moduleId});
    EXPECT_TRUE(TestProject(project->path()).moduleIds().isEmpty());
}

#ifdef PACKAGE_HDF5_MODULE_PATH
TEST_F(ProjectFootprintTest, restoresExternalizedPackageAcrossSaves)
{
    ASSERT_TRUE(
        m_app->loadSingleModule(QString::fromUtf8(PACKAGE_HDF5_MODULE_PATH)));
    const QString moduleId = QStringLiteral("Test Datamodel Interface");
    auto* project =
        createProject(QStringLiteral("ExternalizedPackage"), {moduleId});
    ASSERT_NE(project, nullptr);
    auto* package = project->findPackage(moduleId);
    ASSERT_NE(package, nullptr);
    auto* object = package->findChild<GtExternalizedObject*>();
    ASSERT_NE(object, nullptr);
    const auto packageId = package->uuid();
    const auto objectId = object->uuid();
    const QVector<double> saved{15, 16, 17};
    const QVector<double> edited{26, 27, 28};

    const bool wasEnabled =
        gtExternalizationManager->isExternalizationEnabled();
    const auto restoreSetting = qScopeGuard([wasEnabled] {
        gtExternalizationManager->enableExternalization(wasEnabled);
    });
    gtExternalizationManager->enableExternalization(true);

    ASSERT_TRUE(object->setProperty("values", QVariant::fromValue(saved)));
    ASSERT_TRUE(gtDataModel->saveProject(project));
    ASSERT_FALSE(object->isFetched());
    ASSERT_TRUE(object->internalize());
    ASSERT_TRUE(object->setProperty("values", QVariant::fromValue(edited)));
    const auto beforeDeletion = project->toMemento();
    ASSERT_TRUE(gtDataModel->deleteFromModel(package));
    GtObjectMementoDiff deletion(beforeDeletion, project->toMemento());
    ASSERT_TRUE(gtDataModel->saveProject(project));
    ASSERT_TRUE(project->revertDiff(deletion));

    package = project->findPackage(moduleId);
    ASSERT_NE(package, nullptr);
    EXPECT_EQ(package->uuid(), packageId);
    object = package->findChild<GtExternalizedObject*>();
    ASSERT_NE(object, nullptr);
    EXPECT_EQ(object->uuid(), objectId);
    EXPECT_EQ(object->property("values").value<QVector<double>>(), edited);
    ASSERT_TRUE(gtDataModel->saveProject(project));
    ASSERT_TRUE(gtDataModel->closeProject(project));
    ASSERT_TRUE(gtDataModel->openProject(project));
    package = project->findPackage(moduleId);
    ASSERT_NE(package, nullptr);
    object = package->findChild<GtExternalizedObject*>();
    ASSERT_NE(object, nullptr);
    ASSERT_TRUE(object->internalize());
    EXPECT_EQ(object->property("values").value<QVector<double>>(), edited);

    ASSERT_TRUE(gtDataModel->deleteFromModel(package));
    ASSERT_TRUE(gtDataModel->saveProject(project));
    ASSERT_TRUE(gtDataModel->closeProject(project));
    ASSERT_TRUE(gtDataModel->openProject(project));

    QString error;
    ASSERT_EQ(project->restorePackage(moduleId, error),
              GtProject::PackageRestoreStatus::Success)
        << qPrintable(error);
    object =
        qobject_cast<GtExternalizedObject*>(project->getObjectByUuid(objectId));
    ASSERT_NE(object, nullptr);
    ASSERT_TRUE(object->internalize());
    EXPECT_EQ(object->property("values").value<QVector<double>>(), edited);
    ASSERT_TRUE(gtDataModel->closeProject(project));
}
#endif

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

TEST_F(ProjectFootprintTest, storedFootprintSurvivesProjectReload)
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

    // Load a new project instance from the saved project file. This checks
    // that the footprint and class-provider metadata were persisted; it does
    // not reconstruct and resave the live object tree.
    TestProject reloaded(project->path());
    ASSERT_TRUE(reloaded.isValid());

    // the stored footprint of the project still contains the required module
    // and its transitive dependency, with no unrelated module
    EXPECT_EQ(footprintModuleIds(&reloaded), expected);

    // the persistent class provider metadata still attributes the used class to
    // its providing module, i.e. the dependency can be resolved again after the
    // project reload.
    EXPECT_EQ(reloaded.classModuleId(QStringLiteral("GtObject")),
              QStringLiteral("ModuleA"));
}

TEST_F(ProjectFootprintTest, multipleUsedClassesFromOneModuleCreateSingleEntry)
{
    GtProject* project = createProject(QStringLiteral("FootprintSameModule"));
    ASSERT_NE(project, nullptr);

    ASSERT_TRUE(m_secondModuleFactory.registerClass(
        GT_METADATA(FootprintUnusedClass), QStringLiteral("ModuleA")));
    ASSERT_TRUE(
        m_app->loadSingleModule(QString::fromUtf8(FOOTPRINT_ModuleA_PATH)));

    auto* firstObject = new GtObject;
    firstObject->setFactory(&m_moduleFactory);
    ASSERT_TRUE(project->appendChild(firstObject));

    auto* secondObject = new FootprintUnusedClass;
    secondObject->setFactory(&m_secondModuleFactory);
    ASSERT_TRUE(project->appendChild(secondObject));

    ASSERT_TRUE(gtDataModel->saveProject(project));

    EXPECT_EQ(footprintModuleIds(project),
              QStringList{QStringLiteral("ModuleA")});
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

    // The declaration is still unresolved when its package file is absent.
    // A later recovery may restore the file, so retain the saved footprint.
    ASSERT_TRUE(QFile::remove(project->path() + QDir::separator() +
                              QStringLiteral("ghost.") +
                              GtProject::moduleExtension()));

    ASSERT_TRUE(gtDataModel->saveProject(project));

    modules = GtFootprint{project->readFootprint()}.modules();
    EXPECT_EQ(modules.value(QStringLiteral("Ghost")).toString(),
              QStringLiteral("3.2.1"));
    EXPECT_EQ(TestProject(project->path()).moduleIds(),
              QStringList{QStringLiteral("Ghost")});
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

    auto dummy = createDummy(QStringLiteral("UnknownModuleClass"));
    ASSERT_NE(dummy, nullptr);

    ASSERT_TRUE(project->appendChild(dummy.get()));
    dummy.release();

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
    auto dummy = createDummy(QStringLiteral("LegacyUnknownClass"));
    ASSERT_NE(dummy, nullptr);
    auto* dummyObject = dummy.get();
    ASSERT_TRUE(project->appendChild(dummyObject));
    dummy.release();

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
    delete dummyObject;

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
