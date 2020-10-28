/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QSignalMapper>
#include <QFileDialog>
#include <QKeyEvent>
#include <QUndoView>
#include <QMdiSubWindow>
#include <QDesktopServices>
#include <QProcess>
#include <QThread>
#include <QTimer>
#include <QDebug>

#include "gt_mainwin.h"
#include "ui_gt_mainwin.h"
#include "gt_application.h"
#include "gt_preferencesdialog.h"
#include "gt_mdilauncher.h"
#include "gt_mementoviewer.h"
#include "gt_dockwidget.h"
#include "gt_projectwizard.h"
#include "gt_projectprovider.h"
#include "gt_datamodel.h"
#include "gt_project.h"
#include "gt_filedialog.h"
#include "gt_perspectiveswitchwidget.h"
#include "gt_aboutdialog.h"
#include "gt_changelogdialog.h"
#include "gt_logging.h"
#include "gt_projectui.h"
#include "gt_collectioneditor.h"
#include "gt_startuppage.h"
#include "gt_settings.h"
#include "gt_checkforupdatesdialog.h"
#include "gt_cornerwidget.h"
#include "gt_updatechecker.h"
#include "gt_simpleloadingwidget.h"
#include "gt_abstractloadinghelper.h"
#include "gt_processexecutor.h"
#include "gt_processqueuemodel.h"
#include "gt_processqueuewidget.h"
#include "gt_saveprojectmessagebox.h"

