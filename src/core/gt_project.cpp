/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 29.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_project.h"
#include "gt_coredatamodel.h"
#include "gt_abstractloadinghelper.h"
#include "gt_processdata.h"
#include "gt_projectprovider.h"
#include "gt_task.h"
#include "gt_footprint.h"
#include "gt_projectanalyzer.h"
#include "gt_projectdependencyanalyzer.h"
#include "gt_projectexecutionguard.h"
#include "internal/gt_projectio.h"
#include "internal/gt_moduleupgrader.h"

#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_coreapplication.h"
#include "gt_package.h"
#include "gt_labeldata.h"
#include "gt_label.h"
#include "gt_xmlutilities.h"
#include "gt_versionnumber.h"
#include "gt_externalizedobject.h"
#include "gt_externalizationmanager.h"
#include "gt_qtutilities.h"
#include "gt_filesystem.h"
#include "gt_finally.h"

#include "gt_logging.h"

#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QPointer>
#include <QRegularExpression>
#include <QSet>
#include <QTemporaryDir>
#include <QTextStream>
#include <QXmlStreamReader>

#include <algorithm>
#include <cassert>

namespace
{

// Reject empty, relative, or path-like module IDs so "<moduleId>.gtmod"
// always refers to a single file inside the project directory.
bool
isSafeModuleFileName(const QString& moduleId)
{
    const QString trimmed = moduleId.trimmed();
    if (trimmed.isEmpty() || trimmed == QLatin1String(".") ||
        trimmed == QLatin1String(".."))
    {
        return false;
    }

    // Disallow path separators, Windows-invalid filename characters, and
    // ASCII control characters.
    static const QRegularExpression invalidChars(
        QStringLiteral(R"([/\\:<>"|?*\x00-\x1f])"));
    return !moduleId.contains(invalidChars);
}

// Resolve "<moduleId>.<extension>" inside projectDir and reject directories or
// symlinks that escape the project directory.
QString
moduleFilePath(const QDir& projectDir, const QString& moduleId,
               const QString& extension)
{
    if (!isSafeModuleFileName(moduleId)) return {};

    const QFileInfo file(
        projectDir.filePath(moduleId.toLower() + QLatin1Char('.') + extension));
    if (file.isDir()) return {};
    if (file.exists() || file.isSymLink())
    {
        const auto root = projectDir.canonicalPath();
        const auto target = file.canonicalFilePath();
        if (root.isEmpty() || target.isEmpty()) return {};
        const auto relative = QDir(root).relativeFilePath(target);
        if (QDir::isAbsolutePath(relative) ||
            relative == QStringLiteral("..") ||
            relative.startsWith(QStringLiteral("../")))
            return {};
    }
    return file.filePath();
}

// Return the owning module ID for a top-level GtPackage, or empty if the
// package class is not registered as that module's package type.
QString
packageModuleId(const GtPackage& package)
{
    const QString className =
        QString::fromLatin1(package.metaObject()->className());
    const QString id = gtObjectFactory->moduleId(className);
    return gtApp && gtApp->modulePackageId(id) == className ? id : QString{};
}

// Instantiate a fresh, empty GtPackage for `moduleId` via the object factory.
std::unique_ptr<GtPackage>
newPackage(const QString& moduleId)
{
    if (!gtApp) return {};
    return gt::unique_qobject_cast<GtPackage>(std::unique_ptr<GtObject>(
        gtObjectFactory->newObject(gtApp->modulePackageId(moduleId))));
}

// Validate a <GTLABMODULE> root element against `moduleId` and its registered
// package class, then instantiate and populate the package from `projectDir`.
std::unique_ptr<GtPackage>
loadPackage(const QString& moduleId, const QDomElement& root,
            const QDir& projectDir)
{
    if (root.tagName() != QLatin1String("GTLABMODULE") ||
        root.attribute(QStringLiteral("uuid")).isEmpty())
    {
        return {};
    }

    auto package = newPackage(moduleId);
    if (!package) return {};

    const QString recordedModule = root.attribute(QStringLiteral("module"));
    const QString recordedClass = root.attribute(QStringLiteral("class"));
    if ((!recordedModule.isEmpty() && recordedModule != moduleId) ||
        (!recordedClass.isEmpty() &&
         recordedClass != QLatin1String(package->metaObject()->className())))
    {
        return {};
    }

    package->setUuid(root.attribute(QStringLiteral("uuid")));
    if (!package->readData(root) || !package->readMiscData(projectDir))
        return {};
    return package;
}

// Collect class-to-module mappings stored in saved task files (*.gttask) so
// class provider metadata is preserved even for classes only referenced there.
gt::xml::ClassModuleMap
taskClassModuleIds(const QString& projectPath)
{
    gt::xml::ClassModuleMap result;
    QDirIterator files(projectPath + QDir::separator() + QStringLiteral("tasks"),
                       {QStringLiteral("*.gttask")}, QDir::Files,
                       QDirIterator::Subdirectories);
    while (files.hasNext())
    {
        QFile file(files.next());
        QDomDocument document;
        if (!gt::xml::readDomDocumentFromFile(file, document, true)) continue;

        const auto mappings =
            gt::xml::readClassModuleMap(document.documentElement());
        gt::insertMissingInto(result, mappings);
    }
    return result;
}

} // namespace

struct GtProject::Impl
{
    explicit Impl(const QString& projectPath) :
        path(projectPath),
        pathProp(QStringLiteral("path"), GtProject::tr("Path"),
                 GtProject::tr("Project path"), projectPath)
    {
    }

    /// Path to project directory on hard drive
    QString path;

    /// True if project data was successfully loaded
    bool valid{false};

    /// True if project data is currently loaded into the data model
    bool isOpen{false};

    /// True if project data upgrades are available
    bool upgradesAvailable{false};

    /// Whether to internalize all external data when saving
    bool internalizeOnSave{false};

    GtProjectSettings projectSettings;

    /// Last serialized selection (also used while the project is closed).
    QStringList declaredModuleIds;
    QMap<QString, QList<QDomElement>> moduleDeclarations;

    /// Declarations for which the loader did not produce a package.
    QStringList unresolvedModuleIds;

    /// Backing storage for the reference returned by moduleIds() while open.
    mutable QStringList moduleIdsSnapshot;

    /// Module ids associated with classes used by this project
    QMap<QString, QString> classModuleIds;

    /// Project path property. Only used for visualization in the GUI.
    GtStringProperty pathProp;

    /// User definable comment
    QString comment;
};

GtProject::GtProject(const QString& path) : pimpl(std::make_unique<Impl>(path))
{
    pimpl->valid = loadMetaData();
    pimpl->upgradesAvailable = checkForUpgrades();

    pimpl->pathProp.setReadOnly(true);

    registerProperty(pimpl->pathProp);

    if (gtApp)
    {
        connect(&pimpl->projectSettings, &GtProjectSettings::changed, this,
                []() {
                    if (gtApp->session()) gtApp->session()->save();
                });
    }
}

