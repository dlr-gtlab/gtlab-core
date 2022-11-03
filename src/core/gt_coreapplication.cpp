/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 28.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>
#include <QTranslator>
#include <QProcess>
#include <QUuid>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QtGlobal>
#include <QDebug>

#include "gt_coreapplication.h"
#include "gt_objectfactory.h"
#include "gt_processfactory.h"
#include "gt_session.h"
#include "gt_settings.h"
#include "gt_coremoduleloader.h"
#include "gt_logging.h"
#include "gt_logmodel.h"
#include "gt_coredatamodel.h"
#include "gt_project.h"
#include "gt_abstractloadinghelper.h"
#include "gt_environment.h"
#include "gt_command.h"
#include "gt_versionnumber.h"
#include "gt_globals.h"
#include "gt_algorithms.h"
#include "gt_moduleinterface.h"

#include <gt_logdest.h>

GtCoreApplication* GtCoreApplication::m_self = nullptr;
int GtCoreApplication::m_major = GT_VERSION_MAJOR;
int GtCoreApplication::m_minor = GT_VERSION_MINOR;
int GtCoreApplication::m_patchLevel = GT_VERSION_PATCH;
std::string GtCoreApplication::m_additional = GT_VERSION_ADDITIONAL;

GtCoreApplication::GtCoreApplication(QCoreApplication* parent, AppMode batch) :
    QObject(parent),
    m_settings(new GtSettings),
    m_sessionIds(QStringList() << QStringLiteral("default")),
    m_devMode(false),
    m_appMode(batch),
    m_dataModel(new GtCoreDatamodel(parent))
{
    /// set locale to international standard for unix OS
#if defined(Q_OS_UNIX)
    setlocale(LC_ALL, "C");
#endif
    ///


    //    qDebug() << "GtCoreApplication";
    //    init();
}

GtCoreApplication::~GtCoreApplication()
{
    // clear self ptr -> no dangling ptr
    m_self = nullptr;
}

QString
GtCoreApplication::roamingPath()
{
#ifdef _WIN32
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#else
    return QStandardPaths::writableLocation(
                QStandardPaths::GenericConfigLocation);
#endif
}

QStringList
GtCoreApplication::crashedModules()
{
    QString iniFileName = gtApp->roamingPath() + QDir::separator() +
                          QStringLiteral("last_run.ini");
    QSettings settings(iniFileName, QSettings::IniFormat);

    return settings.value(QStringLiteral("loading_crashed")).toStringList();
}

bool
GtCoreApplication::readSessionIds()
{
    m_sessionIds.clear();

    QDir path(roamingPath());

    if (path.exists() && path.cd(QStringLiteral("session")))
    {
        QStringList entries = path.entryList(QStringList()
                                             << QStringLiteral("*.json"));

        foreach (QString entry, entries)
        {
            m_sessionIds.append(entry.replace(QStringLiteral(".json"),
                                              QStringLiteral("")));
        }

        m_sessionIds.removeDuplicates();

        return true;
    }

    return false;
}

void
GtCoreApplication::init()
{
    if (m_self)
    {
        return;
    }

    m_self = this;

    // TODO: delete after alpha Version
    QString wPath = roamingPath() + QDir::separator() +
                    QStringLiteral("workspace");
    QString sPath = roamingPath() + QDir::separator() +
                    QStringLiteral("session");

    QDir path(wPath);

    if (path.exists())
    {
        bool s = path.rename(wPath, sPath);

        qDebug() << "workspace renamed = " << s;
    }

    // #####

    if (settings()->firstApplicationRun())
    {
        initFirstRun();
    }

    if (qApp->arguments().contains(QStringLiteral("-dev")))
    {
        m_devMode = true;
    }

    // logger
    //    qDebug() << "initializing logger...";
    gt::log::Logger& logger = gt::log::Logger::instance();

    gt::log::DestinationPtr debugDestination(
        gt::log::DestinationFactory::MakeDebugOutputDestination());
    logger.addDestination(debugDestination);

    gtLogModel;

    if (m_devMode)
    {
        logger.setLoggingLevel(gt::log::TraceLevel);
        gtDebug() << "DEV MODE";
    }

    gtEnvironment->setRoamingDir(roamingPath());
    gtEnvironment->loadEnvironment();
}

