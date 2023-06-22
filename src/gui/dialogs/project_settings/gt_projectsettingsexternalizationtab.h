/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_projectsettingsexternalizationtab.h
 *
 *  Created on: 31.03.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef GTPROJECTSETTINGSEXTERNALIZATIONTAB_H
#define GTPROJECTSETTINGSEXTERNALIZATIONTAB_H

#include "gt_projectsettingstab.h"

class GtState;
class QCheckBox;
/**
 * @brief The GtProjectSettingsExternalizationTab class
 */
class GtProjectSettingsExternalizationTab : public GtProjectSettingsTab
{
    Q_OBJECT

public:

    /**
     * @brief GtProjectSettingsExternalizationTab
     * @param project pointer to valid project
     * @param parent parent widget
     */
    explicit GtProjectSettingsExternalizationTab(GtProject* project,
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

    /// checkbox for enabling externalization
    QCheckBox* m_extCheckBox{};
    /// state for enabling externalization
    GtState* m_extState{};
};

#endif // GTPROJECTSETTINGSEXTERNALIZATIONTAB_H
