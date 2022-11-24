/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPREFERENCESAPP_H
#define GTPREFERENCESAPP_H

#include "gt_preferencespage.h"

class QCheckBox;
class QSpinBox;
class QComboBox;

/**
 * @brief The GtPreferencesApp class
 */
class GtPreferencesApp : public GtPreferencesPage
{
    Q_OBJECT

public:
    /**
     * @brief GtPreferencesApp
     */
    GtPreferencesApp();

    /**
     * @brief saveSettings
     */
    void saveSettings(GtSettings&) const override;

    /**
     * @brief loadSettings
     */
    void loadSettings(const GtSettings&) override;

private:
    /// Open welcome page check box
    QCheckBox* m_openWelcomePage;

    /// Search for updates on startup check box
    QCheckBox* m_updateAtStartup;

    /// Open last opened project check box
    QCheckBox* m_lastOpenedProject;

    /// Checbox for extended process executor
    QCheckBox* m_useExtendedProcessExecutor;

    /// Checkbox to autostart process runner
    QCheckBox* m_autostartProcessExecutor;

    /// Max. logging length spin box
    QSpinBox* m_maxLogSpin;

    /// Select the logging verbosity
    QComboBox* m_verbositySelection;

    /// Select the theme to use (by system, dark, bright)
    QComboBox* m_themeSelection;

private slots:
    /**
     * @brief onAutoSaveTriggered
     * @param val
     */
    void onAutoSaveTriggered(bool val);

};

#endif // GTPREFERENCESAPP_H
