/* GTlab - Gas Turbine laboratory
 * Source File: gt_gitprocess.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 18.07.2022
 *  Author: David Nelles (AT-TUR)
 *  Tel.: +495517092708
 */

#ifndef GT_GITPROCESS_H
#define GT_GITPROCESS_H

#include <QString>
#include <QDir>

#include "gt_logging.h"

class GtGitProcess
{
public:

    /**
     * @brief The ProcessOutput struct stores the StdOutput/Err and the execution result of a subprocess.
     */
    class ProcessOutput
    {
    public:
        ProcessOutput(){};
        ProcessOutput(int exitCode)
        {
            this->exitCode = exitCode;
            this->sucessed = !exitCode;
        }

        bool sucessed;
        int exitCode;
        QString standardOutput;
        QString errorOutput;

        /**
         * @brief Prints error messages of the ProcessOutput parameter to gtError and returns a const reference of the PrecessOutput object which is used as prameter.
         * @param output is the ProcessOutput objec which error message should be printed to gtError.
         * @param stream defines the output stream to which the Error message should be printed.
         * @return Reference to the ProcessOutput object used as parameter.
         */
        static const ProcessOutput& logError(const ProcessOutput &output, QDebug &stream = QsLogging::Logger::Helper(QsLogging::ErrorLevel).stream());
    };

    /**
     * @brief To run subprocesses easily.
     * @param The process wich should be executed.
     * @param Argruments of the process
     * @param The directory where the subprocess should be executed.
     * @return StdOutput and result of the subprocess.
     */
    static ProcessOutput runProcess(const QString &program, const QStringList &args, const QDir &workingDirectory);

private:

    GtGitProcess();
};

#endif // GT_GITPROCESS_H
