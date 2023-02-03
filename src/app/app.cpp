/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QApplication>
#include <QCommandLineParser>
#include <QSplashScreen>
#include <QMessageBox>
#include <QSettings>
#include <QProcess>
#include <QDir>
#include <QtGlobal>
#include <QDebug>

#include "gt_mainwin.h"
#include "gt_application.h"
#include "gt_settings.h"
#include "gt_logging.h"
#include "gt_logmodel.h"
#include "gt_environmentdialog.h"
#include "gt_environment.h"
#include "gt_moduleloader.h"
#include "gt_datamodel.h"
#include "gt_refusedpluginsdialog.h"
#include "gt_mdilauncher.h"
#include "gt_icons.h"
#include "gt_versionnumber.h"

#include "gt_mementoviewer.h"
#include "gt_processeditor.h"
#include "gt_templateviewer.h"
#include "gt_labelusageviewer.h"
#include "gt_sessionviewer.h"
#include "gt_stateviewer.h"
#include "gt_collectioneditor.h"
#include "gt_startuppage.h"
#include "gt_examplesmdiwidget.h"
#include "gt_logerrormessagebox.h"

// dock widgets
#include "gt_outputdock.h"
#include "gt_explorerdock.h"
#include "gt_processdock.h"
#include "gt_propertiesdock.h"
#include "gt_labelsdock.h"
#include "gt_postdock.h"
#include "gt_calculatorsdock.h"

#ifdef GT_MODELTEST
#include <QAbstractItemModelTester>
#endif

void
registerWidgets()
{
    gtMdiLauncher->registerClass(GT_METADATA(GtMementoViewer));
    gtMdiLauncher->registerClass(GT_METADATA(GtProcessEditor));
    gtMdiLauncher->registerClass(GT_METADATA(GtTemplateViewer));
    gtMdiLauncher->registerClass(GT_METADATA(GtLabelUsageViewer));
    gtMdiLauncher->registerClass(GT_METADATA(GtSessionViewer));
    gtMdiLauncher->registerClass(GT_METADATA(GtStateViewer));
    gtMdiLauncher->registerClass(GT_METADATA(GtCollectionEditor));
    gtMdiLauncher->registerClass(GT_METADATA(GtExamplesMdiWidget));
    gtMdiLauncher->registerClass(GT_METADATA(GtStartupPage));

    gtMdiLauncher->registerDockWidget(GT_METADATA(GtOutputDock));
    gtMdiLauncher->registerDockWidget(GT_METADATA(GtExplorerDock));
    gtMdiLauncher->registerDockWidget(GT_METADATA(GtProcessDock));
    gtMdiLauncher->registerDockWidget(GT_METADATA(GtPropertiesDock));
    gtMdiLauncher->registerDockWidget(GT_METADATA(GtLabelsDock));
    gtMdiLauncher->registerDockWidget(GT_METADATA(GtPostDock));
    //gtMdiLauncher->registerDockWidget(GT_METADATA(GtCalculatorsDock));
}


int
main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication::setOrganizationDomain("www.dlr.de");
    QApplication::setOrganizationName("DLR");
    QApplication::setApplicationName("GTlab");

    QApplication::setApplicationVersion(GtApplication::version().toString());

#if QT_VERSION >= 0x050900
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication a(argc, argv);

    QSplashScreen splash(gt::gui::pixmap::splash());
    splash.show();

    GtApplication app(qApp);

    splash.showMessage(QObject::tr("initializing..."));
    app.init();

    // error message box logging destination
    auto* errorHandler = new GtlogErrorMessageBoxHandler;
    errorHandler->setParent(&app);

    auto& logger = gt::log::Logger::instance();
    auto dest = gt::log::makeSignalSlotDestination(
                    errorHandler, &GtlogErrorMessageBoxHandler::onLogMessage);
    logger.addDestination("error_message_box", std::move(dest));

    // TODO: we need shared and environment variables
    // load module environments vars and fill in GTlab environment
    QMap<QString, QString> modEnv = GtModuleLoader::moduleEnvironmentVars();
    QStringList modInitEnvSetup;

    for(auto iter = modEnv.constBegin(); iter != modEnv.constEnd(); ++iter)
    {
        const auto& e = iter.key();
        if (!gtEnvironment->environmentVariableExists(e))
        {
            gtEnvironment->addEnvironmentVariable(e);
            QDir envDir(modEnv.value(e));
            gtEnvironment->setValue(e, envDir.absolutePath());
            modInitEnvSetup << e;
        }
    }

    // user dialog to specify unset environment settings
    if (!modInitEnvSetup.isEmpty())
    {
        GtEnvironmentDialog envDialog(modInitEnvSetup,
                                      "Module Environment Settings",
                                      "Some modules require an initial "
                                      "configuration. Please configure the "
                                      "GTlab environment variables.");
        envDialog.exec();
    }

    // save to system environment (temporary)
    app.saveSystemEnvironment();

#ifdef GT_LICENCE
    if (!app.checkLicence())
    {
        QMessageBox::critical(nullptr, "Licence verification",
                              "Could not verify license!",
                              QMessageBox::Ok, QMessageBox::Ok);

        return -1;
    }
    gtDebug() << tr("License verified!");
#endif

    // application initialization
//    gtGuiApp;

//    if (parser.isSet("lang"))
//    {
//        gtGuiApp->setLanguage(parser.value("lang"));
//    }
//    else
//    {
//        gtGuiApp->initLanguage();
//    }
    // language initialization
    app.initLanguage();

    // chort cuts initialization
    app.initShortCuts();

    // datamodel initialization
    app.initDatamodel();

    // mdi launcher initialization
    app.initMdiLauncher();

    registerWidgets();

    // show refused plugins (application last run crash)
    if (!gtApp->crashedModules().isEmpty())
    {
        GtRefusedPluginsDialog refDia;
        refDia.exec();
        refDia.sync();
    }

    // load GTlab modules
    splash.showMessage(QObject::tr("loading modules..."));
    app.loadModules();

    // calculator initialization
    app.initCalculators();

    // session initialization
    app.initSession();

    // perspective initialization
    app.initPerspective();

//    gtApp->setLanguage("de");

//    qDebug() << "system language: " << QLocale::system();

//    gtApp->setToSystemLanguage();

#ifdef GT_MODELTEST
    qDebug() << "Model test enabled!";
    new QAbstractItemModelTester(gtDataModel,
                                 QAbstractItemModelTester::FailureReportingMode::Fatal,
                                 qApp);
#endif

    GtMainWin w;

    QObject::connect(&w, SIGNAL(guiInitialized()),
                     &app, SLOT(onGuiInitializationFinished()));

    w.show();

    splash.finish(&w);

    return a.exec();
}
