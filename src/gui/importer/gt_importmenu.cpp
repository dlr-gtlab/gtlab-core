/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_importmenu.cpp
 *
 *  Created on: 08.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QSignalMapper>

#include "gt_application.h"
#include "gt_importhandler.h"
#include "gt_importermetadata.h"
#include "gt_project.h"
#include "gt_abstractimporter.h"
#include "gt_abstractimporterpage.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"
#include "gt_command.h"
#include "gt_wizard.h"
#include "gt_icons.h"

#include "gt_importmenu.h"

GtImportMenu::GtImportMenu(GtObject* obj, QWidget* parent): QMenu(parent),
    m_signalMapper(new QSignalMapper(this)),
    m_obj(obj)
{
    setTitle(tr("Import"));
    setIcon(gt::gui::icon::import());

    connect(m_signalMapper, SIGNAL(mapped(QObject*)),
            SLOT(onActionTrigger(QObject*)));

    QList<GtImporterMetaData> importerList =
            gtImportHandler->importerMetaData(obj->metaObject()->className());


    if (!importerList.isEmpty())
    {
        foreach (const GtImporterMetaData& imp, importerList)
        {

            QAction* act = addAction(imp.icon(),
                                     imp.id() + QStringLiteral("..."));

            connect(act, SIGNAL(triggered(bool)), m_signalMapper, SLOT(map()));
            m_signalMapper->setMapping(act, act);

            m_actions.insert(act, imp.classname());
        }
    }
}

void
GtImportMenu::onActionTrigger(QObject* obj)
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

    // get classname of importer corresponding to action
    const QString str = m_actions.value(act);

    // check whether classname is empty
    if (str.isEmpty())
    {
        return;
    }

    // get importer corresponding to classname
    GtAbstractImporter* importer = gtImportHandler->newImporter(str);

    // check importer
    if (!importer)
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
    wizard.setWindowTitle(tr("Import Wizard"));

    // get wizard pages
    QList<GtAbstractImporterPage*> pages = importer->pages(objCopy);

    // append pages to wizard
    foreach (GtAbstractImporterPage* page, pages)
    {
        wizard.addPage(page);
    }

    if (wizard.exec())
    {
        GtObjectMemento mementoOrig = m_obj->toMemento();
        GtObjectMemento mementoNew = objCopy->toMemento();

        GtObjectMementoDiff diff(mementoOrig, mementoNew);

        if (!diff.isNull())
        {
            QString msg = tr("Import") +
                             QStringLiteral(" ") +
                             importer->objectName() +
                             QStringLiteral(" ") +
                             m_obj->objectName();

            GtCommand command = gtApp->startCommand(project, msg);

            m_obj->applyDiff(diff);

            gtApp->endCommand(command);
        }
    }

    delete objCopy;
    delete importer;
}
