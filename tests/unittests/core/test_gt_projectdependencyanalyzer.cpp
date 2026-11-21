/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_projectdependencyanalyzer.h"
#include "gt_package.h"
#include "gt_objectlinkproperty.h"
#include "gt_objectmementodiff.h"
#include "gt_projectexecutionguard.h"

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
#include "gt_xmlutilities.h"

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
        for (const auto& name : {QStringLiteral("PackageLinkObject"),
                                 QStringLiteral("PackageExternalizedObject"),
                                 QStringLiteral("ProjectPackageFixture"),
                                 QStringLiteral("AuxiliaryPackageFixture")})
        {
            if (auto* type =
                    gtObjectFactory->GtAbstractObjectFactory::metaObject(name))
            {
                const auto metaObject = *type;
                gtObjectFactory->unregisterClass(metaObject);
            }
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
        EXPECT_FALSE(project->packageDeletionError(*current).isEmpty());
        EXPECT_FALSE(gtDataModel->deleteFromModel(current));
    }
    ASSERT_TRUE(project->packageDeletionError(*current).isEmpty());
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

TEST_F(ProjectPackagesTest, addIgnoresStaleFileAndPreservesItOnSave)
{
    auto* project = createProject(QStringLiteral("StalePackage"), {moduleId});
    ASSERT_NE(project, nullptr);
    auto* oldPackage = project->findPackage(moduleId);
    ASSERT_NE(oldPackage, nullptr);
    auto child = std::make_unique<GtObjectGroup>();
    const auto childId = child->uuid();
    ASSERT_TRUE(oldPackage->appendChild(std::move(child)));
    ASSERT_TRUE(gtDataModel->saveProject(project));
    const auto oldId = oldPackage->uuid();
    ASSERT_TRUE(gtDataModel->deleteFromModel(oldPackage));
    ASSERT_TRUE(gtDataModel->saveProject(project));
    ASSERT_TRUE(gtDataModel->closeProject(project));
    ASSERT_TRUE(gtDataModel->openProject(project));
    EXPECT_TRUE(project->moduleIds().isEmpty());

    QString error;
    EXPECT_FALSE(
        project->importPackage(project->moduleDataPath(moduleId), error));
    EXPECT_TRUE(error.contains(QStringLiteral("retained package file")));
    auto package = project->createPackage(moduleId, error);
    ASSERT_TRUE(package) << qPrintable(error);
    EXPECT_NE(package->uuid(), oldId);
    EXPECT_TRUE(package->findChildren<GtObject*>().isEmpty());
    ASSERT_TRUE(
        gtDataModel->appendChild(std::move(package), project).isValid());
    ASSERT_TRUE(gtDataModel->saveProject(project));
    const QDir backups(gt::project::backupDirPath(*project));
    const auto copies =
        backups.entryList({QStringLiteral("packages-*")}, QDir::Dirs);
    ASSERT_EQ(copies.size(), 1);
    const auto oldFile =
        QDir(backups.filePath(copies.first()))
            .filePath(QFileInfo(project->moduleDataPath(moduleId)).fileName());
    const auto original = gt::xml::loadProjectXmlWithLinkedObjects(oldFile);
    EXPECT_EQ(original.documentElement().attribute(QStringLiteral("uuid")),
              oldId);
    EXPECT_EQ(original.elementsByTagName(QStringLiteral("object"))
                  .at(0)
                  .toElement()
                  .attribute(QStringLiteral("uuid")),
              childId);
    ASSERT_TRUE(gtDataModel->closeProject(project));
    ASSERT_TRUE(gtDataModel->openProject(project));
    ASSERT_NE(project->findPackage(moduleId), nullptr);
    EXPECT_TRUE(
        project->findPackage(moduleId)->findChildren<GtObject*>().isEmpty());
}

