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

    m_saveLinkedObjects = new QCheckBox(
        tr("Object serialization into own files."));
    m_saveLinkedObjects->setToolTip(
        tr("If checked, some objects are written into separate "
           "*gtobj.xml files. Uncheck, to keep GTlab 2.0 compatibility."));

    loadSettings();

    layout()->addWidget(m_ignoreCheck);
    layout()->addWidget(m_saveLinkedObjects);
    layout()->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum,
                                      QSizePolicy::Expanding));
}

void
GtProjectSettingsGeneralTab::saveSettings()
{
    if (!project()) return;

    auto& settings = project()->projectSettings();

    {
        QSignalBlocker _{settings};
        settings.setIgnoreIrregularities(m_ignoreCheck->isChecked());
        settings.setOwnObjectFileSerializationEnabled(
            m_saveLinkedObjects->isChecked());
    }

    emit settings.changed();
}

void
GtProjectSettingsGeneralTab::loadSettings()
{
    auto proj = project();
    if (!proj) return;

    auto& settings = proj->projectSettings();

    m_ignoreCheck->setChecked(settings.ignoringIrregularities());
    m_saveLinkedObjects->setChecked(
        settings.ownObjectFileSerializationEnabled());

}

QString
GtProjectSettingsGeneralTab::tabName() const
{
    return tr("General");
}