void
GtProject::setModuleIds(const QStringList& list)
{
    if (!isOpen())
    {
        pimpl->declaredModuleIds = list;
    }
}

const QString
GtProject::mainFilename()
{
    return QStringLiteral("project.") + mainFileExtension();
}

const QString
GtProject::mainFileExtension()
{
    return QStringLiteral("gtlab");
}

const QString
GtProject::moduleExtension()
{
    return QStringLiteral("gtmod");
}

const GtProjectSettings &
GtProject::getProjectSettings() const
{
    return pimpl->projectSettings;
}

GtProjectSettings &
GtProject::projectSettings()
{
    return pimpl->projectSettings;
}

QString
GtProject::comment() const
{
    return pimpl->comment;
}

void
GtProject::setComment(const QString& comment)
{
    pimpl->comment = comment;
    changed();
}

void
GtProject::setInternalizeOnSave(bool value)
{
    pimpl->internalizeOnSave = value;
}

QStringList
GtProject::availableModuleUpgrades() const
{
    GtProjectAnalyzer analyzer(this);
    GtFootprint footprint = analyzer.footPrint();

    // collect footprints
    QMap<QString, GtVersionNumber> footprints = footprint.fullVersionInfo();

    return gt::detail::GtModuleUpgrader::instance()
        .availableModuleUpgrades(footprints);
}

QList<GtVersionNumber>
GtProject::availableUpgrades(const QString& moduleId) const
{
    GtProjectAnalyzer analyzer(this);
    GtFootprint footprint = analyzer.footPrint();

    QMap<QString, GtVersionNumber> verInfo = footprint.fullVersionInfo();

    if (verInfo.contains(moduleId))
    {
        GtVersionNumber savedVer = verInfo.value(moduleId);

        return gt::detail::GtModuleUpgrader::instance()
            .availableUpgrades(moduleId, savedVer);
    }

    return {};
}

void
GtProject::upgradeProjectData()
{
    if (!isValid())
    {
        return;
    }

    const QStringList availUpgrades = availableModuleUpgrades();

    if (availUpgrades.isEmpty())
    {
        return;
    }

    GtProjectAnalyzer analyzer(this);
    GtFootprint footprint = analyzer.footPrint();

    QDir pdir(pimpl->path);
    QStringList entryList;

    for (const auto& mid : pimpl->declaredModuleIds)
    {
        const auto filename = moduleDataPath(mid);
        if (filename.isEmpty()) continue;
        renameOldModuleFile(path(), mid);
        if (QFile::exists(filename)) entryList.append(filename);
    }

    entryList << pdir.absoluteFilePath(mainFilename());

    //exted list with task files
    QString tasksPath = pimpl->path + "/tasks";
    QDirIterator it(tasksPath,
                    QStringList() << "*.gttask",
                    QDir::Files,
                    QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        entryList << it.next();
    }

    gtDebug() << "upgrading files: " << entryList;

    // collect all version information
    QMap<QString, GtVersionNumber> versInfo = footprint.fullVersionInfo();

    bool saveWithLinkFiles = getProjectSettings()
                                 .ownObjectFileSerializationEnabled();

    gt::detail::GtModuleUpgrader::instance()
        .upgrade(objectName(), saveWithLinkFiles, versInfo, entryList);

    // update project footprint for updated module
    updateModuleFootprint(availUpgrades);

    pimpl->valid = loadMetaData();
    pimpl->upgradesAvailable = checkForUpgrades();
}


gt::filesystem::CopyStatus
gt::project::copyProjectData(const QDir& projectDir,
                             const QDir& targetDir,
                             int copyProjectFlags)
{


    int copyDirFlags = gt::filesystem::Recursive;
    if (copyProjectFlags & ForceOverwrite)
    {
        copyDirFlags |= gt::filesystem::OverwriteFiles;
    }

    QString reString;
    if (copyProjectFlags & IgnoreBackupMd)
    {
        reString += R"(^(?!GTlabBackUpMessage.md$).*)";
    }
    if (!(copyProjectFlags & IncludeBackups))
    {
        reString += R"(^(?!backup\/.*$).*)";
    }

    QRegularExpression regex(reString);
    return filesystem::copyDir(projectDir, targetDir, copyDirFlags, regex);
}



bool
GtProject::updateProjectMetaData(const QDir& projectPath,
                          const GtProject::ProjectMetaData& data)
{
    auto projectDoc = GtProject::readProjectData(projectPath);

    if (projectDoc.isNull())
    {
        return false;
    }

    auto projectRoot = projectDoc.documentElement();

    // we assume to have a valid gtlab project

    // update project name
    projectRoot.setAttribute("projectname", data.projectName);

    // store the project
    QString filename = projectPath.path() + QDir::separator() + mainFilename();
    QFile pfile(filename);

    // we have read from it before so it still should be there
    assert(pfile.exists());

    return gt::xml::writeDomDocumentToFile(filename, projectDoc, true);
}

void
GtProject::createBackup(QString const& message) const
{
    if (!isValid())
    {
        return;
    }

    QDir pdir(pimpl->path);

    assert(pdir.exists());

    QString timeStamp = QDateTime::currentDateTime().toString(
                "yyyyMMddhhmmss");

    QDir bdir(gt::project::backupDirPath(*this) + QDir::separator() + timeStamp);

    if (bdir.exists())
    {
        gtError() << "backup folder already exists!";
        return;
    }

    if (!bdir.mkpath("."))
    {
        gtError() << "could not create backup path!";
        return;
    }

    // backup all project files,
    // do not force overwrite and don't include backup directories
    if (gt::project::copyProjectData(pdir, bdir, 0) !=
        gt::filesystem::CopyStatus::Success)
    {
        gtError() << "Error while copying project files";
    }

    // backup project file
    QFile file(pdir.absoluteFilePath(mainFilename()));
    file.copy(bdir.absoluteFilePath(mainFilename()));

    if (!message.isEmpty())
    {
        QFile messageFile(bdir.absoluteFilePath(
                              gt::project::backUpMessageFileName()));

        if (!messageFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            gtWarning() << tr("Could not add message file to backup folder");
            return;
        }

        QTextStream stream(&messageFile);
        stream << message;

        messageFile.close();
    }

    return;
}

namespace
{

// Move a file identified by its relative path from `srcDir` to `targetDir`,
// creating missing intermediate directories as needed.
bool
moveRelativeFile(const QDir& srcDir, const QDir& targetDir, const QString& rel)
{
    const QString src = srcDir.filePath(rel);
    const QString dst = targetDir.filePath(rel);
    if (!QDir().mkpath(QFileInfo(dst).dir().absolutePath())) return false;
    return QFile::rename(src, dst);
}

/**
 * Stashes existing project files in a same-filesystem temporary folder before
 * copying a backup into place, so the project directory can be restored if
 * copying fails or if the restored project.gtlab manifest fails validation.
 */
struct BackupRestoreTransaction
{
    QDir projectDir;
    QTemporaryDir stashDir;
    QStringList stashedFiles;
    QStringList copiedFiles;

