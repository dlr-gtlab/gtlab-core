/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 12.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_projectwizard.h"
#include "gt_application.h"
#include "gt_colors.h"

#include "gt_projectprovider.h"
#include "gt_projectwizardstart.h"
#include "gt_projectwizardspec.h"
#include "gt_projectwizardmodules.h"
#include "gt_projectwizardimport.h"

GtProjectWizard::GtProjectWizard(GtProjectProvider* provider,
                                 QWidget* parent) :
    GtWizard(parent),
    m_provider(provider)
{
    setWindowTitle(tr("New Project Wizard"));

    // init pages
    if (provider)
    {
        setPage(StartPage, new GtProjectWizardStart(provider, this));
        setPage(SpecPage, new GtProjectWizardSpec(provider, this));
        setPage(ModulesPage, new GtProjectWizardModules(provider, this));
        setPage(ImportPage, new GtProjectWizardImport(provider, this));
    }

}

