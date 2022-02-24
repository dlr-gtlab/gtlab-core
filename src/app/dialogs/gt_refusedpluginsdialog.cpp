/* GTlab - Gas Turbine laboratory
 * Source File: gt_refusedpluginsdialog.cpp
 * copyright 2009-2019 by DLR
 *
 *  Created on: 04.09.2019
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QDir>
#include <QListView>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>

#include "gt_application.h"
#include "gt_checkablestringlistmodel.h"

#include "gt_refusedpluginsdialog.h"

GtRefusedPluginsDialog::GtRefusedPluginsDialog(QWidget* parent) :
    QDialog(parent)
{
    setWindowTitle(tr("Disabled Plugins"));
    setWindowIcon(gtApp->icon(QStringLiteral("pluginIcon_16.png")));

    m_model = new GtCheckableStringListModel(gtApp->crashedModules(), this);

    QListView* crashView = new QListView;
    crashView->setModel(m_model);

    QVBoxLayout* crashLay = new QVBoxLayout;
    QLabel* crashLbl = new QLabel(tr("The following plugins caused a "
                                     "application crash and are "
                                     "skipped in the loading "
                                     "procedure.\nDeactivate the "
                                     "corresponding entry to continue "
                                     "the loading process and include "
                                     "the plugin."));

    QPushButton* crashBtn = new QPushButton(tr("Ok"));

    crashLay->addWidget(crashLbl);
    crashLay->addWidget(crashView);
    crashLay->addWidget(crashBtn);

    setLayout(crashLay);

    QObject::connect(crashBtn, SIGNAL(clicked(bool)),
                     this, SLOT(accept()));
}

void
GtRefusedPluginsDialog::sync() const
{
    QString iniFileName = gtApp->roamingPath() + QDir::separator() +
                          QStringLiteral("last_run.ini");
    QSettings settings(iniFileName, QSettings::IniFormat);

    settings.setValue(QStringLiteral("loading_crashed"),
                      m_model->selectedStringList());
    settings.sync();
}
