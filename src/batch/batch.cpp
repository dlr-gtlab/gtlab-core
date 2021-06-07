/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 24.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <iostream>

#include <QApplication>
#include <QCommandLineParser>
#include <QDomDocument>
#include <QObject>
#include <QDir>
#include <QDebug>

#include "gt_application.h"
#include "gt_coredatamodel.h"
#include "gt_coreprocessexecutor.h"
#include "gt_project.h"
#include "gt_projectprovider.h"
#include "gt_processdata.h"
#include "gt_task.h"
#include "gt_footprint.h"
#include "batchremote.h"

using namespace std;

void
showSplashScreen()
{
    cout << endl;
    cout << "******************************************" << endl;
    cout << "    _____________________      ______  " << endl;
    cout << "    __  ____/__  __/__  /_____ ___  /_ " << endl;
    cout << "    _  / __ __  /  __  /_  __ `/_  __ \\" << endl;
    cout << "    / /_/ / _  /   _  / / /_/ /_  /_/ /" << endl;
    cout << "    \\____/  /_/    /_/  \\__,_/ /_.___/ " << endl;
    cout << "                                 " <<
         GtApplication::versionToString().toStdString() << endl;
    cout << "******************************************" << endl;
    cout << endl;
}

int
checkMetaInput(const QString& fileName)
{
    qDebug() << "meta input check...";

    if (fileName.isEmpty())
    {
        qWarning() << QStringLiteral("ERROR: ") <<
                   QObject::tr("file name is empty!");

        return -1;
    }

    if (!BatchRemote::checkInput(fileName,
                                 GtApplication::versionToString(),
                                 false, false))
    {
        qWarning() << QStringLiteral("ERROR: ") <<
                   QObject::tr("meta input invalid!");

        return -1;
    }

    qDebug() << "meta input ok!";

    return 0;
}

int
runMetaInput(const QString& inputFileName, const QString& outputFileName)
{
    qDebug() << "meta input run...";

    if (inputFileName.isEmpty())
    {
        qWarning() << QStringLiteral("ERROR: ") <<
                   QObject::tr("input file name is empty!");

        return -1;
    }

    if (outputFileName.isEmpty())
    {
        qWarning() << QStringLiteral("ERROR: ") <<
                   QObject::tr("output file name is empty!");

        return -1;
    }

    qDebug() << "   input file name: " << inputFileName;
    qDebug() << "   output file name: " << outputFileName;

    int inputCheck = checkMetaInput(inputFileName);

    if (inputCheck != 0)
    {
        qWarning() << QStringLiteral("ERROR: ") <<
                   QObject::tr("meta input run failed!");

        return inputCheck;
    }


    qDebug() << "executing process...";

    if (!BatchRemote::run(inputFileName, outputFileName,
                          GtApplication::versionToString()))
    {
        qWarning() << QStringLiteral("ERROR: ") <<
                   QObject::tr("process execution failed!");

        return -1;
    }

    qDebug() << "meta input run successful!";

    return 0;
}

int
runProcess(const QString& projectId, const QString& processId,
           bool save = false)
{
    qDebug() << "process run...";

    if (projectId.isEmpty())
    {
        qWarning() << QStringLiteral("ERROR: ") <<
                   QObject::tr("project id is empty!");

        return -1;
    }

    if (processId.isEmpty())
    {
        qWarning() << QStringLiteral("ERROR: ") <<
                   QObject::tr("process id is empty!");

        return -1;
    }

    GtProject* project = gtApp->findProject(projectId);

    if (project == Q_NULLPTR)
    {
        qWarning() << QStringLiteral("ERROR: ") <<
                   QObject::tr("project not found!") <<
                   QStringLiteral(" (") << projectId << QStringLiteral(")");

        return -1;
    }

    if (!gtDataModel->GtCoreDatamodel::openProject(project))
    {
        qWarning() << QStringLiteral("ERROR: ") <<
                   QObject::tr("could not open project!") <<
                   QStringLiteral(" (") << projectId << QStringLiteral(")");

        return -1;
    }

    qDebug() << "project opened!";

    // run process
    GtTask* process = project->findProcess(processId);

    if (process == Q_NULLPTR)
    {
        qWarning() << QStringLiteral("ERROR: ") <<
                   QObject::tr("process not found!") <<
                   QStringLiteral(" (") << processId << QStringLiteral(")");

        return -1;
    }

    // execute process
    GtCoreProcessExecutor executor;
    executor.runTask(process);

    if (process->currentState() != GtProcessComponent::FINISHED)
    {
        qWarning() << "Calculator run failed!";
        return -1;
    }

    qDebug() << "process run successful!";

    if (save)
    {
        if (!gtDataModel->saveProject(project))
        {
            qWarning() << QStringLiteral("ERROR: ") <<
                       QObject::tr("project could not besaved!") <<
                       QStringLiteral(" (") << projectId <<
                       QStringLiteral(")");
            return -1;
        }
    }

    return 0;
}


