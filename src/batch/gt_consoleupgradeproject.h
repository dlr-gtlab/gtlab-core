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
 * @brief A batch command to upgrade a gtlab project
 * @param args Command line parameters passed from main
 *
 * @return 0 on success
 */
int upgradeProjectCommand(const QStringList& args);

}
}

#endif // GT_CONSOLEUPGRADEPROJECT_H
