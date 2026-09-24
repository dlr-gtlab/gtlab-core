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
QList<GtCommandLineOption> runOperationFromMementoOptions();
int runOperationFromMemento(QStringList const& args);
}

#endif
