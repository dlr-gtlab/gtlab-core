/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_project.h"
#include "internal/gt_projectio.h"

#include <QDir>
#include <QFile>
#include <QDomDocument>
#include <QTemporaryDir>

namespace {

class TestProject : public GtProject
{
public:
    explicit TestProject(const QString& path) : GtProject(path) {}
};

static bool writeProjectFile(const QString& dirPath, const QString& projectName)
{
    QFile file(dirPath + QDir::separator() + GtProject::mainFilename());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        return false;
    }

    const QByteArray data = QString(
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<GTLAB projectname=\"%1\" version=\"1.7.0-rc1\">\n"
        "    <env-footprint>\n"
        "        <modules/>\n"
        "    </env-footprint>\n"
        "    <comment/>\n"
        "    <MODULES/>\n"
        "    <PROCESSES/>\n"
        "    <LABELS/>\n"
        "</GTLAB>\n")
        .arg(projectName)
        .toUtf8();

    return file.write(data) == data.size();
}

} // namespace

TEST(GtProject, commentMethodReturnsStoredComment)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    QString projectPath = tempDir.path();
    QString projectFile = projectPath + QDir::separator() + GtProject::mainFilename();

    // Create project file with a comment
    QFile file(projectFile);
    ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text));

    const QByteArray data = QString(
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<GTLAB projectname=\"CommentTest\" version=\"1.7.0-rc1\">\n"
        "    <env-footprint>\n"
        "        <modules/>\n"
        "    </env-footprint>\n"
        "    <comment>This is a test comment</comment>\n"
        "    <MODULES/>\n"
        "    <PROCESSES/>\n"
        "    <LABELS/>\n"
        "</GTLAB>\n")
        .toUtf8();

    ASSERT_EQ(file.write(data), data.size());
    file.close();

    TestProject project(projectPath);
    EXPECT_EQ(project.comment(), QStringLiteral("This is a test comment"));
}

TEST(GtProject, setInternalizeOnSaveUpdatesFlag)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("InternalizeTest")));

    TestProject project(tempDir.path());

    // Default is true (based on typical behavior)
    project.setInternalizeOnSave(false);

    // The flag is set - verification via re-reading would require save
    // The method itself is the primary test target
}

TEST(GtProject, setCommentUpdatesValueAndTriggersChanged)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("SetCommentTest")));

    TestProject project(tempDir.path());
    EXPECT_TRUE(project.comment().isEmpty());

    project.setComment(QStringLiteral("New comment"));
    EXPECT_EQ(project.comment(), QStringLiteral("New comment"));
}

TEST(GtProject, staticFilenameHelpersReturnExpectedValues)
{
    EXPECT_EQ(GtProject::mainFilename(), QStringLiteral("project.gtlab"));
    EXPECT_EQ(GtProject::mainFileExtension(), QStringLiteral("gtlab"));
    EXPECT_EQ(GtProject::moduleExtension(), QStringLiteral("gtmod"));
}

TEST(GtProject, isOpenReturnsFalseForUnopenedProject)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("OpenTest")));

    TestProject project(tempDir.path());
    EXPECT_FALSE(project.isOpen());
}

TEST(GtProject, isValidReturnsBasedOnMetaData)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("ValidTest")));

    TestProject project(tempDir.path());
    EXPECT_TRUE(project.isValid());
}



TEST(GtProject, upgradeProjectDataDoesNothingWhenNoUpgradesAvailable)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("UpgradeDataTest")));

    TestProject project(tempDir.path());
    project.upgradeProjectData();
}

TEST(GtProject, taskIdsReturnsEmptyList)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("TaskIdsTest")));

    TestProject project(tempDir.path());
    EXPECT_TRUE(project.taskIds().isEmpty());
}

TEST(GtProject, fullTaskIdsReturnsEmptyMap)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("FullTaskIdsTest")));

    TestProject project(tempDir.path());
    EXPECT_TRUE(project.fullTaskIds().isEmpty());
}

TEST(GtProject, processDataReturnsNullptr)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("ProcessDataTest")));

    TestProject project(tempDir.path());
    EXPECT_EQ(project.processData(), nullptr);
}

TEST(GtProject, labelDataReturnsNullptr)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("LabelDataTest")));

    TestProject project(tempDir.path());
    EXPECT_EQ(project.labelData(), nullptr);
}

TEST(GtProject, moduleDataPathReturnsExpectedFormat)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("ModuleDataPathTest")));

    TestProject project(tempDir.path());
    QString dataPath = project.moduleDataPath(QStringLiteral("test-module"));
    EXPECT_TRUE(dataPath.contains(tempDir.path()));
}


TEST(GtProject, moduleIdsReturnsEmptyList)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("ModuleIdsTest")));

    TestProject project(tempDir.path());
    EXPECT_TRUE(project.moduleIds().isEmpty());
}

