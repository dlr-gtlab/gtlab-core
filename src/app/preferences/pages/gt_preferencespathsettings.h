/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_preferencespathsettings.h
 *
 *  Created on: 03.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GTPREFERENCESPATHSETTINGS_H
#define GTPREFERENCESPATHSETTINGS_H

#include "gt_preferencespage.h"

class GtEnvironmentModel;

/**
 * @brief The GtPreferencesPathSettings class
 */
class GtPreferencesPathSettings : public GtPreferencesPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    GtPreferencesPathSettings();

    /**
     * @brief Saves environment settings.
     */
    void saveSettings(GtSettings&) const override;

    /**
     * @brief Loades environment settings.
     */
    void loadSettings(const GtSettings&) override;

private:
    /// Envirnoment model
    GtEnvironmentModel* m_model;

    void loadSettingsImpl();
};

#endif // GTPREFERENCESPATHSETTINGS_H
