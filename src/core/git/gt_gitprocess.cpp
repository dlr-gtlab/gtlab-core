/* GTlab - Gas Turbine laboratory
 * Source File: gt_gitprocess.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 18.07.2022
 *  Author: David Nelles (AT-TUR)
 *  Tel.: +495517092708
 */

#include "gt_gitprocess.h"

#include <QProcess>

GtGitProcess::ProcessOutput
GtGitProcess::runProcess(const QString &program, const QStringList &args, const QDir &workingDirectory)
{
    QProcess process;
    process.setWorkingDirectory(workingDirectory.absolutePath());
    process.start(program, args);
    if (process.waitForFinished())
    {
        ProcessOutput output(process.exitCode());
        output.standardOutput = process.readAllStandardOutput();
        output.errorOutput = process.readAllStandardError();

        return output;
    }
    else
    {
        return ProcessOutput(false);
        gtWarning() << "Git command timeouted!";
    }
}

const GtGitProcess::ProcessOutput &GtGitProcess::ProcessOutput::logError(const ProcessOutput &output, QDebug &stream)
{
    if (!output.errorOutput.isEmpty())
    {
        QString errOut = output.errorOutput;
        errOut = errOut.replace(QRegExp("\\s"), " ");

        if (!output.sucessed)
        {
            stream << errOut;
        }
    }

    return output;
}