bool
GtCoreApplication::initFirstRun()
{
    qDebug() << "first application run!";

    // restore application settings to initial values
    settings()->restoreSettings();

    // create application directories
    QString path = GtSession::roamingPath();

    if (!QDir().mkpath(path))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not create application directories!");
        return false;
    }

    // create default session
    if (!GtSession::createDefault())
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not create default session setting!");
        return false;
    }

    return true;
}

bool
GtCoreApplication::loadLastSession()
{
    QString lastSession = settings()->lastSession();

    if (m_sessionIds.contains(lastSession))
    {
        switchSession(lastSession);
    }
    else
    {
        qWarning() << tr("WARNING: ") <<
                   tr("could not find last session");

        return false;
    }

    return true;
}

void
GtCoreApplication::initDatamodel()
{
    gtObjectFactory;
}

void
GtCoreApplication::initCalculators()
{
    gtProcessFactory;
}

void
GtCoreApplication::initLanguage()
{
    QString language = settings()->language();

    if (language.isEmpty())
    {
        setToSystemLanguage();
    }
    else
    {
        setLanguage(language);
    }
}

void
GtCoreApplication::initSession(const QString& id)
{
    if (!m_session)
    {
        // load session info
        if (!readSessionIds())
        {
            qWarning() << tr("WARNING: ") <<
                       tr("could not read session information");
        }
        else
        {
            if (id.isEmpty())
            {
                if (settings()->openLastSession())
                {
                    loadLastSession();
                }
                else
                {
                    switchSession(QStringLiteral("default"));
                }
            }
            else
            {
                switchSession(id);
            }
        }
    }
    else
    {
        qWarning() << tr("WARNING") << ": " <<
                   tr("session already initialized!");
    }
}

void
GtCoreApplication::switchSession(const QString& id)
{
    if (!m_sessionIds.contains(id))
    {
        gtWarning() << tr("WARNING") << ": " << tr("session id not found!");
    }

    // save last used session
    if (m_session)
    {
        gtApp->settings()->setLastProject(QStringLiteral(""));
    }

    // open new session
    m_session.reset(new GtSession(id));

    if (!m_session->isValid())
    {
        m_session.reset();
        qCritical() << "ERROR: could not load session!";
    }
    else
    {
        settings()->setLastSession(id);
        m_dataModel->setSession(m_session.get());
        emit sessionChanged(id);
    }

    switchCurrentProject();
}

bool
GtCoreApplication::renameSession(const QString& oldId, const QString& newId)
{
    if (!sessionIds().contains(oldId))
    {
        qWarning() << tr("WARNING") << ": " <<
                   tr("Session id not found!");
        return false;
    }

    if (sessionIds().contains(newId))
    {
        qWarning() << tr("WARNING") << ": " <<
                   tr("Session id already exists!");
        return false;
    }

    if (sessionId() == oldId)
    {
        qWarning() << tr("WARNING") << ": " <<
                   tr("Current session cannot be renamed!");
        return false;
    }

    if (oldId == QLatin1String("default"))
    {
        qWarning() << tr("WARNING") << ": " <<
                   tr("Default session cannot be renamed!");
        return false;
    }

    QDir path(roamingPath());

    if (!path.exists() || !path.cd(QStringLiteral("session")))
    {
        qWarning() << tr("WARNING") << ": " <<
                   tr("Session directory not found!");
        return false;
    }

    QString filenameOld = oldId + QStringLiteral(".json");
    QString filenameNew = newId + QStringLiteral(".json");

    if (!QFile::rename(path.absoluteFilePath(filenameOld),
                       path.absoluteFilePath(filenameNew)))
    {
        qWarning() << tr("WARNING") << ": " <<
                   tr("Could not rename session file!");
    }

    // refresh session ids
    readSessionIds();

    emit sessionListChanged();

    return true;
}

