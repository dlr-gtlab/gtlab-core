/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_abstractprojectwizardpage.h"
#include "gt_projectprovider.h"
#include "gt_colors.h"
#include "gt_application.h"

GtAbstractProjectWizardPage::GtAbstractProjectWizardPage(
        GtProjectProvider* provider, QWidget* parent) : QWizardPage(parent),
    m_provider(provider)
{

}

bool
GtAbstractProjectWizardPage::isValid()
{
    return (m_provider != nullptr);
}