GtMainWin::GtMainWin(QWidget* parent) : QMainWindow(parent),
    ui(new Ui::GtMainWin),
    m_switchSessionMapper(Q_NULLPTR),
    m_switchPerspectiveMapper(Q_NULLPTR),
    m_cornerWidget(new GtCornerWidget(this)),
    m_forceQuit(false),
    m_firstTimeShowEvent(true),
    m_processQueue(Q_NULLPTR)
{
    // dock widget have to be initialized before setup the ui
    setupDockWidgets();

    ui->setupUi(this);

    ui->mdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);

    // hide some stuff
    ui->menuUpdate_available->menuAction()->setVisible(false);

    // disable some stuff
    //    ui->actionHelpContents->setEnabled(false);

    // dev mode initialization
    if (gtApp->devMode())
    {
    }
    else
    {
        ui->menuDev->menuAction()->setVisible(false);

        ui->menuTools->menuAction()->setVisible(false);

        ui->actionBug_Report->setVisible(false);

        ui->actionDuplicate_Project->setVisible(false);

//        ui->actionHelpContents->setEnabled(false);

        //ui->actionExamples->setVisible(false);
    }

    // session initialization
    updateSessionList();

    // perspective initialization;
    updatePerspectiveList();

    // collection list initialization
    updateCollectionEntries();

    // mdi launcher initialization
    gtMdiLauncher->setMdiArea(ui->mdiArea);

    // undo action
    QAction* undoAct = gtApp->undoStack()->createUndoAction(ui->undoBar,
                       tr("Undo"));
    undoAct->setIcon(gtApp->icon(QStringLiteral("undoIcon_24.png")));
    undoAct->setText(tr("Undo"));
    ui->undoBar->addAction(undoAct);
    undoAct ->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));

    // redo action
    QAction* redoAct = gtApp->undoStack()->createRedoAction(ui->undoBar,
                       tr("Redo"));
    redoAct->setIcon(gtApp->icon(QStringLiteral("redoIcon_24.png")));
    redoAct->setText(tr("Redo"));
    ui->undoBar->addAction(redoAct);
    redoAct ->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));

    // edit menu initialization
    ui->menuEdit->addAction(undoAct);
    ui->menuEdit->addAction(redoAct);
    ui->menuEdit->addAction(ui->actionUndoHistory);
    ui->menuEdit->addSeparator();
    ui->menuEdit->addAction(ui->actionProcessQueue);
    ui->menuEdit->addAction(ui->actionPreferences);

    // corner widget
    ui->menubar->setCornerWidget(m_cornerWidget);

    // connections
    connect(ui->actionNewProject, SIGNAL(triggered(bool)),
            SLOT(showProjectWizard()));
    connect(ui->actionSave_Project, SIGNAL(triggered(bool)),
            SLOT(saveCurrentProject()));
    connect(ui->actionSaveProject, SIGNAL(triggered(bool)),
            SLOT(saveAllProjects()));
    connect(ui->actionSave_As, SIGNAL(triggered(bool)),
            SLOT(saveCurrentProjectAs()));
    connect(ui->actionDuplicate_Project, SIGNAL(triggered(bool)),
            SLOT(duplicateCurrentProject()));
    connect(ui->actionImportProject, SIGNAL(triggered(bool)),
            SLOT(importProject()));
    connect(ui->actionPreferences, SIGNAL(triggered(bool)),
            SLOT(showPreferences()));
    connect(ui->actionPerformance_Map_Editor, SIGNAL(triggered(bool)),
            SLOT(openMapEditor()));
    connect(ui->actionPreDesignPlotTest, SIGNAL(triggered(bool)),
            SLOT(openPreDesignPlot()));
    connect(ui->actionPerformanceModelEditor, SIGNAL(triggered(bool)),
            SLOT(openModelEditor()));
    connect(ui->actionEditSessions, SIGNAL(triggered(bool)),
            SLOT(showSessionPreferences()));
    connect(ui->actionEditPerspectives, SIGNAL(triggered(bool)),
            SLOT(showPerspectivePreferences()));
    connect(ui->actionMemento_Viewer, SIGNAL(triggered(bool)),
            SLOT(openMementoViewer()));
    connect(ui->actionCarpet_Plot_Test, SIGNAL(triggered(bool)),
            SLOT(openCarpetPlotTest()));
    connect(ui->actionExamples, SIGNAL(triggered(bool)),
            SLOT(openExamplesWidget()));
    connect(ui->actionTemplateTest, SIGNAL(triggered(bool)),
            SLOT(openTemplateViewerTest()));
    connect(ui->actionUndoHistory, SIGNAL(triggered(bool)),
            SLOT(openCommandHistory()));
    connect(ui->actionProcessQueue, SIGNAL(triggered(bool)),
            SLOT(openProcessQueue()));
    connect(ui->actionPrint, SIGNAL(triggered(bool)),
            SLOT(printCurrentMdiItem()));
    connect(ui->actionSessionBrowser, SIGNAL(triggered(bool)),
            SLOT(openSessionViewer()));
    connect(ui->actionState_Browser, SIGNAL(triggered(bool)),
            SLOT(openStateViewer()));
    connect(ui->actionBug_Report, SIGNAL(triggered(bool)),
            SLOT(runBugReport()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), SLOT(openAboutDialog()));
    connect(ui->actionChangelog, SIGNAL(triggered(bool)),
            SLOT(openChangelogDialog()));
    connect(ui->actionHelpContents, SIGNAL(triggered(bool)),
            SLOT(openHelpContents()));
    connect(gtApp, SIGNAL(sessionChanged(QString)),
            SLOT(updateSessionList()));
    connect(gtApp, SIGNAL(sessionListChanged()), SLOT(updateSessionList()));
    connect(gtApp, SIGNAL(perspectiveChanged(QString)),
            SLOT(loadPerspectiveSettings(QString)));
    connect(gtApp, SIGNAL(perspectiveChanged(QString)),
            SLOT(updatePerspectiveList()));
    connect(gtApp, SIGNAL(perspectiveListChanged()),
            SLOT(updatePerspectiveList()));
    connect(gtApp, SIGNAL(perspectiveAboutToBeChanged()),
            SLOT(savePerspectiveSettings()), Qt::DirectConnection);
    connect(gtApp, SIGNAL(currentProjectChanged(GtProject*)),
            SLOT(onCurrentProjectChanged(GtProject*)));
    connect(gtApp, SIGNAL(currentProjectChanged(GtProject*)),
            SLOT(updateWindowTitle()));
    connect(gtApp, SIGNAL(loadingProcedureRun(GtAbstractLoadingHelper*)),
            SLOT(runLoadingProcedure(GtAbstractLoadingHelper*)),
            Qt::DirectConnection);
    connect(gtApp, SIGNAL(objectSelected(GtObject*)),
            SLOT(onObjectSelected(GtObject*)));
    connect(ui->actionCascadeWindows, SIGNAL(triggered()), ui->mdiArea,
            SLOT(cascadeSubWindows()), Qt::UniqueConnection);
    connect(ui->actionTileWindows, SIGNAL(triggered()), ui->mdiArea,
            SLOT(tileSubWindows()), Qt::UniqueConnection);
    connect(ui->actionCheck_for_update, SIGNAL(triggered()),
            SLOT(openCheckForUpdatesDialog()));
    connect(ui->actionInstall_Update, SIGNAL(triggered(bool)),
            SLOT(runUpdate()));
    connect(ui->actionOpen_Project, SIGNAL(triggered(bool)),
            SLOT(openProject()));
    connect(ui->actionCloseProject, SIGNAL(triggered(bool)),
            SLOT(closeProject()));
    connect(ui->actionWidgetStructure, SIGNAL(triggered(bool)),
            SLOT(onWidgetStructureClicked()));

    loadPerspectiveSettings();
}

