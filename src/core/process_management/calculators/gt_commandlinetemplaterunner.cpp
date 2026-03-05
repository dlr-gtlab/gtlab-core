/*
 * Author: Matthias Schuff
 */

#include "gt_commandlinetemplaterunner.h"

#include "gt_commandlinescriptrunner.h"
#include "gt_coreapplication.h"
#include "gt_settings.h"
#include "gt_settings.h"
#include "gt_structproperty.h"

#include "gt_commandlinetemplatefinder.h"
#include "gt_commandlinetemplaterendering.h"

#include <QRegularExpression>
#include <QRegExpValidator>
#include <QProcess>

GtCommandlineTemplateRunner::GtCommandlineTemplateRunner() :
    m_tool("tool", "Tool"),
    m_version("version", "Version"),
    m_exec("exectemplate", "Execution template"),

    m_executionDir("execDir", "Execution Directory", "The directory where to run the script", "", new QRegExpValidator{QRegExp{"^[a-zA-Z0-9\\\\\\/\\:\\.\\-_ ~\\|\\&\\#\\(\\)\\$]*$"}}),
    m_executionDirCreate("execDirCreate", "Create directory if necessary?", "", false),
    m_runscriptFilename("runscript", "Run Script Filename", "", "run.sh"),

    m_logfile("logfile", "Write to logfile", "Redirect all output of the tool to a logfile", false),
    m_logfilename("logfilename", "Logfile name", "Filename of stdout", "out.log"   ),
    m_errfilename("errfilename", "Errfile name", "Filename of stderr", "out.err"   ),

    m_timeout("timeout", "Timeout", "", GtIntProperty::BoundType::BoundLow, -1, -1),
    m_variables("envvars", "Env. Variables"),

    m_acceptNonZeroReturn("acceptNonZeroReturn", "Accept non-zero return", "", false),


    m_usedWorkingDirectory("usedWd", "Used Working Directory"),
    m_usedShell("usedShell", "Used shell"),
    m_executedScriptPath("executedScriptpath", "Path of executed script"),
    m_exitCode("exitCode", "Exit code of script"),
    m_logfileStdout("logfileOut", "Path stdout"),
    m_logfileStderr("logfileErr", "Path stderr")

{
    setObjectName("Commandline Template Runner");


    const QString t = "Tool Selection";
    const QString run = "Run";
    const QString opt = "Optional";

    registerProperty(m_tool, t);
    registerProperty(m_version, t);
    registerProperty(m_exec, t);

    registerProperty(m_executionDir, run);
    registerProperty(m_executionDirCreate, run);
    registerProperty(m_runscriptFilename, run);

    registerProperty(m_logfile, opt);
    registerProperty(m_logfilename, opt);
    registerProperty(m_errfilename, opt);
    registerProperty(m_timeout, opt);
    registerProperty(m_acceptNonZeroReturn, opt);


    GtPropertyStructDefinition vDef("Variable");
    vDef.defineMember("NAME", gt::makeStringProperty(""));
    vDef.defineMember("VALUE", gt::makeStringProperty(""));
    m_variables.registerAllowedType(vDef);
    registerPropertyStructContainer(m_variables);

    registerMonitoringProperty(m_usedWorkingDirectory);
    registerMonitoringProperty(m_usedShell);
    registerMonitoringProperty(m_executedScriptPath);
    registerMonitoringProperty(m_exitCode);
    registerMonitoringProperty(m_logfileStdout);
    registerMonitoringProperty(m_logfileStderr);

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


    /**
     *  Error check
     */
    if(m_executionDir.getVal() == "") {
        gtError() << "Run dir not set for calculator:" << this->objectName();
        return false;
    }

    if(m_runscriptFilename.getVal() == "")
    {
        gtError() << "No filename for run script set in calculator:" << this->objectName();
        return false;
    }


    GtCommandlineTemplateFinder toolfinder;



    /**
     * Transfer to local variables
     */

    QString shell = toolfinder.defaultShell();
    QDir execDir(m_executionDir.getVal());
    QString runscriptfilename(m_runscriptFilename);
    QString runscriptFilepath = execDir.absoluteFilePath(runscriptfilename);


    /**
     * Search the tool, version and executable template
     */
    int err=0;
    QString templatepath = toolfinder.getTemplate(m_tool, m_version, m_exec, &err);

    if(err!=0)
    {
        gtDebug() << "getExecTemplatePath("<<m_tool<<","<<m_version<<","<<m_exec<<")="<<err;
        gtDebug() << "templatepath="<<templatepath;
        gtError() << "Error getting the template script";
        return false;
    }

    gtInfo() << "Using template: " << templatepath;


    /**
     * Render template to a script
     */

    GtCommandlineTemplateRendering templaterender(shell);


    auto templatevariables = collectVariables();
    bool templateRendered
        = templaterender.renderTemplateAndWriteScript(
            templatepath,
            templatevariables,
            runscriptFilepath,
            true,
            m_executionDirCreate.getVal()
        );

    if(!templateRendered)
    {
        gtDebug() << "renderTemplateAndWriteScript() is false";
        gtDebug() << "templatepath="<<templatepath;
        gtDebug() << "collectVariables()="<<templatevariables;
        gtDebug() << "runscriptFilepath="<<runscriptFilepath;
        gtError() << "Error rendering the template to script";
        return false;
    }

    gtDebug() << "Script written to: " << runscriptFilepath;



    /**
     * Run the script
     */
    GtCommandlineScriptRunner scriptrunner(shell);
    scriptrunner.setExecutionDir(execDir.path());
    if (m_logfile.getVal())
    {
        gtInfo() << "log to file";
        scriptrunner.setLogToFile(m_logfilename.getVal(), m_errfilename.getVal());
    }

    scriptrunner.run(runscriptFilepath);


    /**
     * Set output/monitoring properties
     */
    m_usedWorkingDirectory.setVal(execDir.path());
    m_usedShell.setVal(shell);
    m_executedScriptPath.setVal(runscriptFilepath);
    m_exitCode.setVal(scriptrunner.exitCode());
    m_logfileStdout.setVal(scriptrunner.logfileStdoutPath());
    m_logfileStderr.setVal(scriptrunner.logfileStderrPath());


    /**
     *  Handle calculator end
     */
    if (scriptrunner.isCrashed()) {
        gtFatal() << "Process crashed";
        return false;
    }

    if (scriptrunner.exitCode() != 0)
    {
        if(m_acceptNonZeroReturn.getVal())
        {
            gtWarning() << "Exit code is not 0.";
        }
        else
        {
            gtFatal() << "Exit code is not 0.";
            return false;
        }
    }

    return true;
}


QMap<QString, QString>
GtCommandlineTemplateRunner::variables()
{
    QMap<QString, QString> r;
    return r;
}


QMap<QString, QString>
GtCommandlineTemplateRunner::collectVariables()
{
    // Get variables defined in virtual function
    QMap<QString, QString> vars = variables();

    // variables from calculator properties overwrite calculator-specific variables
    for(unsigned int i=0; i<m_variables.size(); i++)
    {
        QString varname = m_variables.at(i).getMemberVal<QString>("NAME");
        QString varvalue = m_variables.at(i).getMemberVal<QString>("VALUE");
        vars[varname] = varvalue;
    }

    return vars;
}






