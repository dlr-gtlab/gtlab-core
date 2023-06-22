/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 07.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPREFERENCESLANGUAGE_H
#define GTPREFERENCESLANGUAGE_H

#include "gt_preferencespage.h"

/**
 * @brief The GtPreferencesLanguage class
 */
class GtPreferencesLanguage : public GtPreferencesPage
{
    Q_OBJECT

public:
    /**
     * @brief GtPreferencesLanguage
     */
    GtPreferencesLanguage();

    /**
     * @brief saveSettings
     */
    void saveSettings(GtSettings&) const override;

    /**
     * @brief loadSettings
     */
    void loadSettings(const GtSettings&) override;

};

#endif // GTPREFERENCESLANGUAGE_H