GtMainWin::~GtMainWin()
{
    delete ui;
}

void
GtMainWin::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);

    QTimer::singleShot(0, this, SLOT(initAfterStartup()));
}

void
GtMainWin::closeEvent(QCloseEvent* event)
{
    if (!m_forceQuit)
    {
        /// A process is running
        if (gtProcessExecutor->currentRunningTask() != Q_NULLPTR)
        {
            QMessageBox mb;
            mb.setIcon(QMessageBox::Question);
            mb.setWindowTitle(tr("Confirm Exit"));
            mb.setWindowIcon(gtApp->icon("closeIcon_16.png"));
            mb.setText(tr("Attention: \n"
                          "You try to exit GTlab while  a process is running \n"
                          "Are you sure quit?"));
            mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            mb.setDefaultButton(QMessageBox::No);
            int ret = mb.exec();

            switch (ret)
            {
                case QMessageBox::Yes:
                {
                    break;
                }

                case QMessageBox::No:
                {
                    event->ignore();
                    return;
                }

                default:
                    break;
            }
        }
        /// There is unsaved data
        else if (gtApp->hasProjectChanges())
        {
            GtSaveProjectMessageBox mb;
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
                    event->ignore();
                    return;
                }

                default:
                    break;
            }
        }
        else
        {
            QMessageBox mb;
            mb.setIcon(QMessageBox::Question);
            mb.setWindowTitle("Confirm Exit");
            mb.setWindowIcon(gtApp->icon("closeIcon_16.png"));
            mb.setText(QString("Exit GTlab?"));
            mb.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
            mb.setDefaultButton(QMessageBox::Yes);
            int ret = mb.exec();

            switch (ret)
            {
                case QMessageBox::Yes:
                {
                    break;
                }

                case QMessageBox::Cancel:
                {
                    event->ignore();
                    return;
                }

                default:
                    break;
            }
        }

    }

    savePerspectiveSettings();

    gtProcessExecutor->terminateAllTasks();

    if (m_processQueue != Q_NULLPTR)
    {
        m_processQueue->close();
    }

    if (m_undoView != Q_NULLPTR)
    {
        m_undoView->close();
    }

    QMainWindow::closeEvent(event);
}

void
GtMainWin::keyPressEvent(QKeyEvent* event)
{
    if (event->modifiers() & Qt::AltModifier)
    {
        if (event->key() == Qt::Key_P)
        {
            GtPerspectiveSwitchWidget switchwidget(this);
            switchwidget.setWindowFlags(Qt::Popup);
            //            switchwidget.resize(QSize(switchwidget.width(),
            //                                      switchwidget.height()));
            //            qDebug() << switchwidget.size();
            switchwidget.exec();
        }
    }

    QMainWindow::keyPressEvent(event);
}

