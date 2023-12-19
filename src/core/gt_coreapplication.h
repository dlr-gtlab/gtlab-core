/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 28.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCOREAPPLICATION_H
#define GTCOREAPPLICATION_H

#include "gt_core_exports.h"

#include <QObject>
#include <QStringList>
#include <QMap>

#include <memory>

#include "gt_globals.h"
#include "gt_platform.h"
#include "gt_finally.h"
#include "gt_command.h"

#define gtApp (GtCoreApplication::instance())

class QCoreApplication;
class QTranslator;
class QDir;
class QSettings;

class GtSession;
class GtProject;
class GtSettings;
class GtAbstractProperty;
class GtCoreModuleLoader;
class GtDataModel;
class GtObject;
class GtCoreDatamodel;
class GtAbstractLoadingHelper;
class GtCommand;
class GtVersionNumber;
class GtModuleInterface;
class GtCoreApplication;

namespace gt
{

    struct SettingsConfig
    {
        QString path; /// Directory path of the settings
        std::unique_ptr<QSettings> settings; /// registry path
    };

    enum SettingsVersions
    {
        GT_1_7,
        GT_2_0
    };

    /**
 * @brief Returns the settings configuration for a specified version
 */
    GT_CORE_EXPORT
    SettingsConfig settingsConfig(SettingsVersions version,
                   const GtCoreApplication& app);

} // namespace gt

/**
 * @brief The GtApplication class
 */
class GT_CORE_EXPORT GtCoreApplication : public QObject
{
    Q_OBJECT

public:
    /// Enumeration to handle the application mode of the application
    enum class AppMode
    {
        Batch = 0,
        Gui
    };

    /// Helper functor object to end a GtCommand
    struct EndCommandFunctor
    {
        inline void operator()() const noexcept
        {
            gtApp->endCommand(cmd);
        }
        GtCommand cmd;
    };

    using SmartCommand = gt::Finally<EndCommandFunctor>;

    /**
     * @brief GtApplication
     * @param parent
     * @param mode - indicator if application is started as batch application
     */
    explicit GtCoreApplication(QCoreApplication* parent,
                               AppMode mode = AppMode::Gui);

    /**
     * @brief ~GtApplication
     */
    ~GtCoreApplication() override;

    /**
     * @brief instance
     * @return
     */
    static GtCoreApplication* instance();


    /**
     * @brief Initialization
     */
    void init();

    /**
     * @brief session
     * @return
     */
    GtSession* session();

    /**
     * @brief Returns current session id. Empty if no session is set.
     * @return Session identification string
     */
    QString sessionId();

    /**
     * @brief settings
     * @return
     */
    GtSettings* settings();

    /**
     * @brief Returns pointer to current project. NULL if no current project
     * selected.
     * @return Pointer to current project
     */
    GtProject* currentProject();

    /**
     * @brief findProject
     * @param id
     * @return
     */
    GtProject* findProject(const QString& id);

    /**
     * @brief setCurrentProject
     * @param project
     * @return
     */
    bool setCurrentProject(GtProject* project);

    /**
     * @brief switchCurrentProject
     */
    void switchCurrentProject();

    /**
     * @brief roamingPath
     * @return
     */
    static QString roamingPath();

    /**
     * @brief Returns list of all crashed module identification strings.
     * @return List of all crashed module identification strings.
     */
    static QStringList crashedModules();

    /**
     * @brief Returns the file path to the local application ini file
     * @return file path to local ini file
     */
    static QString const& localApplicationIniFilePath();

    /**
     * @brief setLanguage
     * @param id
     */
    bool setLanguage(const QString& id);

    /**
     * @brief Returns whether application runs in dev mode.
     * @return dev indicator
     */
    bool devMode();

    /**
     * @brief setDevMode to the given val to define
     * whether application runs in dev mode.
     * @param val to set dev indicator
     */
    void setDevMode(bool val);

    /**
     * @brief Returns whether application runs in batch mode.
     * @return batchMode indicator
     */
    bool batchMode();

    /**
     * @brief sessionIds
     * @return
     */
    const QStringList& sessionIds();

