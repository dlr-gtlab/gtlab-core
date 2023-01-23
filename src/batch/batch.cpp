/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 24.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <iostream>
#include <iomanip>

#include <QApplication>
#include <QDomDocument>
#include <QObject>
#include <QDir>
#include <QDebug>

#include "internal/gt_commandlinefunctionhandler.h"
#include "batchremote.h"

#include "gt_coreapplication.h"
#include "gt_coredatamodel.h"
#include "gt_coreprocessexecutor.h"
#include "gt_project.h"
#include "gt_projectprovider.h"
#include "gt_task.h"
#include "gt_footprint.h"
#include "gt_utilities.h"
#include "gt_consoleparser.h"
#include "gt_versionnumber.h"
#include "gt_hostinfo.h"
#include "gt_remoteprocessrunner.h"
#include "settings/gt_settings.h"

void
showSplashScreen()
{
    using namespace std;

    cout << endl;
    cout << "******************************************" << endl;
    cout << "    _____________________      ______  " << endl;
    cout << "    __  ____/__  __/__  /_____ ___  /_ " << endl;
    cout << "    _  / __ __  /  __  /_  __ `/_  __ \\" << endl;
    cout << "    / /_/ / _  /   _  / / /_/ /_  /_/ /" << endl;
    cout << "    \\____/  /_/    /_/  \\__,_/ /_.___/ " << endl;
    cout << "                                 "
         << GtCoreApplication::version().toString().toStdString() << endl;
    cout << "******************************************" << endl;
    cout << endl;
}

int
checkMetaInput(const QStringList& args)
{
    GtCommandLineParser p;
    p.parse(args);

    if (p.positionalArguments().size() != 1)
    {
        qWarning() << QStringLiteral("ERROR: ")
                   << QObject::tr("Invalid size of arguments for check meta!"
                                  "Exactly one argument is required");
        return -1;
    }

    QString fileName = p.positionalArguments().constFirst();

    qDebug() << "meta input check...";

    if (fileName.isEmpty())
    {
        qWarning() << QStringLiteral("ERROR: ")
                   << QObject::tr("file name is empty!");
        return -1;
    }

    if (!gt::batch::checkInput(fileName,
                               GtCoreApplication::version().toString(),
                               false, false))
    {
        qWarning() << QStringLiteral("ERROR: ")
                   << QObject::tr("meta input invalid!");
        return -1;
    }

    qDebug() << "meta input ok!";
    return 0;
}

int
runMetaInput(const QStringList& args)
{
    GtCommandLineParser p;
    p.parse(args);

    if (p.positionalArguments().size() < 2)
    {
        qWarning() << "Invalid arguments for runMetaInput.";
        qWarning() << "Two arguments needed as <Input> <Output>";

        return -1;
    }


    QString inputFileName = p.positionalArguments().constFirst();
    QString outputFileName = p.positionalArguments().at(1);

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

    int inputCheck = checkMetaInput({inputFileName});

    if (inputCheck != 0)
    {
        qWarning() << QStringLiteral("ERROR: ") <<
                   QObject::tr("meta input run failed!");
        return inputCheck;
    }

    qDebug() << "executing process...";

    if (!gt::batch::run(inputFileName, outputFileName,
                        GtCoreApplication::version().toString()))
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

    if (!project)
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

    if (!process)
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
                       QObject::tr("project could not be saved!") <<
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

    if (!project)
    {
        gtError() << "Cannot load project";
        return -1;
    }

    gtApp->session()->appendChild(project);

    if (!gtDataModel->GtCoreDatamodel::openProject(project))
    {
        qWarning() << QStringLiteral("ERROR: ")
                   << QObject::tr("could not open project!")
                   << QStringLiteral(" (") << projectFile
                   << QStringLiteral(")");

        return -1;
    }

    qDebug() << "project opened!";

    // run process
    GtTask* process = project->findProcess(processId);

    if (!process)
    {
        qWarning() << QStringLiteral("ERROR: ")
                   << QObject::tr("process not found!")
                   << QStringLiteral(" (") << processId << QStringLiteral(")");

        return -1;
    }

    // execute process
    GtCoreProcessExecutor executor{gt::DryExecution};
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
            qWarning() << QStringLiteral("ERROR: ")
                       << QObject::tr("project could not be saved!")
                       << QStringLiteral(" (") << projectFile
                       << QStringLiteral(")");
            return -1;
        }
    }

    return 0;
}