    explicit BackupRestoreTransaction(const QDir& dir) :
        projectDir(dir),
        // Keep the stash inside the project folder so QFile::rename stays on
        // the same filesystem and avoids cross-device move failures.
        stashDir(dir.filePath(QStringLiteral(".restore-XXXXXX")))
    { }

    bool isValid() const
    {
        return stashDir.isValid();
    }

    bool stashExistingFiles(const QStringList& relativePaths)
    {
        const QDir tempDir(stashDir.path());
        for (const auto& file : relativePaths)
        {
            const QFileInfo original(projectDir.filePath(file));
            if (!original.isFile() && !original.isSymLink()) continue;
            if (!moveRelativeFile(projectDir, tempDir, file))
            {
                gtError() << GtProject::tr(
                                 "Could not stash original file '%1'.")
                                 .arg(file);
                return false;
            }
            stashedFiles.append(file);
        }
        return true;
    }

    bool copyFromBackup(const QDir& backupDir, const QStringList& backupFiles)
    {
        for (const auto& file : backupFiles)
        {
            const QFileInfo target(projectDir.filePath(file));
            if (!QDir().mkpath(target.absolutePath()) || target.exists() ||
                target.isSymLink())
            {
                gtError() << GtProject::tr(
                                 "Could not prepare restore for file '%1'.")
                                 .arg(file);
                return false;
            }
            copiedFiles.append(file);
            if (!QFile::copy(backupDir.filePath(file), target.filePath()))
            {
                gtError()
                    << GtProject::tr("Could not restore file '%1'.").arg(file);
                return false;
            }
        }
        return true;
    }

    void rollback()
    {
        bool rollbackOk = true;
        for (const auto& file : copiedFiles)
        {
            const QFileInfo target(projectDir.filePath(file));
            if ((target.exists() || target.isSymLink()) &&
                !QFile::remove(target.filePath()))
            {
                gtError() << GtProject::tr(
                                 "Could not remove restored file '%1'.")
                                 .arg(file);
                rollbackOk = false;
            }
        }

        const QDir tempDir(stashDir.path());
        for (const auto& file : stashedFiles)
        {
            if (!moveRelativeFile(tempDir, projectDir, file))
            {
                gtError() << GtProject::tr(
                                 "Could not recover original file '%1'.")
                                 .arg(file);
                rollbackOk = false;
            }
        }

        if (!rollbackOk)
        {
            // Keep the stash directory so the user can recover files manually.
            stashDir.setAutoRemove(false);
            gtError() << GtProject::tr(
                             "Backup rollback incomplete. Recovery folder: %1")
                             .arg(stashDir.path());
        }
    }
};

} // namespace

GtProject::RestoreStatus
GtProject::restoreBackupFiles(const QString& timeStamp)
{
    using S = GtProject::RestoreStatus;

    assert(!isOpen());

    const QDir projectDir(path());
    assert(projectDir.exists());

    const QDir backupDir(path() + QDir::separator() + "backup" +
                         QDir::separator() + timeStamp);
    if (!backupDir.exists())
    {
        gtError().nospace().noquote()
            << "Backup folder '" << timeStamp << "' not found";
        return S::ErrorNoBackupFound;
    }

    // Restoring a backup can change the project's declared module packages.
    // We reload project.gtlab after copying so this closed GtProject reflects
    // the restored package set on reopen; if copying or manifest validation
    // fails, we roll back the directory and restore the previous metadata.
    static const QRegularExpression projectModuleTasksRex(
        R"(^[\w\s\-{}]+\.(gtlab|gtmod)$|^tasks\/[\w\s\-{}\/]+\.gttask$)");
    static const QRegularExpression backupContents(
        QStringLiteral(R"(^(?!GTlabBackUpMessage\.md$|backup/).*$)"));

    const auto backupFiles =
        gt::filesystem::directoryEntries(backupDir, true, backupContents);
    auto filesToStash = gt::filesystem::directoryEntries(projectDir, true,
                                                         projectModuleTasksRex);
    // Also stash any existing auxiliary files that the backup will overwrite.
    filesToStash.append(backupFiles);

    BackupRestoreTransaction tx(projectDir);
    if (!tx.isValid())
    {
        gtError() << "Failed to create temporary directory";
        return S::ErrorCopyFailed;
    }

    // Declared after `tx` so rollback runs while `tx.stashDir` is still alive.
    auto rollback = gt::finally([&] {
        tx.rollback();
        pimpl->valid = loadMetaData();
        pimpl->upgradesAvailable = checkForUpgrades();
        if (!pimpl->valid)
            gtError() << tr("Could not reload original project metadata.");
    });

    if (!tx.stashExistingFiles(filesToStash)) return S::ErrorCopyFailed;
    if (!tx.copyFromBackup(backupDir, backupFiles)) return S::ErrorCopyFailed;

    // Refresh cached manifest metadata (declared module packages and available
    // upgrades) from the restored project.gtlab before the project is reopened.
    pimpl->valid = loadMetaData();
    if (!pimpl->valid) return S::ErrorCopyFailed;

    pimpl->upgradesAvailable = checkForUpgrades();
    rollback.clear();
    return S::Success;
}

bool
GtProject::loadMetaData()
{
    QDomElement root = readProjectData(path()).documentElement();

    if (root.isNull())
    {
        gtWarning() << tr("Root is null!");
        return false;
    }

    QString projectname = root.attribute(QStringLiteral("projectname"));
    if (projectname.isEmpty())
    {
        gtError() << tr("Invalid GTlab project file!");
        return false;
    }

    setObjectName(projectname);

    // read comment
    QDomElement cdata = root.firstChildElement(QStringLiteral("comment"));

    if (!cdata.isNull())
    {
        //gtDebug() << "(" << projectname << ") comment found!";
        pimpl->comment = cdata.text();
    }

    // module meta data
    readModuleMetaData(root);

    pimpl->classModuleIds = gt::xml::readClassModuleMap(root);
    gt::insertMissingInto(pimpl->classModuleIds, taskClassModuleIds(path()));

    return true;
}

bool
GtProject::checkForUpgrades() const
{
    if (!pimpl->valid)
    {
        return false;
    }

    GtProjectAnalyzer analyzer(this);
    GtFootprint footprint = analyzer.footPrint();

    // collect all version information
    QMap<QString, GtVersionNumber> versInfo = footprint.fullVersionInfo();

    return gt::detail::GtModuleUpgrader::instance()
        .upgradesAvailable(versInfo);
}

