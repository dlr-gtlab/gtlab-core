/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 08.06.2022
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECTUPGRADESETTINGSPAGE_H
#define GTPROJECTUPGRADESETTINGSPAGE_H

#include <QWizardPage>

class GtProject;
class GtProjectSpecWidget;
class GtProjectUpgradeDialog;
class QRadioButton;

/**
 * @brief This wizard page is intended to collect any input required from the
 * user for the upgrade process of project data.
 */
class GtProjectUpgradeSettingsPage : public QWizardPage
{
public:
    /**
     * @brief Constructor.
     * @param project Project whose settings should be collected.
     * @param parent Parent widget.
     */
    explicit GtProjectUpgradeSettingsPage(GtProject* project,
                                          GtProjectUpgradeDialog* parent);

    /**
     * @brief The validation routine stores the user input in the upgrade
     * dialog settings.
     *
     * @return True if all user input is correct stored.
     */
    bool validatePage() override;

    bool isComplete() const override;

private:
    /// RadioButton widget to allow the user to specify whether the project
    /// data should be overwritten by the data upgrade process.
    QRadioButton* m_overwrite;

    /// Specification widget for collecting new project settings. Mainly
    /// new project identification string and storage location.
    GtProjectSpecWidget* m_specWid;

    /// Pointer to the main dialog to store needed information after page
    /// validation.
    GtProjectUpgradeDialog* m_wiz;

};

#endif // GTPROJECTUPGRADESETTINGSPAGE_H
