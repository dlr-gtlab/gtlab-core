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

#include "gt_project.h"
#include "gt_projectprovider.h"
#include "gt_consolerunprocess.h"

#include <gt_commandlineparser.h>
#include <iostream>

namespace
{

void
printUpgradeProjectHelp()
{
    std::cout << std::endl;
    std::cout << "This is the help for the GTlab upgrade_project function\n\n";

    std::cout << "There are two basic methods to upgrade a project:\n\n"
              << "1. Define the project by name from the current session "
                 "and overwrite the project with the upgraded version"
                 "(default option). A backup is still created!: \n\n";

    std::cout << "\tGTlabConsole.exe upgrade_project <projectPath>\n\n";

    std::cout << "2. Define the destination of the upgraded project "
                 "by output path (use the option --output or -o):\n\n"
              << "\tGTlabConsole.exe overwrite_project -o <projectPath> <outputPath>"
              << std::endl;
}

int
upgradeRoutine(const QString& projectPath,
               const QString& newProjectFilePath = "")
{
    QFileInfo fi(projectPath);
    QString projectFile = projectPath;

    if (fi.isDir())
    {
        projectFile = projectPath + QDir::separator() + "project.gtlab";
    }

    QFile file(projectFile);

    if (!file.exists())
    {
        gtError() << QObject::tr("Project file %1 not found").arg(projectFile);
        return -1;
    }

    // we need to create a temporary session in which the project is imported
    auto tmpSession = gt::console::enterTempSession();
    Q_UNUSED(tmpSession);

    GtProjectProvider provider(projectFile);
    GtProject* project = provider.project();

    if (!project)
    {
        gtError() << QObject::tr("Cannot load project");
        return -1;
    }

    if (!project->upgradesAvailable())
    {
        gtInfo() <<  QObject::tr("Project is up to date. "
                                 "No further upgrades needed at the moment.");
        return 0;
    }

    // Return 0 if the upgrade was successful
    if (project->upgradeProject(newProjectFilePath)) {
        gtInfo() << QObject::tr("Project %1 updated successfully")
                        .arg(project->objectName());
        return 0;
    }

    else return -1;
}

} // namespace


int
gt::console::upgradeProjectCommand(const QStringList &upgradeProjectArguments)
{
    GtCommandLineParser upgradeProjectParser;
    upgradeProjectParser.addHelpOption();

    auto outputOption = GtCommandLineOption{{"output", "o"},
                                            "Write project to output path"};

    upgradeProjectParser.addOption(outputOption.names.first(), outputOption);

    if (!upgradeProjectParser.parse(upgradeProjectArguments))
    {
        std::cerr << QObject::tr("\n\nrunning upgrade_project "
                                 "without arguments is invalid\n\n")
                         .toStdString();
        return -1;
    }

    if (upgradeProjectParser.helpOption())
    {
        printUpgradeProjectHelp();
        return 0;
    }

    size_t posArgSize = upgradeProjectParser.positionalArguments().size();

    // positionalArgument 1 is the path to the project which is to be upgraded
    // positionalArgument 2 is the path to the upgraded project
    //   if an output path is defined on the command line

    if (upgradeProjectParser.option("output"))
    {
        if (posArgSize != 2)
        {
            std::cerr << QObject::tr("\n\nInvalid usage "
                                     "of output argument!\n\n").toStdString();
            printUpgradeProjectHelp();
            return -1;
        }

        return upgradeRoutine(upgradeProjectParser.positionalArguments().at(0),
                              upgradeProjectParser.positionalArguments().at(1));
    }

    //default
    if (posArgSize == 1)
    {
        return upgradeRoutine(upgradeProjectParser.positionalArguments().at(0));
    }
    else
    {
        std::cerr << QObject::tr("\n\nInvalid usage "
                                 "of upgrade_project routine!\n\n")
                         .toStdString();
        printUpgradeProjectHelp();
        return -1;
    }

    return 0;
}