void
GtProject::readModuleMetaData(const QDomElement& root)
{
    pimpl->declaredModuleIds.clear();
    pimpl->moduleDeclarations.clear();

    /* module informations */
    QDomElement mdata = root.firstChildElement(QStringLiteral("MODULES"));

    if (mdata.isNull())
    {
        return;
    }

    QDomElement module = mdata.firstChildElement(QStringLiteral("MODULE"));
    while (!module.isNull())
    {
        QString mid = module.attribute(QStringLiteral("name"));

        if (!isSafeModuleFileName(mid))
        {
            gtWarning() << objectName() << tr("Invalid module ID:") << mid;
        }
        else
        {
            auto& declarations = pimpl->moduleDeclarations[mid.toCaseFolded()];
            if (declarations.isEmpty())
            {
                if (!gtApp || !gtApp->moduleIds().contains(mid))
                {
                    gtWarning().nospace() << objectName() << ": "
                                          << tr("Unknown module: ") << mid;
                }
                pimpl->declaredModuleIds.append(mid);
                declarations.append(module);
            }
            else
            {
                const auto name = QStringLiteral("name");
                const auto firstId = declarations.first().attribute(name);
                gtWarning() << objectName()
                            << tr("Module declarations '%1' and '%2' refer to "
                                  "the same package slot.")
                                   .arg(firstId, mid);
                // Module filenames are lowercased, so case variants share one
                // slot. Promote a variant matching a loaded module while
                // keeping the other XML declarations intact for saving.
                if (gtApp && gtApp->modulePackageId(firstId).isEmpty() &&
                    !gtApp->modulePackageId(mid).isEmpty())
                {
                    pimpl->declaredModuleIds[pimpl->declaredModuleIds.indexOf(
                        firstId)] = mid;
                    declarations.prepend(module);
                }
                else
                {
                    declarations.append(module);
                }
            }
        }

        module = module.nextSiblingElement(QStringLiteral("MODULE"));
    }
}

void
GtProject::updateClassModuleIds()
{
    gt::xml::ClassModuleMap updated;
    for (const GtObject* object : findChildren<GtObject*>())
    {
        const QString className = object->isDummy()
                                      ? object->toMemento().className()
                                      : object->metaObject()->className();
        // skip visited classes
        if (className.isEmpty() || updated.contains(className)) continue;
        QString moduleId;
        if (const auto* factory = object->factory())
        {
            moduleId = factory->moduleId(className);
        }
        if (moduleId.isEmpty())
        {
            moduleId = gtObjectFactory->moduleId(className);
        }
        if (moduleId.isEmpty())
        {
            moduleId = pimpl->classModuleIds.value(className);
        }
        if (!moduleId.isEmpty())
        {
            updated.insert(className, moduleId);
        }
    }

    // merge remaining classes not handled by loaded modules
    for (auto kv = pimpl->classModuleIds.cbegin();
         kv != pimpl->classModuleIds.cend(); ++kv)
    {
        // skip visited classes
        if (kv.value().isEmpty() || updated.contains(kv.key())) continue;
        updated.insert(kv.key(), kv.value());
    }

    pimpl->classModuleIds = std::move(updated);
}

GtObject*
GtProject::readProcessData()
{
    GtProcessData* data = new GtProcessData;
    data->setFactory(gtObjectFactory);
    data->setDefault(true);
    data->read(path());

    return data;
}

GtObject*
GtProject::readLabelData(const GtObjectList& moduleData)
{
    GtLabelData* data = new GtLabelData;
    data->setDefault(true);
    data->setFactory(gtObjectFactory);

    // read saved label data here
    QDomElement root = readProjectData(path()).documentElement();

    if (!root.isNull())
    {
        /* label informations */
        QDomElement ldata = root.firstChildElement(QStringLiteral("LABELS"));

        if (!ldata.isNull())
        {
            QDomElement le = ldata.firstChildElement(QStringLiteral("object"));
            while (!le.isNull())
            {
                QString fieldClass = le.attribute(QStringLiteral("class"));

                if (fieldClass ==
                        QLatin1String(GtLabel::staticMetaObject.className()))
                {
                    GtObjectMemento memento(le);
                    if (!memento.isNull())
                    {
                        auto label =
                            memento.restore_unique<GtLabel>(gtObjectFactory);

                        if (label)
                        {
                            if (data->addLabel(label.get()))
                            {
                                // data takes ownership of the label
                                auto* addedLabel = label.release();
                                Q_ASSERT(addedLabel);
                            }
                            else
                            {
                                gtWarning() << tr("could not add label") << " ("
                                            << tr("already exists") << ")";
                            }
                        }
                    }
                }

                le = le.nextSiblingElement(QStringLiteral("object"));
            }
        }
    }

    // read datamodel label entries
    foreach (GtObject* obj, moduleData)
    {
        QList<GtLabel*> labels = obj->findChildren<GtLabel*>();
        foreach (GtLabel* label, labels)
        {
            data->addLabel(label->objectName());
        }
    }

    // add default label
    data->addDefaultLabel();

    return data;
}

GtObjectList
GtProject::readModuleData()
{
    GtObjectList packages;
    for (const QString& mid : pimpl->declaredModuleIds)
    {
        if (!gtApp || gtApp->modulePackageId(mid).isEmpty()) continue;

        const auto filename = moduleDataPath(mid);
        if (filename.isEmpty())
        {
            gtWarning() << tr("Unsafe module package path for '%1'.").arg(mid);
            continue;
        }
        renameOldModuleFile(path(), mid);
        QStringList warnings;
        QDomDocument document =
            gt::xml::loadProjectXmlWithLinkedObjects(filename, &warnings);
        for (const auto& warning : warnings)
            gtWarning() << warning;

        QDomElement root = document.documentElement();
        gt::insertMissingInto(pimpl->classModuleIds,
                              gt::xml::takeClassModuleMap(root));

        auto package =
            warnings.isEmpty() ? loadPackage(mid, root, QDir(path())) : nullptr;
        if (!package)
        {
            gtWarning() << tr("Could not load the module package for '%1'. Its "
                              "data will be preserved.")
                               .arg(mid);
            continue;
        }

        gtExternalizationManager->initExternalizedObjects(*package);
        packages.append(package.release());
    }
    return packages;
}

namespace
{

// Return true if neither an active package nor an unresolved manifest
// declaration currently occupies `moduleId`'s slot (case-insensitively).
bool
hasFreePackageSlot(const GtProject& project, const QString& moduleId)
{
    return !moduleId.isEmpty() &&
           !project.moduleIds().contains(moduleId, Qt::CaseInsensitive);
}

// Validate preconditions shared by createPackage() and restorePackage()
// (active unlocked project, loaded package provider, and unoccupied slot).
// Returns an empty string on success or a user-facing error message.
QString
packagePreparationError(const GtProject& project, const QString& moduleId)
{
    if (!project.isOpen() || !gtApp || gtApp->currentProject() != &project ||
        GtProjectExecutionGuard::isLocked(&project))
    {
        return GtProject::tr("Module packages can only be edited in the active "
                             "project while no task is executing.");
    }

    const auto packageClass = gtApp->modulePackageId(moduleId);
    const auto* type =
        gtObjectFactory->GtAbstractObjectFactory::metaObject(packageClass);
    if (!type || !type->inherits(&GtPackage::staticMetaObject))
    {
        return GtProject::tr(
                   "No loaded module provides a module package for '%1'.")
            .arg(moduleId);
    }

    if (!hasFreePackageSlot(project, moduleId))
    {
        return GtProject::tr(
                   "The project already contains a module package or "
                   "an unresolved module package declaration for '%1'.")
            .arg(moduleId);
    }
    return {};
}

} // namespace

