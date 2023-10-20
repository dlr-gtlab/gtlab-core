/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.06.2022
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECTUPGRADESTARTPAGE_H
#define GTPROJECTUPGRADESTARTPAGE_H

#include <QWizardPage>

/**
 * @brief This wizard page for the introduction of the data upgrade process.
 */
class GtProjectUpgradeStartPage : public QWizardPage
{
public:
    /**
     * @brief Constructor.
     * @param parent Parent widget.
     */
    explicit GtProjectUpgradeStartPage(QWidget* parent = nullptr);

};

#endif // GTPROJECTUPGRADESTARTPAGE_H