bool
GtCoreApplication::newSession(const QString& id)
{
    if (m_sessionIds.contains(id))
    {
        qWarning() << tr("WARNING") << ": " <<
                   tr("Session id already exists!");
        return false;
    }

    if (!GtSession::createEmptySession(id))
    {
        qWarning() << tr("WARNING") << ": " <<
                   tr("Could not create session!");
        return false;
    }

    readSessionIds();

    emit sessionListChanged();

    return true;
}

bool
GtCoreApplication::duplicateSession(const QString& source,
                                    const QString& target)
{
    if (!m_sessionIds.contains(source))
    {
        qWarning() << tr("WARNING") << ": " <<
                   tr("Session id not found!");
        return false;
    }

    return GtSession::duplicateSession(source, target);
}

bool
GtCoreApplication::deleteSession(const QString& id)
{
    if (!m_sessionIds.contains(id))
    {
        qWarning() << tr("WARNING") << ": " <<
                   tr("Session id not found!");
        return false;
    }

    if (sessionId() == id)
    {
        qWarning() << tr("WARNING") << ": " <<
                   tr("Current session cannot be deleted!");
        return false;
    }

    if (id == QLatin1String("default"))
    {
        qWarning() << tr("WARNING") << ": " <<
                   tr("Default session cannot be deleted!");
        return false;
    }

    QDir path(roamingPath());

    if (!path.exists() || !path.cd(QStringLiteral("session")))
    {
        qWarning() << tr("WARNING") << ": " <<
                   tr("Session directory not found!");
        return false;
    }

    QString filename = path.absoluteFilePath(id + QStringLiteral(".json"));

    QFile file(filename);

    if (!file.remove())
    {
        qWarning() << tr("WARNING") << ": " <<
                   tr("Could not delete session file!");
        return false;
    }

    readSessionIds();

    emit sessionListChanged();

    return true;
}

void
GtCoreApplication::loadModules()
{
    //    qDebug() << "GtCoreApplication::loadModules";
    if (!m_moduleLoader)
    {
        m_moduleLoader = std::make_unique<GtCoreModuleLoader>();
        m_moduleLoader->load();
    }
}

QStringList
GtCoreApplication::moduleIds()
{
    if (!m_moduleLoader)
    {
        return QStringList();
    }

    return m_moduleLoader->moduleIds();
}

QString
GtCoreApplication::modulePackageId(const QString& id)
{
    if (!m_moduleLoader)
    {
        return QString();
    }

    return m_moduleLoader->modulePackageId(id);
}

QStringList
GtCoreApplication::moduleDatamodelInterfaceIds()
{
    if (!m_moduleLoader)
    {
        return QStringList();
    }

    return m_moduleLoader->moduleDatamodelInterfaceIds();
}

GtVersionNumber
GtCoreApplication::moduleVersion(const QString& id) const
{
    if (!m_moduleLoader)
    {
        return GtVersionNumber();
    }

    return m_moduleLoader->moduleVersion(id);
}

QString
GtCoreApplication::moduleDescription(const QString& id) const
{
    if (!m_moduleLoader)
    {
        return QString();
    }

    return m_moduleLoader->moduleDescription(id);
}

QString
GtCoreApplication::moduleAuthor(const QString& id) const
{
    if (!m_moduleLoader)
    {
        return QString();
    }

    return m_moduleLoader->moduleAuthor(id);
}

QString
GtCoreApplication::moduleContact(const QString& id) const
{
    if (!m_moduleLoader)
    {
        return QString();
    }

    return m_moduleLoader->moduleContact(id);
}

QString
GtCoreApplication::moduleLicence(const QString& id) const
{
    if (!m_moduleLoader)
    {
        return QString();
    }

    return m_moduleLoader->moduleLicence(id);
}

