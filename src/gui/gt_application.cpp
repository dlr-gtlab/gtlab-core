/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 07.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_application.h"

#include "gt_mdilauncher.h"
#include "gt_guimoduleloader.h"
#include "gt_perspective.h"
#include "gt_settings.h"
#include "gt_session.h"
#include "gt_propertychangecommand.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"
#include "gt_mementochangecommand.h"
#include "gt_processrunner.h"
#include "gt_processexecutor.h"

#include "gt_datamodel.h"
#include "gt_command.h"
#include "gt_applicationprivate.h"
#include "gt_logging.h"
#include "gt_icons.h"
#include "gt_shortcuts.h"
#include "gt_projectui.h"

#include <QMessageBox>
#include <QIcon>
#include <QApplication>
#include <QDir>
#include <QClipboard>
#include <QUndoStack>
#include <QUuid>
#include <QKeyEvent>
#include <QStandardPaths>
#include <QSettings>
#include <QProcess>


GtApplication::GtApplication(QCoreApplication* parent,
                             bool devMode,
                             AppMode batchMode) :
    GtCoreApplication(parent),
    m_guiModuleLoader(nullptr),
    m_d(new GtApplicationPrivate(this)),
    m_selectedObject(nullptr)
{
    gt::processExecutorManager().clearAllExecutors();
    gt::registerExecutorType<GtProcessExecutor>();
    gt::registerExecutorType<GtProcessRunner>();

    if (m_dataModel)
    {
        delete m_dataModel;
        m_dataModel = new GtDataModel(parent);
    }

    m_undoStack.setUndoLimit(50);

    connect(this, SIGNAL(objectSelected(GtObject*)),
            SLOT(onObjectSelected(GtObject*)));

    connect(&m_undoStack, SIGNAL(canRedoChanged(bool)),
            SLOT(onUndoStackChange()));

    m_devMode = devMode;
    m_appMode = batchMode;
    setDarkMode(settings()->darkMode());

    // apppend shortcuts object
    auto sc = new GtShortCuts(this);

    connect(sc, SIGNAL(changed()),
            this, SIGNAL(shortCutsChanged()));
}

GtApplication::~GtApplication()
{
    QApplication::clipboard()->clear();

    // remove temp directory
    QString tmpPath = QCoreApplication::applicationDirPath() +
                      QDir::separator() + QStringLiteral("temp");

    gtDebug().medium().quote() << "Deleting temp dir:" << tmpPath;

    QDir dir(tmpPath);
    dir.removeRecursively();
}

void
GtApplication::initMdiLauncher()
{
    gtMdiLauncher;
}

void
GtApplication::loadModules()
{
    if (!m_moduleLoader)
    {
        m_guiModuleLoader = new GtGuiModuleLoader;
        m_moduleLoader.reset(m_guiModuleLoader);
        m_moduleLoader->load();
    }
}

const QStringList&
GtApplication::perspectiveIds()
{
    readPerspectiveIds();

    return m_perspectiveIds;
}

QString
GtApplication::perspectiveId()
{
    if (m_perspective)
    {
        return m_perspective->objectName();
    }

    return QString();
}

void
GtApplication::initPerspective(const QString& id)
{
    if (!readPerspectiveIds())
    {
        if (!GtPerspective::createDefault())
        {
            gtWarning() << tr("Could not create default perspective setting!");
        }
    }

    if (!m_perspective)
    {
        // load perspective info

        if (id.isEmpty())
        {
            switchPerspective(settings()->lastPerspective());
        }
        else
        {
            switchPerspective(id);
        }
    }
    else
    {
        gtWarning() << tr("Perspectives have already been initialized!");
    }
}

void
GtApplication::switchPerspective(const QString& id)
{
    QString tmpId = QStringLiteral("default");

    if (!id.isEmpty())
    {
        if (!m_perspectiveIds.contains(id))
        {
            gtWarning() << tr("Perspective '%1' not found!").arg(id);
        }
        else
        {
            tmpId = id;
        }
    }

    emit perspectiveAboutToBeChanged();

    // save last used perspective and open new perspective
    m_perspective.reset(new GtPerspective(tmpId));

    gtDebug().medium() << tr("Loaded perspective:")
                       << m_perspective->objectName();
    settings()->setLastPerspective(tmpId);
    emit perspectiveChanged(tmpId);
}

