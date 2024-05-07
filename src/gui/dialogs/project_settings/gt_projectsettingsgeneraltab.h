/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_projectsettingsexternalizationtab.h
 *
 *  Created on: 07.05.2022
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECTSETTINGSGENERALTAB_H
#define GTPROJECTSETTINGSGENERALTAB_H

#include "gt_projectsettingstab.h"

class QCheckBox;

/**
 * @brief The GtProjectSettingsGeneralTab class
 */
class GtProjectSettingsGeneralTab : public GtProjectSettingsTab
{
    Q_OBJECT

public:

    /**
     * @brief GtProjectSettingsGeneralTab
     * @param project pointer to valid project
     * @param parent parent widget
     */
    GtProjectSettingsGeneralTab(GtProject* project,
                                QWidget* parent = {});

    /**
     * @brief method called when dialog was accepted
     */
    void saveSettings() override;

    /**
     * @brief display name of this tab
     * @return tab name
     */
    QString tabName() const override;

private:

    /// checkbox for setup ignore of project irregularity alerts
    QCheckBox* m_ignoreCheck{};

};

#endif // GTPROJECTSETTINGSGENERALTAB_H