std::unique_ptr<GtPackage>
GtProject::createPackage(const QString& moduleId, QString& error) const
{
    error = packagePreparationError(*this, moduleId);
    if (!error.isEmpty()) return {};

    auto package = newPackage(moduleId);
    if (!package)
    {
        error = tr("Could not create a module package for '%1'.").arg(moduleId);
    }
    return package;
}

bool
GtProject::childAccepted(GtObject* child)
{
    const auto* package = qobject_cast<GtPackage*>(child);
    // Opening a project attaches the saved tree before marking it open.
    return !package || !isOpen() ||
           hasFreePackageSlot(*this, packageModuleId(*package));
}

GtProject::PackageRestoreStatus
GtProject::restorePackage(const QString& moduleId, QString& error,
                          bool allowUnversioned)
{
    error = packagePreparationError(*this, moduleId);
    if (!error.isEmpty()) return PackageRestoreStatus::Failed;

    const auto filename = moduleDataPath(moduleId);
    if (filename.isEmpty())
    {
        error = tr("Unsafe module package path for '%1'.").arg(moduleId);
        return PackageRestoreStatus::Failed;
    }
    // Ask for legacy-file confirmation before reading the body or its links.
    if (!allowUnversioned)
    {
        QFile file(filename);
        QXmlStreamReader header(&file);
        if (!file.open(QIODevice::ReadOnly) || !header.readNextStartElement() ||
            header.name() != QLatin1String("GTLABMODULE"))
        {
            error = tr("Could not read the saved module package for '%1'.")
                        .arg(moduleId);
            return PackageRestoreStatus::Failed;
        }
        if (!header.attributes().hasAttribute(QStringLiteral("version")))
        {
            return PackageRestoreStatus::NeedsVersionConfirmation;
        }
    }

    const QDir projectDir(path());
    const auto installedVersion = gtApp->moduleVersion(moduleId);
    auto* const owningThread = thread();
    const QPointer<GtProject> target(this);

    std::unique_ptr<GtPackage> package;
    gt::xml::ClassModuleMap providers;
    auto failure = PackageRestoreStatus::Failed;
    error = tr("Could not restore the saved module package for '%1'.")
                .arg(moduleId);
    auto helper = gt::makeLoadingHelper([&] {
        QStringList warnings;
        auto document =
            gt::xml::loadProjectXmlWithLinkedObjects(filename, &warnings);
        auto root = document.documentElement();
        if (root.tagName() != QLatin1String("GTLABMODULE") ||
            !warnings.isEmpty())
        {
            error = tr("Could not read the saved module package for '%1' or "
                       "its linked objects.")
                        .arg(moduleId);
            return;
        }

        if (root.hasAttribute(QStringLiteral("version")))
        {
            const GtVersionNumber savedVersion(
                root.attribute(QStringLiteral("version")));
            if (savedVersion.isNull())
            {
                error = tr("The saved module package for '%1' has an invalid "
                           "module version.")
                            .arg(moduleId);
                return;
            }
            if (installedVersion < savedVersion)
            {
                error = tr("The module package for '%1' was saved with a newer "
                           "module version (%2).")
                            .arg(moduleId, savedVersion.toString());
                return;
            }
            if (!gt::detail::GtModuleUpgrader::instance()
                     .availableUpgrades(moduleId, savedVersion)
                     .isEmpty())
            {
                error = tr("The saved module package for '%1' requires a data "
                           "upgrade before it can be restored.")
                            .arg(moduleId);
                return;
            }
        }
        else if (!allowUnversioned)
        {
            error.clear();
            failure = PackageRestoreStatus::NeedsVersionConfirmation;
            return;
        }

        providers = gt::xml::takeClassModuleMap(root);
        package = loadPackage(moduleId, root, projectDir);
        if (!package) return;

        // Fetch retained package data from the current project storage rather
        // than the initial-open HDF5 backup.
        gtExternalizationManager->setFetchInitialVersion(*package, false);
        gt::moveToThread(*package, owningThread);
    });
    gtApp->loadingProcedure(std::move(helper));

    auto* project = target.data();
    if (!project || !package) return failure;

    if (!gtDataModel->appendChild(std::move(package), project).isValid())
    {
        error = tr("Could not add the module package to the project.");
        return PackageRestoreStatus::Failed;
    }
    gt::insertMissingInto(project->pimpl->classModuleIds, providers);
    error.clear();
    return PackageRestoreStatus::Success;
}

bool
GtProject::saveModuleData()
{
    if (!isOpen()) return false;

    QMap<QString, GtPackage*> packages;
    QSet<QString> savedSlots;
    for (auto* package : findDirectChildren<GtPackage*>())
    {
        const auto mid = packageModuleId(*package);
        if (mid.isEmpty() || savedSlots.contains(mid.toLower()) ||
            pimpl->unresolvedModuleIds.contains(mid, Qt::CaseInsensitive))
        {
            gtError() << tr("Cannot save a module package with an unknown "
                            "or duplicate module slot.");
            return false;
        }
        savedSlots.insert(mid.toLower());
        packages.insert(mid, package);
    }

    if (!saveExternalizedObjectData())
    {
        gtError() << tr("Could not save externalized project data.");
        return false;
    }

    updateClassModuleIds();
    for (auto it = packages.cbegin(); it != packages.cend(); ++it)
    {
        const auto& mid = it.key();
        auto* package = it.value();
        const auto filename = moduleDataPath(mid);
        if (filename.isEmpty())
        {
            gtError() << tr("Unsafe module package path for '%1'.").arg(mid);
            return false;
        }
        QDomDocument document;
        document.appendChild(document.createProcessingInstruction(
            QStringLiteral("xml"),
            QStringLiteral("version=\"1.0\" encoding=\"utf-8\"")));
        auto root = document.createElement(QStringLiteral("GTLABMODULE"));
        root.setAttribute(QStringLiteral("uuid"), package->uuid());
        root.setAttribute(QStringLiteral("module"), mid);
        const auto version = gtApp->moduleVersion(mid);
        if (!version.isNull())
            root.setAttribute(QStringLiteral("version"), version.toString());
        root.setAttribute(
            QStringLiteral("class"),
            QString::fromLatin1(package->metaObject()->className()));
        document.appendChild(root);

        if (!package->saveData(root, document) ||
            !package->saveMiscData(QDir(path())))
        {
            gtError()
                << tr("Could not save the module package for '%1'.").arg(mid);
            return false;
        }

        const auto moduleMappings =
            gt::xml::classModuleMapForObjects(pimpl->classModuleIds, root);
        gt::xml::writeClassModuleMap(root, document, moduleMappings);
        if (!saveProjectFiles(filename, document)) return false;
    }

    return true;
}

