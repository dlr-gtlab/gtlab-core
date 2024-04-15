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

#include <QDebug>
#include <iostream>
#include <ostream>

int
gt::console::run(const QStringList &args)
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

    // default
    if (p.positionalArguments().size() != 2)
    {
        qWarning() << "Invalid usage of name option";
        return -1;
    }

    return runProcess(p.positionalArguments().at(0),
                      p.positionalArguments().at(1), save);
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
    std::cout << "\tGTlabConsole.exe run [-n] <fileName> <processname> [-s]  "
              << std::endl;

    std::cout << std::endl;
    std::cout << "\tDefine the project by file (use the option --file or -f"
              << std::endl;
    std::cout << "\tGTlabConsole.exe run -f <projectName> <processname> [-s] "
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
                        bool save)
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
    gt::currentProcessExecutor().runTask(process);

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
gt::console::runProcessByFile(const QString& projectFile,
                              const QString& processId,
                              bool save)
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
    auto& executor = gt::currentProcessExecutor();
    executor.setCoreExecutorFlags(gt::DryExecution);
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
