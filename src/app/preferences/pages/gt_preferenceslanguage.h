/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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
    void saveSettings();

private:
    /**
     * @brief loadSettings
     */
    void loadSettings();

};

#endif // GTPREFERENCESLANGUAGE_H