bool
GtProject::saveProjectOverallData()
{
    if (!isOpen())
    {
        return false;
    }

    gtDebug().noquote() << tr("Saving project '%1'...").arg(objectName());

    updateClassModuleIds();

    QDomDocument document;
    QDomProcessingInstruction header = document.createProcessingInstruction(
            QStringLiteral("xml"),
            QStringLiteral("version=\"1.0\" encoding=\"utf-8\""));
    document.appendChild(header);

    QDomElement rootElement =
            document.createElement(QStringLiteral("GTLAB"));

    rootElement.setAttribute(QStringLiteral("projectname"), objectName());
    rootElement.setAttribute(QStringLiteral("version"),
                             gtApp->version().toString());

    // footprint is derived from the modules that the project requires,
    // not from all modules loaded in the current GTlab environment
    QDomDocument footPrintDoc;
    GtFootprint footPrint = createProjectFootprint();
    footPrintDoc.setContent(footPrint.exportToXML(), true);

    rootElement.appendChild(footPrintDoc.documentElement());

    QDomElement commentElement =
            document.createElement(QStringLiteral("comment"));
    QDomText cTxt = document.createTextNode(pimpl->comment);
    commentElement.appendChild(cTxt);
    rootElement.appendChild(commentElement);

    saveModuleMetaData(rootElement, document);

    gt::xml::writeClassModuleMap(rootElement, document, pimpl->classModuleIds);

    if (!saveProcessData(rootElement, document))
    {
        return false;
    }

    saveLabelData(rootElement, document);

    document.appendChild(rootElement);

    QString filename = pimpl->path + QDir::separator() + mainFilename();

    if (!saveProjectFiles(filename, document)) return false;

    // Keep the closed-project selection at the last successful save, even
    // when subsequent edits are discarded on close.
    readModuleMetaData(rootElement);
    return true;
}

bool
GtProject::saveExternalizedObjectData()
{
    bool success{true};
    auto objects = findChildren<GtExternalizedObject*>();

    // force internalization of all objects
    if (pimpl->internalizeOnSave)
    {
        gtDebug() << "Internalizing object data...";

        int counter = 0;
        for (auto* obj : qAsConst(objects))
        {
            success &= obj->internalize();
            counter += obj->isFetched();
        }

        gtInfo() << tr("Successfully internalized") << counter
                 << tr("out of") << objects.count() << tr("objects!");

        return success;
    }

    if (!gtExternalizationManager->isExternalizationEnabled())
    {
        return true;
    }

    gtDebug() << "Saving externalized object data...";

    for (auto* obj : qAsConst(objects))
    {
        // only externalize the object if its not referenced as the data wont be
        // cleared otherwise and will be saved as memento
        if (obj->refCount() == 0)
        {
            success &= obj->externalize();
        }
    }

    return success;
}

void
GtProject::saveModuleMetaData(QDomElement& root, QDomDocument& doc)
{
    QDomElement modulesElement = doc.createElement(QStringLiteral("MODULES"));

    for (const QString& mid : moduleIds())
    {
        const auto originals =
            pimpl->moduleDeclarations.value(mid.toCaseFolded());
        const bool hasPackage = findPackage(mid);
        if (!hasPackage && !originals.isEmpty())
        {
            for (const auto& original : originals)
                modulesElement.appendChild(doc.importNode(original, true));
            continue;
        }

        QDomElement moduleElement = doc.createElement(QStringLiteral("MODULE"));
        moduleElement.setAttribute(QStringLiteral("name"), mid);
        if (hasPackage)
        {
            const auto version = gtApp->moduleVersion(mid);
            if (!version.isNull())
                moduleElement.setAttribute(QStringLiteral("version"),
                                           version.toString());
        }
        modulesElement.appendChild(moduleElement);
        // Alternate declarations share this slot but may carry unknown metadata.
        for (qsizetype i = 1; i < originals.size(); ++i)
            modulesElement.appendChild(doc.importNode(originals.at(i), true));
    }

    root.appendChild(modulesElement);
}

bool
GtProject::saveProcessData(QDomElement& /*root*/, QDomDocument& /*doc*/)
{
    const GtProcessData* pd = processData();

    if (pd)
    {
        return pd->save(path());
    }

    return true;
}

void
GtProject::saveLabelData(QDomElement& root, QDomDocument& doc)
{
    QDomElement ldElement = doc.createElement(QStringLiteral("LABELS"));

    GtLabelData* ld = labelData();

    if (ld)
    {
        foreach (const GtLabel* label, ld->findDirectChildren<GtLabel*>())
        {
            GtObjectMemento memento = label->toMemento();
            ldElement.appendChild(memento.documentElement());
        }
    }

    root.appendChild(ldElement);
}

QDomDocument
GtProject::readProjectData(const QDir& projectPath)
{
    return GtProjectIO::readProjectData(projectPath);
}

QList<GtLabel*>
GtProject::findLabelUsagesHelper(GtObject* obj, GtLabel* label)
{
    QList<GtLabel*> retval;

    QList<GtObject*> childs = obj->findDirectChildren<GtObject*>();

    foreach (GtObject* c, childs)
    {
        GtLabel* l = qobject_cast<GtLabel*>(c);

        if (l)
        {
            if (l->objectName() == label->objectName())
            {
                retval.append(l);
            }
        }
        else
        {
            retval.append(findLabelUsagesHelper(c, label));
        }
    }

    return retval;
}

void
GtProject::renameOldModuleFile(const QString& path, const QString& modId)
{
    const QDir dir(path);
    const auto current = moduleFilePath(dir, modId, moduleExtension());
    const auto legacy = moduleFilePath(dir, modId, mainFileExtension());
    if (legacy.isEmpty() || current.isEmpty() ||
        QFileInfo(legacy).fileName().compare(mainFilename(),
                                             Qt::CaseInsensitive) == 0)
        return;

    if (QFile::exists(legacy)) QFile::rename(legacy, current);
}


bool
GtProject::saveProjectFiles(const QString& filePath, const QDomDocument& doc)
{
    return GtProjectIO::saveProjectFiles(
        filePath,
        doc,
        projectSettings().ownObjectFileSerializationEnabled());
}

