/*
 * Author: Matthias Schuff
 */

#include "gt_commandlinetemplaterunner.h"

#include "gt_coreapplication.h"
#include "gt_settings.h"
#include "gt_settings.h"
#include "gt_structproperty.h"
#include "gt_commandlinetemplatefinder.h"

#include <QRegularExpression>
#include <QRegExpValidator>
#include <QProcess>

GtCommandlineTemplateRunner::GtCommandlineTemplateRunner() :
    m_tool("tool", "Tool"),
    m_version("version", "Version"),
    m_exec("exectemplate", "Execution template"),

    m_executionDir("execDir", "Execution Directory", "The directory where to run the script", "", new QRegExpValidator{QRegExp{"^[a-zA-Z0-9\\\\\\/\\:\\.\\-_ ~\\|\\&\\#\\(\\)\\$]*$"}}),
    m_executionDirCreate("execDirCreate", "Create directory if necessary?", "", false),
    m_command("cmd", "Command", "", "", new QRegExpValidator{QRegExp{"^[a-zA-Z0-9\\\\\\/\\.\\-_ ~\\|\\&\\;\\>\\*\\#\\(\\)\\$\\^\\!]*$"}}),
    m_runscriptFilename("runscript", "Run Script Filename", "", "run.sh"),

    m_logfile("logfile", "Write to logfile", "Redirect all output of the tool to a logfile", false),
    m_logfilename("logfilename", "Logfile name", "Filename of stdout and stderr without extension, e.g. 'output' results n 'output.log' and 'output.err'", "log"   ),

    m_shell("shell", "Custom Shell", "", ""),
    m_timeout("timeout", "Timeout", "", GtIntProperty::BoundType::BoundLow, -1, -1),
    m_variables("envvars", "Env. Variables"),

    m_acceptNonZeroReturn("acceptNonZeroReturn", "Accept non-zero return", "", false),
    m_scriptReturn("scriptReturnCode", "Return Code")
{
    setObjectName("Commandline Template Runner");


    const QString t = "Tool Selection";
    const QString run = "Run";
    const QString opt = "Optional";
    const QString output = "Output";

    registerProperty(m_tool, t);
    registerProperty(m_version, t);
    registerProperty(m_exec, t);

    registerProperty(m_executionDir, run);
    registerProperty(m_executionDirCreate, run);
    registerProperty(m_command, run);
    registerProperty(m_runscriptFilename, run);

    registerProperty(m_logfile, opt);
    registerProperty(m_logfilename, opt);
    registerProperty(m_shell, opt);
    registerProperty(m_timeout, opt);
    registerProperty(m_acceptNonZeroReturn, opt);
    registerProperty(m_scriptReturn, output);

    m_scriptReturn.setReadOnly(true);

    GtPropertyStructDefinition vDef("Variable");
    vDef.defineMember("NAME", gt::makeStringProperty(""));
    vDef.defineMember("VALUE", gt::makeStringProperty(""));
    m_variables.registerAllowedType(vDef);
    registerPropertyStructContainer(m_variables);

}



GtCalculatorData
GtCommandlineTemplateRunner::calculatorData()
{
    GtCalculatorData x = GT_CALC_DATA(GtCommandlineTemplateRunner);
    x->id = QStringLiteral("Commandline Template Runner");
    x->version = GtVersionNumber(1,0);
    x->status = GtCalculatorDataImpl::RELEASE;
    x->category = "Core";
    x->description = "Run a template of a commandline tool.";
    return x;
}


