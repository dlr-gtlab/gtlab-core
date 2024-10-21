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

#include <gt_logging.h>
#include <gt_application.h>
#include <gt_session.h>

#include <QCoreApplication>
#include <QStringList>

class GtProject;
class GtTask;

namespace gt
{
namespace console
{
/**
 * @brief runOptions
 * @return list of the command line options for the run command
 */
QList<GtCommandLineOption> runOptions();

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

/**
 * @brief Enters a temporary session
 *
 * The return value must be kept until the session is not needed anymore.
 * It is used to switch back to the current session
 */
inline auto enterTempSession()
{
    auto tmpSessionID = QString("_tmp_batch_session_%1").arg(QCoreApplication::applicationPid());
    QString currentSessionID = gtApp->session() ? gtApp->session()->objectName() : "default";

    gtDebug() << QObject::tr("Creating temporary batch session '%1'").arg(tmpSessionID);

    gtApp->newSession(tmpSessionID);
    gtApp->switchSession(tmpSessionID);

    // cleanup
    return gt::finally([tmpSessionID, currentSessionID](){
        gtApp->switchSession(currentSessionID);
        gtApp->deleteSession(tmpSessionID);
    });
}


} // namespace console
} // namespace gt

#endif // GTCONSOLERUNPROCESS_H