GtFootprint
GtProject::createProjectFootprint() const
{
    // determine the modules that the current project uses directly
    GtProjectDependencyAnalyzer analyzer(this);

    // resolve the complete set of modules that the project requires,
    // including transitive module dependencies
    const gt::Modules modules = gtApp ? gtApp->modules() : gt::Modules{};
    const auto requirements = modules.requirementsFor(
        QStringList(analyzer.directlyRequiredModuleIds().cbegin(),
                    analyzer.directlyRequiredModuleIds().cend()));

    QSet<QString> requiredModules{requirements.moduleIds.cbegin(),
                                  requirements.moduleIds.cend()};

    // entries that cannot be resolved must not lead to a pruning of the
    // previously stored footprint
    QSet<QString> unresolved{requirements.unresolved.cbegin(),
                             requirements.unresolved.cend()};
    unresolved.unite(analyzer.unknownUsedClassNames());

    // A declaration without an instantiated package may still have recoverable
    // data and dependencies, even if its package file is currently missing.
    const auto packageSelection = moduleIds();
    for (const QString& mid : packageSelection)
    {
        requiredModules.insert(mid);
        if (!findPackage(mid))
        {
            unresolved.insert(mid);
        }
    }

    // read the previously stored footprint to preserve version information
    QMap<QString, GtVersionNumber> oldModules;
    const QString oldFootprintData = readFootprint();
    if (!oldFootprintData.isEmpty())
    {
        oldModules = GtFootprint(oldFootprintData).modules();
    }

    QMap<QString, GtVersionNumber> moduleVersions;

    for (const QString& mid : requiredModules)
    {
        const bool unresolvedPackage =
            packageSelection.contains(mid) && !findPackage(mid);
        auto version =
            unresolvedPackage ? GtVersionNumber{} : modules.version(mid);
        if (version.isNull())
        {
            version = oldModules.value(mid, GtVersionNumber());
        }
        if (version.isNull() && unresolvedPackage)
        {
            version = GtVersionNumber(
                pimpl->moduleDeclarations.value(mid.toCaseFolded())
                    .value(0)
                    .attribute(QStringLiteral("version")));
        }
        moduleVersions.insert(mid, version);
    }

    // If the dependency metadata of a required module is unavailable, its
    // transitive dependency closure cannot be recomputed safely. In this
    // case, keep previously stored footprint entries that cannot be proven
    // unused because of the unresolved dependency. Once all required module
    // metadata is available again, a later save can fully recompute the
    // closure and remove stale entries.
    if (!unresolved.isEmpty())
    {
        gtDebug() << tr("Project footprint cannot be recalculated completely"
                        " (unresolved dependencies: %1).")
                         .arg(
                             QStringList(unresolved.cbegin(), unresolved.cend())
                                 .join(", "));

        for (auto it = oldModules.cbegin(); it != oldModules.cend(); ++it)
        {
            if (!moduleVersions.contains(it.key()))
            {
                moduleVersions.insert(it.key(), it.value());
            }
        }
    }

    return GtFootprint::fromProjectModules(moduleVersions);
}

void
GtProject::updateModuleFootprint(const QStringList& modIds)
{
    gtDebug() << "GtProject::updateModuleFootprint...";

    auto document = readProjectData(pimpl->path);
    if (document.isNull())
    {
        return;
    }

    QString filename = path() + QDir::separator() + mainFilename();

    QDomElement pdata = document.documentElement();
    QDomElement footprint = pdata.firstChildElement(
                QStringLiteral("env-footprint"));


    if (footprint.isNull())
    {
        return;
    }

    if (modIds.contains(GtFootprint::frameworkIdentificationString()))
    {
        QDomElement core_ver =
                footprint.firstChildElement(QStringLiteral("core-ver"));

        if (core_ver.isNull())
        {
            return;
        }

        QDomElement core_ver_new = document.createElement("core-ver");
        footprint.replaceChild(core_ver_new, core_ver);

        QDomText t = document.createTextNode(gtApp->version().toString());
        core_ver_new.appendChild(t);
    }

    QDomElement mods = footprint.firstChildElement(QStringLiteral("modules"));

    if (mods.isNull())
    {
        return;
    }

    QDomElement mod = mods.firstChildElement(QStringLiteral("module"));
    while (!mod.isNull())
    {
        QDomElement modIdEl = mod.firstChildElement(QStringLiteral("id"));
        QDomElement modVerEl = mod.firstChildElement(QStringLiteral("ver"));

        if (!modIdEl.isNull() && !modVerEl.isNull())
        {
            if (modIds.contains(modIdEl.text()))
            {
                gtDebug() << "updating footprint " << modIdEl.text() << "...";
                gtDebug() << "file " << filename << "...";
                gtDebug() << "  |-> old = " << modVerEl.text();
                gtDebug() << "  |-> new = "
                          << gtApp->moduleVersion(modIdEl.text()).toString();

                modVerEl.firstChild().setNodeValue(
                            gtApp->moduleVersion(modIdEl.text()).toString());
            }
        }

        mod = mod.nextSiblingElement(QStringLiteral("module"));
    }

    if (!gt::xml::writeDomDocumentToFile(filename, document, true))
    {
        gtError() << filename << QStringLiteral(": ")
                  << "Failed to save project data!";
    }
}

GtProject::~GtProject() = default;

void
GtProject::markOpen()
{
    pimpl->unresolvedModuleIds.clear();
    for (const auto& mid : pimpl->declaredModuleIds)
    {
        if (!findPackage(mid)) pimpl->unresolvedModuleIds.append(mid);
    }
    pimpl->isOpen = true;
}

void
GtProject::markClosed()
{
    pimpl->isOpen = false;
    pimpl->unresolvedModuleIds.clear();
}

QMap<QString, QString>&
GtProject::classModuleIds()
{
    return pimpl->classModuleIds;
}

const QString&
GtProject::path() const
{
    return pimpl->path;
}

QString
GtProject::moduleDataPath(const QString &moduleId) const
{
    return moduleFilePath(QDir(path()), moduleId, moduleExtension());
}

bool
GtProject::isValid() const
{
    return pimpl->valid;
}

bool
GtProject::isOpen() const
{
    return pimpl->isOpen;
}

bool
GtProject::upgradesAvailable() const
{
    return pimpl->upgradesAvailable;
}

bool
GtProject::upgradeProject(const QString& newProjectFilePath)
{
    if (newProjectFilePath.isEmpty() || path() == newProjectFilePath)
    {
        gtDebug() << "backup and overwriting project data...";

        // upgrade project data in separate thread if possible
        gtApp->loadingProcedure(gt::makeLoadingHelper([this] {
            createBackup();
            upgradeProjectData();
        }));

        return true;
    }
    else
    {
        gtDebug() << "upgrading data as new project...";
        gtDebug() << "  |-> " << QFileInfo(newProjectFilePath).fileName();
        gtDebug() << "  |-> " << newProjectFilePath;

        auto newProject = GtProjectProvider::duplicateExistingProject(
            QDir(path()),
            QDir(newProjectFilePath),
            QFileInfo(newProjectFilePath).fileName()
            );

        if (!newProject)
        {
            gtError() << "Could not save project to new directory";
            return false;
        }

        gtApp->loadingProcedure(gt::makeLoadingHelper(
            [&newProject] { newProject->upgradeProjectData(); }));

        gtDataModel->newProject(newProject.release(), false);

        return true;
    }
}

GtProcessData*
GtProject::processData()
{
    return findDirectChild<GtProcessData*>(QStringLiteral("Process Data"));
}