bool GtCommandlineTemplateRunner::run()
{


    gtDebug() << "commandlineTemplatesOs:" << gtApp->settings()->commandlineTemplatesOs();
    gtDebug() << "commandlineTemplatesPath:" << gtApp->settings()->commandlineTemplatesPath();
    gtDebug() << "commandlineTemplatesMachine:" << gtApp->settings()->commandlineTemplatesMachine();
    gtDebug() << "commandlineTemplatesDefaultShell:" << gtApp->settings()->commandlineTemplatesDefaultShell();


    /**
     * Search the tool, version and executable template
     */
    GtCommandlineTemplateFinder toolfinder;




    QString shell;
    if (m_shell.getVal() != "")
    {
        shell = m_shell.getVal();
    }
    else
    {
        shell = toolfinder.getDefaultShell();
    }







    int err=0;
    QString templatepath = toolfinder.getExecTemplatePath(m_tool, m_version, m_exec, &err);

    if(err!=0)
    {
        gtDebug() << "getExecTemplatePath("<<m_tool<<","<<m_version<<","<<m_exec<<")="<<err;
        gtDebug() << "templatepath="<<templatepath;
        gtError() << "Error getting the template script";
        return false;
    }


    /**
     *  Error check
     */
    if(m_executionDir.getVal() == "") {
        gtError() << "Run dir not set for calculator:" << this->objectName();
        return false;
    }
    QDir execDir(m_executionDir.getVal());

    if(!execDir.exists())
    {
        if(m_executionDirCreate)
        {
            if (!execDir.mkpath(m_executionDir.getVal())) {
                gtError() << "Failed to create directory:" << m_executionDir.getVal();
                return false;
            }
        }
        else
        {
            gtError() << "Run dir does not exist:" << m_executionDir;
            return false;
        }
    }
    if(m_executionDir.getVal() == "")
    {
        gtError() << "No filename for run script set in calculator:" << this->objectName();
        return false;
    }

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

    /**
     * Replace placeholders/variables in template file
     */
    QList<QString> keys ;
    QMap<QString, QString> replacements;
    QMap<QString, QString> x = placeholders();

    keys = x.keys();
    for(int i=0; i < keys.count() ; i++){
        replacements.insert("{"+keys.at(i)+"}", x[keys.at(i)]);
    }

    for(int i = 0; i < t.count(); i++)
    {
        QString _line = t.at(i);
        QRegularExpression regex(R"(\{(\w+)\})");

        // Use QRegularExpression to replace placeholders
        QString modifiedString = _line;
        QRegularExpressionMatchIterator iter = regex.globalMatch(_line);

        // Iterate over all matches and replace them
        while (iter.hasNext()) {
            QRegularExpressionMatch match = iter.next();
            QString placeholder = match.captured(0);

            // Replace with value from the map if found, otherwise leave as is
            if (replacements.contains(placeholder)) {
                modifiedString.replace(placeholder, replacements[placeholder]);
            }
        }

        t[i] = modifiedString;
    }

    /**
     * add additional variables add the top of the script
     */
    unsigned int j = 0;

    // first check the first none comment line
    for(int i = 0; i < t.count(); i++)
    {
        j = i;
        QString _line = t.at(i);
        gtInfo()<<i<<":" << _line;
        if (!_line.trimmed().startsWith("#"))
        {
            break;
        }
    }

    // add variables from virtual function
    QMap<QString, QString> vars = variables();
    keys = vars.keys();
    for(unsigned int i=0; i<keys.size(); i++)
    {
        QString varname = keys.at(i);
        QString varvalue = vars[varname];

        if (toolfinder.getOsType() == "win")
        {
            if (shell.toLower()=="powershell.exe")
            {
                t.insert(j, "$"+varname+" = \""+varvalue+"\"");
            }
            else if (shell.toLower()=="cmd.exe")
            {
                t.insert(j, "set "+varname+"=\""+varvalue+"\"");
            }
        }
        else if(toolfinder.getOsType() == "unix")
        {
            t.insert(j, varname+"="+varvalue);
        }

        j++;
    }

    // add variables from calculator properties
    for(unsigned int i=0; i<m_variables.size(); i++)
    {
        QString varname = m_variables.at(i).getMemberVal<QString>("NAME");
        QString varvalue = m_variables.at(i).getMemberVal<QString>("VALUE");



        if (toolfinder.getOsType() == "win")
        {
            if (shell.toLower()=="powershell.exe")
            {
                t.insert(j, "$"+varname+" = \""+varvalue+"\"");
            }
            else if (shell.toLower()=="cmd.exe")
            {
                t.insert(j, "set "+varname+"=\""+varvalue+"\"");
            }
        }
        else if(toolfinder.getOsType() == "unix")
        {
            t.insert(j, varname+"="+varvalue);
        }


        j++;
    }

    /**
     * Add the execution of the command line
     */
    t.append(cmd());



    QString runscriptfilename(m_runscriptFilename);

    /**
     * WINDOWS SPECIAL
     */

    if (toolfinder.getOsType() == "win")
    {
        if (shell.toLower()=="powershell.exe")
        {
            if(!runscriptfilename.endsWith(".ps1"))
            {
                runscriptfilename += ".ps1";
            }
        }
        else if (shell.toLower()=="cmd.exe")
        {
            if(!runscriptfilename.endsWith(".cmd"))
            {
                runscriptfilename += ".cmd";
            }
        }
    }
    else if(toolfinder.getOsType() == "unix")
    {
        if(!runscriptfilename.endsWith(".sh"))
        {
            runscriptfilename += ".sh";
        }
    }

    /**
     * END OF WINDOWS SPECIAL
     */



    /**
     * Write the script-to-run to the file
     */
    QFile runScript(QDir(m_executionDir).absoluteFilePath(runscriptfilename));
    if (runScript.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&runScript);
        for (int i = 0; i < t.size(); ++i)
        {
            out << t.at(i) << '\n';
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


    /**
     * Execute script
     */
    QProcess proc;
    proc.setWorkingDirectory(m_executionDir);




    gtDebug() << "Run Script: " << runscriptfilename << "in:" << m_executionDir ;

    if (m_logfile.getVal())
    {
        if (m_logfilename.getVal() == "")
        {
            gtError() << "No logfile name given!";
            return false;
        }

        QString logfile, errfile;
        logfile = QDir(m_executionDir.getVal()).absoluteFilePath(m_logfilename.getVal()+".log");
        errfile = QDir(m_executionDir.getVal()).absoluteFilePath(m_logfilename.getVal()+".err");

        gtDebug() << "Redirecting output to:";
        gtDebug() << "STDOUT:" << logfile;
        gtDebug() << "STDERR:" << errfile;

        proc.setStandardOutputFile(logfile);
        proc.setStandardErrorFile(errfile);
    }


    QStringList cmdchain;
    cmdchain << runscriptfilename << args();






    /**
     * WINDOWS SPECIAL
     */

    if (toolfinder.getOsType() == "win")
    {
        if (shell=="cmd.exe")
        {
            cmdchain.insert(0, "/c");
        }
        else if (shell.toLower()=="powershell.exe")
        {
            cmdchain.insert(0, "-File");

        }
    }

    /**
     * END OF WINDOWS SPECIAL
     */


    gtInfo() << "Executing:" << shell << cmdchain;


    proc.start(shell, cmdchain);

    double wait;
    if (m_timeout > 0)
    {
        wait = m_timeout*1000.;
    }
    else
    {
        wait = -1;
    }
    proc.waitForFinished(wait);



    gtInfo() << "Process STDOUT:" << proc.readAllStandardOutput();
    gtInfo() << "Process STDERR:" << proc.readAllStandardError();

    if (proc.exitStatus() == QProcess::NormalExit)
    {
        gtInfo() << "Process Termination:" << "Normal Exit";
    }
    else if (proc.exitStatus() == QProcess::CrashExit)
    {
        gtInfo() << "Process Termination:" << "Crash Exit";
        return false;
    }



    if (proc.exitCode() != 0)
    {
        if(m_acceptNonZeroReturn.getVal())
        {
            gtWarning() << "Process Termination:" << "Execution is not 0," << "Exit code:" << proc.exitCode();
            return true;
        }
        else
        {
            gtError() << "Process Termination:" << "Execution Failed" << "Exit code:" << proc.exitCode();
            return false;
        }
    }
    else
    {
        gtInfo() << "### Execution finished succesfully ###";
    }

    return true;
}


QString
GtCommandlineTemplateRunner::cmd()
{
    return m_command.getVal();
}


QMap<QString, QString>
GtCommandlineTemplateRunner::variables()
{
    QMap<QString, QString> r;
    return r;
}

QMap<QString, QString>
GtCommandlineTemplateRunner::placeholders()
{
    QMap<QString, QString> r;
    return r;
}

QStringList
GtCommandlineTemplateRunner::args()
{
    QStringList r;
    return r;
}