void
GtMainWin::setupDockWidgets()
{
    foreach (const QString& id, gtMdiLauncher->dockWidgetIds())
    {
        const QMetaObject& metaObj = gtMdiLauncher->dockWidget(id);

        GtDockWidget* dock = qobject_cast<GtDockWidget*>(metaObj.newInstance());

        if (dock)
        {
            dock->setWindowTitle(dock->objectName());
            dock->setFeatures(QDockWidget::AllDockWidgetFeatures);
            addDockWidget(dock->getDockWidgetArea(), dock);

            // add dockwidgets to map and initialize actions with null pointer
            m_dockWidgets.insert(dock, Q_NULLPTR);
        }
    }
}

void
GtMainWin::updateCollectionEntries()
{
    QStringList collectionIds = gtMdiLauncher->collectionIds();

    qSort(collectionIds);

    if (collectionIds.isEmpty())
    {
        ui->menuTools->menuAction()->setVisible(false);
        return;
    }

    ui->menuTools->menuAction()->setVisible(true);

    foreach (const QString& str, collectionIds)
    {
        QAction* action = ui->menuTools->addAction(
                              gtMdiLauncher->collectionIcon(str), str);

        connect(action, SIGNAL(triggered(bool)),
                SLOT(onCollectionEntryClicked()));
    }
}

void
GtMainWin::checkForUpdate()
{
    ui->menuUpdate_available->menuAction()->setVisible(false);
    m_cornerWidget->showUpdateWidget(true);

    QThread* thread = new QThread;
    GtUpdateChecker* check = new GtUpdateChecker;
    check->moveToThread(thread);

    connect(thread, SIGNAL(started()), check, SLOT(checkForUpdate()));
    connect(check, SIGNAL(updateAvailable()), thread, SLOT(quit()));
    connect(check, SIGNAL(updateAvailable()), check, SLOT(deleteLater()));
    connect(check, SIGNAL(updateAvailable()), this, SLOT(updateAvailable()));
    connect(check, SIGNAL(error(int, QString)), thread, SLOT(quit()));
    connect(check, SIGNAL(error(int, QString)),
            SLOT(noUpdateAvailable(int, QString)));
    connect(check, SIGNAL(error(int, QString)), check, SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed(QObject*)), thread, SLOT(terminate()));

    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void GtMainWin::widgetStructureHelper(QWidget* wid, int indent)
{
    QList<QWidget*> childs =
            wid->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);

    QString iStr;

    for (int i = 0; i < indent; ++i)
    {
        iStr = iStr + "  ";
    }

    foreach (QWidget* c, childs)
    {
        gtDebug() << iStr << "|-> " << c->objectName() << " | " <<
                     c->metaObject()->className();

        widgetStructureHelper(c, indent + 1);
    }
}

void
GtMainWin::showProjectWizard()
{
    GtProjectProvider provider;

    GtProjectWizard wizard(&provider);

    if (wizard.exec())
    {
        GtProject* project = provider.project();

        if (!gtDataModel->newProject(project))
        {
            delete project;
        }

        GtProjectUI pui;

        pui.openProject(project);
    }
}

void
GtMainWin::importProject()
{
    QString selfilter = tr("GTlab Project (*.gtlab)");

    QString filename = GtFileDialog::getOpenFileName(this,
                       tr("Import Project to Session"),
                       QString(), selfilter);

    // project provider from file loader
    if (!filename.isEmpty())
    {
        GtProjectProvider provider(filename);
        GtProject* project = provider.project();

        if (!gtDataModel->newProject(project))
        {
            delete project;
        }
    }
}

void
GtMainWin::showPreferences()
{
    GtPreferencesDialog dialog;

    dialog.exec();
}

void
GtMainWin::showSessionPreferences()
{
    GtPreferencesDialog dialog(1);

    dialog.exec();
}

void
GtMainWin::showPerspectivePreferences()
{
    GtPreferencesDialog dialog(2);

    dialog.exec();
}

