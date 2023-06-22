/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.06.2022
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECTUPGRADEOVERVIEWPAGE_H
#define GTPROJECTUPGRADEOVERVIEWPAGE_H

#include <QWizardPage>

class GtProject;

/**
 * @brief This wizard page automatically displays the available data upgrades
 * of a given project.
 */
class GtProjectUpgradeOverviewPage : public QWizardPage
{
public:
    /**
     * @brief Constructor.
     * @param project Project whose upgrade information should be displayed.
     * @param parent Parent widget.
     */
    explicit GtProjectUpgradeOverviewPage(GtProject* project,
                                          QWidget* parent = nullptr);

};

#endif // GTPROJECTUPGRADEOVERVIEWPAGE_H