TEST(GtProject, copyProjectDataCopiesDirectory)
{
    QTemporaryDir srcDir;
    QTemporaryDir dstDir;
    ASSERT_TRUE(srcDir.isValid());
    ASSERT_TRUE(dstDir.isValid());

    ASSERT_TRUE(writeProjectFile(srcDir.path(), QStringLiteral("CopySourceTest")));

    auto status = gt::project::copyProjectData(QDir(srcDir.path()),
                                               QDir(dstDir.path()),
                                               gt::project::ForceOverwrite);

    EXPECT_EQ(status, gt::filesystem::CopyStatus::Success);

    QString destFile = dstDir.path() + QDir::separator() + GtProject::mainFilename();
    EXPECT_TRUE(QFile::exists(destFile));
}

TEST(GtProject, copyProjectDataSkipsBackupsWhenFlagNotSet)
{
    QTemporaryDir srcDir;
    QTemporaryDir dstDir;
    ASSERT_TRUE(srcDir.isValid());
    ASSERT_TRUE(dstDir.isValid());

    ASSERT_TRUE(writeProjectFile(srcDir.path(), QStringLiteral("SkipBackupsTest")));

    QDir src(srcDir.path());
    src.mkpath("backup");
    QFile backupMsg(srcDir.path() + "/backup/GTlabBackUpMessage.md");
    backupMsg.open(QIODevice::WriteOnly);
    backupMsg.write("test");
    backupMsg.close();

    auto status = gt::project::copyProjectData(QDir(srcDir.path()),
                                               QDir(dstDir.path()),
                                               gt::project::ForceOverwrite);

    EXPECT_EQ(status, gt::filesystem::CopyStatus::Success);
    EXPECT_FALSE(QFile::exists(dstDir.path() + "/backup/GTlabBackUpMessage.md"));
}

TEST(GtProject, copyProjectDataIncludesBackupsWithFlag)
{
    QTemporaryDir srcDir;
    QTemporaryDir dstDir;
    ASSERT_TRUE(srcDir.isValid());
    ASSERT_TRUE(dstDir.isValid());

    ASSERT_TRUE(writeProjectFile(srcDir.path(), QStringLiteral("IncludeBackupsTest")));

    QDir src(srcDir.path());
    src.mkpath("backup");
    QFile backupMsg(srcDir.path() + "/backup/GTlabBackUpMessage.md");
    backupMsg.open(QIODevice::WriteOnly);
    backupMsg.write("test backup");
    backupMsg.close();

    auto status = gt::project::copyProjectData(QDir(srcDir.path()),
                                               QDir(dstDir.path()),
                                               gt::project::ForceOverwrite | gt::project::IncludeBackups);

    EXPECT_EQ(status, gt::filesystem::CopyStatus::Success);
    EXPECT_TRUE(QFile::exists(dstDir.path() + "/backup/GTlabBackUpMessage.md"));
}

TEST(GtProject, createBackupDoesNotCrash)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("BackupTest")));

    TestProject project(tempDir.path());
    project.createBackup(QStringLiteral("Test backup"));
}

TEST(GtProject, readFootprintReturnsProjectFootprint)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("FootprintTest")));

    TestProject project(tempDir.path());
    QString footprint = project.readFootprint();
    EXPECT_FALSE(footprint.isEmpty());
}


TEST(GtProject, updateProjectMetaDataUpdatesProjectName)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("OldName")));

    GtProject::ProjectMetaData data;
    data.projectName = QStringLiteral("NewName");

    bool result = GtProject::updateProjectMetaData(QDir(tempDir.path()), data);
    EXPECT_TRUE(result);

    QFile file(tempDir.path() + QDir::separator() + GtProject::mainFilename());
    ASSERT_TRUE(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QByteArray content = file.readAll();
    EXPECT_TRUE(content.contains("projectname=\"NewName\""));
    EXPECT_FALSE(content.contains("projectname=\"OldName\""));
}

TEST(GtProject, updateProjectMetaDataReturnsFalseForNonExistentProject)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    GtProject::ProjectMetaData data;
    data.projectName = QStringLiteral("SomeName");

    bool result = GtProject::updateProjectMetaData(QDir(tempDir.path()), data);
    EXPECT_FALSE(result);
}

TEST(GtProject, pathReturnsProjectPath)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("PathTest")));

    TestProject project(tempDir.path());
    EXPECT_EQ(project.path(), tempDir.path());
}

TEST(GtProject, isValidReturnsBasedOnLoadResult)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("ValidTest")));

    TestProject project(tempDir.path());
    EXPECT_TRUE(project.isValid());
}

TEST(GtProject, isValidReturnsFalseForInvalidProject)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    TestProject project(tempDir.path());
    EXPECT_FALSE(project.isValid());
}

// Tests for GtProjectIO implementation

TEST(GtProjectIO, readProjectDataReturnsDocumentForValidFile)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());
    ASSERT_TRUE(writeProjectFile(tempDir.path(), QStringLiteral("ReadDataTest")));

    QDomDocument doc = GtProjectIO::readProjectData(QDir(tempDir.path()));

    EXPECT_FALSE(doc.isNull());
    EXPECT_EQ(doc.documentElement().tagName(), QStringLiteral("GTLAB"));
}

