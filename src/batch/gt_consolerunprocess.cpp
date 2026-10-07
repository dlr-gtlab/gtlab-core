/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 *  Created on: 15.04.2024
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gt_consolerunprocess.h"
#include "gt_commandlineparser.h"

#include "gt_coredatamodel.h"
#include "gt_project.h"
#include "gt_projectprovider.h"
#include "gt_coreapplication.h"
#include "gt_coreprocessexecutor.h"
#include "gt_task.h"
#include "gt_processdata.h"


#include <QCommandLineOption>
#include <QCommandLineParser>

#include <iostream>
#include <ostream>

QList<GtCommandLineOption>
gt::console::runOptions()
{
    QList<GtCommandLineOption> runOptions;
    runOptions.append(GtCommandLineOption{
                          {"save", "s"},
                           "Saves datamodel after successfull process run"});
    runOptions.append(GtCommandLineOption{
                          {"name", "n"}, "Define project by name"});
    runOptions.append(GtCommandLineOption{
                          {"file", "f"}, "Define project by file"});
    runOptions.append(GtCommandLineOption{
                          {"output", "o"}, "Write project to output path"});
    runOptions.append(GtCommandLineOption{
                          {"set"},
                           "Sets a task property before running the task. "
                           "Use --set \"<path>=<value>\", e.g. "
                           "--set \"iterations=100\" or "
                           "--set \"Solver.tolerance=1e-6\". "
                           "This option can be repeated."});

    return runOptions;
}

int
gt::console::run(const QStringList &args)
{
    QCommandLineParser parser;

    parser.setApplicationDescription(
        QObject::tr("Executes a task of a GTlab project"));
    parser.addHelpOption();

    const QCommandLineOption saveOption {
        QStringList {"save", "s"},
        QObject::tr("Saves datamodel after successfull process run") };
    const QCommandLineOption nameOption {
        QStringList {"name", "n"},
        QObject::tr("Define project by name") };
    const QCommandLineOption fileOption {
        QStringList {"file", "f"},
        QObject::tr("Define project by file") };
    const QCommandLineOption outputOption {
        QStringList {"output", "o"},
        QObject::tr("Write project to output path"),
        QObject::tr("path") };
    const QCommandLineOption setOption {
        QStringList {"set"},
        QObject::tr("Sets a task property before running the task. "
                    "Use --set \"<path>=<value>\". This option can be "
                    "repeated."),
        QStringLiteral("path=value") };

    parser.addOptions({saveOption, nameOption, fileOption, outputOption,
                       setOption});

    QStringList commandLine {QStringLiteral("run")};
    commandLine.append(args);

    if (args.isEmpty())
    {
        gtError() << QObject::tr("Run method without arguments is invalid");
        return -1;
    }

    if (!parser.parse(commandLine))
    {
        std::cerr << "Run method arguments are invalid: "
                  << parser.errorText().toStdString() << std::endl;
        gtError() << QObject::tr("Parsing the run arguments failed");
        return -1;
    }

    if (parser.isSet(QStringLiteral("help")))
    {
        printRunHelp();
        return 0;
    }

    const bool save = parser.isSet(saveOption);

    if (save)
    {
        std::cout << "Activate save option" << std::endl;
    }

    // parse the repeated "--set" options into property overrides
    QStringList overrideErrors;
    const QList<PropertyOverride> overrides = parsePropertyOverrides(
        parser.values(setOption), &overrideErrors);

    if (!overrideErrors.isEmpty())
    {
        for (const QString& error : qAsConst(overrideErrors))
        {
            std::cerr << "ERROR: " << error.toStdString() << std::endl;
        }

        return -1;
    }

    QString taskGroup = "";

    const QStringList posArgs = parser.positionalArguments();
    size_t posArgSize = posArgs.size();

    if (parser.isSet(fileOption))
    {

        if (posArgSize == 3)
        {
            taskGroup = posArgs.at(2);
        }
        else if (posArgs.size() < 2 ||
                posArgs.size() > 3)
        {
            gtError() << QObject::tr("Invalid number of arguments of file option");
            return -1;
        }

        return runProcessByFile(posArgs.at(0),
                posArgs.at(1), taskGroup, save, overrides);
    }

    // default
    if (posArgSize == 3)
    {
        taskGroup = posArgs.at(2);
    }
    else if (posArgs.size() < 2 ||
             posArgs.size() > 3)
    {
        gtError() << QObject::tr("Invalid usage of file option");
        return -1;
    }

    return runProcess(posArgs.at(0),
                      posArgs.at(1),
                      taskGroup,
                      save,
                      overrides);
}

