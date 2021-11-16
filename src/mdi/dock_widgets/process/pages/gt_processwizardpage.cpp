/* GTlab - Gas Turbine laboratory
 * Source File: gt_processwizardpage.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 17.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_abstractprocessprovider.h"
#include "gt_project.h"
#include "gt_processwizard.h"
#include "gt_palette.h"
#include "gt_application.h"

#include "gt_processwizardpage.h"

GtProcessWizardPage::GtProcessWizardPage() :
    m_provider(Q_NULLPTR),
    m_scope(Q_NULLPTR)
{
    GtPalette::applyThemeToWidget(this, gtApp->inDarkMode());
}

GtAbstractProcessProvider*
GtProcessWizardPage::provider()
{
    return m_provider;
}

void
GtProcessWizardPage::setProvider(GtAbstractProcessProvider* prov)
{
    m_provider = prov;
}

GtProject*
GtProcessWizardPage::scope()
{
    return m_scope;
}

void
GtProcessWizardPage::setScope(GtProject* project)
{
    m_scope = project;
}

void
GtProcessWizardPage::cleanupPage()
{
    GtProcessWizard* wiz = qobject_cast<GtProcessWizard*>(wizard());

    if (wiz != Q_NULLPTR)
    {
        wiz->setEnableExecutionSettings(false);
    }
}
