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

#ifdef GT_MODELTEST
#include <QAbstractItemModelTester>
#endif

int
main(int argc, char* argv[])
{
    QApplication::setOrganizationDomain("www.dlr.de");
    QApplication::setOrganizationName("DLR");
    QApplication::setApplicationName("GTlab");

    QApplication::setApplicationVersion(GtApplication::versionToString());

#if QT_VERSION >= 0x050900
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication a(argc, argv);

    QPixmap pixmap(":pixmaps/splash.png");

    QSplashScreen splash(pixmap);
    splash.show();

    GtApplication app(qApp);

    splash.showMessage(QObject::tr("initializing..."));
    app.init();

    // TODO: we need shared and environment variables
    // load module environments vars and fill in GTlab environment
    QMap<QString, QString> modEnv = GtModuleLoader::moduleEnvironmentVars();
    QStringList modInitEnvSetup;

    for(auto e : modEnv.keys())
    {
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
        QMessageBox::critical(Q_NULLPTR, "Licence verification",
                              "Could not verify license!",
                              QMessageBox::Ok, QMessageBox::Ok);

        return -1;
    }
#else
    qDebug() << "licence check disabled!";
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

    int mll = gtApp->settings()->maxLogLength();

    if (mll < 10)
    {
        mll = 2000;
        gtApp->settings()->setMaxLogLength(mll);
    }

    gtLogModel->setMaxLogLength(mll);

#ifdef GT_MODELTEST
    new QAbstractItemModelTester(gtDataModel,QAbstractItemModelTester::FailureReportingMode::Fatal, qApp);
#else
    qDebug() << "model test disabled!";
#endif

    GtMainWin w;

    QObject::connect(&w, SIGNAL(guiInitialized()),
                     &app, SLOT(onGuiInitializationFinished()));

    w.show();

    splash.finish(&w);

    return a.exec();
}
