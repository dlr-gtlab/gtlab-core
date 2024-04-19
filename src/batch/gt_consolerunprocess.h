/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 *  Created on: 15.04.2024
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTCONSOLERUNPROCESS_H
#define GTCONSOLERUNPROCESS_H

#include "gt_commandlineparser.h"
#include <QStringList>

class GtProject;
class GtTask;

namespace gt
{
namespace console
{
/**
 * @brief options
 * @return list of the command line options
 */
QList<GtCommandLineOption> options();

int run(QStringList const& args);

void printRunHelp();

GtTask* getTask(GtProject* project,
                QString const& taskId,
                QString const& groupid = "");

/**
 * @brief runProcess
 * @param projectId - id of the project in the given session
 * @param processId - id of the task to start
 * @param taskGroupId - if empty string the default task group will be used
 * @param save
 * @return
 */
int
runProcess(const QString& projectId, const QString& processId,
           const QString& taskGroupId = "",
           bool save = false);

/**
 * @brief runProcessByFile
 * @param projectFile - project file to run the process from
 * @param processId - id of the task to start
 * @param taskGroupId - if empty string the default task group will be used
 * @param save
 * @return
 */
int
runProcessByFile(const QString& projectFile, const QString& processId,
                 const QString& taskGroupId = "",
                 bool save = false);

} // namespace console
} // namespace gt

#endif // GTCONSOLERUNPROCESS_H
