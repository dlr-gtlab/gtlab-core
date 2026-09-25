/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTCONSOLERUNOPERATIONFROMMEMENTO_H
#define GTCONSOLERUNOPERATIONFROMMEMENTO_H

#include "gt_commandlineparser.h"

#include <QStringList>

namespace gt::console
{

    /**
 * @brief Returns the command-line options supported by the operation runner.
 * @return Options for operation, data, project, events and working directory.
 */
    QList<GtCommandLineOption> runOperationFromMementoOptions();

    /**
 * @brief Executes an operation restored from command-line Memento files.
 * @param args Command-line arguments passed to the operation runner.
 * @return Process exit code; zero indicates successful execution.
 */
    int runOperationFromMemento(QStringList const& args);

} // namespace gt::console

#endif
