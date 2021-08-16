/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 07.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QApplication>
#include <QDir>
#include <QClipboard>
#include <QUndoStack>
#include <QUuid>
#include <QDebug>
#include <QKeyEvent>

#include "gt_mdilauncher.h"
#include "gt_guimoduleloader.h"
#include "gt_perspective.h"
#include "gt_settings.h"
#include "gt_session.h"
#include "gt_propertychangecommand.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"
#include "gt_mementochangecommand.h"
#include "gt_processexecutor.h"
#include "gt_datamodel.h"
#include "gt_command.h"
#include "gt_applicationprivate.h"
#include "gt_project.h"
#include "gt_logging.h"
#include "gt_saveprojectmessagebox.h"
#include "gt_shortcuts.h"

#include "gt_application.h"

GtApplication::GtApplication(QCoreApplication* parent, bool devMode) :
    GtCoreApplication(parent),
    m_perspective(Q_NULLPTR),
    m_guiModuleLoader(Q_NULLPTR),
    m_d(new GtApplicationPrivate(this)),
    m_selectedObject(Q_NULLPTR)
{
    // init process executor in gui mode
    m_processExecutor = new GtProcessExecutor(this);

    if (m_dataModel != Q_NULLPTR)
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

    GtShortCuts* cuts = new GtShortCuts(this);
}

GtApplication::~GtApplication()
{
    // cleanup
    if (m_perspective != Q_NULLPTR)
    {
        delete m_perspective;
    }

    QApplication::clipboard()->clear();

    // remove temp directory
    gtDebug() << "deleting temp dir...";
    QString tmpPath = QCoreApplication::applicationDirPath() + QDir::separator()
                      + QStringLiteral("temp");

    QDir dir(tmpPath);
    dir.removeRecursively();
}

QIcon
GtApplication::icon(const QString iconName)
{
    QIcon icon;
    icon.addFile(QString(":/icons/" + iconName), QSize(), QIcon::Normal,
                 QIcon::Off);
    return icon;
}

void
GtApplication::initMdiLauncher()
{
    //    qDebug() << "GtApplication::initMdiLauncher";
    gtMdiLauncher;
}

void
GtApplication::loadModules()
{
    //    qDebug() << "GtApplication::loadModules";
    if (m_moduleLoader == Q_NULLPTR)
    {
        m_guiModuleLoader = new GtGuiModuleLoader;
        m_moduleLoader = m_guiModuleLoader;
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
            qWarning() << tr("WARNING") << ": "
                       << tr("could not create default perspective setting!");
        }
    }

    if (m_perspective == Q_NULLPTR)
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
        qWarning() << tr("WARNING") << ": " <<
                   tr("perspective already initialized!");
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
            qWarning() << tr("WARNING") << ": " <<
                       tr("perspective id not found!");
        }
        else
        {
            tmpId = id;
        }
    }

    emit perspectiveAboutToBeChanged();

    // save last used perspective
    if (m_perspective)
    {
        delete m_perspective;
    }

    // open new perspective
    m_perspective = new GtPerspective(tmpId);

    qDebug() << tr("loaded perspective: ") << m_perspective->objectName();
    settings()->setLastPerspective(tmpId);
    emit perspectiveChanged(tmpId);
}

