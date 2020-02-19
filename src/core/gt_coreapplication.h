/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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

#define gtApp (GtCoreApplication::instance())

class QCoreApplication;
class QTranslator;
class QDir;

class GtSession;
class GtProject;
class GtSettings;
class GtModuleLoader;
class GtDataModel;
class GtObject;
class GtCoreDatamodel;
class GtAbstractLoadingHelper;
class GtCommand;

/**
 * @brief The GtApplication class
 */
class GT_CORE_EXPORT GtCoreApplication : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief GtApplication
     * @param parent
     */
    GtCoreApplication(QCoreApplication* parent);

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
     * @brief sessionIds
     * @return
     */
    const QStringList& sessionIds();

    /**
     * @brief ~GtApplication
     */
    virtual ~GtCoreApplication();

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
    int moduleVersion(const QString& id);

    /**
     * @brief Returns description of module for given id. Returns empty
     * string for non existing modules.
     * @param module identification string
     * @return module description
     */
    QString moduleDescription(const QString& id);

    /**
     * @brief Returns true if project has unsaved changes. Otherwise
     * false is returned.
     * @return Whether project has unsaved changes.
     */
    bool hasProjectChanges();

    /**
     * @brief Returns the major release number of the application.
     * @return major release number
     */
    static int majorRelease();

    /**
     * @brief Returns the minor release number of the application.
     * @return minor release number
     */
    static int minorRelease();

    /**
     * @brief Returns the patch level of the application.
     * @return patch level
     */
    static int patchLevel();

    /**
     * @brief Returns version string including major, minor and patch version.
     * @return Version string.
     */
    static QString versionToString();

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
     * @brief startCommand
     * @param root
     * @param commandId
     * @return
     */
    virtual GtCommand startCommand(GtObject* root, const QString& commandId);

    /**
     * @brief endCommand
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

protected:
    /// Current session
    GtSession* m_session;

    /// Settings
    GtSettings* m_settings;

    /// List of user specific session ids
    QStringList m_sessionIds;

    /// Translator
    QTranslator* m_translator;

    /// Module loader
    GtModuleLoader* m_moduleLoader;

    /// Development run indicator
    bool m_devMode;

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
     * @brief generateCommand
     * @param uuid
     * @return
     */
    GtCommand generateCommand(const QString& uuid = QString());

private:
    ///
    static GtCoreApplication* m_self;

    /// Major application version
    static int m_major;

    /// Minor application version
    static int m_minor;

    /// Application patch level
    static int m_patchLevel;

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