void
gt::console::printRunHelp()
{
    std::cout << std::endl;
    std::cout << "This is the help for the GTlab run function" << std::endl;
    std::cout << std::endl;

    std::cout << "There are two basic methods to start a process:" << std::endl;
    std::cout << "\tDefine the project by name from the current session "
                 "(default option or --name or -n)" << std::endl;
    std::cout << "\tGTlabConsole.exe run [-n] <projectName> <processname> [-s]  "
              << std::endl;

    std::cout << std::endl;
    std::cout << "\tDefine the project by file (use the option --file or -f"
              << std::endl;
    std::cout << "\tGTlabConsole.exe run -f <fileName> <processname> [-s] "
              << std::endl;

    std::cout << std::endl;

    std::cout << "\tIf the desired task is not part of the default task-group "
                 "define this (with an optional third argument)"
              << std::endl;
    std::cout << "\tGTlabConsole.exe run <projectName> <processname> <task-group-name> [-s] "
              << std::endl;

    std::cout << std::endl;

    std::cout << "\tAdditionally you can set the option -s or --save"
              << std::endl;
    std::cout << "\tWith this option the results of the successfull process are"
              << " saved in the datamodel" << std::endl;

    std::cout << std::endl;

    std::cout << "\tWith the repeatable option --set \"<path>=<value>\" you can"
              << std::endl;
    std::cout << "\toverwrite task properties before the task is executed."
              << std::endl;
    std::cout << "\tThe option can be used multiple times, the overrides are"
              << std::endl;
    std::cout << "\tapplied in command line order (the last value wins)."
              << std::endl;
    std::cout << "\tThe path is relative to the selected task:" << std::endl;
    std::cout << "\t\t\"iterations=100\"                  "
                 "property of the task itself" << std::endl;
    std::cout << "\t\t\"Solver.tolerance=1e-6\"           "
                 "property of a child object" << std::endl;
    std::cout << "\t\t\"Solver/My Calculator[1].relaxation=0.5\" "
                 "child objects are separated by '/', [n] selects the "
                 "nth child with the same name" << std::endl;
    std::cout << "\t\t\"Solver.points[2].pressure=420000\" "
                 "nth entry of a sequential property container"
              << std::endl;
    std::cout << "\t\t\"Solver.boundaries[{inlet}].pressure=420000\" "
                 "entry of an associative property container"
              << std::endl;
    std::cout << std::endl;
    std::cout << "\tExample:" << std::endl;
    std::cout << "\tGTlabConsole.exe run MyProject MyTask "
                 "--set \"iterations=100\" --set \"Solver.tolerance=1e-6\""
              << std::endl;
    std::cout << "\tIf any override cannot be applied, the task is not "
                 "executed" << std::endl;
    std::cout << "\tand the project is not saved." << std::endl;

    std::cout << std::endl;
}

