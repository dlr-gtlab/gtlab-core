/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPREFERENCESPLUGINS_H
#define GTPREFERENCESPLUGINS_H

#include "gt_preferencespage.h"

/**
 * @brief The GtPreferencesPlugins class
 */
class GtPreferencesPlugins : public GtPreferencesPage
{
    Q_OBJECT

public:
    /**
     * @brief GtPreferencesPlugins
     */
    GtPreferencesPlugins();

    /**
     * @brief saveSettings
     */
    void saveSettings() override;

private:
    /**
     * @brief loadSettings
     */
    void loadSettings() override;

};

#endif // GTPREFERENCESPLUGINS_H