void
GtMainWin::updateSessionList()
{
    ui->menuSession->clear();
    QStringList list = gtApp->sessionIds();

    if (!gtApp->sessionId().isEmpty())
    {
        QAction* current = ui->menuSession->addAction(gtApp->sessionId());
        current->setEnabled(false);
        list.removeOne(gtApp->sessionId());
    }
    else
    {
        qDebug() << "empty item...";
        QAction* current =
            ui->menuSession->addAction(tr("no session loaded"));
        current->setEnabled(false);
    }

    if (!list.isEmpty())
    {
        ui->menuSession->addSeparator();

        if (m_switchSessionMapper == Q_NULLPTR)
        {
            m_switchSessionMapper = new QSignalMapper(this);
            connect(m_switchSessionMapper, SIGNAL(mapped(QObject*)),
                    SLOT(onSessionAction(QObject*)));
        }
    }

    foreach (const QString& str, list)
    {
        //        ui->menuSession->addAction(str);
        QAction* action = ui->menuSession->addAction(str);
        connect(action, SIGNAL(triggered()), m_switchSessionMapper,
                SLOT(map()));
        m_switchSessionMapper->setMapping(action, action);
    }

    updateWindowTitle();
}

void
GtMainWin::updatePerspectiveList()
{
    ui->menuPerspective->clear();
    QStringList list = gtApp->perspectiveIds();

    if (!gtApp->perspectiveId().isEmpty())
    {
        QAction* current =
            ui->menuPerspective->addAction(gtApp->perspectiveId());
        current->setEnabled(false);
        list.removeOne(gtApp->perspectiveId());
    }
    else
    {
        qDebug() << "empty item...";
        QAction* current =
            ui->menuPerspective->addAction(tr("no perspective loaded"));
        current->setEnabled(false);
    }

    if (!list.isEmpty())
    {
        ui->menuPerspective->addSeparator();

        if (m_switchPerspectiveMapper == Q_NULLPTR)
        {
            m_switchPerspectiveMapper = new QSignalMapper(this);
            connect(m_switchPerspectiveMapper, SIGNAL(mapped(QObject*)),
                    SLOT(onPerspectiveAction(QObject*)));
        }
    }

    foreach (const QString& str, list)
    {
        //        ui->menuSession->addAction(str);
        QAction* action = ui->menuPerspective->addAction(str);
        connect(action, SIGNAL(triggered()), m_switchPerspectiveMapper,
                SLOT(map()));
        m_switchPerspectiveMapper->setMapping(action, action);
    }
}

void
GtMainWin::openMapEditor()
{
    gtMdiLauncher->open("GtMapEditor");
}

void
GtMainWin::openMementoViewer()
{
    gtMdiLauncher->open(GtMementoViewer::staticMetaObject.className());
}

void GtMainWin::openCarpetPlotTest()
{
    if (!gtMdiLauncher->open("GttCarpetPlot"))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not opencarpet plot!");
    }
}

void GtMainWin::openTemplateViewerTest()
{
    if (!gtMdiLauncher->open("GtTemplateViewer"))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not open template viewer plot!");
    }
}

void
GtMainWin::openPreDesignPlot()
{
    if (!gtMdiLauncher->open("GtdPreDesignPlot"))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not open pre design plot!");
    }
}

void
GtMainWin::openSessionViewer()
{
    if (!gtMdiLauncher->open("GtSessionViewer"))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not open session viewer!");
    }
}

void
GtMainWin::openStateViewer()
{
    if (!gtMdiLauncher->open("GtStateViewer"))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not open state viewer!");
    }
}

void
GtMainWin::openModelEditor()
{
    if (!gtMdiLauncher->open("GtpModelEditor"))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not open performance model editor!");
    }
}

void
GtMainWin::openExamplesWidget()
{
    if (!gtMdiLauncher->open("GtExamplesMdiWidget"))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not open examples viewer!");
    }
}

void
GtMainWin::openCommandHistory()
{
    if (m_undoView.data() == Q_NULLPTR)
    {
        m_undoView = new QUndoView();
        m_undoView->setWindowTitle(QStringLiteral("GTlab - Command History"));
        m_undoView->setWindowIcon(gtApp->icon(QStringLiteral(
                "historyIcon_16.png")));
        m_undoView->setStack(gtApp->undoStack());
        m_undoView->show();
        m_undoView->setAttribute(Qt::WA_DeleteOnClose, true);
    }
}

void
GtMainWin::openProcessQueue()
{
    if (m_processQueue.data() == Q_NULLPTR)
    {
        m_processQueue = new GtProcessQueueWidget(
            new GtProcessQueueModel(gtProcessExecutor));

        m_processQueue->show();
        m_processQueue->setAttribute(Qt::WA_DeleteOnClose, true);
    }
}

