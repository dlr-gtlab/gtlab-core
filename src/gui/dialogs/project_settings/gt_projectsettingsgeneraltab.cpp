/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_projectsettingsexternalizationtab.h
 *
 *  Created on: 07.05.2022
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include <QCheckBox>
#include <QVBoxLayout>
#include <QSpacerItem>

#include "gt_project.h"

#include "gt_projectsettingsgeneraltab.h"

GtProjectSettingsGeneralTab::GtProjectSettingsGeneralTab(
    GtProject* project,
    QWidget* parent):
    GtProjectSettingsTab{project, parent}
{
    m_ignoreCheck = new QCheckBox(
        tr("Hide irregularity alerts for this project"));

    if (project) m_ignoreCheck->setChecked(project->ignoringIrregularities());

    layout()->addWidget(m_ignoreCheck);
    layout()->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum,
                                      QSizePolicy::Expanding));
}

void
GtProjectSettingsGeneralTab::saveSettings()
{
    if (!project()) return;

    project()->setIgnoreIrregularities(m_ignoreCheck->isChecked());
}

QString
GtProjectSettingsGeneralTab::tabName() const
{
    return tr("General");
}
