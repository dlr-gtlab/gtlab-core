#include "gt_commandlinescriptrunner.h"

#include "gt_logging.h"

GtCommandlineScriptRunner::GtCommandlineScriptRunner(const QString &shell)
{
    m_shell = shell;
    m_logToFile = false;

    m_timeout = -1;
    m_crashed = false;
}


void GtCommandlineScriptRunner::setLogToFile(const QString stdoutFilename, const QString stderrFilename)
{
    m_logToFile = true;
    m_logFilenameStdout = stdoutFilename;
    m_logFilenameStderr = stderrFilename;

    if(m_logToFile)
    {
        if (m_logFilenameStderr == "")
        {
            m_proc.setProcessChannelMode(QProcess::MergedChannels);
        }
        else
        {
            m_proc.setStandardErrorFile(m_logFilenameStderr);
        }
        m_proc.setStandardOutputFile(m_logFilenameStdout);
    }

}


void
GtCommandlineScriptRunner::setExecutionDir(const QString &execdir)
{
    m_execDir = execdir;
    m_proc.setWorkingDirectory(m_execDir);
}

int
GtCommandlineScriptRunner::run(const QString &scriptpath, int timeout)
{

    gtInfo() << "Execution Directory: " << m_execDir;

    QStringList cmdchain;
    cmdchain << scriptpath;


   if (m_shell.toLower()=="cmd.exe")
    {
        cmdchain.insert(0, "/c");
    }
    else if (m_shell.toLower()=="powershell.exe")
    {
        cmdchain.insert(0, "-File");
    }


    gtInfo() << "Executing:" << m_shell << cmdchain;


    m_timeout = timeout;
    m_crashed = false;

    m_proc.start(m_shell, cmdchain);


    double wait;
    if (m_timeout > 0)
    {
        wait = m_timeout*1000.;
    }
    else
    {
        wait = -1;
    }
    m_proc.waitForFinished(wait);


    if (!m_logToFile)
    {
        gtInfo() << "Process STDOUT:" << m_proc.readAllStandardOutput();
        gtInfo() << "Process STDERR:" << m_proc.readAllStandardError();
    }
    gtDebug() << "Process exit status: " << m_proc.exitStatus();
    gtDebug() << "Process exit code: " << m_proc.exitCode();
    m_exitCode = m_proc.exitCode();

    if (!(m_proc.exitStatus() == QProcess::NormalExit))
    {
        m_crashed = true;
        if (m_exitCode==0)
        {
            return -1;
        }
    }

    return m_exitCode;

}


bool GtCommandlineScriptRunner::isCrashed()
{
    return m_crashed;
}

const int
GtCommandlineScriptRunner::exitCode()
{
    return m_exitCode;
}
