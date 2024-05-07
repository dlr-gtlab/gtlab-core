/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 29.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECT_H
#define GTPROJECT_H

#include "gt_core_exports.h"

#include <QStringList>

#include "gt_object.h"
#include "gt_session.h"
#include "gt_stringproperty.h"
#include "gt_filesystem.h"

class QDomDocument;
class QXmlStreamWriter;
class QDomElement;
class GtProcessData;
class GtTask;
class GtProjectProvider;
class GtPackage;
class GtLabelData;
class GtLabel;
class GtVersionNumber;

/**
 * @brief The GtProject class
 */
class GT_CORE_EXPORT GtProject : public GtObject
{
    Q_OBJECT

    friend class GtSession;
    friend class GtProjectProvider;
    friend class GtLoadProjectHelper;
    friend class GtSaveProjectHelper;
    friend class GtProjectUI;

public:
    /**
     * @brief ~GtProject
     */
    ~GtProject() override;

    /**
     * @brief Returns path to project directory on hard drive
     * @return path to project directory
     */
    const QString& path() const;

    /**
     * @brief Returns dataset storage location for given module.
     * @param moduleId Module identification string.
     * @return storage location of module specific dataset.
     */
    QString moduleDataPath(const QString& moduleId) const;

    /**
     * @brief Returns whether project data is successfully loaded or not
     * @return validation indicator
     */
    bool isValid() const;

    /**
     * @brief isOpen
     * @return
     */
    bool isOpen() const;

    /**
     * @brief Indicates whether upgrades are available for project data.
     * @return True if upgrades are available.
     */
    bool upgradesAvailable() const;

    /**
     * @brief processData
     * @return
     */
    GtProcessData* processData();
    GtProcessData const* processData() const;

    /**
     * @brief Returns project specific label data.
     * @return Label data.
     */
    GtLabelData* labelData();
    GtLabelData const* labelData() const;

    /**
     * @brief findProcess
     * @param val
     * @return
     */
    GtTask* findProcess(const QString& name);
    GtTask const* findProcess(const QString& name) const;

    /**
     * @brief Returns the names of all root tasks in the current task
     * @return String list of root task names
     */
    QStringList taskIds() const;

    /**
     * @brief Returns the names of all root tasks in the current task
     * @return String list of root task names
     */
    QMap<QString, QStringList> fullTaskIds() const;

    /**
     * @brief Returns the package specified by the module id
     * @param mid Module id
     * @return Package
     */
    GtPackage* findPackage(const QString& mid);

    /**
     * @brief moduleIds
     * @return
     */
    const QStringList& moduleIds() const;

    /**
     * @brief Returns number of label usages. Returns -1 if given label is
     * coruppted.
     * @param Label.
     * @return Number of label usages.
     */
    int numberOfLabelUsages(GtLabel* label) const;

    /**
     * @brief Returns all label usages.
     * @param Label.
     * @return Label usages.
     */
    QList<GtLabel*> findLabelUsages(GtLabel* label);

    /**
     * @brief toProjectDataMemento
     * @return
     */
    GtObjectMemento toProjectDataMemento();

    /**
     * @brief fromProjectDataMemento
     * @param memento
     * @return
     */
    bool fromProjectDataMemento(GtObjectMemento& memento);

    /**
     * @brief Renames project identification string within application and
     * project save file. If other project with same id exists, renaming fails.
     * @param str New projcet identification string
     * @return Whether project was renamed or not.
     */
    bool renameProject(const QString& str);

    /**
     * @brief Reads footprint out of project file.
     * @return Footprint
     */
    QString readFootprint() const;

    /**
     * @brief Returns user defined comment string.
     * @return User defined comment string.
     */
    QString comment() const;

    /**
     * @brief Sets user defined comment string.
     * @param comment New user defined comment string.
     */
    void setComment(const QString& comment);

    /**
     * @brief Returns whether project irregularity warnings are ignored for this
     * project.
     * @return True if irregularity warning are ignored.
     */
    bool ignoringIrregularities() const;

    /**
     * @brief Sets whether project irregularities should be ignored for this
     * project.
     * @param ignore
     */
    void setIgnoreIrregularities(bool ignore);

    /**
     * @brief sets whether externalized objects should be internalized on save
     * @param value true if data should be internalized on save
     */
    void setInternalizeOnSave(bool value);

    /**
     * @brief eturns the identification strings of all modules for which at
     * least one upgrade is available.
     * @return List of module identification strings
     */
    QStringList availableModuleUpgrades() const;

    /**
     * @brief Gives a list of version numbers of all available upgrades of a
     * specific module.
     * @param moduleId identifications string of the module for which upgrades
     * are to be checked.
     * @return List of version numbers of upgrad routines.
     */
    QList<GtVersionNumber> availableUpgrades(const QString& moduleId) const;

    /**
     * @brief Triggers upgrade routine of project data.
     */
    void upgradeProjectData();

    /**
     * @brief Generates a backup of all relevant project data. the backup is
     * stored in the project directory in a separate folder (/backup). A current
     * timestamp is used for identification.
     */
    void createBackup(const QString& message = {}) const;