bool
GtApplication::deletePerspective(const QString& id)
{
    if (!m_perspectiveIds.contains(id))
    {
        gtWarning() << tr("Perspective '%1' not found!").arg(id);
        return false;
    }

    if (perspectiveId() == id)
    {
        gtWarning() << tr("Current perspective cannot be deleted!");
        return false;
    }

    if (id == QLatin1String("default"))
    {
        gtWarning() << tr("Default perspective cannot be deleted!");
        return false;
    }

    QDir path(roamingPath());

    if (!path.exists() || !path.cd(QStringLiteral("perspective")))
    {
        gtWarning() << tr("Perspective directory not found!");
        return false;
    }

    QString filename1 = path.absoluteFilePath(id + QStringLiteral(".geom"));
    QString filename2 = path.absoluteFilePath(id + QStringLiteral(".state"));

    QFile file1(filename1);

    if (!file1.remove())
    {
        gtWarning() << tr("Could not delete perspective geometry file!");
        return false;
    }

    QFile file2(filename2);

    if (!file2.remove())
    {
        gtWarning() << tr("Could not delete perspective state file!");
        return false;
    }

    readPerspectiveIds();

    emit perspectiveListChanged();

    return true;
}

bool
GtApplication::renamePerspective(const QString& oldId, const QString& newId)
{
    if (!perspectiveIds().contains(oldId))
    {
        gtWarning() << tr("Perspective id not found!");
        return false;
    }

    if (perspectiveIds().contains(newId))
    {
        gtWarning() << tr("Perspective id already exists!");
        return false;
    }

    if (perspectiveId() == oldId)
    {
        gtWarning() << tr("Current perspective cannot be renamed!");
        return false;
    }

    if (oldId == QLatin1String("default"))
    {
        gtWarning() << tr("Default perspective cannot be renamed!");
        return false;
    }

    QDir path(roamingPath());

    if (!path.exists() || !path.cd(QStringLiteral("perspective")))
    {
        gtWarning() << tr("Perspective directory not found!");
        return false;
    }

    QString filenameOld1 = oldId + QStringLiteral(".geom");
    QString filenameNew1 = newId + QStringLiteral(".geom");

    QString filenameOld2 = oldId + QStringLiteral(".state");
    QString filenameNew2 = newId + QStringLiteral(".state");

    if (!QFile::rename(path.absoluteFilePath(filenameOld1),
        path.absoluteFilePath(filenameNew1)))
    {
        gtWarning() << tr("Could not rename perspective geometry file!");
    }

    if (!QFile::rename(path.absoluteFilePath(filenameOld2),
        path.absoluteFilePath(filenameNew2)))
    {
        gtWarning() << tr("Could not rename perspective state file!");
    }

    // refresh perspective ids
    readPerspectiveIds();

    emit perspectiveListChanged();

    return true;
}

bool
GtApplication::newPerspective(const QString& id)
{
    if (m_perspectiveIds.contains(id))
    {
        gtWarning() << tr("Perspective id already exists!");
        return false;
    }

    if (!GtPerspective::createEmptyPerspective(id))
    {
        gtWarning() << tr("Could not create perspective!");
        return false;
    }

    readPerspectiveIds();

    emit perspectiveListChanged();

    return true;
}

bool
GtApplication::duplicatePerspective(const QString& source,
                                    const QString& target)
{
    if (!m_perspectiveIds.contains(source))
    {
        gtWarning() << tr("Perspective '%1' not found!").arg(source);
        return false;
    }

    return GtPerspective::duplicatePerspective(source, target);
}

void
GtApplication::savePerspectiveData(const QByteArray& geometry,
                                   const QByteArray& state)
{
    if (m_perspective)
    {
        m_perspective->saveGeometry(geometry);
        m_perspective->saveState(state);
    }
}

