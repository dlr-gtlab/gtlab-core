/* GTlab - Gas Turbine laboratory
 * Source File: gt_projectsettingsdialog.cpp
 * copyright 2009-2021 by DLR
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
#include "gt_projectsettingsexternalizationtab.h"
#include "gt_icons.h"


GtProjectSettingsDialog::GtProjectSettingsDialog(GtProject* project)
{
    setWindowTitle(tr("Project Settings"));

    if (project == nullptr)
    {
        gtError() << tr("Invalid project! Unable do create Project Settings");
        return;
    }

    m_layout = new QVBoxLayout{this};
    m_tabWidget = new QTabWidget{};
    m_tabWidget->setMinimumWidth(400);
    m_tabWidget->setMinimumHeight(300);

    auto* saveButton = new QPushButton{tr("Save")};
    saveButton->setIcon(GtGUI::Icon::saveProject16());
    auto* closeButton = new QPushButton{tr("Cancel")};
    closeButton->setIcon(GtGUI::Icon::close24());

    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveChanges()));

    auto* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setContentsMargins(4, 4, 4, 4);
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(saveButton);
    buttonsLayout->addWidget(closeButton);

    m_layout->addWidget(m_tabWidget);
    m_layout->addLayout(buttonsLayout);

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