    enum class RestoreStatus
    {
        Success = 0,
        ErrorNoBackupFound,
        ErrorCopyFailed
    };

    /**
     * @brief Restores the files of the project by the given backup
     *
     * The project must be closed before calling the function!
     *
     * The following errors can occur
     *  -
     *
     * @return true if successfull
     */
    RestoreStatus restoreBackupFiles(const QString& timestamp);

    struct ProjectMetaData
    {
        QString projectName;
    };

    /**
     * @brief Updates the project's metadata such as the project name
     * @param projectPath The path to the project
     * @param data
     * @return
     */
    static bool updateProjectMetaData(const QDir& projectPath,
                                      const ProjectMetaData& data);

    /**
     * @brief Returns filename of main GTlab project file
     * @return
     */
    static const QString mainFilename();

    /**
     * @brief Returns the file extension of main GTlab project file without a period
     * @return
     */
    static const QString mainFileExtension();

    /**
     * @brief Returns default extension of module files.
     * @return
     */
    static const QString moduleExtension();

protected:
    /**
     * @brief Constructor
     * @param path
     */
    explicit GtProject(const QString& path);

    /**
     * @brief Sets module identification strings loaded by application.
     * @param list module identification strings
     */
    void setModuleIds(const QStringList& list);

private:
    /// Path to project directory on hard drive
    QString m_path;

    /// True if project data was successfully loaded
    bool m_valid;

    /// True if project data upgrades are available
    bool m_upgradesAvailable{false};

    /// Whether to internalize all external data when saving
    bool m_internalizeOnSave{false};

    /// List of all project module ids
    QStringList m_moduleIds;

    /// Project path property. Only used for visualization in the GUI.
    GtStringProperty m_pathProp;

    /// User definable comment
    QString m_comment;

    /**
     * @brief loadMetaData
     * @return success
     */
    bool loadMetaData();

    /**
     * @brief Checks for available project data upgrades provided by framework
     * modules.
     * @return True if upgrades are available.
     */
    bool checkForUpgrades() const;

    /**
     * @brief readModuleMetaData
     * @param root element to start the read function from
     */
    void readModuleMetaData(const QDomElement& root);

    /**
     * @brief readProcessData
     * @return
     */
    GtObject* readProcessData();

    /**
     * @brief readLabelData
     * @return
     */
    GtObject* readLabelData(const GtObjectList& moduleData);

    /**
     * @brief readModuleData
     * @return
     */
    GtObjectList readModuleData();

    /**
     * @brief saveModuleData
     * @return success
     */
    bool saveModuleData();

    /**
     * @brief saveProjectOverallData
     * @return success
     */
    bool saveProjectOverallData();

    /**
     * @brief externalize all objects. If internalizeOnSave flag is set to true
     * all objects will be internalized instead.
     * @return success
     */
    bool saveExternalizedObjectData();

    /**
     * @brief saveModuleMetaData
     * @param root
     * @param doc
     * @return success
     */
    bool saveModuleMetaData(QDomElement& root, QDomDocument& doc);

    /**
     * @brief saveProcessData
     * @param root
     * @param doc
     * @return success
     */
    bool saveProcessData(QDomElement& root, QDomDocument& doc);

    /**
     * @brief saveLabelData
     * @param root
     * @param doc
     * @return success
     */
    bool saveLabelData(QDomElement& root, QDomDocument& doc);

    /**
     * @brief readProjectData
     * @return
     */
    static QDomDocument readProjectData(const QDir& projectPath);

    /**
     * @brief findLabelUsagesHelper
     * @param obj
     * @param label
     * @return
     */
    static QList<GtLabel*> findLabelUsagesHelper(GtObject* obj, GtLabel* label);

    /**
     * @brief renameOldModuleFile
     * @param path
     * @param modId
     */
    void renameOldModuleFile(const QString& path, const QString& modId);

    /**
     * @brief save project files
     * @param filePath : file path
     * @param doc : document
     * @return success
     */
    bool saveProjectFiles(const QString& filePath, const QDomDocument& doc);

    /**
     * @brief Updates module footprint version stored in project file.
     * New version matches the current module version of the framework.
     * @param modIds Module identification strings.
     */
    void updateModuleFootprint(const QStringList &modIds);

};

namespace gt {

namespace project {

/**
 * @brief backUpMessageFileName
 * @return name of the file to store the backup message to
 */
GT_CORE_EXPORT QString backUpMessageFileName();

/**
 * @brief backupDirPath
 * @param proj
 * @return
 */
GT_CORE_EXPORT QString backupDirPath(const GtProject& proj);

enum CopyProjectFlags
{
    ForceOverwrite = 1, /// Forces overwriting existing project files
    IncludeBackups = 2, /// Include Backups while copying
    IgnoreBackupMd = 4  /// Do not copy backup message file
};

GT_CORE_EXPORT
filesystem::CopyStatus copyProjectData(const QDir& srcPath,
                const QDir& targetPath,
                int copyProjectFlags);

} // namespace project

} // namespace gt


#endif // GTPROJECT_H
