#include "gt_commandlinetemplaterendering.h"

#include <QTextStream>
#include <QDir>

#include "gt_logging.h"

GtCommandlineTemplateRendering::GtCommandlineTemplateRendering(const QString shell)
{
    m_shell = shell;
}

bool
GtCommandlineTemplateRendering::loadTemplateFromFile(const QString &templatepath)
{
    /**
     * load the template script
     */
    QStringList t;
    QFile templatefile(templatepath);

    if (templatefile.open(QFile::ReadOnly))
    {
        QTextStream in(&templatefile);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            t.append(line);
        }
        templatefile.close();
    }
    else
    {
        gtError() << "Could not read the template: " << templatepath;
        return false;
    }

    m_template = t;

    return true;
}

bool
GtCommandlineTemplateRendering::updateVariables(const QMap<QString, QString> &variables)
{
    QStringList keys = variables.keys();

    // variables from calculator properties overwrite calculator-specific variables
    for(unsigned int i=0; i<variables.size(); i++)
    {
        QString key = keys[i];
        QString value = variables[key];

        m_variables[key] = value;
    }
    return true;
}


bool
GtCommandlineTemplateRendering::renderTemplateToScript()
{
    QStringList keys = m_variables.keys();

    unsigned int j = 0;

    // check the first non-comment line
    for(int i = 0; i < m_template.count(); i++)
    {
        j = i;
        QString _line = m_template.at(i);
        if (!_line.trimmed().startsWith("#"))
        {
            break;
        }
    }

    // add variables from calculator properties
    for(unsigned int i=0; i<m_variables.size(); i++)
    {
        QString varname = keys[i];
        QString varvalue = m_variables[varname];
        QString x;

        // TODO: escaping of quotes in varvalue


        if (m_shell.toLower()=="powershell.exe")
        {
            x = "$"+varname+" = \""+varvalue+"\"";
        }
        else if (m_shell.toLower()=="cmd.exe")
        {
            x = "set "+varname+"=\""+varvalue+"\"";
        }
        else
        {
            x = varname+"="+varvalue;
        }

        m_template.insert(j, x);

        j++;
    }



    return true;
}

bool
GtCommandlineTemplateRendering::writeScript(const QString &targetpath, bool overwriteExisting, bool createTargetDirectory)
{
    QDir execDir(QFileInfo(targetpath).dir());
    QString directoryPath = execDir.path();

    if(!execDir.exists())
    {
        if(createTargetDirectory)
        {
            if (!execDir.mkpath(directoryPath)) {
                gtError() << "Failed to create directory:" << targetpath;
                return false;
            }
        }
        else
        {
            gtError() << "Run dir does not exist:" << targetpath;
            return false;
        }
    }

    QString runscriptfilename = targetpath;


    if (m_shell.toLower()=="powershell.exe")
    {
        if(!runscriptfilename.endsWith(".ps1"))
        {
            runscriptfilename += ".ps1";
        }
    }
    else if (m_shell.toLower()=="cmd.exe")
    {
        if(!runscriptfilename.endsWith(".bat"))
        {
            runscriptfilename += ".bat";
        }
    }
    else
    {
        if(!runscriptfilename.endsWith(".sh"))
        {
            runscriptfilename += ".sh";
        }
    }


    /**
     * Write the script-to-run to the file
     */
    QFile runScript(runscriptfilename);
    if (runScript.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&runScript);
        for (int i = 0; i < m_template.size(); ++i)
        {
            out << m_template.at(i) << '\n';
        }
        runScript.setPermissions(
            QFile::ExeOwner | QFile::ReadOwner | QFile::WriteOwner |
            QFile::ExeGroup | QFile::ReadGroup | QFile::WriteGroup |
            QFile::ExeOther | QFile::ReadOther | QFile::WriteOther);
        runScript.close();
    }
    else
    {
        gtError() << "Could not write run script";
        return false;
    }


    return true;
}


bool
GtCommandlineTemplateRendering::renderTemplateAndWriteScript(const QString &templatepath, const QMap<QString, QString> &variables, const QString &targetfilepath, bool overwriteExisting, bool createTargetDirectory)
{
    if (!loadTemplateFromFile(templatepath))
    {
        return false;
    }

    if (!updateVariables(variables))
    {
        return false;
    }

    if (!renderTemplateToScript())
    {
        return false;
    }

    if (!writeScript(targetfilepath, overwriteExisting, createTargetDirectory))
    {
        return false;
    }

    return true;
}
