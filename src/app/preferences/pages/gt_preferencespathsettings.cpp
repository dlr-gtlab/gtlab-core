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
#include "gt_logging.h"

#include "gt_preferencespathsettings.h"

GtPreferencesPathSettings::GtPreferencesPathSettings()
{
    setTitle(tr("Environment Variables"));

    QTableView* view = new QTableView;

    m_model = new GtEnvironmentModel(gtEnvironment->varIds(), this);
    view->setModel(m_model);

    view->horizontalHeader()->setStretchLastSection(true);
    view->resizeColumnsToContents();
    view->verticalHeader()->setVisible(false);
    view->verticalHeader()->setDefaultSectionSize(16);

    layout()->addWidget(view);

    loadSettings();
}

void
GtPreferencesPathSettings::saveSettings()
{
    m_model->saveVariables();
}

void
GtPreferencesPathSettings::loadSettings()
{
//    gtEnvironment->debugEnvironmentVariables();
}
