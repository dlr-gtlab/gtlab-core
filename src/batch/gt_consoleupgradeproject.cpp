/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 *  Created on: 14.10.2024
 *  Author: Jannis Kruse (AT-TWK)
 *  E-Mail: jannis.kruse@dlr.de
 */

#include "gt_consoleupgradeproject.h"

#include "gt_application.h"
#include "gt_project.h"
#include "gt_projectprovider.h"
#include "gt_consolerunprocess.h"
#include "gt_datamodel.h"

#include <gt_commandlineparser.h>
#include <iostream>

void
gt::console::printUpgradeProjectHelp()
{
    std::cout << std::endl;
    std::cout << "This is the help for the GTlab upgrade_project function" << std::endl;
    std::cout << std::endl;

    std::cout << "There are two basic methods to upgrade a project:" << std::endl;
    std::cout << "\tDefine the project by name from the current session and overwrite the project with the upgraded version"
                 "(default option)" << std::endl;
    std::cout << "A backup is still created!" << std::endl;

    std::cout << "\tGTlabConsole.exe upgrade_project <projectPath>"
              << std::endl;

    std::cout << std::endl;
    std::cout << "\tDefine the destination of the upgraded project by output path (use the option --output or -o"
              << std::endl;
    std::cout << "\tGTlabConsole.exe overwrite_project -o <projectPath> <outputPath>"
              << std::endl;
}

int
gt::console::upgradeRoutine(const QString& projectFile,
                            const QString& newProjectFilePath)
{
    GtProject* project = gtApp->findProject(projectFile);

    if (!project)
    {
        gtError() << QObject::tr("Project not found!")
        << QStringLiteral(" (") << projectFile << QStringLiteral(")");

        return -1;
    }

    if (!project->upgradesAvailable())
    {
        gtDebug() << "Project is up to date no further upgrades needed at the moment.";
        return 0;
    }

    if (newProjectFilePath.isEmpty())
    {
        gtDebug() << "backup and overwriting project data...";
        project->createBackup();
        project->upgradeProjectData();

        return 1;
    }
    else
    {
        gtDebug() << "upgrading data as new project...";
        auto newProject = GtProjectProvider::duplicateExistingProject(
            QDir(project->path()),
            QDir(newProjectFilePath),
            QFileInfo(newProjectFilePath).fileName()
            );

        if (!newProject)
        {
            gtError() << "Could not save project to new directory";
            return 0;
        }

        newProject->upgradeProjectData();
        gtDataModel->newProject(newProject.release(), false);

        return 1;
    }

    gtDebug() << "Project is up to date no further upgrades needed at the moment.";

    return 0;
}

int
gt::console::upgrade_project(const QStringList &upgradeProjectArguments)
{
    GtCommandLineParser upgradeProjectParser;
    upgradeProjectParser.addHelpOption();

    auto outputOption = GtCommandLineOption{{"output", "o"}, "Write project to output path"};

    upgradeProjectParser.addOption(outputOption.names.first(), outputOption);

    if (!upgradeProjectParser.parse(upgradeProjectArguments))
    {
        gtError() << QObject::tr("running upgrade_project without arguments is invalid");
        return -1;
    }

    if (upgradeProjectParser.helpOption())
    {
        gt::console::printRunHelp();
        return 0;
    }

    size_t posArgSize = upgradeProjectParser.positionalArguments().size();

    //positionalArgument 1 is the path to the project which is to be upgraded
    // positionalArgument 2 is the path to the upgraded project if an output path is defined on the command line

    if (upgradeProjectParser.option("output"))
    {
        if (posArgSize != 2)
        {
            gtError() << QObject::tr("Invalid usage of output option");
            return -1;
        }

        return upgradeRoutine(upgradeProjectParser.positionalArguments().at(0), upgradeProjectParser.positionalArguments().at(1));
    }

    //default
    if (posArgSize == 1)
    {
        return upgradeRoutine(upgradeProjectParser.positionalArguments().at(0));
    }
    else
    {
        gtError() << QObject::tr("Invalid usage of upgrade_project routine");
        return -1;
    }

    return 0;
}