QPair<QByteArray, QByteArray>
GtApplication::loadPerspectiveData()
{
    if (m_perspective)
    {
        return QPair<QByteArray, QByteArray>(m_perspective->loadGeometry(),
                                             m_perspective->loadState());
    }

    return QPair<QByteArray, QByteArray>();
}

void
GtApplication::initShortCuts()
{
    /// Short cuts from settings
    QList<GtShortCutSettingsData> tab = settings()->shortcutsList();

    /// Short cuts from default list
    QList<GtShortCutSettingsData> listBasic = settings()->intialShortCutsList();

    /// if short cut in default list, but not in settings add it to settings
    for (GtShortCutSettingsData const& k : qAsConst(listBasic))
    {
        bool contains = std::any_of(std::begin(tab),
                                    std::end(tab),
                                    [&k](const GtShortCutSettingsData& k2) {
            return (k.id == k2.id);
        });

        if (contains == false)
        {
            tab.append(k);
        }
    }

    settings()->setShortcutsTable(tab);

    /// initialize the short cut objects of the application
    shortCuts()->initialize(tab);
}

QList<GtObjectUI*>
GtApplication::objectUI(GtObject* obj)
{
    return m_guiModuleLoader->objectUI(obj);
}

QList<GtObjectUI*>
GtApplication::objectUI(const QString& classname)
{
    return m_guiModuleLoader->objectUI(classname);
}

GtObjectUI*
GtApplication::defaultObjectUI(GtObject* obj)
{
    QList<GtObjectUI*> ouis = gtApp->objectUI(obj);

    if (!ouis.isEmpty())
    {
        return ouis.first();
    }

    return nullptr;
}

GtObjectUI*
GtApplication::defaultObjectUI(const QString& classname)
{
    QList<GtObjectUI*> ouis = gtApp->objectUI(classname);

    if (!ouis.isEmpty())
    {
        return ouis.first();
    }

    return nullptr;
}

QStringList
GtApplication::knownUIObjects()
{
    return m_guiModuleLoader->knownUIObjects();
}

void
GtApplication::switchSession(const QString& id)
{
    if (gt::currentProcessExecutor().taskCurrentlyRunning())
    {
        QMessageBox mb;
        mb.setIcon(QMessageBox::Information);
        mb.setWindowTitle(tr("Switch Session"));
        mb.setWindowIcon(gt::gui::icon::session());
        mb.setText(tr("Cannot switch session while a task is running."));
        mb.setStandardButtons(QMessageBox::Ok);
        mb.setDefaultButton(QMessageBox::Ok);
        mb.exec();
        return;
    }

    if (!GtProjectUI::saveAndCloseCurrentProject())
    {
        return;
    }


    GtCoreApplication::switchSession(id);

    // TODO: check assert !!!!
    //    Q_ASSERT(m_session == nullptr);

    m_undoStack.clear();
}

QUndoStack*
GtApplication::undoStack()
{
    return &m_undoStack;
}

void
GtApplication::propertyCommand(GtObject* obj,
                               GtAbstractProperty* prop,
                               const QVariant& newValue,
                               const QString& unit,
                               GtObject* root)
{
    auto* cmd = new GtPropertyChangeCommand(obj, prop, newValue, unit, root);
    undoStack()->push(cmd);
}

GtCommand
GtApplication::startCommand(GtObject* root, const QString& commandId)
{
    QMutexLocker locker(&m_commandMutex);

    if (!root)
    {
        gtWarning().medium() << tr("root object == NULL!");
        return GtCommand();
    }

    if (commandId.isEmpty())
    {
        gtWarning().medium() << tr("cannot start command with empty id!");
        return GtCommand();
    }

    if (!m_d->m_commandId.isEmpty())
    {
        gtDebug().medium() << tr("already recording command")
                           << QStringLiteral("...");
        gtDebug().medium() << QStringLiteral("    |-> ") << m_d->m_commandId;
        gtDebug().medium() << QStringLiteral("    |-> vs") << commandId;
        return GtCommand();
    }

    m_d->m_commandMemento = GtObjectMemento(root);
    m_d->m_commandRoot = root;
    m_d->m_commandId = commandId;
    m_d->m_commandUuid = QUuid::createUuid().toString();

    gtDebug().verbose() << "######## COMMAND STARTED!"
                        << gt::brackets(m_d->m_commandId);

    gtDataModel->beginResetModelView();

    return generateCommand(m_d->m_commandUuid);
}

