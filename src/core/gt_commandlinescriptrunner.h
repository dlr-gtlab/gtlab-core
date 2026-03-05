#ifndef GT_COMMANDLINESCRIPTRUNNER_H
#define GT_COMMANDLINESCRIPTRUNNER_H

#include <QObject>
#include <QMap>
#include <QProcess>


class GtCommandlineScriptRunner
{
public:
    Q_INVOKABLE GtCommandlineScriptRunner(const QString &shell="");

    Q_INVOKABLE void setShell(const QString &shell);

    Q_INVOKABLE void setLogToFile(const QString stdoutFilename="", const QString stderrFilename="");

    Q_INVOKABLE void setExecutionDir(const QString &execdir);

    Q_INVOKABLE int run(const QString &scriptpath, const int timeout=-1);


    Q_INVOKABLE bool isCrashed();
    Q_INVOKABLE const int exitCode();

    Q_INVOKABLE const QString logfileStdoutPath();
    Q_INVOKABLE const QString logfileStderrPath();

private:
    QProcess m_proc;

    QString m_shell;
    QString m_execDir;
    bool m_logToFile;
    QString m_logFilenameStdout;
    QString m_logFilenameStderr;

    int m_timeout;
    bool m_crashed;
    int m_exitCode;
    QString m_logFileStdoutPath;
    QString m_logFileStderrPath;
};

#endif // GT_COMMANDLINESCRIPTRUNNER_H
