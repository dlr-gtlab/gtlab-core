
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