bool
GtApplication::deletePerspective(const QString& id)
{
    if (!m_perspectiveIds.contains(id))
    {
        gtWarning() << tr("WARNING") << QStringLiteral(": ") <<
                    tr("Perspective id not found!");
        return false;
    }

    if (perspectiveId() == id)
    {
        gtWarning() << tr("WARNING") << QStringLiteral(": ") <<
                    tr("Current perspective cannot be deleted!");
        return false;
    }

    if (id == QLatin1String("default"))
    {
        gtWarning() << tr("WARNING") << QStringLiteral(": ") <<
                    tr("Default perspective cannot be deleted!");
        return false;
    }

    QDir path(roamingPath());

    if (!path.exists() || !path.cd(QStringLiteral("perspective")))
    {
        gtWarning() << tr("WARNING") << QStringLiteral(": ") <<
                    tr("Perspective directory not found!");
        return false;
    }

    QString filename1 = path.absoluteFilePath(id + QStringLiteral(".geom"));
    QString filename2 = path.absoluteFilePath(id + QStringLiteral(".state"));

    QFile file1(filename1);

    if (!file1.remove())
    {
        gtWarning() << tr("WARNING") << QStringLiteral(": ") <<
                    tr("Could not delete perspective geometry file!");
        return false;
    }

    QFile file2(filename2);

    if (!file2.remove())
    {
        gtWarning() << tr("WARNING") << QStringLiteral(": ") <<
                    tr("Could not delete perspective state file!");
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
        gtWarning() << tr("WARNING") << QStringLiteral(": ") <<
                    tr("Perspective id not found!");
        return false;
    }

    if (perspectiveIds().contains(newId))
    {
        gtWarning() << tr("WARNING") << QStringLiteral(": ") <<
                    tr("Perspective id already exists!");
        return false;
    }

    if (perspectiveId() == oldId)
    {
        gtWarning() << tr("WARNING") << QStringLiteral(": ") <<
                    tr("Current perspective cannot be renamed!");
        return false;
    }

    if (oldId == QLatin1String("default"))
    {
        gtWarning() << tr("WARNING") << QStringLiteral(": ") <<
                    tr("Default perspective cannot be renamed!");
        return false;
    }

    QDir path(roamingPath());

    if (!path.exists() || !path.cd(QStringLiteral("perspective")))
    {
        gtWarning() << tr("WARNING") << QStringLiteral(": ") <<
                    tr("Perspective directory not found!");
        return false;
    }

    QString filenameOld1 = oldId + QStringLiteral(".geom");
    QString filenameNew1 = newId + QStringLiteral(".geom");

    QString filenameOld2 = oldId + QStringLiteral(".state");
    QString filenameNew2 = newId + QStringLiteral(".state");

    if (!QFile::rename(path.absoluteFilePath(filenameOld1),
        path.absoluteFilePath(filenameNew1)))
    {
        gtWarning() << tr("WARNING") << QStringLiteral(": ") <<
                    tr("Could not rename perspective geometry file!");
    }

    if (!QFile::rename(path.absoluteFilePath(filenameOld2),
        path.absoluteFilePath(filenameNew2)))
    {
        gtWarning() << tr("WARNING") << QStringLiteral(": ") <<
                    tr("Could not rename perspective state file!");
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
        gtWarning() << tr("WARNING") << QStringLiteral(": ") <<
                    tr("Perspective id already exists!");
        return false;
    }

    if (!GtPerspective::createEmptyPerspective(id))
    {
        gtWarning() << tr("WARNING") << QStringLiteral(": ") <<
                    tr("Could not create perspective!");
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
        qWarning() << tr("WARNING") << ": " <<
                   tr("Perspective id not found!");
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
    QMap<QString, QStringList> tab = settings()->shortcutsTable();

    /// Short cuts from default list
    QMap<QString, QStringList> tabBasic = settings()->intialShortCutsMap();

    /// if short cut in default list, but not in settings add it to settings
    for (QString k : tabBasic.keys())
    {
        if (!tab.keys().contains(k))
        {
            tab.insert(k, tabBasic.value(k));
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

    return Q_NULLPTR;
}

GtObjectUI*
GtApplication::defaultObjectUI(const QString& classname)
{
    QList<GtObjectUI*> ouis = gtApp->objectUI(classname);

    if (!ouis.isEmpty())
    {
        return ouis.first();
    }

    return Q_NULLPTR;
}

QStringList
GtApplication::knownUIObjects()
{
    return m_guiModuleLoader->knownUIObjects();
}

void
GtApplication::switchSession(const QString& id)
{
    if (gtProcessExecutor->taskCurrentlyRunning())
    {
        QMessageBox mb;
        mb.setIcon(QMessageBox::Information);
        mb.setWindowTitle(tr("Switch Session"));
        mb.setWindowIcon(gtApp->icon("sessionIcon_16.png"));
        mb.setText(tr("Cannot switch session while a task is running."));
        mb.setStandardButtons(QMessageBox::Ok);
        mb.setDefaultButton(QMessageBox::Ok);
        mb.exec();
        return;
    }

    if (gtApp->hasProjectChanges())
    {
        QString text = tr("Found changes in current project.\n"
                          "Do you want to save all your changes "
                          "before switching session?");

        GtSaveProjectMessageBox mb(text);
        int ret = mb.exec();

        switch (ret)
        {
            case QMessageBox::Yes:
            {
                gtDataModel->saveProject(gtApp->currentProject());
                break;
            }

            case QMessageBox::No:
            {
                break;
            }

            case QMessageBox::Cancel:
            {
                return;
                break;
            }

            default:
                break;
        }
    }


    GtCoreApplication::switchSession(id);

    // TODO: check assert !!!!
    //    Q_ASSERT(m_session == Q_NULLPTR);

    m_undoStack.clear();
}

QUndoStack*
GtApplication::undoStack()
{
    return &m_undoStack;
}

void
GtApplication::propertyCommand(GtObject* obj, GtAbstractProperty* prop,
                               const QVariant& newValue, const QString& unit,
                               GtObject* root)
{
    GtPropertyChangeCommand* command = new GtPropertyChangeCommand(obj, prop,
            newValue,
            unit, root);
    undoStack()->push(command);
}

GtCommand
GtApplication::startCommand(GtObject* root, const QString& commandId)
{
    m_commandMutex.lock();

    if (root == Q_NULLPTR)
    {
        m_commandMutex.unlock();
        qDebug() << tr("root object == NULL!");
        return GtCommand();
    }

    if (commandId.isEmpty())
    {
        m_commandMutex.unlock();
        qDebug() << tr("cannot start comamnd with empty id!");
        return GtCommand();
    }

    if (!m_d->m_commandId.isEmpty())
    {
        m_commandMutex.unlock();
        gtDebug() << tr("already recording command") << QStringLiteral("...");
        gtDebug() << QStringLiteral("    |-> ") << m_d->m_commandId;
        return GtCommand();
    }

    m_d->m_commandMemento = GtObjectMemento(root);
    m_d->m_commandRoot = root;
    m_d->m_commandId = commandId;
    m_d->m_commandUuid = QUuid::createUuid().toString();

    m_commandMutex.unlock();

    qDebug() << "######## COMMAND STARTED! (" << m_d->m_commandId << ")";

    return generateCommand(m_d->m_commandUuid);
}

void
GtApplication::endCommand(const GtCommand& command)
{
    m_commandMutex.lock();

    if (m_d->m_commandUuid.isEmpty())
    {
        m_commandMutex.unlock();
        qDebug() << tr("command uuid is empty!");
        return;
    }

    if (!command.isValid())
    {
        m_commandMutex.unlock();
        qDebug() << tr("command is invlid!");
        return;
    }

    if (m_d->m_commandUuid != command.id())
    {
        m_commandMutex.unlock();
        qDebug() << tr("wrong command uuid!");
        return;
    }

    if (m_d->m_commandRoot == Q_NULLPTR)
    {
        m_commandMutex.unlock();
        qDebug() << tr("invlid command root!");
        return;
    }

    GtObjectMemento newMemento = gtApp->currentProject()->toMemento();

    GtObjectMementoDiff diff(m_d->m_commandMemento, newMemento);

    qDebug() << "######## COMMAND END! (" << m_d->m_commandId << ")";

    GtSession* root =  m_d->m_commandRoot->findRoot<GtSession*>();

    if (root == Q_NULLPTR)
    {
        m_commandMutex.unlock();
        qDebug() << tr("no root object found!");
        return;
    }

    GtMementoChangeCommand* changeCommand =
        new GtMementoChangeCommand(diff, m_d->m_commandId, root);
    undoStack()->push(changeCommand);

    //    // cleanup
    m_d->m_commandRoot = Q_NULLPTR;
    m_d->m_commandId = QString();

    m_commandMutex.unlock();
}

bool
GtApplication::commandIsRunning()
{
    m_commandMutex.lock();

    if (m_d->m_commandRoot != Q_NULLPTR)
    {
        m_commandMutex.unlock();
        return true;
    }

    m_commandMutex.unlock();
    return false;
}

void
GtApplication::loadingProcedure(GtAbstractLoadingHelper* helper)
{
    if (helper == Q_NULLPTR)
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
GtApplication::getShortCutSequence(const QString& id) const
{
    GtShortCuts* s = shortCuts();

    if (s == nullptr)
    {
        return QKeySequence();
    }

    return s->getKey(id);
}

bool
GtApplication::compareKeyEvent(QKeyEvent* keyEvent, const QString& id) const
{
    GtShortCuts* s = shortCuts();

    if (s == nullptr)
    {
        gtError() << tr("Short cuts list not found");
        return false;
    }

    if (s->isEmpty())
    {
        gtError() << tr("No shortcut registrations found");
        return false;
    }

    QKeySequence k = s->getKey(id);

    if (k.isEmpty())
    {
        gtError() << tr("No ShortCut registered for ") << id;
        return false;
    }

    /// a key sequence may contain multiple alternatives to use as short-cut
    /// but for a correct comparison only one can be compared
    if (k.count() != 1)
    {
        return false;
    }

    return k[0] == (keyEvent->key() | keyEvent->modifiers());
}

GtShortCuts*
GtApplication::shortCuts() const
{
    return findChild<GtShortCuts*>();
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
    GtCoreApplication::initFirstRun();
    qDebug() << "gui first run!";

    // create application directories
    QString path = roamingPath() + QDir::separator() + "perspective";

    if (!QDir().mkpath(path))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not create application directories!");
        return false;
    }

    // create default perspective
    if (!GtPerspective::createDefault())
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not create default perspective setting!");
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
}

