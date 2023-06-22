/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 13.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QLabel>

#include "gt_projectwizardstart.h"

GtProjectWizardStart::GtProjectWizardStart(GtProjectProvider* provider,
                                           QWidget* parent) :
    GtAbstractProjectWizardPage(provider, parent)
{
    setTitle(tr("Create New Project"));

    QVBoxLayout* layout = new QVBoxLayout;

    QLabel* label = new QLabel(tr("This wizard will help you "
                                  "create a new GTlab Project"));

    layout->addWidget(label);
    layout->addStretch(1);

    setLayout(layout);
}

