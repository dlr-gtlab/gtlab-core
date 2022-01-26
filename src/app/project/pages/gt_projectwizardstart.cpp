/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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

