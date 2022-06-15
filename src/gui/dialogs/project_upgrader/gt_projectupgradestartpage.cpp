/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 08.06.2022
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QLabel>

#include "gt_projectupgradestartpage.h"

GtProjectUpgradeStartPage::GtProjectUpgradeStartPage(QWidget* parent) :
    QWizardPage(parent)
{
    setTitle(tr("Outdated Project Data"));

    QVBoxLayout* layout = new QVBoxLayout;

    QLabel* label = new QLabel(tr("Some of the project data is outdated and "
                                  "requires an upgrade before the project can "
                                  "be opened.\n"
                                  "This wizard will help you "
                                  "to upgrade GTlab Project data"));

    layout->addWidget(label);
    layout->addStretch(1);

    setLayout(layout);
}
