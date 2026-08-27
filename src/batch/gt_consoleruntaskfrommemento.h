/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTCONSOLERUNTASKFROMMEMENTO_H
#define GTCONSOLERUNTASKFROMMEMENTO_H

#include "gt_commandlineparser.h"

#include <QStringList>

namespace gt::console
{

    QList<GtCommandLineOption> runTaskFromMementoOptions();

    int runTaskFromMemento(QStringList const& args);

} // namespace gt::console

#endif // GTCONSOLERUNTASKFROMMEMENTO_H
