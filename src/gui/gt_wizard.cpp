/* GTlab - Gas Turbine laboratory
 * Source File: gt_wizard.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 02.12.2021
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */

#include "gt_wizard.h"
#include "gt_application.h"


GtWizard::GtWizard(QWidget* parent) :
    QWizard(parent)
{
    setWizardStyle(QWizard::ModernStyle);
    setAutoFillBackground(false);

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);
}
