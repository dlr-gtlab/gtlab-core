/*
 * Author: Matthias Schuff
 */

#ifndef GT_COMMANDLINETEMPLATERUNNER_H
#define GT_COMMANDLINETEMPLATERUNNER_H

#include "gt_calculator.h"
#include "gt_stringproperty.h"
#include "gt_boolproperty.h"
#include "gt_intproperty.h"
#include "gt_propertystructcontainer.h"

class GtCommandlineTemplateRunner : public GtCalculator
{
    Q_OBJECT
public:
    Q_INVOKABLE GtCommandlineTemplateRunner();

    /**
     * @brief Main run method of the calculator.
     * @return Whether run process was successful or not.
     */
    bool run() override;

    static GtCalculatorData calculatorData();

    virtual QString cmd();
    virtual QMap<QString, QString> variables();
    virtual QMap<QString, QString> placeholders();
    virtual QStringList args();

protected:
    GtStringProperty m_tool;
    GtStringProperty m_version;
    GtStringProperty m_exec;

    GtStringProperty m_executionDir;
    GtBoolProperty m_executionDirCreate;
    GtStringProperty m_command;
    GtStringProperty m_runscriptFilename;

    GtBoolProperty m_logfile;
    GtStringProperty m_logfilename;

    GtStringProperty m_shell;
    GtIntProperty m_timeout;
    GtPropertyStructContainer m_variables;
    GtBoolProperty m_acceptNonZeroReturn;
    GtIntProperty m_scriptReturn;

};




#endif // GT_COMMANDLINETEMPLATERUNNER_H
