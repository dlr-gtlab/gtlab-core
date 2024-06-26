/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_projectsettingsdialog.cpp
 *
 *  Created on: 31.03.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_projectsettingsdialog.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>

#include "gt_logging.h"
#include "gt_projectsettingstab.h"
#include "gt_projectsettingsgeneraltab.h"
#include "gt_projectsettingsexternalizationtab.h"
#include "gt_icons.h"


GtProjectSettingsDialog::GtProjectSettingsDialog(GtProject* project) :
    GtDialog()
{
    setWindowTitle(tr("Project Settings"));
    setWindowIcon(gt::gui::icon::config());

    if (!project)
    {
        gtError() << tr("Invalid project! Unable do create Project Settings");
        return;
    }

    m_layout = new QVBoxLayout{this};
    m_tabWidget = new QTabWidget{};
    m_tabWidget->setMinimumWidth(400);
    m_tabWidget->setMinimumHeight(300);

    auto* saveButton = new QPushButton{tr("Save")};
    saveButton->setIcon(gt::gui::icon::save());
    auto* closeButton = new QPushButton{tr("Cancel")};
    closeButton->setIcon(gt::gui::icon::cancel());

    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveChanges()));

    auto* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setContentsMargins(4, 4, 4, 4);
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(saveButton);
    buttonsLayout->addWidget(closeButton);

    m_layout->addWidget(m_tabWidget);
    m_layout->addLayout(buttonsLayout);

    addTab(std::make_unique<GtProjectSettingsGeneralTab>(project));
    addTab(std::make_unique<GtProjectSettingsExternalizationTab>(project));
}

void
GtProjectSettingsDialog::saveChanges()
{
    for (int i = 0; i < m_tabWidget->count(); i++)
    {
        if (auto* tab = dynamic_cast<GtProjectSettingsTab*>(
                    m_tabWidget->widget(i)))
        {
            tab->saveSettings();
        }
    }

    accept();
}

void
GtProjectSettingsDialog::addTab(std::unique_ptr<GtProjectSettingsTab>&& tab)
{
    auto* tabPtr = tab.release();
    m_tabWidget->addTab(tabPtr, tabPtr->tabName());

    // forwarding singal
    connect(tabPtr, SIGNAL(projectSaveRequested()),
            this, SIGNAL(projectSaveRequested()));
    connect(tabPtr, SIGNAL(projectCloseRequested()),
            this, SIGNAL(projectCloseRequested()));
}