int
processRunner(QStringList const& args)
{
    auto port    = gt::process_runner::S_DEFAULT_PORT;
    auto address = gt::process_runner::S_DEFAULT_HOST;
    int timeout  = 25;

    GtCommandLineParser p;
    p.addHelpOption();
    p.addOption("port", {"port", "p"},
                "Defines the port to listen to "
                "(default value is " + QString::number(port) + ")");
    p.addOption("client", {"client", "c"},
                "Defines the client address/hostname to accept requests from "
                "(by default this is " + address + "). Use 0.0.0.0 to accept "
                "requests from any client.");
    p.addOption("timeout", {"timeout", "t"},
                "Timeout in seconds to wait for the initial connection "
                "(default value is "  + QString::number(timeout) + "s).");

    // args are optional
    p.parse(args);

    if (p.helpOption())
    {
        p.printHelp("process_runner");
        return 0;
    }

    // update port
    if (p.option("port"))
    {
        bool ok = true;
        port = p.optionValue("port").toInt(&ok);

        if (!ok || !gt::checkNumericalLimits<quint16>(port))
        {
            std::cout << "Invalid port specified! "
                         "Set port using --port=<port>"
                      << std::endl;
            return -1;
        }
    }

    // update client address
    if (p.option("client"))
    {
        address = p.optionValue("client").toString();

        if (address.isEmpty())
        {
            std::cout << "Invalid client address specified! "
                         "Set address using --client=<address>"
                      << std::endl;
            return -1;
        }
    }

    // update timeout
    if (p.option("timeout"))
    {
        bool ok = true;
        timeout = p.optionValue("timeout").toInt(&ok);

        if (!ok)
        {
            std::cout << "Invalid timeout specified! "
                         "Set timeout using --timeout=<timeout_sec>"
                      << std::endl;
            return -1;
        }
    }

    // resolve client address name into an ip-address
    std::cout << "Resolving client address '" << address.toStdString()
              << "'..." << std::endl;

    QHostAddress client = GtHostInfo::lookupHost(address);

    if (client.isNull())
    {
        std::cout << "Failed to resolve client address!" << std::endl;
        return 404;
    }

    GtRemoteProcessRunner runner;

    return runner.exec(client, port, timeout * 1000);
}

void
printRunHelp()
{
    std::cout << std::endl;
    std::cout << "This is the help for the GTlab run function" << std::endl;
    std::cout << std::endl;

    std::cout << "There are two basic methods to start a process:" << std::endl;
    std::cout << "\tDefine the project by name from the current session"
                 "(default option or --name or -n)" << std::endl;
    std::cout  << "\tGTlabConsole.exe run -f <projectName> <processname> [-s] "
               << std::endl;

    std::cout << std::endl;
    std::cout << "\tDefine the project by file (use the option --file or -f"
              << std::endl;
    std::cout << "\tGTlabConsole.exe run [-n] <fileName> <processname> [-s]  "
              << std::endl;

    std::cout << std::endl;

    std::cout << "\tAdditionally you can set the option -s or --save"
              << std::endl;
    std::cout << "\tWith this option the results of the successfull process are"
              << " saved in the datamodel" << std::endl;

    std::cout << std::endl;
}

int
run(QStringList const& args)
{
    GtCommandLineParser p;
    p.addHelpOption();
    p.addOption("save", {"save", "s"},
                "save the process result after successfull run");
    p.addOption("file", {"file", "f"},
                "file to gtlab project");
    p.addOption("name", {"name", "n"},
                "name of project in current session");

    if (!p.parse(args))
    {
        qWarning() << "Run method without arguments is invalid";
        return -1;
    }

    if (p.helpOption())
    {
        printRunHelp();
        return 0;
    }

    bool save = false;

    if (p.option("save"))
    {
        save = true;
        std::cout << "Activate save option" << std::endl;
    }

    if (p.option("file"))
    {
        if (p.positionalArguments().size() != 2)
        {
            qWarning() << "Invalid usage of file option";
            return -1;
        }

        return runProcessByFile(p.positionalArguments().at(0),
                p.positionalArguments().at(1), save);
    }

    if (p.option("name"))
    {
        if (p.positionalArguments().size() != 2)
        {
            qWarning() << "Invalid usage of name option";
            return -1;
        }

        return runProcess(p.positionalArguments().at(0),
                p.positionalArguments().at(1), save);
    }

    return -1;
}

