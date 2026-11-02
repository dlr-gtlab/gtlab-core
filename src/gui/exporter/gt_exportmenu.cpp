/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_exportmenu.cpp
 *
 *  Created on: 10.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_icons.h"
#include "gt_exporthandler.h"
#include "gt_exportermetadata.h"
#include "gt_project.h"
#include "gt_abstractexporter.h"
#include "gt_abstractexporterpage.h"
#include "gt_wizard.h"

#include "gt_exportmenu.h"

GtExportMenu::GtExportMenu(GtObject* obj, QWidget* parent): QMenu(parent),
    m_obj(obj)
{
    setTitle(tr("Export"));
    setIcon(gt::gui::icon::export_());

    const QList<GtExporterMetaData> exporterList =
        gtExportHandler->exporterMetaData(obj->metaObject()->className());

    if (!exporterList.isEmpty())
    {
        for (const GtExporterMetaData& exp : exporterList)
        {
            QAction* act = addAction(exp.icon(),
                                     exp.id() + QStringLiteral("..."));

            connect(act, &QAction::triggered, this, [this, act](){
                onActionTrigger(act);
            });

            m_actions.insert(act, exp.classname());
        }
    }
}

void
GtExportMenu::onActionTrigger(QObject* obj)
{
    if (!m_obj) return;

    const GtProject* project = m_obj->findParent<GtProject*>();
    if (!project) return;

    // get action
    QAction* act = qobject_cast<QAction*>(obj);
    if (!act) return;

    // get classname of exporter corresponding to action
    const QString str = m_actions.value(act);
    if (str.isEmpty()) return;

    // get exporter corresponding to classname
    auto exporter = gtExportHandler->makeExporter(str);
    if (!exporter) return;

    // clone object data
    std::unique_ptr<GtObject> copy{m_obj->clone()};
    if (!copy) return;

    // create wizard
    GtWizard wizard(parentWidget());
    wizard.setWindowTitle(tr("Export Wizard"));

    // get wizard pages
    const QList<GtAbstractExporterPage*> pages = exporter->pages(copy.get());

    // append pages to wizard
    for (GtAbstractExporterPage* page : pages)
    {
        wizard.addPage(page);
    }

    wizard.exec();
}
