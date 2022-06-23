/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 08.06.2022
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_projectupgradestartpage.h"
#include "gt_projectupgradeoverviewpage.h"
#include "gt_projectupgradesettingspage.h"

#include "gt_projectupgradedialog.h"

GtProjectUpgradeDialog::GtProjectUpgradeDialog(GtProject* project,
                                               QWidget* parent) :
    GtWizard(parent),
    m_project(project)
{
    setWindowTitle(tr("Upgrade Project Data"));

    setPage(StartPage, new GtProjectUpgradeStartPage(this));
    setPage(OverviewPage, new GtProjectUpgradeOverviewPage(project, this));
    setPage(SettingsPage, new GtProjectUpgradeSettingsPage(project, this));

    setButtonText(QWizard::FinishButton, tr("Upgrade"));
}

void
GtProjectUpgradeDialog::updateSettings(bool overwrite, const QString& newId,
                                       const QString& newPath)
{
    gtDebug() << "GtProjectUpgradeDialog::updateSettings";
    gtDebug() << "  |-> " << m_overwrite;
    gtDebug() << "  |-> " << m_newId;
    gtDebug() << "  |-> " << m_newPath;

    m_overwrite = overwrite;
    m_newId = newId;
    m_newPath = newPath;
}

bool
GtProjectUpgradeDialog::overwriteExistingDataAllowed() const
{
    return m_overwrite;
}

QString
GtProjectUpgradeDialog::newProjectName() const
{
    return m_newId;
}

QString
GtProjectUpgradeDialog::newProjectPath() const
{
    return m_newPath;
}