void
GtApplication::endCommand(const GtCommand& command)
{
    QMutexLocker locker(&m_commandMutex);

    if (m_d->m_commandUuid.isEmpty())
    {
        gtWarning().medium() << tr("command uuid is empty!");
        return;
    }

    if (!command.isValid())
    {
        gtDebug().medium() << tr("command is invalid!");
        return;
    }

    if (m_d->m_commandUuid != command.id())
    {
        gtWarning().medium() << tr("wrong command uuid!");
        return;
    }

    if (!m_d->m_commandRoot)
    {
        gtWarning().medium() << tr("invlid command root!");
        return;
    }

    GtObjectMemento newMemento = m_d->m_commandRoot->toMemento();

    GtObjectMementoDiff diff(m_d->m_commandMemento, newMemento);

    gtDebug().verbose() << "######## COMMAND END! (" << m_d->m_commandId << ")";

    auto* root =  m_d->m_commandRoot->findRoot<GtSession*>();
    if (!root)
    {
        gtWarning().medium() << tr("no root object found!");
        return;
    }

    auto* changeCmd = new GtMementoChangeCommand(diff, m_d->m_commandId, root);
    undoStack()->push(changeCmd);

    // cleanup
    m_d->m_commandRoot = nullptr;
    m_d->m_commandId = QString();

    gtDataModel->endResetModelView();
}

bool
GtApplication::commandIsRunning()
{
    QMutexLocker locker(&m_commandMutex);

    if (m_d->m_commandRoot)
    {
        return true;
    }

    return false;
}

void
GtApplication::loadingProcedure(GtAbstractLoadingHelper* helper)
{
    if (!helper)
    {
        return;
    }

    emit loadingProcedureRun(helper);
}

GtObject*
GtApplication::selectedObject()
{
    return m_selectedObject;
}

QKeySequence
GtApplication::getShortCutSequence(const QString& id,
                                   const QString& category) const
{
    GtShortCuts* s = shortCuts();

    if (!s)
    {
        gtWarning() << tr("No shortcut registered for") << id
                    << tr("(empty list of shotcuts)");
        return {};
    }

    return s->getKey(id, category);
}

bool
GtApplication::compareKeyEvent(QKeyEvent* keyEvent,
                               const QString& id,
                               const QString& category) const
{
    GtShortCuts* s = shortCuts();

    if (!s)
    {
        gtError() << tr("Short cuts list not found");
        return false;
    }

    if (s->isEmpty())
    {
        gtError() << tr("No shortcut registrations found");
        return false;
    }

    QKeySequence k = s->getKey(id, category);

    return compareKeyEvent(keyEvent, k);
}


bool
GtApplication::compareKeyEvent(QKeyEvent* keyEvent,
                               const QKeySequence& k) const
{
    /// shortcut may be empty/not set OR
    /// a key sequence may contain multiple alternatives to use as short-cut
    /// but for a correct comparison only one can be compared
    if (k.isEmpty() || k.count() == 0)
    {
        return false;
    }

    return k[0] == (keyEvent->key() | static_cast<int>(keyEvent->modifiers()));
}

GtShortCuts*
GtApplication::shortCuts() const
{
    return findChild<GtShortCuts*>();
}

void
GtApplication::extendShortCuts(const QList<GtShortCutSettingsData>& list)
{
    for (auto const& shortcut : list)
    {
        extendShortCuts(shortcut);
    }
}

void
GtApplication::extendShortCuts(const GtShortCutSettingsData& shortcut)
{
    GtShortCuts* sList = shortCuts();
    if (!sList)
    {
        gtWarning() << tr("Failed to register additional shortcut '%1'!")
                       .arg(shortcut.id);
        return;
    }

    bool isRegistered = std::any_of(std::cbegin(m_moduleShortCuts),
                                    std::cend(m_moduleShortCuts),
                                    [&](GtShortCutSettingsData const& data){
        return data.id == shortcut.id && data.category == shortcut.category;
    });

    if (isRegistered)
    {
        gtWarning() << tr("Skipping duplicate shortcut '%1'!")
                       .arg(shortcut.id);
        return;
    }

    m_moduleShortCuts.append(shortcut);

    sList->initialize(shortcut);
}


