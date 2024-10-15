/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 *  Created on: 14.10.2024
 *  Author: Jannis Kruse (AT-TWK)
 *  E-Mail: jannis.kruse@dlr.de
 */

#ifndef GT_CONSOLEUPGRADEPROJECT_H
#define GT_CONSOLEUPGRADEPROJECT_H

#include <QStringList>

namespace gt
{
namespace console
{

/**
 * @brief printRunHelp
 */
void printUpgradeProjectHelp();

/**
 * @brief upgradeRoutine
 * @param projectFile
 * @param newProjectFilePath
 * @param newProjectName
 * @return
 */
int upgradeRoutine(const QString& projectFile,
                   const QString& newProjectFilePath = "");

/**
 * @brief upgrade_Project
 * @param upgradeProjectArguments
 * @return
 */
int upgrade_project(const QStringList& upgradeProjectArguments);

}
}

#endif // GT_CONSOLEUPGRADEPROJECT_H
