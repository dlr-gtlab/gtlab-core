#include "gt_abstractdataconfigwizard.h"
//#include "gt_barprovider.h"
//#include "gt_xyprovider.h"
#include "gt_abstractchartprovider.h"
#include "gt_palette.h"
#include "gt_application.h"

//GtAbstractDataConfigWizard::GtAbstractDataConfigWizard(
//        GtBarProvider* provider, QWidget* parent) : QWizardPage(parent),
//    m_provider(provider)
//{

//}

GtAbstractDataConfigWizard::GtAbstractDataConfigWizard(
        GtAbstractChartProvider* /*provider*/, QWidget* parent) :
    QWizardPage(parent)
{
    GtPalette::applyThemeToWidget(this);
}

GtAbstractDataConfigWizard::GtAbstractDataConfigWizard()
{

}

bool
GtAbstractDataConfigWizard::isValid()
{
//    if (m_provider)
//    {
//        return true;
//    }

    return true;
}

