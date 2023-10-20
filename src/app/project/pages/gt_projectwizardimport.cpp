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
#include <QRadioButton>

#include "gt_projectwizardimport.h"

GtProjectWizardImport::GtProjectWizardImport(GtProjectProvider* provider,
                                             QWidget* parent) :
    GtAbstractProjectWizardPage(provider, parent)
{
    setTitle(tr("Import Data"));

    QVBoxLayout* layout = new QVBoxLayout;

    // import selections
    QRadioButton* emptyButton = new QRadioButton(tr("Empty Project"));
    emptyButton->setChecked(true);
    layout->addWidget(emptyButton);

    QRadioButton* importButton = new QRadioButton(tr("From Source"));
    importButton->setChecked(false);
    importButton->setDisabled(true);
    layout->addWidget(importButton);

    layout->addStretch(1);

    setLayout(layout);
}