void
GtMainWin::onSessionAction(QObject* widget)
{
    QAction* action = qobject_cast<QAction*>(widget);

    if (action)
    {
        qDebug() << "switching session to " << action->text() << "...";
        gtApp->switchSession(action->text());
    }
}

void
GtMainWin::onPerspectiveAction(QObject* widget)
{
    QAction* action = qobject_cast<QAction*>(widget);

    if (action)
    {
        qDebug() << "switching perspective to " << action->text() << "...";
        gtApp->switchPerspective(action->text());
    }
}

void
GtMainWin::loadPerspectiveSettings(const QString& /*id*/)
{
    QPair<QByteArray, QByteArray> persData = gtApp->loadPerspectiveData();
    restoreGeometry(persData.first);
    restoreState(persData.second);
}

void
GtMainWin::savePerspectiveSettings()
{
    gtApp->savePerspectiveData(saveGeometry(), saveState());
}

void
GtMainWin::saveCurrentProject()
{
    gtDataModel->saveProject(gtApp->currentProject());
}

void
GtMainWin::saveCurrentProjectAs()
{
    GtProjectUI pui;

    pui.saveProjectAs(gtApp->currentProject());
}

void
GtMainWin::duplicateCurrentProject()
{
    GtProjectUI pui;

    pui.duplicateProject(gtApp->currentProject());
}

void
GtMainWin::saveAllProjects()
{
    foreach (GtProject* project, gtDataModel->projects())
    {
        gtDataModel->saveProject(project);
    }
}

void
GtMainWin::onCurrentProjectChanged(GtProject* project)
{
    if (project == Q_NULLPTR)
    {
        ui->actionSave_Project->setEnabled(false);
        ui->actionSave_As->setEnabled(false);
        ui->actionDuplicate_Project->setEnabled(false);
        ui->actionCloseProject->setEnabled(false);
    }
    else
    {
        ui->actionSave_Project->setEnabled(true);
        ui->actionSave_As->setEnabled(true);
        ui->actionDuplicate_Project->setEnabled(true);
        ui->actionCloseProject->setEnabled(true);
        ui->actionOpen_Project->setEnabled(false);
    }
}

void
GtMainWin::printCurrentMdiItem()
{
    qDebug() << "print requested...";

    if (ui->mdiArea->subWindowList().size() >= 1)
    {
        gtMdiLauncher->print(ui->mdiArea->currentSubWindow());
    }
    else
    {
        QMessageBox::information(Q_NULLPTR, "Print error", "No view open!",
                                 QMessageBox::Ok);
    }
}

void
GtMainWin::runBugReport()
{
    QString link =
        QStringLiteral("https://mantis.dlr.de/mantis/login_page.php");
    QDesktopServices::openUrl(QUrl(link));
}

void
GtMainWin::openHelpContents()
{
    QDir root(QApplication::applicationDirPath());
    root.cdUp();

    QString path(root.absolutePath() +
                 QDir::separator() +
                 QStringLiteral("documentation"));

    QProcess* process = new QProcess;
    QStringList args;
    args << QLatin1String("-collectionFile")
         << path + QDir::separator() +
         QStringLiteral("GTlab_Basics.qhc")
         << QLatin1String("-enableRemoteControl");

#ifdef Q_OS_WIN32
    QString binary = qApp->applicationDirPath() + QDir::separator() + "help";
#else
    QString binary = qApp->applicationDirPath() + QDir::separator() +
            "run_help.sh";
#endif

    qDebug() << args;

    process->start(binary, args);

    if (!process->waitForStarted())
    {
        QMessageBox::critical(Q_NULLPTR, QObject::tr("GTlab"),
                              QObject::tr("Unable to launch HelpContents.\n"
                                          "Please install documentation."));

        gtError() << process->errorString();

        delete process;

        return;
    }

//    QDir docDir(path);
//    docDir.setNameFilters(QStringList() << QStringLiteral("*.qch"));

//    QStringList fileList = docDir.entryList();

//    gtDebug() << fileList.size() << tr(" documentation files found!");

//    QByteArray ba;

//    foreach (const QString& file, fileList)
//    {


//        QString command =  QStringLiteral("register ") + path +
//                           QDir::separator() + file + QStringLiteral(";");

//        gtDebug() << "remote command: " << command;

//        ba.append(command);


//    }

//    if (process->write(ba) < 0)
//    {
//        gtError() << "remote command failed!";
//    }
//    else
//    {
//        gtDebug() << "remote command successful!";
//    }

    //    process->close();

    //    QString command =  QStringLiteral("register ") + path + QDir::separator() +
    //                       QStringLiteral("GTlab_Help_Performance.qch");

    //    qDebug() << "remote command: " << command;

    //    ba.append(command);

}