int
runProcessByFile(const QString& projectFile, const QString& processId,
                 bool save = false)
{
    qDebug() << "process run...";

    if (projectFile.isEmpty())
    {
        qWarning() << QStringLiteral("ERROR: ")
                   << QObject::tr("project file is empty!");

        return -1;
    }

    if (processId.isEmpty())
    {
        qWarning() << QStringLiteral("ERROR: ")
                   << QObject::tr("process id is empty!");

        return -1;
    }

    QFile file(projectFile);

    if (!file.exists())
    {
        qWarning() << QStringLiteral("ERROR: ")
                   << QObject::tr("project file")
                   << projectFile
                   << QObject::tr("not found!");

        return -1;
    }
    //gtDebug() << "  |-> is existing file!";

    GtProjectProvider provider(projectFile);
    GtProject* project = provider.project();

    if (!gtDataModel->GtCoreDatamodel::openProject(project))
    {
        qWarning() << QStringLiteral("ERROR: ") <<
                   QObject::tr("could not open project!") <<
                   QStringLiteral(" (") << projectFile << QStringLiteral(")");

        return -1;
    }

    qDebug() << "project opened!";

    // run process
    GtTask* process = project->findProcess(processId);

    if (process == Q_NULLPTR)
    {
        qWarning() << QStringLiteral("ERROR: ") <<
                   QObject::tr("process not found!") <<
                   QStringLiteral(" (") << processId << QStringLiteral(")");

        return -1;
    }

    // execute process
    GtCoreProcessExecutor executor(nullptr, false);
    executor.runTask(process);

    if (process->currentState() != GtProcessComponent::FINISHED)
    {
        qWarning() << "Calculator run failed!";
        return -1;
    }

    qDebug() << "process run successful!";

    if (save)
    {
        if (!gtDataModel->saveProject(project))
        {
            qWarning() << QStringLiteral("ERROR: ") <<
                       QObject::tr("project could not besaved!") <<
                       QStringLiteral(" (") << projectFile <<
                       QStringLiteral(")");
            return -1;
        }
    }

    return 0;
}

bool
isExactlyOneTrue(bool* boolAry, int size)
{
    bool areAnyTrue = false;
    bool areTwoTrue = false;

    for (int i = 0; (!areTwoTrue) && (i < size); i++)
    {
        areTwoTrue = (areAnyTrue && boolAry[i]);
        areAnyTrue |= boolAry[i];
    }

    return ((areAnyTrue) && (!areTwoTrue));
}

// comamnd line options
const QStringList CLO_SESSION = QStringList() << QStringLiteral("session") <<
                                QStringLiteral("se");
const QString CLO_PROJECT = QStringLiteral("project");
const QString CLO_PROCESS = QStringLiteral("process");
const QStringList CLO_CHECK_META =
    QStringList() << QStringLiteral("check_meta") <<
    QStringLiteral("cm");
const QStringList CLO_RUN_META =
    QStringList() << QStringLiteral("run_meta") <<
    QStringLiteral("rm");
const QStringList CLO_RUN = QStringList() << QStringLiteral("run") <<
                            QStringLiteral("r");
const QStringList CLO_RUNPRO = QStringList() << QStringLiteral("run-pro") <<
                            QStringLiteral("rp");
const QStringList CLO_SAVE = QStringList() << QStringLiteral("save") <<
                             QStringLiteral("s");
const QStringList CLO_FOOTPRINT = QStringList() << QStringLiteral("footprint") <<
                             QStringLiteral("fp");

