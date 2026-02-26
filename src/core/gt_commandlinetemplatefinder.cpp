#include "gt_commandlinetemplatefinder.h"

#include "gt_coreapplication.h"
#include "gt_settings.h"
#include "gt_logging.h"

#include <QDir>
#include <QFile>


GtCommandlineTemplateFinder::GtCommandlineTemplateFinder(const bool reinitializeEverytime):
    m_reinitializeEverytime(reinitializeEverytime)
{
    makeMainPathAbs();
}

const void GtCommandlineTemplateFinder::reinitialize()
{
    makeMainPathAbs();
}


void
GtCommandlineTemplateFinder::makeMainPathAbs()
{


    QString mainpath = gtApp->settings()->commandlineTemplatesPath();
    QString machine = gtApp->settings()->commandlineTemplatesMachine();

    if ((mainpath == "~") || (mainpath.startsWith("~/")))
    {
        mainpath.replace (0, 1, QDir::homePath());
    }
    m_mainpath = mainpath;
    m_machine = machine;
}

const QString
GtCommandlineTemplateFinder::makeExecTemplatePath(const QString &toolname, const QString &version, const QString &exectemplate)
{
    if (m_reinitializeEverytime)
    {
        reinitialize();
    }

    QDir pathMachine(QDir(m_mainpath).absoluteFilePath(m_machine));
    QDir pathTool(pathMachine.absoluteFilePath(toolname));
    QDir pathVersion(pathTool.absoluteFilePath(version));
    QFile pathExec(pathVersion.absoluteFilePath(exectemplate));

    return QString(pathExec.fileName());
}

bool
GtCommandlineTemplateFinder::hasToolVersion(const QString &toolname, const QString &version)
{
    if (m_reinitializeEverytime)
    {
        reinitialize();
    }

    QDir pathXpe(m_mainpath);

    gtDebug() << "mainpath:" << m_mainpath;
    gtDebug() << "machine:" << m_machine;

    if (!pathXpe.exists())
    {
        gtInfo() << "Templates path:" << pathXpe.path();
        gtError() << "Templates path not found";
        return false;
    }


    QDir pathMachine(pathXpe.absoluteFilePath(m_machine));

    if (!pathMachine.exists())
    {
        gtInfo() << "Templates machine name:" << pathMachine.path();
        gtError() << "Machine name not found:" << m_machine;
        return false;
    }


    QDir pathTool(pathMachine.absoluteFilePath(toolname));

    if (!pathTool.exists())
    {
        gtInfo() << "Path to tool:" << pathTool.path();
        gtError() << "Tool not found:" << toolname;
        return false;
    }


    if (version == "")
    {
        return true;
    }

    QDir pathVersion(pathTool.absoluteFilePath(version));

    if (!pathVersion.exists())
    {
        gtInfo() << "Path to version:" << pathVersion.path();
        gtError() << "Tool version not found:" << version;
        return false;
    }

    return true;
}

bool
GtCommandlineTemplateFinder::hasExecutionTemplate(const QString &toolname, const QString &version, const QString &exectemplate)
{
    if (m_reinitializeEverytime)
    {
        reinitialize();
    }

    if (!hasToolVersion(toolname, version))
    {
        return false;
    }


    if (exectemplate=="")
    {
        gtError() << "execution template is empty";
        return false;
    }

    QFile pathExec(makeExecTemplatePath(toolname, version, exectemplate));

    if (!pathExec.exists())
    {
        gtInfo() << "pathExec:" << pathExec;
        gtError() << "Template file name not found:" << toolname << "/" << version << "/" << exectemplate;
        return false;
    }

    return true;
}

const QString
GtCommandlineTemplateFinder::getExecTemplatePath(const QString &toolname, const QString &version, const QString &exectemplate, int *error)
{
    if (m_reinitializeEverytime)
    {
        reinitialize();
    }

    if(error!=nullptr)
    {
        *error = 0;
    }

    if (!hasExecutionTemplate(toolname, version, exectemplate))
    {
        if(error!=nullptr)
        {
            *error = 1;
        }
        return QString();
    }

    return makeExecTemplatePath(toolname, version, exectemplate);
}

const QString
GtCommandlineTemplateFinder::getDefaultShell()
{
    if (m_reinitializeEverytime)
    {
        reinitialize();
    }
    QString shell = gtApp->settings()->commandlineTemplatesDefaultShell();
    return shell;
}

const QString
GtCommandlineTemplateFinder::getOsType()
{
    if (m_reinitializeEverytime)
    {
        reinitialize();
    }
    QString ostype = gtApp->settings()->commandlineTemplatesOs();
    return ostype;
}

