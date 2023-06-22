/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processwizardpage.cpp
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
    m_provider(nullptr),
    m_scope(nullptr)
{
    gt::gui::applyThemeToWidget(this);
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

    if (wiz)
    {
        wiz->setEnableExecutionSettings(false);
    }
}
