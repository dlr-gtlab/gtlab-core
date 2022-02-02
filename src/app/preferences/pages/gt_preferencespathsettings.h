/* GTlab - Gas Turbine laboratory
 * Source File: gt_preferencespathsettings.h
 * copyright 2009-2017 by DLR
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
    void saveSettings() override;

private:
    /// Envirnoment model
    GtEnvironmentModel* m_model;

    /**
     * @brief Loades environment settings.
     */
    void loadSettings() override;

};

#endif // GTPREFERENCESPATHSETTINGS_H
