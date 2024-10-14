/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 24.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <iostream>
#include <iomanip>
#include <gt_projectprovider.h>

#include <QApplication>
#include <QDomDocument>
#include <QObject>
#include <QDir>
#include <QDebug>
#include <QSettings>

#include "internal/gt_commandlinefunctionhandler.h"
#include "batchremote.h"
#include "gt_consolerunprocess.h"
#include "gt_consoleupgradeproject.h"

#include "gt_coreapplication.h"
#include "gt_coreprocessexecutor.h"

//#include "gt_coreapplication.h"
#include "gt_coredatamodel.h"
#include "gt_project.h"
#include "gt_footprint.h"
#include "gt_utilities.h"
#include "gt_consoleparser.h"
#include "gt_versionnumber.h"
#include "gt_hostinfo.h"
#include "gt_remoteprocessrunner.h"
#include "settings/gt_settings.h"

int list(const QStringList&);

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
        gtError() << QObject::tr("Invalid size of arguments for check meta!"
                                 "Exactly one argument is required");
        return -1;
    }

    QString fileName = p.positionalArguments().constFirst();

    gtDebug() << QObject::tr("meta input check...");

    if (fileName.isEmpty())
    {
        gtError() << QObject::tr("File name is empty!");
        return -1;
    }

    if (!gt::batch::checkInput(fileName,
                               GtCoreApplication::version().toString(),
                               false, false))
    {
        gtError() << QObject::tr("Meta input invalid!");
        return -1;
    }

    gtDebug() << QObject::tr("meta input ok!");
    return 0;
}

