/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2023 by DLR
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

#include <gt_logging.h>
#include <iostream>
#include <ostream>

QList<GtCommandLineOption>
gt::console::options()
{
    QList<GtCommandLineOption> runOptions;
    runOptions.append(GtCommandLineOption{
                          {"save", "s"},
                           "Saves datamodel after successfull process run"});
    runOptions.append(GtCommandLineOption{
                          {"name", "n"}, "Define project by name"});
    runOptions.append(GtCommandLineOption{
                          {"file", "f"}, "Define project by file"});

    return runOptions;
}

int
gt::console::run(const QStringList &args)
{
    GtCommandLineParser p;
    p.addHelpOption();

    for (const auto& o : options())
    {
        p.addOption(o.names.first(), o);
    }

    if (!p.parse(args))
    {
        gtWarning() << QObject::tr("Run method without arguments is invalid");
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

    QString taskGroup = "";

    size_t posArgSize = p.positionalArguments().size();

    if (p.option("file"))
    {

        if (posArgSize == 3)
        {
            taskGroup = p.positionalArguments().at(2);
        }
        else if (p.positionalArguments().size() < 2 ||
                p.positionalArguments().size() > 3)
        {
            gtWarning() << QObject::tr("Invalid usage of file option");
            return -1;
        }

        return runProcessByFile(p.positionalArguments().at(0),
                p.positionalArguments().at(1), taskGroup, save);
    }

    // default
    if (posArgSize == 3)
    {
        taskGroup = p.positionalArguments().at(2);
    }
    else if (p.positionalArguments().size() < 2 ||
            p.positionalArguments().size() > 3)
    {
        gtWarning() << QObject::tr("Invalid usage of file option");
        return -1;
    }

    return runProcess(p.positionalArguments().at(0),
                      p.positionalArguments().at(1),
                      taskGroup,
                      save);
}

void
gt::console::printRunHelp()
{
    std::cout << std::endl;
    std::cout << "This is the help for the GTlab run function" << std::endl;
    std::cout << std::endl;

    std::cout << "There are two basic methods to start a process:" << std::endl;
    std::cout << "\tDefine the project by name from the current session"
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
                 "define this (with a optional thir argument)"
              << std::endl;
    std::cout << "\tGTlabConsole.exe run <projectName> <processname> <task-group-name> [-s] "
              << std::endl;

    std::cout << std::endl;

    std::cout << "\tAdditionally you can set the option -s or --save"
              << std::endl;
    std::cout << "\tWith this option the results of the successfull process are"
              << " saved in the datamodel" << std::endl;

    std::cout << std::endl;
}

int
gt::console::runProcess(const QString& projectId,
                        const QString& processId,
                        const QString& taskGroupId,
                        bool save)
{
    gtDebug() << QObject::tr("process run...");

    if (projectId.isEmpty())
    {
        gtWarning() << QStringLiteral("ERROR: ")
                    << QObject::tr("project id is empty!");

        return -1;
    }

    if (processId.isEmpty())
    {
        gtWarning() << QStringLiteral("ERROR: ")
                    << QObject::tr("process id is empty!");

        return -1;
    }

    GtProject* project = gtApp->findProject(projectId);

    if (!project)
    {
        gtWarning() << QStringLiteral("ERROR: ")
                    << QObject::tr("project not found!")
                    << QStringLiteral(" (") << projectId << QStringLiteral(")");

        return -1;
    }

    if (!gtDataModel->GtCoreDatamodel::openProject(project))
    {
        gtWarning() << QStringLiteral("ERROR: ")
                    << QObject::tr("could not open project!")
                    << QStringLiteral(" (") << projectId << QStringLiteral(")");

        return -1;
    }

    gtDebug() << QObject::tr("project opened!");

    GtTask* process = nullptr;

    if (taskGroupId.isEmpty())
    {
        process = project->findProcess(processId);
    }
    else
    {
        process = getTask(project, taskGroupId, processId);
    }

    if (!process)
    {
        gtWarning() << QStringLiteral("ERROR: ")
                    << QObject::tr("process not found!")
                    << QStringLiteral(" (") << processId << QStringLiteral(")");

        return -1;
    }

    // execute process
    gt::currentProcessExecutor().runTask(process);

    if (process->currentState() != GtProcessComponent::FINISHED)
    {
        gtWarning() << QObject::tr("Calculator run failed!");
        return -1;
    }

    gtDebug() << QObject::tr("process run successful!");

    if (save)
    {
        if (!gtDataModel->saveProject(project))
        {
            gtWarning() << QStringLiteral("ERROR: ")
                        << QObject::tr("project could not be saved!")
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
                              bool save)
{
    gtDebug() << QObject::tr("process run...");

    if (projectFile.isEmpty())
    {
        gtWarning() << QStringLiteral("ERROR: ")
                    << QObject::tr("project file is empty!");

        return -1;
    }

    if (processId.isEmpty())
    {
        gtWarning() << QStringLiteral("ERROR: ")
                    << QObject::tr("process id is empty!");

        return -1;
    }

    QFile file(projectFile);

    if (!file.exists())
    {
        gtWarning() << QStringLiteral("ERROR: ")
                    << QObject::tr("project file")
                    << projectFile
                    << QObject::tr("not found!");

        return -1;
    }

    GtProjectProvider provider(projectFile);
    GtProject* project = provider.project();

    if (!project)
    {
        gtError() << QObject::tr("Cannot load project");
        return -1;
    }

    gtApp->session()->appendChild(project);

    if (!gtDataModel->GtCoreDatamodel::openProject(project))
    {
        gtWarning() << QStringLiteral("ERROR: ")
                    << QObject::tr("could not open project!")
                    << QStringLiteral(" (") << projectFile
                    << QStringLiteral(")");

        return -1;
    }

    gtDebug() << QObject::tr("project opened!");

    // run process
    GtTask* process = project->findProcess(processId);
    if (!process)
    {
        gtWarning() << QStringLiteral("ERROR: ")
                    << QObject::tr("process not found!")
                    << QStringLiteral(" (") << processId << QStringLiteral(")");

        return -1;
    }

    // execute process
    auto& executor = gt::currentProcessExecutor();
    executor.setCoreExecutorFlags(gt::DryExecution);
    executor.runTask(process);

    if (process->currentState() != GtProcessComponent::FINISHED)
    {
        gtWarning() << QObject::tr("Calculator run failed!");
        return -1;
    }

    gtDebug() << QObject::tr("process run successful!");

    if (save)
    {
        if (!gtDataModel->saveProject(project))
        {
            gtWarning() << QStringLiteral("ERROR: ")
                        << QObject::tr("project could not be saved!")
                        << QStringLiteral(" (") << projectFile
                        << QStringLiteral(")");
            return -1;
        }
    }

    return 0;
}



GtTask*
gt::console::getTask(GtProject* project, const QString& groupid,
                     const QString& taskId)
{
    if (!project) return nullptr;

    GtProcessData* processData = project->processData();

    if (!processData)
    {
        gtError() << QStringLiteral("ERROR:")
                  << QObject::tr("Invalid Process data in project!")
                  << QStringLiteral(" (") << project->objectName()
                  << QStringLiteral(")");
        return nullptr;
    }

    bool check = processData->switchCurrentTaskGroup(groupid,
                                                     GtTaskGroup::CUSTOM,
                                                     project->path());

    if (!check)
    {
        processData->switchCurrentTaskGroup(groupid, GtTaskGroup::USER,
                                            project->path());
    }

    if (!check)
    {
        gtError() << QStringLiteral("ERROR:")
                  << QObject::tr("Cannot switch to grouId '%1'!").arg(groupid);
        return nullptr;
    }

    return project->findProcess(taskId);
}
