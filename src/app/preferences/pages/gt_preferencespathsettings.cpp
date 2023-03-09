/* GTlab - Gas Turbine laboratory
 * Source File: gt_preferencespathsettings.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 03.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QTableView>
#include <QHeaderView>
#include <QVBoxLayout>

#include "gt_environmentmodel.h"
#include "gt_environment.h"
#include "gt_icons.h"
#include "gt_preferencespathsettings.h"

GtPreferencesPathSettings::GtPreferencesPathSettings() :
    GtPreferencesPage(tr("Environment Variables"))
{
    setTitleShort(tr("Environment"));
    setIcon(gt::gui::icon::applicationVar());

    QTableView* view = new QTableView;

    m_model = new GtEnvironmentModel(gtEnvironment->varIds(), this);
    view->setModel(m_model);

    view->horizontalHeader()->setStretchLastSection(true);
    view->resizeColumnsToContents();
    view->verticalHeader()->setVisible(false);
    view->verticalHeader()->setDefaultSectionSize(16);

    auto layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(view);
    setLayout(layout);

    loadSettingsImpl();
}

void
GtPreferencesPathSettings::saveSettings(GtSettings& ) const
{
    m_model->saveVariables();
}

void
GtPreferencesPathSettings::loadSettingsImpl()
{
//    gtEnvironment->debugEnvironmentVariables();
}

void
GtPreferencesPathSettings::loadSettings(const GtSettings&)
{
    loadSettingsImpl();
}
