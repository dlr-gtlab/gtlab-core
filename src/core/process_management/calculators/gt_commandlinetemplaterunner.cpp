#include "gt_commandlinetemplaterunner.h"

#include "gt_coreapplication.h"
#include "gt_settings.h"

GtCommandlineTemplateRunner::GtCommandlineTemplateRunner():
    m_executionDir("execDir", tr("Execution Directory"),tr("Execution Directory"), "")
{
    setObjectName("Commandline Template Runner");
    registerProperty(m_executionDir);

}

bool GtCommandlineTemplateRunner::run()
{
    gtInfo() << "hello world";

    gtInfo() << "commandlineTemplatesOs:" << gtApp->settings()->commandlineTemplatesOs();
    gtInfo() << "commandlineTemplatesPath:" << gtApp->settings()->commandlineTemplatesPath();
    gtInfo() << "commandlineTemplatesMachine:" << gtApp->settings()->commandlineTemplatesMachine();
    gtInfo() << "commandlineTemplatesDefaultShell:" << gtApp->settings()->commandlineTemplatesDefaultShell();
    return true;
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