int
set_variable(const QStringList& args)
{
    if (args.size() != 2)
    {
        std::cerr << "Error: Invalid arguments\n\n";
        std::cout << "Usage: set_variable id value" << std::endl;
        return 255;
    }

    auto name = args[0];
    auto value = args[1];

    assert(gtApp && gtApp->settings());

    auto settings = gtApp->settings();

    // check, that the setting exists
    if (!settings->hasSetting(name))
    {
        gtError().nospace() << "Setting '" << name << "' does not exist";
        return 255;
    }

    std::cout << "\nSetting " << name.toStdString() << "="
              << value.toStdString() << std::endl;
    gtApp->settings()->setSetting(name, value);

    return 0;
}

int
list_variables(const QStringList&)
{
    assert(gtApp && gtApp->settings());

    auto settings = gtApp->settings();
    auto ids = settings->getAllSettingIds();

    std::cout << "\n\nThe following GTlab variables have been set:\n\n";

    for (auto&& id : ids)
    {
        std::cout << "  \"" << std::left << std::setw(50) << std::setfill(' ')
                  << id.toStdString() + "\""
                  << "\""
                  << settings->getSetting(id).toString().toStdString() << "\"";
        std::cout << std::endl;
    }

    return 0;
}

int
showFootprint(const QStringList& args)
{
    Q_UNUSED(args)
    std::cout << GtFootprint().exportToXML().toStdString() << std::endl;

    return 0;
}

int
listFun(const QStringList& args)
{
    std::cout << "List elements of GTlab application" << std::endl;

    GtCommandLineParser p;
    p.addHelpOption();
    p.addOption("project", {"project", "p"}, "show projects");
    p.addOption("session", {"session", "s"}, "show sessions");
    p.addOption("tasks", {"taks", "t"}, "show tasks");

    if (!p.parse(args))
    {
        std::cout << "List method without arguments is invalid" << std::endl;
        return -1;
    }

    if (p.helpOption())
    {
        std::cout << std::endl;
        std::cout << "This is the help for the GTlab list function" << std::endl;
        std::cout << std::endl;

        std::cout << "You can list the sessions, projects in the "
                     "current session or tasks in the project" << std::endl;

        std::cout << std::endl;
        std::cout << "\tUse the options" << std::endl;
        std::cout << "\tGTlabConsole.exe list [-s] [-p] [-t]=<ProjectID>"
                  << std::endl;

        std::cout << std::endl;

        std::cout << "\tFor the tasks the project has to be specified"
                  << std::endl;

        std::cout << std::endl;
        return 0;
    }

    bool anySelection = false;
    if (p.option("session"))
    {
        QStringList sessions = gtApp->sessionIds();

        std::cout << "Sessions in GTlab:" << std::endl;
        for (QString const& s : sessions)
        {
            std::cout << "\t" << s.toStdString() << "\n";
        }
        std::cout << std::endl;
        anySelection = true;
    }

    if (p.option("project"))
    {
        GtSession* s = gtApp->session();

        if (!s)
        {
            std::cout << "Cannot find current session" << std::endl;
            return -1;
        }

        QStringList projects = s->projectIds();

        std::cout << "Projects in GTlab session "
                  << s->objectName().toStdString()
                  << std::endl;
        for (QString const& pro : qAsConst(projects))
        {
            std::cout << "\t" << pro.toStdString() << "\n";
        }
        std::cout << std::endl;
        anySelection = true;
    }

    if (p.option("tasks"))
    {
        QString projectId = p.optionValue("tasks").toString();

        if (projectId.isEmpty())
        {
            std::cout << "Specify project id for taks list like "
                      << " list -t=<PROJECT_ID>" << std::endl;

            return -1;
        }

        GtProject* project = gtApp->findProject(projectId);

        if (!project)
        {
            std::cout << "The given project is not a valid project of the "
                         "current session";
            return -1;
        }

        if (!gtDataModel->GtCoreDatamodel::openProject(project))
        {
            std::cout << "Could not open the given project" << std::endl;

            return -1;
        }

        QStringList taskNames = project->taksIds();

        std::cout << "Taks in GTlab project "
                  << project->objectName().toStdString()
                  << std::endl;
        for (QString const& t : qAsConst(taskNames))
        {
            std::cout << "\t" << t.toStdString() << "\n";
        }
        anySelection = true;
    }

    if (!anySelection)
    {
        std::cout << "List elements needs at least one option to run"
                  << std::endl;
        std::cout << "open help for more details"
                  << std::endl;
    }


    return 0;
}


