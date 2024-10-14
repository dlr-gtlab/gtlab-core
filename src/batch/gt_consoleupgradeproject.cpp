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


int
gt::console::upgradeRoutine(const QString& projectFile,
                            const QString& projectFilePath)
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

    if (projectFilePath.isEmpty())
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
            QDir(projectFilePath),
            QFileInfo(projectFilePath).fileName()
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
gt::console::upgrade_project(const QStringList &args)
{
    GtCommandLineParser p;
    p.addHelpOption();

    for (const auto& o : gt::console::options())
    {
        p.addOption(o.names.first(), o);
    }

    if (!p.parse(args))
    {
        gtError() << QObject::tr("Run method without arguments is invalid");
        return -1;
    }

    if (p.helpOption())
    {
        gt::console::printRunHelp();
        return 0;
    }

    size_t posArgSize = p.positionalArguments().size();

    if (p.option("output"))
    {
        if (posArgSize == 2)
        {
            gtError() << p.positionalArguments().at(0);
            gtError() << p.positionalArguments().at(1);

            return upgradeRoutine(p.positionalArguments().at(0), p.positionalArguments().at(1));
        }
        else
        {
            gtError() << QObject::tr("Invalid usage of file option");
            return -1;
        }
    }

    //default
    if (posArgSize == 1)
    {
        return upgradeRoutine(p.positionalArguments().at(0));
    }
    else
    {
        gtError() << QObject::tr("Invalid usage of file option");
        return -1;
    }

    return 0;
}