    /**
     * @brief initDatamodel
     */
    void initDatamodel();

    /**
     * @brief initCalculators
     */
    void initCalculators();

    /**
     * @brief initLanguage
     */
    void initLanguage();

    /**
     * @brief initSession
     * @param id
     */
    void initSession(const QString& id = QString());

    /**
     * @brief switchSession
     * @param id
     */
    virtual void switchSession(const QString& id);

    /**
     * @brief propertyCommand - virtual helper method for creating an undo
     * command for a property change event in graphical mode.
     * \warning Undo commands are not supported in batch mode!
     * @param obj
     * @param prop
     * @param newValue
     * @param unit
     * @param root
     */
    virtual void propertyCommand(GtObject* /*obj*/,
                                 GtAbstractProperty* /*prop*/,
                                 const QVariant& /*newValue*/,
                                 const QString& /*unit*/ = QString(),
                                 GtObject* /*root*/ = nullptr) {}

    /**
     * @brief renameSession
     * @param oldId
     * @param newId
     * @return
     */
    bool renameSession(const QString& oldId, const QString& newId);

    /**
     * @brief newSession
     * @param id
     * @return
     */
    bool newSession(const QString& id);

    /**
     * @brief duplicateSession
     * @param source
     * @param target
     * @return
     */
    bool duplicateSession(const QString& source, const QString& target);

    /**
     * @brief Deletes workpsace with given id
     * @param id
     * @return
     */
    bool deleteSession(const QString& id);

    /**
     * @brief loadModules
     */
    virtual void loadModules();

    /**
     * @brief Loads a single module and all its dependencies
     *
     * The module might be located outside of the modules directory.
     * The given module overrides any module in the module directory
     * with the same moduleId.
     *
     * @param moduleFilePath The filename/path of the module.
     * @return True, if loading was successful
     */
    bool loadSingleModule(const QString& moduleFilePath);

    /**
     * @brief Returns identification strings of all loaded modules.
     * @return module identification strings
     */
    QStringList moduleIds();

    /**
     * @brief Returns classname of package for a given module id. Returns empty
     * string for non existing modules.
     * @param module identification string
     * @return package classname
     */
    QString modulePackageId(const QString& id);

    /**
     * @brief Returns identification strings of all loaded modules which
     * inherit the datamodel interface.
     * @return module identification strings
     */
    QStringList moduleDatamodelInterfaceIds();

    /**
     * @brief Returns version number of module for given id. Returns -1
     * for non existing modules.
     * @param module identification string
     * @return module version number
     */
    GtVersionNumber moduleVersion(const QString& id) const;

    /**
     * @brief Returns description of module for given id. Returns empty
     * string for non existing modules.
     * @param module identification string
     * @return module description
     */
    QString moduleDescription(const QString& id) const;

    /**
     * @brief Returns name of modules author for given id. Returns empty
     * string for non existing modules.
     * @param module identification string
     * @return module author name
     */
    QString moduleAuthor(const QString& id) const;

    /**
     * @brief Returns contact of modules author for given id. Returns empty
     * string for non existing modules.
     * @param module identification string
     * @return module author contact
     */
    QString moduleContact(const QString& id) const;

    /**
     * @brief Returns licence information of module for given id. Returns empty
     * string for non existing modules.
     * @param module identification string
     * @return module licence information
     */
    QString moduleLicence(const QString& id) const;
    /**
     * @brief Returns true if project has unsaved changes. Otherwise
     * false is returned.
     * @return Whether project has unsaved changes.
     */
    bool hasProjectChanges();

    /**
     * @brief Returns version information including major, minor and patch
     * version.
     * @return Version information.
     */
    static GtVersionNumber version();

    /**
     * @brief Returns global temporary directory of the application.
     * @return
     */
    static QDir applicationTempDir();

    /**
     * @brief Creates temp directory within given path.
     * @param path
     * @return
     */
    static QDir createTempDir(const QString& path);

    /**
     * @brief Removes temporary directory within given path.
     * @param path
     * @return
     */
    static bool removeTempDir(const QString& path);