int
gt::console::runProcess(const QString& projectId,
                        const QString& processId,
                        const QString& taskGroupId,
                        bool save,
                        const QList<PropertyOverride>& overrides)
{
    gtDebug() << QObject::tr("process run...");

    if (projectId.isEmpty())
    {
        gtError() << QObject::tr("Project id is empty!");

        return -1;
    }

    if (processId.isEmpty())
    {
        gtError() << QObject::tr("Process id is empty!");

        return -1;
    }

    GtProject* project = gtApp->findProject(projectId);

    if (!project)
    {
        gtError() << QObject::tr("Project not found!")
                  << QStringLiteral(" (") << projectId << QStringLiteral(")");

        return -1;
    }

    if (!gtDataModel->GtCoreDatamodel::openProject(project))
    {
        gtError() << QObject::tr("could not open project!")
                  << QStringLiteral(" (") << projectId << QStringLiteral(")");

        return -1;
    }

    gtDebug() << QObject::tr("project opened!");

    GtTask* process = getTask(project, processId, taskGroupId);

    if (!process)
    {
        gtError() << QObject::tr("Process not found!")
                  << QStringLiteral(" (") << processId << QStringLiteral(")");

        return -1;
    }

    // apply the property overrides before the execution. If any override
    // fails, the task is not executed and the project is not saved.
    for (const PropertyOverride& override : qAsConst(overrides))
    {
        const QString overrideError = applyPropertyOverride(
            *process, override.path, override.value);

        if (!overrideError.isEmpty())
        {
            std::cerr << "ERROR: "
                      << overrideError.toStdString() << std::endl;
            return -1;
        }

        std::cout << "Property override applied: "
                  << override.path.toStdString() << " = "
                  << override.value.toStdString() << std::endl;
    }

    // execute process
    gt::currentProcessExecutor().startTask(process);

    if (process->currentState() != GtProcessComponent::FINISHED)
    {
        gtError() << QObject::tr("Calculator run failed!");
        return -1;
    }

    gtDebug() << QObject::tr("process run successful!");

    if (save)
    {
        if (!gtDataModel->saveProject(project))
        {
            gtError() << QObject::tr("Project could not be saved!")
                      << QStringLiteral(" (") << projectId
                      << QStringLiteral(")");
            return -1;
        }
    }

    return 0;
}

int
gt::console::runProcessByFile(const QString& projectFile,
                              const QString& processId,
                              const QString& taskGroupId,
                              bool save,
                              const QList<PropertyOverride>& overrides)
{
    gtDebug() << QObject::tr("process run...");

    if (projectFile.isEmpty())
    {
        gtError() << QObject::tr("Project file is empty!");

        return -1;
    }

    if (processId.isEmpty())
    {
        gtError() << QObject::tr("Process id is empty!");

        return -1;
    }

    QFile file(projectFile);

    if (!file.exists())
    {
        gtError() << QObject::tr("project file")
                  << projectFile
                  << QObject::tr("not found!");

        return -1;
    }

    auto _ = enterTempSession();
    Q_UNUSED(_);

    GtProjectProvider provider(projectFile);
    GtProject* project = provider.project();

    if (!project)
    {
        gtError() << QObject::tr("Cannot load project");
        return -1;
    }

    gtApp->session()->appendChild(project);
    return runProcess(project->objectName(), processId, taskGroupId, save,
                      overrides);
}



GtTask*
gt::console::getTask(GtProject* project,
                     const QString& taskId, const QString& groupid)
{
    if (!project) return nullptr;

    if (groupid.isEmpty())
    {
        return project->findProcess(taskId);
    }

    GtProcessData* processData = project->processData();

    if (!processData)
    {
        gtError() << QObject::tr("Invalid Process data in project!")
                  << QStringLiteral(" (") << project->objectName()
                  << QStringLiteral(")");
        return nullptr;
    }

    bool check = processData->switchCurrentTaskGroup(groupid,
                                                     GtTaskGroup::CUSTOM,
                                                     project->path());

    if (!check)
    {
        check = processData->switchCurrentTaskGroup(groupid, GtTaskGroup::USER,
                                                    project->path());
    }

    if (!check)
    {
        gtError() << QObject::tr("Cannot switch to grouId '%1'!").arg(groupid);
        return nullptr;
    }

    return project->findProcess(taskId);
}
