/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 08.06.2022
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECTUPGRADEDIALOG_H
#define GTPROJECTUPGRADEDIALOG_H

#include <QPointer>

#include "gt_wizard.h"
#include "gt_project.h"

/**
 * @brief This dialog guides the user step by step through the upgrade process
 * of project data, if framework modules offer updates for their datasets
 * After all the steps have been successfully completed, the user inputs can be
 * read out.
 * NOTE: The dialog only collects information. The upgrade process of the
 * project data must be done in a subsequent step.
 */
class GtProjectUpgradeDialog : public GtWizard
{
    Q_OBJECT

public:
    enum {
        StartPage = 0,
        OverviewPage,
        SettingsPage
    };

    /**
     * @brief Constructor.
     * @param project Project whose data must be updated.
     * @param parent Parent widget pointer.
     */
    explicit GtProjectUpgradeDialog(GtProject* project,
                                    QWidget* parent = nullptr);

    /**
     * @brief Updates settings for project data upgrade
     * @param overwrite Indicator whether the current project should be
     * overwritten.
     * @param newId If the project is not overwritten, it is cloned. An ID for
     * the new project can be specified here.
     * @param newPath If the project is not overwritten, it is cloned. The
     * storage location for the new project can be specified here.
     */
    void updateSettings(bool overwrite, const QString& newId,
                        const QString& newPath);

    /**
     * @brief Returns the current value of the indicator that indicates whether
     * the current project should be overwritten when upgrading project data.
     * @return True if project data will be overwritten, otherwise a new
     * (cloned) project will be created.
     */
    bool overwriteExistingDataAllowed() const;

    /**
     * @brief If the project is not overwritten, it is cloned. This function
     * returns the new project identification string.
     * @return Project identification string of cloned data.
     */
    QString newProjectName() const;

    /**
     * @brief If the project is not overwritten, it is cloned. This function
     * returns the new project storage location on hard drive.
     * @return Storage location on hard drive of cloned data.
     */
    QString newProjectPath() const;

private:
    /// Project
    QPointer<GtProject> m_project;

    /// Indicator whether the current project should be overwritten on data
    /// upgrade
    bool m_overwrite = true;

    /// Project identification string of cloned data
    QString m_newId;

    /// Storage location on hard drive of cloned data
    QString m_newPath;

};

#endif // GTPROJECTUPGRADEDIALOG_H