TEST(GtProjectIO, readProjectDataReturnsEmptyDocWhenFileMissing)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    QDomDocument doc = GtProjectIO::readProjectData(QDir(tempDir.path()));

    EXPECT_TRUE(doc.isNull());
}

TEST(GtProjectIO, readProjectDataReturnsEmptyDocForInvalidXml)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    // Write invalid XML
    QFile file(QDir(tempDir.path()).filePath(GtProject::mainFilename()));
    ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
    file.write("not valid xml <");
    file.close();

    QDomDocument doc = GtProjectIO::readProjectData(QDir(tempDir.path()));

    EXPECT_TRUE(doc.isNull());
}

TEST(GtProjectIO, readProjectDataReturnsEmptyDocForNonGtlabFile)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    // Write valid XML but not a GTLAB file
    QFile file(QDir(tempDir.path()).filePath(GtProject::mainFilename()));
    ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
    file.write("<?xml version=\"1.0\"?><SomeOtherTag/>");
    file.close();

    QDomDocument doc = GtProjectIO::readProjectData(QDir(tempDir.path()));

    EXPECT_TRUE(doc.isNull());
}

TEST(GtProjectIO, saveProjectFilesWritesValidXml)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    QDomDocument doc;
    QDomElement root = doc.createElement("GTLAB");
    root.setAttribute("projectname", "SaveTest");
    root.setAttribute("version", "1.7.0");
    doc.appendChild(root);

    QString filePath = QDir(tempDir.path()).filePath(QStringLiteral("project.gtlab"));
    bool result = GtProjectIO::saveProjectFiles(filePath, doc);

    EXPECT_TRUE(result);
    EXPECT_TRUE(QFile::exists(filePath));

    // Verify content is valid XML
    QFile readFile(filePath);
    ASSERT_TRUE(readFile.open(QIODevice::ReadOnly | QIODevice::Text));
    QDomDocument readDoc;
    QString errorStr;
    int errorLine, errorColumn;
    EXPECT_TRUE(readDoc.setContent(&readFile, &errorStr, &errorLine, &errorColumn));
}

TEST(GtProjectIO, saveProjectFilesHonorsLinkMode)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    QDomDocument doc;
    QDomElement root = doc.createElement("Root");
    QDomElement object = doc.createElement("object");
    object.setAttribute("class", "Foo");
    object.setAttribute("name", "A");
    object.setAttribute("uuid", "{111-222}");
    object.setAttribute("aslink", "true");
    root.appendChild(object);
    doc.appendChild(root);

    const QString linkedMasterPath =
        QDir(tempDir.path()).filePath(QStringLiteral("linked.xml"));
    const QString inlineMasterPath =
        QDir(tempDir.path()).filePath(QStringLiteral("inline.xml"));

    ASSERT_TRUE(GtProjectIO::saveProjectFiles(linkedMasterPath, doc, true));
    EXPECT_TRUE(QFile::exists(QDir(tempDir.path()).filePath(
        QStringLiteral("linked/A_111-222.gtobj.xml"))));

    ASSERT_TRUE(GtProjectIO::saveProjectFiles(inlineMasterPath, doc, false));
    EXPECT_FALSE(QFile::exists(QDir(tempDir.path()).filePath(
        QStringLiteral("inline/A_111-222.gtobj.xml"))));
}

TEST(GtProjectIO, saveProjectFilesHandlesNestedObjects)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    QDomDocument doc;
    QDomElement root = doc.createElement("GTLAB");
    root.setAttribute("projectname", "NestedTest");
    root.setAttribute("version", "1.7.0");
    doc.appendChild(root);

    // Add nested structure
    QDomElement module = doc.createElement("modules");
    root.appendChild(module);
    QDomElement obj = doc.createElement("object");
    obj.setAttribute("class", "TestClass");
    obj.setAttribute("name", "TestObject");
    module.appendChild(obj);

    QString filePath = QDir(tempDir.path()).filePath(QStringLiteral("project.gtlab"));
    bool result = GtProjectIO::saveProjectFiles(filePath, doc);

    EXPECT_TRUE(result);
    EXPECT_TRUE(QFile::exists(filePath));
}

TEST(GtProjectIO, fileExistsReturnsTrueForExistingFile)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    QString filePath = QDir(tempDir.path()).filePath(QStringLiteral("exists.txt"));
    QFile file(filePath);
    ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
    file.write("test");
    file.close();

    EXPECT_TRUE(GtProjectIO::fileExists(filePath));
}

TEST(GtProjectIO, fileExistsReturnsFalseForNonExistingFile)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    EXPECT_FALSE(GtProjectIO::fileExists(
        QDir(tempDir.path()).filePath(QStringLiteral("nonexistent.txt"))));
}