    /**
     * @brief Records an undo/redo command. Any changes to the datamodel will
     * be tracked using mementos. Once the returned object goes out of scope
     * the command will automatically be ended.
     * @param root Root object to create memento diffs from
     * @param commandId Name of command, should be in a readable format, so that
     * the user can identify the undo/redo command as such
     * @return Command helper object.
     */
    GT_NO_DISCARD
    static SmartCommand makeCommand(GtObject* root, QString const& commandId);

    /**
     * @brief Begins recording of an undo/redo command. Any changes to the
     * datamodel will be tracked using mementos.
     *
     * Note: When starting a command always make sure to end the same command!
     * Prefer "makeCommand" instead.
     *
     * @param root Root object to create memento diffs from
     * @param commandId Name of command, should be in a readable format, so that
     * the user can identify the undo/redo command as such
     * @return Command
     */
    GT_NO_DISCARD
    virtual GtCommand startCommand(GtObject* root, const QString& commandId);

    /**
     * @brief Ends the command.
     * @param command Command to end
     */
    virtual void endCommand(const GtCommand& command);

    /**
     * @brief loadingProcedure
     * @param helper
     */
    virtual void loadingProcedure(GtAbstractLoadingHelper* helper);

    /**
     * @brief checkLicence
     * @return
     */
    bool checkLicence();

    /**
     * @brief Initializes all loaded modules.
     */
    void initModules();

    /**
     * @brief Saves module environemnt variables temporary in the system
     * environment.
     */
    void saveSystemEnvironment() const;

    /**
     * @brief The function allows to suppress the loading of a module from
     *        another module (the suppressor).
     *
     * The typical use case is a conditional loading of module, e.g. only if
     * certain requirements are met before another module can be loaded.
     *
     * The function needs to be called during the `GtModuleInterface::onLoad`
     * method of the suppressor module. To be successful, the suppressor module
     * needs to be loaded before the suppressed module (i.e. the suppressor
     * module is a dependency of the suppressed module) and the suppressed
     * module needs to allow suppression.
     *
     * @param suppressor Suppressor module.
     * @param supressedModuleId Module id of the module to be suppressed.
     */
    void addSuppression(const GtModuleInterface& suppressor,
        const QString& suppressedModuleId);

    /**
     * @brief Copies GTlab config files from src to target
     */
    void migrateConfigData(gt::SettingsVersions srcVer,
                           gt::SettingsVersions targetVer);

    /**
     * @brief licenseFolder
     * @return path of the folder for licenses of GTlab, its dependencies
     * and its modules
     */
    QString licenseFolder() const;
protected:
    /// Current session
    std::unique_ptr<GtSession> m_session;

    /// Settings
    std::unique_ptr<GtSettings> m_settings;

    /// List of user specific session ids
    QStringList m_sessionIds;

    /// Translator
    std::unique_ptr<QTranslator> m_translator;

    /// Module loader
    std::unique_ptr<GtCoreModuleLoader> m_moduleLoader;

    /// Development run indicator
    bool m_devMode;

    /// Application is started in batch mode
    AppMode m_appMode;

    /// Date model pointer
    GtCoreDatamodel* m_dataModel;

    /**
     * @brief readSessionIds
     * @return
     */
    bool readSessionIds();

    /**
     * @brief initFirstRun
     */
    virtual bool initFirstRun();

    /**
     * @brief inits the logging system
     */
    void initLogging();

    /**
     * @brief loadLastSession
     * @return
     */
    bool loadLastSession();

    /**
     * @brief setToSystemLanguage
     * @return
     */
    bool setToSystemLanguage();

    /**
     * @brief Generates command based on command identification string and
     * root object (scope) for change detection.
     * @param uuid command identification string (usually an uuid)
     */
    GtCommand generateCommand(const QString& uuid = QString());

private:
    ///
    static GtCoreApplication* m_self;

    /// application version
    static GtVersionNumber m_version;

signals:
    /**
     * @brief sessionChanged
     */
    void sessionChanged(QString);

    /**
     * @brief sessionListChanged
     */
    void sessionListChanged();

    /**
     * @brief currentProjectChanged
     */
    void currentProjectChanged(GtProject*);
};

#endif // GTCOREAPPLICATION_H
