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
#include "gt_icons.h"

#include "gt_refusedpluginsdialog.h"

GtRefusedPluginsDialog::GtRefusedPluginsDialog(QWidget* parent) :
    GtDialog(parent)
{
    assert(gtApp);

    setWindowTitle(tr("Disabled Plugins"));
    setWindowIcon(gt::gui::icon::plugin16());

    m_model = new GtCheckableStringListModel(this);
    m_model->setStringList(gtApp->crashedModules(), Qt::Unchecked);

    auto* pluginsView = new QListView;
    pluginsView->setModel(m_model);
    pluginsView->setSizeAdjustPolicy(QListView::AdjustToContents);

    auto* infoLabel = new QLabel(
        tr("The following plugins caused a crash in a previous application "
           "run and will be skipped in the loading procedure. "
           "To reenable a plugin in the next loading procedure reactivte the "
           "corresponding entry.")
    );
    infoLabel->setWordWrap(true);

    auto* applyBtn = new QPushButton(tr("Apply"));

    auto* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(applyBtn);

    auto* baseLay = new QVBoxLayout;
    baseLay->addWidget(infoLabel);
    baseLay->addWidget(pluginsView);
    baseLay->addLayout(buttonsLayout);

    setLayout(baseLay);

    QObject::connect(applyBtn, SIGNAL(clicked(bool)), this, SLOT(accept()));

    /* because I cannot get this **shitty** view to autoresize properly... */
    auto margins = baseLay->contentsMargins();
    // first we substract the actual width of the view and
    // add the preferred contents width
    int width = this->width() - pluginsView->width() +
                pluginsView->sizeHintForColumn(0);
    // append margins (+ some for good measure)
    width += margins.left() + 2 * margins.right();

    resize(width, sizeHint().height());
}

void
GtRefusedPluginsDialog::sync() const
{
    QStringList disabledPlugins = gtApp->crashedModules();
    for (auto const& plugin : m_model->selectedStringList())
    {
        disabledPlugins.removeOne(plugin);
    }

    QSettings settings(gtApp->localApplicationIniFilePath(),
                       QSettings::IniFormat);
    settings.setValue(QStringLiteral("loading_crashed"), disabledPlugins);
    settings.sync();
}
