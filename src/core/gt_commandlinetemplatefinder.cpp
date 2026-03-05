/*
 * Author: Matthias Schuff
 */

#include "gt_commandlinetemplatefinder.h"

#include "gt_coreapplication.h"
#include "gt_settings.h"
#include "gt_logging.h"

#include <QDir>
#include <QFile>


GtCommandlineTemplateFinder::GtCommandlineTemplateFinder()
{

    loadGtlabSettings();

}

void
GtCommandlineTemplateFinder::loadGtlabSettings()
{
    if (gtApp != nullptr)
    {
        gtDebug() << "commandlineTemplatesOs:" << gtApp->settings()->commandlineTemplatesOs();
        gtDebug() << "commandlineTemplatesPath:" << gtApp->settings()->commandlineTemplatesPath();
        gtDebug() << "commandlineTemplatesMachine:" << gtApp->settings()->commandlineTemplatesMachine();
        gtDebug() << "commandlineTemplatesDefaultShell:" << gtApp->settings()->commandlineTemplatesDefaultShell();

        setOsType(gtApp->settings()->commandlineTemplatesOs());
        setTemplateSearchPath(gtApp->settings()->commandlineTemplatesPath());
        setMachine(gtApp->settings()->commandlineTemplatesMachine());
        setDefaultShell(gtApp->settings()->commandlineTemplatesDefaultShell());
    }
    else
    {
        gtWarning() << "Could not load settings for Commandline Templates: gtApp was not found on startup of GtCommandlineTemplateFinder";
    }
}

const QString
GtCommandlineTemplateFinder::osType()
{
    return m_osType;
}

const QString
GtCommandlineTemplateFinder::templateSearchPath()
{
    return m_templatepath;
}

const QString
GtCommandlineTemplateFinder::machine()
{
    return m_machine;
}

const QString
GtCommandlineTemplateFinder::defaultShell()
{
    return m_shell;
}

void
GtCommandlineTemplateFinder::setOsType(const QString &ostype)
{
    m_osType = ostype;
}

void
GtCommandlineTemplateFinder::setTemplateSearchPath(const QString &templatepath)
{
    m_templatepath = templatepath;
}

void
GtCommandlineTemplateFinder::setMachine(const QString &machine)
{
    m_machine = machine;
}

void
GtCommandlineTemplateFinder::setDefaultShell(const QString &shell)
{
    m_shell = shell;
}


const QString
GtCommandlineTemplateFinder::resolveTemplateSearchPath()
{
    QString mainpath(m_templatepath);

    if ((mainpath == "~") || (mainpath.startsWith("~/")))
    {
        mainpath.replace (0, 1, QDir::homePath());
    }
    return mainpath;
}

const QString
GtCommandlineTemplateFinder::makeExecTemplatePath(const QString &toolname, const QString &version, const QString &exectemplate)
{
    QDir pathMachine(QDir(resolveTemplateSearchPath()).absoluteFilePath(m_machine));
    QDir pathTool(pathMachine.absoluteFilePath(toolname));
    QDir pathVersion(pathTool.absoluteFilePath(version));
    QFile pathExec(pathVersion.absoluteFilePath(exectemplate));

    return QString(pathExec.fileName());
}

bool
GtCommandlineTemplateFinder::hasToolVersion(const QString &toolname, const QString &version)
{
    QDir searchDir(resolveTemplateSearchPath());

    gtDebug() << "search path:" << searchDir.path();
    gtDebug() << "machine:" << m_machine;

    if (!searchDir.exists())
    {
        gtError() << "Template search path not found:" << searchDir.path();
        return false;
    }


    QDir pathMachine(searchDir.absoluteFilePath(m_machine));

    if (!pathMachine.exists())
    {
        gtError() << "Machine name not found:" << m_machine << " in:" << pathMachine.path();
        return false;
    }


    QDir pathTool(pathMachine.absoluteFilePath(toolname));

    if (!pathTool.exists())
    {
        gtError() << "Tool not found:" << toolname << " in:" << pathTool.path();
        return false;
    }


    if (version == "")
    {
        return true;
    }

    QDir pathVersion(pathTool.absoluteFilePath(version));

    if (!pathVersion.exists())
    {
        gtError() << "Tool version not found:" << version << " in:" << pathVersion.path();
        return false;
    }

    return true;
}

bool
GtCommandlineTemplateFinder::hasTemplate(const QString &toolname, const QString &version, const QString &templatename)
{

    if (!hasToolVersion(toolname, version))
    {
        return false;
    }


    if (templatename=="")
    {
        gtError() << "template name is empty";
        return false;
    }

    QFile pathExec(makeExecTemplatePath(toolname, version, templatename));

    if (!pathExec.exists())
    {
        gtInfo() << "pathExec:" << pathExec;
        gtError() << "Template file name not found:" << toolname << "/" << version << "/" << templatename;
        return false;
    }

    return true;
}

const QString
GtCommandlineTemplateFinder::getTemplate(const QString &toolname, const QString &version, const QString &templatename, int *error)
{
    if(error!=nullptr)
    {
        *error = 0;
    }

    if (!hasTemplate(toolname, version, templatename))
    {
        if(error!=nullptr)
        {
            *error = 1;
        }
        return QString();
    }

    return makeExecTemplatePath(toolname, version, templatename);
}