void
initPosArgument(QString const& id,
                std::function<int(const QStringList&)> func,
                QString const& brief,
                QList<GtCommandLineOption> const& options = {},
                QList<GtCommandLineArgument> const& args = {},
                bool defaultHelp = true)
{
    auto fun = gt::makeCommandLineFunction(id, func, brief);
    fun.setOptions(options)
            .setArgs(args)
            .setUseDefaultHelp(defaultHelp);

    gt::commandline::registerFunction(fun);
}

void
initSystemOptions()
{
    /// Add options for positional arguments to the command line interface
    initPosArgument("footprint", showFootprint, "Displays framework footprint");
    initPosArgument("check_meta", checkMetaInput,
                    "Checks given meta process data."
                    "\n\t\t\tUsage: check_meta <input.xml>");
    initPosArgument("run_meta", runMetaInput,
                    "Executes given meta process data. "
                    "Results are stored in given output file."
                    "\n\t\t\tUsage; run_meta <input.xml> <output.xml>");
    QList<GtCommandLineOption> runOptions;
    runOptions.append(GtCommandLineOption{
                          {"save", "s"},
                           "Saves datamodel after successfull process run"});
    runOptions.append(GtCommandLineOption{
                          {"name", "n"}, "Define project by name"});
    runOptions.append(GtCommandLineOption{
                          {"file", "f"}, "Define project by file"});
    initPosArgument("run", run,
                    "\tExecutes a process. \n\t\t\t"
                    "To define a project name and a process name is the "
                    "default used option to execute this command."
                    "\n\t\t\tUse --help for more details.",
                    runOptions,
                    QList<GtCommandLineArgument>(),
                    false);

    initPosArgument("list", listFun,
                    "\tShow list of session, projects or tasks.");

    initPosArgument("process_runner", processRunner, "Starts a TCP server, "
                    "which handles and executes task requests.",
                    {}, {}, false);

    initPosArgument("set_variable", set_variable,
                    "\tSets a global variable defined in settings",
                    {},
                    QList<GtCommandLineArgument>(),
                    true);

    initPosArgument("list_variables", list_variables,
                    "\tLists the contents of all variables");
}

int
initModuleTest(QStringList const& arguments, GtCoreApplication& app)
{
    GtCommandLineParser p;
    p.parse(arguments);

    if (p.positionalArguments().size() != 2)
    {
        qCritical().noquote() << QObject::tr("Error: missing module file\n");
        qInfo().noquote()     << QObject::tr("Usage: load_module <module_file_path>");
        return -1;
    }

    // extract path to the module to load
    QString moduleToLoad = p.positionalArguments().at(1);

    qDebug().noquote().nospace()
        << "\n" << QObject::tr("Try loading module '%1'\n").arg(moduleToLoad);

    // load GTlab modules
    bool success = app.loadSingleModule(moduleToLoad);

    // calculator initialization
    app.initCalculators();

    // initialize modules
    app.initModules();

    const auto status = success ? QObject::tr("SUCCESS") : QObject::tr("ERROR");

    qDebug().noquote() << QObject::tr("\n%1 loading module '%2'")
                              .arg(status, moduleToLoad);

    return success ? 0 : -1;
}


