/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2023 by DLR
 *
 *  Created on: 15.04.2024
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTCONSOLERUNPROCESS_H
#define GTCONSOLERUNPROCESS_H

#include <QStringList>

namespace gt
{
namespace console
{

int run(QStringList const& args);

void printRunHelp();

int
runProcess(const QString& projectId, const QString& processId,
           bool save = false);

int
runProcessByFile(const QString& projectFile, const QString& processId,
                 bool save = false);

} // namespace console
} // namespace gt

#endif // GTCONSOLERUNPROCESS_H