void
GtMainWin::openAboutDialog()
{
    GtAboutDialog dialog;

    dialog.exec();
}

void
GtMainWin::openChangelogDialog()
{
    GtChangelogDialog dialog;

    dialog.exec();
}

void
GtMainWin::updateWindowTitle()
{
    if (gtApp->session())
    {
        QString newTitle = QStringLiteral("GTlab - ") + gtApp->sessionId();

        if (gtApp->currentProject() != Q_NULLPTR)
        {
            newTitle = newTitle + QStringLiteral(" - ") +
                       gtApp->currentProject()->objectName() +
                       QStringLiteral(" - ") + gtApp->currentProject()->path();
        }

        setWindowTitle(newTitle);
    }
    else
    {
        setWindowTitle(QStringLiteral("GTlab"));
    }
}

void
GtMainWin::onCollectionEntryClicked()
{
    QAction* action = qobject_cast<QAction*>(sender());

    if (action == Q_NULLPTR)
    {
        return;
    }

    const QString colId = action->text();

    GtMdiItem* item = gtMdiLauncher->open(GT_CLASSNAME(GtCollectionEditor),
                                          colId);

    GtCollectionEditor* coll = qobject_cast<GtCollectionEditor*>(item);

    if (coll == Q_NULLPTR)
    {
        return;
    }

    coll->setCollection(gtMdiLauncher->collection(colId));

    QMdiSubWindow* subWin = item->subWin();

    if (subWin == Q_NULLPTR)
    {
        return;
    }

    subWin->setWindowIcon(gtMdiLauncher->collectionIcon(colId));
}

void
GtMainWin::openCheckForUpdatesDialog()
{
    GtCheckForUpdatesDialog dialog;

    if (dialog.exec())
    {
        runUpdate();
    }
}

void
GtMainWin::updateAvailable()
{
    m_cornerWidget->showUpdateWidget(false);
    gtDebug() << "new update found!";
    ui->menuUpdate_available->menuAction()->setVisible(true);
    //    GtdApplication::notify("GTlab Update available!",
    //                           GtdUtilitiesOld::icon("updateToIcon.png"));
}

void
GtMainWin::noUpdateAvailable(int errorCode, const QString& str)
{
    m_cornerWidget->showUpdateWidget(false);

    gtDebug() << "Error code = " << errorCode;
    gtDebug() << "Error message = " << str;
}

void
GtMainWin::runLoadingProcedure(GtAbstractLoadingHelper* helper)
{
    if (helper == Q_NULLPTR)
    {
        return;
    }

    GtSimpleLoadingWidget* loadingWidget = new GtSimpleLoadingWidget(helper,
            this);
    loadingWidget->exec();

}

void
GtMainWin::runUpdate()
{
    if (gtApp->hasProjectChanges())
    {
        QString text =
            tr("GTlab has to be closed for updates.\n"
               "Found unsaved project.\nDo you want to ") +
            tr("save all your changes before closing GTlab?");

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
            }

            default:
                break;
        }
    }
    else
    {
        QMessageBox mb;
        mb.setIcon(QMessageBox::Question);
        mb.setWindowTitle("Confirm Exit");
        mb.setWindowIcon(gtApp->icon("closeIcon_16.png"));
        mb.setText(QString("Exit GTlab?"));
        mb.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        mb.setDefaultButton(QMessageBox::Yes);
        int ret = mb.exec();

        switch (ret)
        {
            case QMessageBox::Yes:
            {
                break;
            }

            case QMessageBox::Cancel:
            {
                return;
            }

            default:
                break;
        }
    }

    m_forceQuit = true;

    QDir path(QApplication::applicationDirPath());
    path.cdUp();

    QProcess::startDetached(path.absoluteFilePath("MaintenanceTool"));
    close();
}