int
runMetaInput(const QStringList& args)
{
    GtCommandLineParser p;
    p.parse(args);

    if (p.positionalArguments().size() < 2)
    {
        gtError() << QObject::tr("Invalid arguments for runMetaInput.");
        gtError() << QObject::tr("Two arguments needed as <Input> <Output>");

        return -1;
    }


    QString inputFileName = p.positionalArguments().constFirst();
    QString outputFileName = p.positionalArguments().at(1);

    gtDebug() << QObject::tr("meta input run...");

    if (inputFileName.isEmpty())
    {
        gtError() << QObject::tr("input file name is empty!");
        return -1;
    }

    if (outputFileName.isEmpty())
    {
        gtError() << QObject::tr("output file name is empty!");
        return -1;
    }

    gtDebug() << "   input file name: " << inputFileName;
    gtDebug() << "   output file name: " << outputFileName;

    int inputCheck = checkMetaInput({inputFileName});

    if (inputCheck != 0)
    {
        gtError() << QObject::tr("Meta input run failed!");
        return inputCheck;
    }

    gtDebug() << QObject::tr("executing process...");

    if (!gt::batch::run(inputFileName, outputFileName,
                        GtCoreApplication::version().toString()))
    {
        gtError() << QObject::tr("process execution failed!");
        return -1;
    }

    gtDebug() << QObject::tr("meta input run successful!");

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
enableModules(const QStringList& args)
{
    GtCommandLineParser p;
    p.addHelpOption();
    p.addOption("all", {"all", "a"},
                "Reenables/clears all disabled modules");

    QStringList disabledModules = GtCoreApplication::crashedModules();

    p.parse(args);

    std::cout << std::endl;

    if (p.helpOption())
    {
        std::cout << std::endl;
        std::cout << "This is the help for the 'enable_modules' function:"
                  << std::endl;
        std::cout << std::endl;

        std::cout << "To reenable a module pass the module library name as a "
                     "positional argument."
                  << std::endl;
        std::cout << "Alternatively use option --all to reenable all modules."
                  << std::endl;
        std::cout << std::endl;

        std::cout << "\tUsage: enable_modules [--all] <module_a> <module_b> ..."
                  << std::endl;
        std::cout << std::endl;
        return 0;
    }

    QSettings settings(GtCoreApplication::localApplicationIniFilePath(),
                       QSettings::IniFormat);

    // enable all
    if (p.option("all"))
    {
        settings.setValue(QStringLiteral("loading_crashed"), QStringList{});
        settings.sync();
        std::cout << "Successfully cleared list of disabled modules!"
                  << std::endl;
        std::cout << std::endl;
        return 0;
    }

    // reenable specific modules
    if (!disabledModules.empty())
    {
        auto const posArgs = p.positionalArguments();

        if (posArgs.empty())
        {
            std::cout << "Invalid module libraries specified! "
                         "Reenable modules using 'enable_modules'"
                         " \"Module_A\" \"Module_B\" ...\""
                      << std::endl;
            return -1;
        }

        std::cout << "Reenabling modules..." << std::endl;
        std::cout << std::endl;

        for (QString module : posArgs)
        {
            module = module.trimmed();
            QString m = QChar{'/'} + module + QChar{'.'};
            // Match module name with module path. Example:
            // GTlabBasicTools
            // /path/to/GTlab/modules/GTlabBasicTools.dll'
            //                       ^               ^
            //                     prefix          suffix
            auto iter = std::find_if(std::begin(disabledModules),
                                     std::end(disabledModules),
                                     [&](QString const& path){
                return path.contains(m);
            });

            if (iter == std::end(disabledModules))
            {
                std::cout << "\tThe module '" << module.toStdString()
                          << "' is currently not disabled!" << std::endl;
                continue;
            }

            std::cout << "\tSucessfully reenabled the Module "
                      << gt::squoted(module.toStdString()) << std::endl;
            disabledModules.erase(iter);
        }

        settings.setValue(QStringLiteral("loading_crashed"), disabledModules);
        settings.sync();
    }

    // list disabled modules
    if (disabledModules.empty())
    {
        std::cout << std::endl;
        std::cout << "No more modules are currently disabled!" << std::endl;
        std::cout << std::endl;
        return 0;
    }

    // delegate to list command to list all active and still disabled modules
    return list(QStringList{"--modules"});
}

int
showFootprint(const QStringList& args)
{
    Q_UNUSED(args)
    std::cout << GtFootprint().exportToXML().toStdString() << std::endl;

    return 0;
}

int
list(const QStringList& args)
{
    GtCommandLineParser p;
    p.addHelpOption();
    p.addOption("project", {"project", "p"},
                "Lists all projects in the active projects");
    p.addOption("session", {"session", "s"}, "Lists all avaiable sessions");
    p.addOption("tasks", {"taks", "t"}, "Lists all tasks of a project");
    p.addOption("modules", {"modules", "m"},
                "Lists active and disabled modules");

    if (!p.parse(args))
    {
        std::cout << std::endl;
        std::cout << "Invalid arguments. Use --help for more deails." << std::endl;
        std::cout << std::endl;
        return -1;
    }

    if (p.helpOption())
    {
        std::cout << std::endl;
        std::cout << "This is the help for the 'list' function:" << std::endl;
        std::cout << std::endl;

        std::cout << "You can list the all loaded and disabled modules, the "
                     "available sessions, the projects in the current session "
                     "or the tasks in the project specified." << std::endl;
        std::cout << std::endl;

        std::cout << "The following options are available:" << std::endl;
        std::cout << std::endl;

        std::cout << "\t-m\tlists all active and disabled modules" << std::endl
                  << "\t-s\tList all available sessions" << std::endl
                  << "\t-p\tDisplays all projects in the current session" << std::endl
                  << "\t-t\tLists all taks of the project provided." << std::endl;

        std::cout << "\t\tRequires a project Id"
                  << std::endl;
        std::cout << std::endl;

        std::cout << "\tUsage: list [-m] [-s] [-p] [-t]=<ProjectID>"
                  << std::endl;

        std::cout << std::endl;
        return 0;
    }

    bool anySelection = false;
    if (p.option("modules"))
    {
        anySelection = true;
        std::cout << std::endl;

        // list enabled modules
        QStringList const modules = gtApp->moduleIds();

        std::cout << "Active modules:" << std::endl;
        std::cout << std::endl;

        for (QString const& s : modules)
        {
            std::cout << "\t" << std::left << std::setw(30) << std::setfill(' ')
                      << s.toStdString()
                      << gtApp->moduleVersion(s).toString().toStdString()
                      << std::endl;
        }

        // list disabled modules
        QStringList const disabledModules = GtCoreApplication::crashedModules();

        if (!disabledModules.empty())
        {
            std::cout << std::endl;
            std::cout << "The following modules are currently disabled due to "
                         "a previous application crash:" << std::endl;
            std::cout << "(Use 'enable_modules --all' to clear all disabled "
                         "modules)" << std::endl;
            std::cout << std::endl;

            for (auto&& id : disabledModules)
            {
                std::cout << "\t\"" << id.toStdString() + "\"";
                std::cout << std::endl;
            }
        }
    }

    if (p.option("session"))
    {
        anySelection = true;
        std::cout << std::endl;

        QStringList const sessions = gtApp->sessionIds();
        QString currentSession;

        if (GtSession* s = gtApp->session())
        {
            currentSession = s->objectName();
        }

        std::cout << "Available sessions:" << std::endl;
        std::cout << std::endl;

        for (QString const& s : sessions)
        {
            std::cout << "\t" << s.toStdString();
            // highlight current session
            if (currentSession == s)
            {
                std::cout << " *";
            }
            std::cout << std::endl;
        }
    }

    if (p.option("project"))
    {
        anySelection = true;
        std::cout << std::endl;

        GtSession* s = gtApp->session();
        if (!s)
        {
            std::cout << "Cannot find the current session!" << std::endl;
            return -1;
        }

        QStringList const projects = s->projectIds();

        std::cout << "Projects in the current session "
                  << gt::squoted(s->objectName().toStdString()) << ':' << std::endl;
        std::cout << std::endl;

        for (QString const& pro : projects)
        {
            std::cout << "\t" << pro.toStdString() << std::endl;
        }
    }

    if (p.option("tasks"))
    {
        anySelection = true;
        std::cout << std::endl;

        QString projectId = p.optionValue("tasks").toString();
        if (projectId.isEmpty())
        {
            std::cout << "Invalid project specified! Set the project id using "
                      << "list -t=<PROJECT_ID>" << std::endl;
            return -1;
        }

        GtProject* project = gtApp->findProject(projectId);
        if (!project)
        {
            std::cout << "The given project is not in the current session!"
                      << std::endl;
            return -1;
        }

        if (!gtDataModel->GtCoreDatamodel::openProject(project))
        {
            std::cout << "Could not open the given project!" << std::endl;
            return -1;
        }

        QMap<QString, QStringList> const taskNamesMap = project->fullTaskIds();

        std::cout << std::endl;
        std::cout << "Taks in the project "
                  << gt::squoted(project->objectName().toStdString()) << ':' << std::endl;

        for (QString const& group : taskNamesMap.keys())
        {
            std::cout << std::endl;
            std::cout << "\t" << group.toStdString() << ':' << std::endl;
            std::cout << std::endl;

            for (QString const& t : taskNamesMap.value(group))
            {
                std::cout << "\t  " << t.toStdString() << std::endl;
            }
        }
    }

    std::cout << std::endl;

    if (!anySelection)
    {
        std::cout << "The command 'list' requires at least one option to run" << std::endl;
        std::cout << "Use --help for more details." << std::endl;
        std::cout << std::endl;
        return -1;
    }

    return 0;
}

int
create_session(const QStringList& args)
{
    if (args.size() != 1)
    {
        // print usage message
        std::cout << QObject::tr("create_session: Invalid arguments\n\n")
                         .toStdString();

        auto func = GtCommandLineFunctionHandler::instance().getFunction(
            "create_session");

        assert(func);

        func.showDefaultHelp();

        return 1;
    }

    auto sessionID = args[0];

    if (!gtApp->newSession(sessionID))
    {
        return 1;
    }

    std::cout << QObject::tr("Session '%1' has been created\n")
                     .arg(sessionID)
                     .toStdString();
    return 0;
}

int
delete_session(const QStringList& args)
{
    if (args.size() != 1)
    {
        // print usage message
        std::cout << QObject::tr("delete_session: Invalid arguments\n\n")
                         .toStdString();

        auto func = GtCommandLineFunctionHandler::instance().getFunction(
            "delete_session");

        assert(func);

        func.showDefaultHelp();

        return 1;
    }
    auto sessionID = args[0];
    if (!gtApp->deleteSession(sessionID))
    {
        return 1;
    }
    std::cout << QObject::tr("Session '%1' has been deleted\n")
                     .arg(sessionID)
                     .toStdString();
    return 0;
}

int
switch_session(const QStringList& args)
{
    if (args.size() != 1)
    {
        // print usage message
        std::cout << QObject::tr("switch_session: Invalid arguments\n\n")
                         .toStdString();

        auto func = GtCommandLineFunctionHandler::instance().getFunction(
            "switch_session");

        assert(func);

        func.showDefaultHelp();

        return 1;
    }
    auto const& sessionID = args[0];
    gtApp->switchSession(sessionID);
    if (gtApp->sessionIds().contains(sessionID))
    {
        std::cout << QObject::tr("Switched to Session '%1'\n")
                         .arg(sessionID)
                         .toStdString();
    }
    else
    {
        std::cout << QObject::tr("Session '%1' doesn't exist\n")
                         .arg(sessionID)
                         .toStdString();
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

    // run a process
    initPosArgument("run", gt::console::run,
                    "\tExecutes a process. \n\t\t\t"
                    "To define a project name and a process name is the "
                    "default used option to execute this command."
                    "\n\t\t\tUse --help for more details.",
                    gt::console::options(),
                    QList<GtCommandLineArgument>(),
                    false);

    initPosArgument("list", list,
                    "\tShows list of modules, session, projects and tasks.",
                    {}, {}, false);

    initPosArgument("process_runner", processRunner, "Starts a TCP server, "
                    "which handles and executes task requests.",
                    {}, {}, false);

    initPosArgument("set_variable", set_variable,
                    "\tSets a global variable that already exists in settings.",
                    {},
                    QList<GtCommandLineArgument>(),
                    true);
    initPosArgument("list_variables", list_variables,
                    "\tLists the contents of all variables.");

    initPosArgument("enable_modules", enableModules,
                    "\tEnables the modules specified. A module is disabled if "
                    "it caused a crash on a previous application run.",
                    {}, {}, false);

    initPosArgument("create_session", create_session,
                    "Creates a session if it doesn't exist already", {},
                    {GtCommandLineArgument{"session_id", "Session ID"}});

    initPosArgument("delete_session", delete_session,
                    "Deletes the given session", {},
                    {GtCommandLineArgument{"session_id", "Session ID"}});

    initPosArgument("switch_session", switch_session,
                    "Switches to the given session", {},
                    {GtCommandLineArgument{"session_id", "Session ID"}});

    initPosArgument("upgrade_project", gt::console::upgrade_project,
                    "Upgrades All Modules in the current project",
                    gt::console::options(),
                    QList<GtCommandLineArgument>(),
                    false);
}

int
initModuleTest(QStringList const& arguments, GtCoreApplication& app)
{
    GtCommandLineParser p;
    p.parse(arguments);

    if (p.positionalArguments().size() != 2)
    {
        gtError().noquote() << QObject::tr("Missing module file\n");
        gtInfo().noquote() << QObject::tr("Usage: load_module <module_file_path>");
        return -1;
    }

    // extract path to the module to load
    QString moduleToLoad = p.positionalArguments().at(1);

    gtDebug().noquote().nospace()
        << "\n" << QObject::tr("Try loading module '%1'\n").arg(moduleToLoad);

    // load GTlab modules
    bool success = app.loadSingleModule(moduleToLoad);

    // calculator initialization
    app.initCalculators();

    // initialize modules
    app.initModules();

    const auto status = success ? QObject::tr("SUCCESS") : QObject::tr("ERROR");

    gtDebug().noquote() << QObject::tr("\n%1 loading module '%2'")
                              .arg(status, moduleToLoad);

    return success ? 0 : -1;
}


int main(int argc, char* argv[])
{

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
    parser.addOption("medium", {"medium"}, "Enables medium verbose output");
    parser.addOption("verbose", {"verbose"}, "\tEnables very verbose output");
    parser.addOption("trace", {"trace"}, "Enables trace output and higher");
    parser.addOption("debug", {"debug"}, "Enables debug output and higher");

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
        gtError() << QObject::tr("no session loaded!");
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

        gtError() << QObject::tr("No valid argument could be found in the "
                                 "arguments:");
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


        // remove all argument before and including the command
        auto commandArgsStartIdx = parser.arguments().indexOf(mainArg) + 1;
        assert(commandArgsStartIdx > 0);
        return f(parser.arguments().mid(commandArgsStartIdx));
    }
    else
    {
        gtError() << QObject::tr("Invalid command") << mainArg;
        return -1;
    }

    gtError() << QObject::tr("invalid arguments! "
                             "use run --help for further information.");

    parser.showHelp();

    return -1;
}