bool
GtCoreApplication::hasProjectChanges()
{
    if (!currentProject())
    {
        return false;
    }

    if (currentProject()->hasChanges() || currentProject()->hasChildChanged())
    {
        return true;
    }

    return false;
}

int
GtCoreApplication::majorRelease()
{
    return m_major;
}

int
GtCoreApplication::minorRelease()
{
    return m_minor;
}

int
GtCoreApplication::patchLevel()
{
    return m_patchLevel;
}

std::string
GtCoreApplication::additionalVersionInfo()
{
    return m_additional;
}

QString
GtCoreApplication::versionToString()
{
    return GtCoreApplication::version().toString();
}

GtVersionNumber
GtCoreApplication::version()
{
    return GtVersionNumber(GtCoreApplication::majorRelease(),
                           GtCoreApplication::minorRelease(),
                           GtCoreApplication::patchLevel(),
                           QString::fromStdString(m_additional));
}

QDir
GtCoreApplication::applicationTempDir()
{
    QDir retval(QCoreApplication::applicationDirPath() + QDir::separator()
                + QStringLiteral("temp"));

    if (!retval.exists())
    {
        retval = QDir(QCoreApplication::applicationDirPath());

        if (!retval.mkpath(retval.absolutePath() + QDir::separator()
                           + QStringLiteral("temp")))
        {
            gtWarning() << tr("Could not create temporary directory!");
            return QDir();
        }

        if (!retval.cd(QStringLiteral("temp")))
        {
            gtWarning() << tr("Could not create temporary directory!");
            return QDir();
        }
    }

    return GtCoreApplication::createTempDir(retval.absolutePath());
}

QDir
GtCoreApplication::createTempDir(const QString& path)
{
    if (path.isEmpty())
    {
        gtWarning() << tr("Could not create temporary directory!")
                    << QStringLiteral(" - ")
                    << tr("Target path is empty");
        return QDir();
    }

    QString uuid = QUuid::createUuid().toString();

    QDir retval(path);

    if (!retval.exists())
    {
        gtWarning() << tr("Could not create temporary directory!")
                    << QStringLiteral(" - ")
                    << tr("Target path does not exists!");
        return QDir();
    }

    if (!retval.mkpath(retval.absolutePath() + QDir::separator() + uuid))
    {
        gtWarning() << tr("Could not create temporary directory!");
        return QDir();
    }

    if (!retval.cd(uuid))
    {
        gtWarning() << tr("Could not create temporary directory!");
        return QDir();
    }

    return retval;
}

bool
GtCoreApplication::removeTempDir(const QString& path)
{
    QDir dir(path);

    if (!dir.exists())
    {
        gtWarning() << tr("Could not remove temporary directory!")
                    << QStringLiteral(" - ")
                    << tr("Target path does not exists!");
        return false;
    }

    const QString uuid = dir.dirName();

    if (uuid.contains(QStringLiteral("{")) &&
            uuid.contains(QStringLiteral("}")))
    {
        qDebug() << "removing " << path << "...";
        return dir.removeRecursively();
    }

    return false;
}

GtCommand
GtCoreApplication::startCommand(GtObject* /*root*/,
                                const QString& /*commandId*/)
{
    gtError() << tr("Redo/Undo cannot be used in batch mode!");
    return GtCommand();
}

void
GtCoreApplication::endCommand(const GtCommand& /*command*/)
{
    gtError() << tr("Redo/Undo cannot be used in batch mode!");
}

void
GtCoreApplication::loadingProcedure(GtAbstractLoadingHelper* helper)
{
    if (!helper)
    {
        return;
    }

    helper->exec();
}

bool
GtCoreApplication::checkLicence()
{
    qDebug() << QSslSocket::sslLibraryBuildVersionString();

    QUrl url(QStringLiteral("https://www.gtlab.de/lic/lic.txt"));

    QEventLoop eventLoop;

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            &eventLoop, SLOT(quit()));

    QNetworkReply* reply = manager->head(QNetworkRequest(url));

    eventLoop.exec();

    return (reply->error() == QNetworkReply::NoError);
}