void
GtMainWin::initAfterStartup()
{
    if (!m_firstTimeShowEvent)
    {
        return;
    }

    // open startup page
    if (gtApp->settings()->showStartupPage())
    {
        GtStartupPage* page = new GtStartupPage;

        connect(page, SIGNAL(newProject()), SLOT(showProjectWizard()));
        //connect(page, SIGNAL(importProject()), SLOT(importProject()));
        connect(page, SIGNAL(openExamplesWidget()), SLOT(openExamplesWidget()));
        connect(page, SIGNAL(helpContents()), SLOT(openHelpContents()));
        connect(page, SIGNAL(showInfo()), SLOT(openAboutDialog()));

        QMdiSubWindow* subWin = ui->mdiArea->addSubWindow(page);
        subWin->setWindowTitle(tr("Welcome"));
        subWin->setWindowIcon(page->icon());
        subWin->show();
    }

    // search for updates
    if (gtApp->settings()->searchForUpdate())
    {
        checkForUpdate();
    }

    emit guiInitialized();

    // initialize dock widgets
    for (auto e : m_dockWidgets.keys())
    {
        // add entries to menu
        QAction* dockAct =
            ui->menuDock_Widgets->addAction(e->windowTitle());
        dockAct->setCheckable(true);
        dockAct->setChecked(e->isVisible());

        m_dockWidgets[e] = dockAct;

        connect(e, SIGNAL(visibilityChanged(bool)),
                SLOT(onDockVisibilityChange(bool)));
        connect(dockAct, SIGNAL(triggered(bool)), SLOT(onDockActionClicked()));

        e->initAfterStartup();
    }

    m_firstTimeShowEvent = false;
}

void
GtMainWin::openProject()
{
    GtProject* project = qobject_cast<GtProject*>(gtApp->selectedObject());

    if (project != Q_NULLPTR && project != gtApp->currentProject())
    {
        if (!project->isOpen())
        {
            GtProjectUI pui;

            pui.openProject(project);
        }
    }
}

void
GtMainWin::closeProject()
{
    GtProjectUI pui;

    pui.closeProject(gtApp->currentProject());
}

void
GtMainWin::onObjectSelected(GtObject* obj)
{
    ui->actionOpen_Project->setEnabled(false);

    GtProject* project = qobject_cast<GtProject*>(obj);

    if (project != Q_NULLPTR)
    {
        if (project != gtApp->currentProject())
        {
            ui->actionOpen_Project->setEnabled(true);
        }
    }
}

void
GtMainWin::onDockVisibilityChange(bool /*val*/)
{
    GtDockWidget* dock = qobject_cast<GtDockWidget*>(sender());

    if (dock == Q_NULLPTR)
    {
        return;
    }

    if (!m_dockWidgets.contains(dock))
    {
        return;
    }

    QAction* act = m_dockWidgets.value(dock);

    if (act == Q_NULLPTR)
    {
        return;
    }

    bool isVisible = false;

    if ((dock->isVisible() || !tabifiedDockWidgets(dock).isEmpty()))
    {
        isVisible = true;
    }

    if (act->isChecked() == isVisible)
    {
        return;
    }

    act->setChecked(isVisible);
}

void
GtMainWin::onDockActionClicked()
{
    QAction* action = qobject_cast<QAction*>(sender());

    if (action == Q_NULLPTR)
    {
        return;
    }

    for (auto e : m_dockWidgets.keys())
    {
        if (m_dockWidgets.value(e) == action)
        {
            e->setVisible(action->isChecked());
        }
    }
}

void
GtMainWin::onWidgetStructureClicked()
{
    gtDebug() << "widget structure...";

    QWidgetList wids = QApplication::topLevelWidgets();

    foreach (QWidget* wid, wids)
    {
        widgetStructureHelper(wid, 1);
    }
}
