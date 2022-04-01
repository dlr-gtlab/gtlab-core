/* GTlab - Gas Turbine laboratory
 * Source File: gt_exportmenu.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QSignalMapper>
#include <QDebug>

#include "gt_application.h"
#include "gt_exporthandler.h"
#include "gt_exportermetadata.h"
#include "gt_project.h"
#include "gt_abstractexporter.h"
#include "gt_abstractexporterpage.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"
#include "gt_wizard.h"

#include "gt_exportmenu.h"

GtExportMenu::GtExportMenu(GtObject* obj, QWidget* parent): QMenu(parent),
    m_signalMapper(new QSignalMapper(this)),
    m_obj(obj)
{
    setTitle(tr("Export"));
    setIcon(gtApp->icon(QStringLiteral("exportIcon_16.png")));

    connect(m_signalMapper, SIGNAL(mapped(QObject*)),
            SLOT(onActionTrigger(QObject*)));

    QList<GtExporterMetaData> exporterList =
            gtExportHandler->exporterMetaData(obj->metaObject()->className());


    if (!exporterList.isEmpty())
    {
        foreach (const GtExporterMetaData& exp, exporterList)
        {

            QAction* act = addAction(exp.icon(),
                                     exp.id() + QStringLiteral("..."));

            connect(act, SIGNAL(triggered(bool)), m_signalMapper, SLOT(map()));
            m_signalMapper->setMapping(act, act);

            m_actions.insert(act, exp.classname());
        }
    }
}

void
GtExportMenu::onActionTrigger(QObject* obj)
{
    // check object
    if (!m_obj)
    {
        return;
    }

    GtProject* project = m_obj->findParent<GtProject*>();

    if (!project)
    {
        return;
    }

    // get action
    QAction* act = qobject_cast<QAction*>(obj);

    // check action
    if (!act)
    {
        return;
    }

    // get classname of exporter corresponding to action
    const QString str = m_actions.value(act);

    // check whether classname is empty
    if (str.isEmpty())
    {
        return;
    }

    qDebug() << "exporter triggered! (" << str << ")";

    // get exporter corresponding to classname
    GtAbstractExporter* exporter = gtExportHandler->newExporter(str);

    // check exporter
    if (!exporter)
    {
        return;
    }

    // clone object data
    GtObject* objCopy = m_obj->clone();

    // check clones object data
    if (!objCopy)
    {
        return;
    }

    // create wizard
    GtWizard wizard(parentWidget());
    wizard.setWindowTitle(tr("Export Wizard"));

    // get wizard pages
    QList<GtAbstractExporterPage*> pages = exporter->pages(objCopy);

    // append pages to wizard
    foreach (GtAbstractExporterPage* page, pages)
    {
        wizard.addPage(page);
    }

    wizard.exec();

    delete objCopy;
    delete exporter;
}