void
GtCoreApplication::initModules()
{
    if (m_moduleLoader)
    {
        m_moduleLoader->initModules();
    }
}

void
GtCoreApplication::saveSystemEnvironment() const
{
     QMap<QString, QString> modEnv = GtModuleLoader::moduleEnvironmentVars();

     gt::for_each_key(modEnv, [](const QString& e)
     {
         gtDebug() << "sys env var (" << e << ") = " << gtEnvironment->value(e);

         const QByteArray sysEnvVar = gtEnvironment->value(e).toByteArray();
         qputenv(e.toUtf8().constData(), sysEnvVar);
     });
}

void
GtCoreApplication::addSuppression(const GtModuleInterface& suppressor,
    const QString& suppressedModuleId)
{
    assert(m_moduleLoader);
    m_moduleLoader->addSuppression(suppressor.ident(), suppressedModuleId);
}

bool
GtCoreApplication::setLanguage(const QString& id)
{
    if (!m_translator)
    {
        m_translator = std::make_unique<QTranslator>();
    }

    if (id == QLatin1String("de"))
    {
        if (!m_translator->load(QStringLiteral("gtlab_de")))
        {
            qWarning() << "WARNING: " << tr("could not load language!") <<
                       " (" << id << ")";
            return false;
        }

        qApp->installTranslator(m_translator.get());
    }
    else
    {
        qWarning() << "WARNING: " << tr("unknown language!") << " (" << id
                   << ")";
        return false;
    }

    qDebug() << tr("language package = ") << id;
    return true;
}

bool
GtCoreApplication::setToSystemLanguage()
{
    if (!m_translator)
    {
        m_translator = std::make_unique<QTranslator>();
    }

    bool s = m_translator->load(QLocale::system(), QStringLiteral("gtlab"),
                                QStringLiteral("_"));

    if (s)
    {
        qApp->installTranslator(m_translator.get());
        return true;
    }

    return false;
}

GtCommand
GtCoreApplication::generateCommand(const QString& uuid)
{
    return GtCommand(uuid);
}

bool
GtCoreApplication::devMode()
{
    return m_devMode;
}

void
GtCoreApplication::setDevMode(bool val)
{
    m_devMode = val;
}

bool
GtCoreApplication::batchMode()
{
    return m_appMode == AppMode::Batch;
}

const QStringList&
GtCoreApplication::sessionIds()
{
    readSessionIds();

    return m_sessionIds;
}

GtCoreApplication*
GtCoreApplication::instance()
{
    return m_self;
    //    static GtCoreApplication* retval = 0;
    //    if (m_self == 0)
    //    {
    //        m_self = new GtCoreApplication(qApp);
    //        m_self->init();
    //    }
    //    return m_self;
}

GtSession*
GtCoreApplication::session()
{
    return m_session.get();
}

QString
GtCoreApplication::sessionId()
{
    GtSession* w = session();

    if (w)
    {
        return w->objectName();
    }

    return QString();
}

GtSettings*
GtCoreApplication::settings()
{
    return m_settings.get();
}

GtProject*
GtCoreApplication::currentProject()
{
    GtSession* w = session();

    if (w)
    {
        return w->currentProject();
    }

    return nullptr;
}

GtProject*
GtCoreApplication::findProject(const QString& id)
{
    GtSession* w = session();

    if (w)
    {
        return w->findProject(id);
    }

    return nullptr;
}

bool
GtCoreApplication::setCurrentProject(GtProject* project)
{
    GtSession* w = session();

    if (w)
    {
        if (currentProject() == project)
        {
            return false;
        }

        if (w->setCurrentProject(project))
        {
            emit currentProjectChanged(project);
            return true;
        }
    }

    return false;
}

void
GtCoreApplication::switchCurrentProject()
{
    GtSession* w = session();

    if (w)
    {
        m_session->switchCurrentProject();
        emit currentProjectChanged(m_session->currentProject());
    }
    else
    {
        emit currentProjectChanged(nullptr);
    }
}
