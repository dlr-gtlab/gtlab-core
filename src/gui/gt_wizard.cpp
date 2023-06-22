/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_wizard.cpp
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