int main(int argc, char* argv[])
{
    QCoreApplication::setOrganizationDomain("www.dlr.de");
    QCoreApplication::setOrganizationName("DLR");
    QCoreApplication::setApplicationName("GTlab");

    QCoreApplication::setApplicationVersion(
                GtCoreApplication::version().toString());

    QApplication a(argc, argv);

    QStringList args = a.arguments();

    if (args.isEmpty())
    {
        return -1;
    }
    /// Remove the function name itself from arguments
    args.removeFirst();

    showSplashScreen();

    // Setup parser for main function
    GtConsoleParser parser;
    parser.addOption("dev",
                     {QStringLiteral("dev")},
                     "Activate the developer mode");
    parser.addHelpOption();
    parser.addOption("session",
                     {"session", "se"},
                     "Defines a session to be used for "
                     "execution."
                     "\n\t\t\tUsage: --session <session_id>");
    parser.addOption("version",
                     {"version", "v"},
                     "\tDisplays the version number of GTlab");

    // logging options (will be handled by app-init)
    parser.addOption("medium", {"medium"}, "Enable medium verbose output");
    parser.addOption("verbose", {"verbose"}, "\tEnable very verbose output");
    parser.addOption("trace", {"trace"}, "Enable trace output and higher");
    parser.addOption("debug", {"debug"}, "Enable debug output and higher");

    if (!parser.parse(args))
    {
        std::cout << "Parsing arguments failed" << std::endl;
        return -1;
    }

    /// Add options for positional arguments to the command line interface
    initSystemOptions();

    // application initialization
    GtCoreApplication app(qApp, GtCoreApplication::AppMode::Batch);

    // version option
    if (parser.option("version"))
    {
        std::cout << "GTlab version:" << std::endl;
        std::cout << app.version().toString().toStdString() << std::endl;
        return 0;
    }

    app.init();

    // save to system environment (temporary)
    app.saveSystemEnvironment();

    // language initialization
    app.initLanguage();

    // datamodel initialization
    app.initDatamodel();

    // Load module option
    if (parser.argument("load_module"))
    {
        return initModuleTest(parser.arguments(), app);
    }

    // load GTlab modules
    app.loadModules();

    // calculator initialization
    app.initCalculators();

    // session initialization
    if (parser.option("session"))
    {
        QString sessionValue = parser.optionValue("session").toString();
        std::cout << "Select session: " << sessionValue.toStdString()
                  << std::endl;
        app.initSession(sessionValue);
    }
    else
    {
        app.initSession();
    }

    if (!app.session())
    {
        qWarning() << "no session loaded!";
        return -1;
    }

    // initialize modules
    app.initModules();

    QStringList commands =
            GtCommandLineFunctionHandler::instance().getRegisteredFunctionIDs();

    for (QString const& s: qAsConst(commands))
    {
        GtCommandLineFunction f =
                GtCommandLineFunctionHandler::instance().getFunction(s);
        parser.addPositionalArgument(f);
    }

    QString mainArg = parser.firstPositionalArgument();
    if (mainArg.isEmpty())
    {
        /// show main application help if it is requested
        if (parser.helpOption())
        {
            parser.showHelp();
            return 0;
        }

        qWarning() << "No valid argument could be found in the arguments:";
        parser.debugArguments();
        return -1;
    }

    if (commands.contains(mainArg))
    {
        GtCommandLineFunction f =
                GtCommandLineFunctionHandler::instance().getFunction(mainArg);

        /// check if the default help flag is part of the arguments
        if (parser.helpOption())
        {
            if (f.useDefaultHelp())
            {
                f.showDefaultHelp();
                return 0;
            }
        }

        /// remove the argument which lead to this function call
        parser.removeArg(mainArg);

        /// if a customHelpIs
        return f(parser.arguments());
    }
    else
    {
        qCritical() << "Invalid command" << mainArg;
        return -1;
    }

    qWarning() << QObject::tr("invalid arguments! "
                              "use run --help for further information.");

    parser.showHelp();

    return -1;
}