GtProcessData const*
GtProject::processData() const
{
    return const_cast<GtProject*>(this)->processData();
}

GtLabelData*
GtProject::labelData()
{
    return findDirectChild<GtLabelData*>(QStringLiteral("Label Data"));
}

const GtLabelData*
GtProject::labelData() const
{
    return const_cast<GtProject*>(this)->labelData();
}

GtTask*
GtProject::findProcess(const QString& name)
{
    if (auto* pdata = processData())
    {
        return pdata->findProcess(name);
    }

    return nullptr;
}

const GtTask*
GtProject::findProcess(const QString& name) const
{
    return const_cast<GtProject*>(this)->findProcess(name);
}

QStringList
GtProject::taskIds() const
{
    auto const* pdata = processData();
    if (!pdata)
    {
        return {};
    }

    return gt::objectNames(pdata->currentProcessList());
}

QMap<QString, QStringList>
GtProject::fullTaskIds() const
{
    auto const* pdata = processData();
    if (!pdata)
    {
        return {};
    }

    QMap<QString, QStringList> retval;

    QStringList const groupIds = pdata->userGroupIds() +
                                 pdata->customGroupIds();

    for (QString const& group : groupIds)
    {
        retval.insert(group, gt::objectNames(pdata->processList(group)));
    }

    return retval;
}

GtPackage*
GtProject::findPackage(const QString& mid)
{
    // class name of package
    const QString pkgId = gtApp ? gtApp->modulePackageId(mid) : QString{};
    if (pkgId.isEmpty()) return nullptr;

    auto childs = findDirectChildren<GtPackage*>();
    auto iter = std::find_if(std::begin(childs), std::end(childs),
                 [&pkgId](const GtPackage* pkg) {
        return pkg->metaObject()->className() == pkgId;
    });

    return iter != std::end(childs) ? *iter : nullptr;
}

const GtPackage*
GtProject::findPackage(const QString& mid) const
{
    return const_cast<GtProject*>(this)->findPackage(mid);
}

const QStringList&
GtProject::moduleIds() const
{
    if (!isOpen()) return pimpl->declaredModuleIds;

    QStringList ids = pimpl->unresolvedModuleIds;
    for (const auto* package : findDirectChildren<GtPackage*>())
    {
        const auto mid = packageModuleId(*package);
        if (!mid.isEmpty() && !ids.contains(mid)) ids.append(mid);
    }
    if (pimpl->moduleIdsSnapshot != ids)
        pimpl->moduleIdsSnapshot = std::move(ids);
    return pimpl->moduleIdsSnapshot;
}

QString
GtProject::classModuleId(const QString& className) const
{
    return pimpl->classModuleIds.value(className);
}

int
GtProject::numberOfLabelUsages(GtLabel* label) const
{
    if (!label)
    {
        return -1;
    }

    return const_cast<GtProject*>(this)->findLabelUsages(label).size();
}

QList<GtLabel*>
GtProject::findLabelUsages(GtLabel* label)
{
    QList<GtLabel*> usages;

    if (!label)
    {
        return usages;
    }

    QList<GtPackage*> packages = findDirectChildren<GtPackage*>();

    foreach (GtPackage* package, packages)
    {
        usages.append(findLabelUsagesHelper(package, label));
    }

    return usages;
}

GtObjectMemento
GtProject::toProjectDataMemento()
{
    GtObjectGroup* group = new GtObjectGroup;
    group->setUuid(QStringLiteral("-"));

    QList<GtPackage*> packages = findDirectChildren<GtPackage*>();

    foreach (GtPackage* package, packages)
    {
        GtObject* obj = package->clone();

        if (obj)
        {
            group->appendChild(obj);
        }
    }

    GtObjectMemento memento = group->toMemento();

    delete group;

    return memento;
}

bool
GtProject::fromProjectDataMemento(GtObjectMemento& memento)
{
    if (memento.isNull())
    {
        return false;
    }

    auto group = memento.restore_unique<GtObjectGroup>(gtObjectFactory);

    if (!group)
    {
        return false;
    }

    QList<GtPackage*> packages = group->findDirectChildren<GtPackage*>();

    foreach (GtPackage* package, packages)
    {
        GtPackage* oldPkg = findDirectChild<GtPackage*>(package->objectName());

        if (!oldPkg)
        {
            return false;
        }

        oldPkg->fromMemento(package->toMemento());
    }

    return true;
}

bool
GtProject::renameProject(const QString& str)
{
    if (isOpen())
    {
        return false;
    }

    if (str.isEmpty())
    {
        return false;
    }

    if (str == objectName())
    {
        return false;
    }

    if (!gtApp->session())
    {
        return false;
    }

    if (gtApp->session()->projectIds().contains(str))
    {
        return false;
    }

    // rename project within project file
    QDomElement projectData = readProjectData(path()).documentElement();

    if (projectData.isNull())
    {
        return false;
    }

    projectData.setAttribute(QStringLiteral("projectname"), str);

    QDomDocument document;
    QDomProcessingInstruction header = document.createProcessingInstruction(
            QStringLiteral("xml"),
            QStringLiteral("version=\"1.0\" encoding=\"utf-8\""));
    document.appendChild(header);

    document.appendChild(projectData);

    QString filename = pimpl->path + QDir::separator() + mainFilename();

    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gtWarning() << objectName() << QStringLiteral(": ")
                    << tr("Failed to save project data!");
        return false;
    }

    QTextStream stream(&file);
    stream << document.toString(5);

    file.close();

    setObjectName(str);

    acceptChanges();

    return true;
}

QString
GtProject::readFootprint() const
{
    QString filename = pimpl->path + QDir::separator() + mainFilename();

    QFile file(filename);

    if (!file.exists())
    {
        qWarning() << "WARNING: file does not exists!";
        qWarning() << " |-> " << filename;

        return QString();
    }

    QDomDocument document;

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!gt::xml::readDomDocumentFromFile(file, document, true, &errorStr,
                                          &errorLine, &errorColumn))
    {
        gtDebug() << tr("XML ERROR!") << " " << tr("line") << ": "
                  << errorLine << " " << tr("column") << ": "
                  << errorColumn << " -> " << errorStr;

        return QString();
    }

    QDomElement root = document.documentElement();

    if (root.isNull() || (root.tagName() != QLatin1String("GTLAB")))
    {
        gtDebug() << "ERROR: Invalid GTlab project file!";
        return QString();
    }

    QString retval;

    QDomElement ftprnt =
            root.firstChildElement(QStringLiteral("env-footprint"));

    if (ftprnt.isNull())
    {
        gtDebug() << "Footprint not found in project file!";
        return QString();
    }

    QTextStream stream(&retval);
    ftprnt.save(stream, 4);

    return retval;
}

QString
gt::project::backUpMessageFileName()
{
    return QStringLiteral("GTlabBackUpMessage.md");
}

QString
gt::project::backupDirPath(const GtProject& proj)
{
    return {proj.path() + QDir::separator() + "backup"};
}