QList<GtShortCutSettingsData>
GtApplication::moduleShortCuts() const
{
    return m_moduleShortCuts;
}

void
GtApplication::showPreferences(const QString& title)
{
    emit preferencesDialogRequested(title);
}

QList<GtApplication::PageFactory>&
customPages()
{
    static QList<GtApplication::PageFactory> pages;
    return pages;
}

const QList<GtApplication::PageFactory> &
GtApplication::customPreferencePages()
{
    return customPages();
}

void
GtApplication::addCustomPreferencePage(PageFactory f)
{
    customPages().append(std::move(f));
}

bool
GtApplication::inDarkMode()
{
    return m_darkMode;
}

void
GtApplication::setDarkMode(bool dark)
{
    static bool initial = true;

    if (!initial && m_darkMode != dark)
    {
        gtInfo() << tr("The Theme was changed.")
                 << tr("It is recommended to restart the application as some "
                       "color or theme changes may not be applied correctly.");
    }

    m_darkMode = dark;


    initial = false;
    emit themeChanged(dark);
}

bool
GtApplication::readPerspectiveIds()
{
    m_perspectiveIds.clear();

    QDir path(roamingPath());

    if (path.exists() && path.cd("perspective"))
    {
        QStringList entries = path.entryList(QStringList() <<
                                             QStringLiteral("*.geom"));

        foreach (QString entry, entries)
        {
            m_perspectiveIds.append(entry.replace(QStringLiteral(".geom"),
                                                  QStringLiteral("")));
        }

        m_perspectiveIds.removeDuplicates();

        return true;
    }

    return false;
}

bool
GtApplication::initFirstRun()
{
    auto oldVersion = gt::GT_1_7;
    auto newVersion = gt::GT_2_0;

    // check existence of old roaming path directory
    auto oldConfigPath = gt::settingsConfig(oldVersion, *this).path;
    auto configPath = gt::settingsConfig(newVersion, *this).path;

    if (oldConfigPath != configPath &&
        QDir(oldConfigPath).exists() &&
       !QDir(configPath).exists())
    {
        auto button = QMessageBox::question(nullptr, tr("Migrate configuration"),
                              tr("GTlab found a configuration of an older GTlab"
                                 " version. Do you want to migrate the data?"));


        if (button == QMessageBox::Yes)
        {
            migrateConfigData(oldVersion, newVersion);

            gtInfo() << tr("Data migration done, restarting GTlab");
            auto args = qApp->arguments();
            args.removeFirst();
            QProcess::startDetached(QApplication::applicationFilePath(), args);
            exit(0);
        }
    }

    GtCoreApplication::initFirstRun();

    // create application directories
    QString path = roamingPath() + QDir::separator() + "perspective";

    if (!QDir().mkpath(path))
    {
        gtWarning() << tr("Could not create application directories!");
        return false;
    }

    // create default perspective
    if (!GtPerspective::createDefault())
    {
        gtWarning() << tr("Could not create default perspective setting!");
        return false;
    }

    return true;
}

void
GtApplication::onPropertyChange(GtObject* /*obj*/, GtAbstractProperty* /*prop*/)
{
    //    gtDebug() << "property change!";
    //    gtDebug() << "  |-> " << obj->objectName();
    //    gtDebug() << "  |-> " << prop->objectName();

    //    m_undoStack.push(new GtPropertyChangeCommand(obj, prop));
}

void
GtApplication::onObjectSelected(GtObject* obj)
{
    m_selectedObject = obj;
}

void
GtApplication::onUndoStackChange()
{
    //gtDebug() << "on undo stack change!";
}

void
GtApplication::onGuiInitializationFinished()
{
    initModules();

    // update theme
    emit themeChanged(m_darkMode);
}