int main(int argc, char* argv[])
{
    QCoreApplication::setOrganizationDomain("www.dlr.de");
    QCoreApplication::setOrganizationName("DLR");
    QCoreApplication::setApplicationName("GTlab");

    QCoreApplication::setApplicationVersion(GtApplication::versionToString());

    QApplication a(argc, argv);

    showSplashScreen();

    // parse arguments
    QCommandLineParser parser;
    parser.setApplicationDescription("GTlab Console");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption sessionOption(QStringList() << CLO_SESSION,
                                     "Defines a session to be used for "
                                     "execution."
                                     "\nUsage: --session=<session_id>",
                                     "name");
    //    QCommandLineOption projectOption(QStringList() << CLO_PROJECT,
    //                                     "Use project <name>.", "name");
    //    QCommandLineOption processOption(QStringList() << CLO_PROCESS,
    //                                     "Run process <name>.", "name");
    QCommandLineOption checkMetaOption(CLO_CHECK_META,
                                       "Checks given meta process data."
                                       "\nUsage: -- check_meta <input.xml>");
    QCommandLineOption runMetaOption(CLO_RUN_META,
                                     "Executes given meta process data. "
                                     "Results are stored in given output file."
                                     "\nUsage; --run_meta <input.xml> "
                                     "<output.xml>");
    QCommandLineOption runOption(
                CLO_RUN, "Executes a given process of a given project. "
                         "\nUsage: --run <project_id> <process_id>");
    QCommandLineOption runOptionPro(
                CLO_RUNPRO, "Executes a given process of a given project file. "
                            "\nUsage: --run-pro <project_file> <process_id>");
    QCommandLineOption saveOption(CLO_SAVE,
                                  "Saves the project after a successful "
                                  "process execution.");
    QCommandLineOption footprintOption(CLO_FOOTPRINT,
                                  "Displays framework footprint.");


    parser.addOption(sessionOption);
    //    parser.addOption(projectOption);
    //    parser.addOption(processOption);
    parser.addOption(checkMetaOption);
    parser.addOption(runMetaOption);
    parser.addOption(runOption);
    parser.addOption(runOptionPro);
    parser.addOption(saveOption);
    parser.addOption(footprintOption);

    parser.process(a);

    bool arr[5] = {false};
    arr[0] = parser.isSet(checkMetaOption);
    arr[1] = parser.isSet(runMetaOption);
    arr[2] = parser.isSet(runOption);
    arr[3] = parser.isSet(runOptionPro);
    arr[4] = parser.isSet(footprintOption);

    if (!isExactlyOneTrue(arr, 5))
    {
        qWarning() << QStringLiteral("ERROR: ") <<
                   QObject::tr("Invalid argument! "
                               "Use --help for command overview.");

        return -1;
    }

    // application initialization
    GtApplication app(qApp, true);
    app.init();

    // save to system environment (temporary)
    app.saveSystemEnvironment();

    // language initialization
    app.initLanguage();

    // datamodel initialization
    app.initDatamodel();

    // load GTlab modules
    app.loadModules();

    // calculator initialization
    app.initCalculators();

    // session initialization
    if (parser.isSet(sessionOption))
    {
        app.initSession(parser.value(sessionOption));
    }
    else
    {
        app.initSession();
    }

    if (app.session() == Q_NULLPTR)
    {
        qWarning() << "no session loaded!";
        return -1;
    }

    // initialize modules
    app.initModules();

    if (parser.isSet(footprintOption))
    {
        std::cout << GtFootprint().exportToXML().toStdString() <<
                     std::endl;

        return 0;
    }

    // check meta input
    if (parser.isSet(checkMetaOption))
    {
        QStringList metaInputs = parser.positionalArguments();

        if (metaInputs.size() != 1)
        {
            qWarning() << QStringLiteral("ERROR: ") <<
                       QObject::tr("please specifify one meta input file!");

            return -1;
        }

        return checkMetaInput(metaInputs.first());
    }

    // run meta input
    if (parser.isSet(runMetaOption))
    {
        QStringList metaInputs = parser.positionalArguments();

        if (metaInputs.size() != 2)
        {
            qWarning() << QStringLiteral("ERROR: ") <<
                       QObject::tr("please specifify one meta input file "
                                   "and one output file!");

            return -1;
        }

        return runMetaInput(metaInputs[0], metaInputs[1]);
    }

    // run process
    if (parser.isSet(runOption))
    {
        const bool saveProject = parser.isSet(saveOption);

        QStringList processIds = parser.positionalArguments();

        if (processIds.size() != 2)
        {
            qWarning() << QStringLiteral("ERROR: ") <<
                       QObject::tr("invalid process id!");

            return -1;
        }

        return runProcess(processIds[0], processIds[1], saveProject);
    }

    // run process by file
    if (parser.isSet(runOptionPro))
    {
        const bool saveProject = parser.isSet(saveOption);

        QStringList processIds = parser.positionalArguments();

        if (processIds.size() != 2)
        {
            qWarning() << QStringLiteral("ERROR: ") <<
                       QObject::tr("invalid process id!");

            return -1;
        }

        return runProcessByFile(processIds[0], processIds[1], saveProject);
    }

    qWarning() << QObject::tr("invalid arguments! "
                              "use --help for further information.");

    return -1;
}