TEST_F(ProjectPackagesTest, importKeepsLinkedObjectsReferencesAndSourceFiles)
{
    auto* project = createProject(QStringLiteral("ImportPackage"));
    ASSERT_NE(project, nullptr);
    auto destination = std::make_unique<GtObjectGroup>();
    destination->setObjectName(QStringLiteral("Destination"));
    destination->setSaveAsOwnFile(true);
    const auto destinationId = destination->uuid();
    std::unique_ptr<GtObject> linkOwner(
        gtObjectFactory->newObject(QStringLiteral("PackageLinkObject")));
    ASSERT_TRUE(linkOwner);
    const auto linkOwnerId = linkOwner->uuid();
    linkOwner->setObjectName(QStringLiteral("Link Owner"));
    auto* link = qobject_cast<GtObjectLinkProperty*>(
        linkOwner->findProperty(QStringLiteral("link")));
    ASSERT_NE(link, nullptr);
    link->setVal(destinationId);

    QDomDocument document;
    auto root = document.createElement(QStringLiteral("GTLABMODULE"));
    const auto packageId = QUuid::createUuid().toString();
    root.setAttribute(QStringLiteral("uuid"), packageId);
    document.appendChild(root);
    root.appendChild(
        document.importNode(destination->toMemento().documentElement(), true));
    root.appendChild(
        document.importNode(linkOwner->toMemento().documentElement(), true));
    const auto source = gtTestHelper->newTempDir();
    const auto file =
        source.filePath(moduleId.toLower() + QStringLiteral(".gtmod"));
    ASSERT_TRUE(gt::xml::saveProjectXmlWithLinkedObjects(
        QStringLiteral("project"), document, source, file,
        gt::xml::LinkFileSaveType::WithLinkedFiles, nullptr));
    QFile sourceFile(file);
    ASSERT_TRUE(sourceFile.open(QIODevice::ReadOnly));
    const auto sourceBytes = sourceFile.readAll();
    sourceFile.close();
    ASSERT_TRUE(sourceBytes.contains("objectref"));

    QString error;
    const auto missingLinks =
        gtTestHelper->newTempDir().filePath(QFileInfo(file).fileName());
    ASSERT_TRUE(QFile::copy(file, missingLinks));
    EXPECT_FALSE(project->importPackage(missingLinks, error));
    EXPECT_TRUE(error.contains(QStringLiteral("linked objects")));

    const auto sidecar = source.filePath(QStringLiteral(".package-sidecar"));
    ASSERT_TRUE(writeFile(sidecar, "module-owned data"));
    EXPECT_FALSE(project->importPackage(file, error));
    EXPECT_TRUE(error.contains(QStringLiteral("auxiliary")));
    ASSERT_TRUE(QFile::remove(sidecar));

    project->setUuid(destinationId);
    EXPECT_FALSE(project->importPackage(file, error));
    EXPECT_TRUE(project->moduleIds().isEmpty());
    project->newUuid();
    auto package = project->importPackage(file, error);
    ASSERT_TRUE(package) << qPrintable(error);
    EXPECT_EQ(package->uuid(), packageId);
    const auto before = project->toMemento();
    ASSERT_TRUE(
        gtDataModel->appendChild(std::move(package), project).isValid());
    EXPECT_FALSE(project->importPackage(file, error));
    GtObjectMementoDiff import(before, project->toMemento());
    ASSERT_TRUE(project->revertDiff(import));
    EXPECT_TRUE(project->moduleIds().isEmpty());
    ASSERT_TRUE(project->applyDiff(import));
    ASSERT_TRUE(gtDataModel->saveProject(project));
    ASSERT_TRUE(gtDataModel->closeProject(project));
    ASSERT_TRUE(gtDataModel->openProject(project));
    ASSERT_NE(project->findPackage(moduleId), nullptr);
    EXPECT_EQ(project->findPackage(moduleId)->uuid(), packageId);
    auto* object = project->getObjectByUuid(linkOwnerId);
    ASSERT_NE(object, nullptr);
    auto* importedLink = qobject_cast<GtObjectLinkProperty*>(
        object->findProperty(QStringLiteral("link")));
    ASSERT_NE(importedLink, nullptr);
    EXPECT_EQ(importedLink->linkedObjectUUID(), destinationId);
    EXPECT_EQ(importedLink->linkedObject(),
              project->getObjectByUuid(destinationId));
    ASSERT_TRUE(sourceFile.open(QIODevice::ReadOnly));
    EXPECT_EQ(sourceFile.readAll(), sourceBytes);
}

TEST_F(ProjectPackagesTest,
       rejectsUnsupportedImportAndRetainsUnreadableDeclaration)
{
    const QString auxiliaryId = QStringLiteral("AuxiliaryPackages");
    ASSERT_TRUE(m_app->loadSingleModule(
        QString::fromUtf8(FOOTPRINT_AuxiliaryPackages_PATH)));
    auto* project = createProject(QStringLiteral("AuxiliaryPackage"));
    ASSERT_NE(project, nullptr);
    const QDir source = gtTestHelper->newTempDir();
    const auto file =
        source.filePath(auxiliaryId.toLower() + QStringLiteral(".gtmod"));
    ASSERT_TRUE(writeFile(
        file,
        "<GTLABMODULE uuid=\"{d17e8426-532f-43c1-ba02-80a3f548259a}\"/>"));
    QString error;
    EXPECT_FALSE(project->importPackage(file, error));
    EXPECT_TRUE(error.contains(QStringLiteral("auxiliary")));
    EXPECT_TRUE(project->moduleIds().isEmpty());

    const auto externalizedFile =
        source.filePath(moduleId.toLower() + QStringLiteral(".gtmod"));
    ASSERT_TRUE(writeFile(
        externalizedFile,
        "<GTLABMODULE uuid=\"{d17e8426-532f-43c1-ba02-80a3f548259a}\">"
        "<object class=\"PackageExternalizedObject\" name=\"Externalized\""
        " uuid=\"{d17e8426-532f-43c1-ba02-80a3f548259b}\"/>"
        "</GTLABMODULE>"));
    EXPECT_FALSE(project->importPackage(externalizedFile, error));
    EXPECT_TRUE(error.contains(QStringLiteral("externalized")));
    EXPECT_TRUE(project->moduleIds().isEmpty());

    ASSERT_TRUE(gtDataModel->closeProject(project));

    project = createProject(QStringLiteral("BrokenPackage"), {moduleId});
    ASSERT_NE(project, nullptr);
    ASSERT_TRUE(gtDataModel->closeProject(project));
    const auto brokenFile = project->moduleDataPath(moduleId);
    ASSERT_TRUE(writeFile(brokenFile, "<GTLABMODULE"));
    ASSERT_TRUE(gtDataModel->openProject(project));
    EXPECT_FALSE(project->findPackage(moduleId));
    EXPECT_FALSE(project->importPackage(brokenFile, error));
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
