/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 12.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QString>
#include <QApplication>

#include "gt_guimoduleloader.h"
#include "gt_moduleinterface.h"
#include "gt_mdiinterface.h"
#include "gt_importerinterface.h"
#include "gt_exporterinterface.h"
#include "gt_mdilauncher.h"
#include "gt_posttemplatefactory.h"
#include "gt_importhandler.h"
#include "gt_exporthandler.h"
#include "gt_logging.h"
#include "gt_propertyinterface.h"
#include "gt_propertyitemfactory.h"
#include "gt_collectioninterface.h"
#include "gt_collectionsettings.h"
#include "gt_application.h"

// default ui object includes
#include "gt_project.h"
#include "gt_projectui.h"
#include "gt_task.h"
#include "gt_processui.h"
#include "gt_objectgroup.h"
#include "gt_objectgroupui.h"
#include "gt_calculator.h"
#include "gt_calculatorui.h"
#include "gt_label.h"
#include "gt_tasklink.h"
#include "gt_processlinkui.h"
#include "gt_label.h"
#include "gt_labelui.h"
#include "gt_loop.h"
#include "gt_residualloop.h"
#include "gt_parameterloop.h"
#include "gt_loopui.h"
#include "gt_algorithms.h"
#include "gt_utilities.h"
#include "gt_qtutilities.h"

GtGuiModuleLoader::GtGuiModuleLoader()
{
    registerObjectUI(GT_CLASSNAME(GtProject),
                     GT_METADATA(GtProjectUI));
    registerObjectUI(GT_CLASSNAME(GtTask),
                     GT_METADATA(GtProcessUI));
    registerObjectUI(GT_CLASSNAME(GtTaskLink),
                     GT_METADATA(GtProcessLinkUI));
    registerObjectUI(GT_CLASSNAME(GtObjectGroup),
                     GT_METADATA(GtObjectGroupUI));
    registerObjectUI(GT_CLASSNAME(GtCalculator),
                     GT_METADATA(GtCalculatorUI));
    registerObjectUI(GT_CLASSNAME(GtLabel),
                     GT_METADATA(GtLabelUI));
    registerObjectUI(GT_CLASSNAME(GtLoop),
                     GT_METADATA(GtLoopUI));
    registerObjectUI(GT_CLASSNAME(GtResidualLoop),
                     GT_METADATA(GtLoopUI));
    registerObjectUI(GT_CLASSNAME(GtParameterLoop),
                     GT_METADATA(GtLoopUI));
}

GtGuiModuleLoader::~GtGuiModuleLoader()
{
    gt::for_each_key(m_uiObjs, [this](const QString& key)
    {
        qDeleteAll(m_uiObjs.value(key));
    });
}

GtObjectUIList
GtGuiModuleLoader::objectUI(GtObject* obj) const
{
    assert(obj);
    assert(obj->metaObject());
    return objectUI(obj->metaObject()->className());
}

GtObjectUIList
GtGuiModuleLoader::objectUI(const QString& classname) const
{
    if (!m_uiObjs.contains(classname))
    {
        return GtObjectUIList();
    }

    return m_uiObjs.value(classname);
}

QStringList
GtGuiModuleLoader::knownUIObjects() const
{
    QStringList retval;

    gt::for_each_key(m_uiObjs, [&retval](const QString& key)
    {
        retval << key;
    });

    return retval;
}

bool
GtGuiModuleLoader::check(GtModuleInterface* plugin) const
{
    QApplication::processEvents();
    const auto errorString = [=](){
        return QObject::tr("Loading module '%1' failed:").arg(plugin->ident());
    };

    if (!GtCoreModuleLoader::check(plugin))
    {
        return false;
    }

    if (gtApp->batchMode())
    {
        return true;
    }

    GtMdiInterface* mdip = checkInterface<GtMdiInterface>(
        plugin->ident(), plugin);

    // contains dynamic linked mdi classes
    if (mdip)
    {
        if (gtMdiLauncher->containsDuplicates(mdip->mdiItems()))
        {
            gtWarning() << errorString()
                        << QObject::tr("A mdi item is already defined!");
            return false;
        }

        if (!gtMdiLauncher->allInvokable(mdip->mdiItems()))
        {
            gtWarning() << errorString()
                        << QObject::tr("Not all mdi items are invokable!");
            return false;
        }

        if (gtMdiLauncher->dockWidgetsExist(mdip->dockWidgets()))
        {
            gtWarning() << errorString()
                        << QObject::tr("A dock widget is already defined!");
            return false;
        }

        // TODO: widget invokable check
        //            if (!gtMdiLauncher->allInvokable(mdip->dockWidgets()))
        //            {
        //                return false;
        //            }

        // TODO: ui items duplicate and invokable check

        if (gtPostTemplateFactory->containsDuplicates(mdip->postItems()))
        {
            gtWarning() << errorString()
                        << QObject::tr("A post item is already defined!");
            return false;
        }

        if (!gtPostTemplateFactory->allInvokable(mdip->postItems()))
        {
            gtWarning() << errorString()
                        << QObject::tr("Not all post items are invokable!");
            return false;
        }

    }

    // importer interface
    GtImporterInterface* impp = checkInterface<GtImporterInterface>(
        plugin->ident(), plugin);

    if (impp)
    {
        if (gtImportHandler->containsDuplicates(impp->importer()))
        {
            gtWarning() << errorString()
                        << QObject::tr("An importer is already defined!");
            return false;
        }

        if (!gtImportHandler->allInvokable(impp->importer()))
        {
            gtWarning() << errorString()
                        << QObject::tr("Not all importers are invokable!");
            return false;
        }
    }

    // exporter interface
    GtExporterInterface* expp = checkInterface<GtExporterInterface>(
        plugin->ident(), plugin);

    if (expp)
    {
        if (gtExportHandler->containsDuplicates(expp->exporter()))
        {
            gtWarning() << errorString()
                        << QObject::tr("An exporter is already defined!");
            return false;
        }

        if (!gtExportHandler->allInvokable(expp->exporter()))
        {
            gtWarning() << errorString()
                        << QObject::tr("Not all exporters are invokable!");
            return false;
        }
    }

    // property interface
    GtPropertyInterface* prop = checkInterface<GtPropertyInterface>(
        plugin->ident(), plugin);

    if (prop)
    {
        if (gtPropertyItemFactory->propertyItemsExists(
                prop->propertyItems()))
        {
            gtWarning() << errorString()
                        << QObject::tr("A property item is already defined!");
            return false;
        }

        if (!gtPropertyItemFactory->propertyItemsInvokable(
                prop->propertyItems()))
        {
            gtWarning() << errorString()
                        << QObject::tr("Not all propety items are invokable!");
            return false;
        }
    }

    // collection interface
    GtCollectionInterface* coll = checkInterface<GtCollectionInterface>(
        plugin->ident(), plugin);

    if (coll)
    {
        QMetaObject meta = coll->collectionSettings();

        std::unique_ptr<QObject> obj(meta.newInstance());

        if (!gt::unique_qobject_cast<GtCollectionSettings>(std::move(obj)))
        {
            gtWarning() << errorString()
                        << QObject::tr("Invalid collection settings!");
            return false;
        }
    }

    return true;
}

void
GtGuiModuleLoader::insert(GtModuleInterface* plugin)
{
    if (!plugin) return;

    QApplication::processEvents();
    GtCoreModuleLoader::insert(plugin);

    GtMdiInterface* mdip = checkInterface<GtMdiInterface>(plugin->ident(),
                                                          plugin);

    // contains dynamic linked mdi classes
    if (mdip && !gtApp->batchMode())
    {
        gtPostTemplateFactory->registerClasses(mdip->postItems());

        gtPostTemplateFactory->registerPlots(mdip->postPlots());

        gtMdiLauncher->registerClasses(mdip->mdiItems());
        gtMdiLauncher->registerDockWidgets(mdip->dockWidgets());

        QMap<const char*, QMetaObject> uis = mdip->uiItems();

        gt::for_each_key(uis, [&](const char* key)
        {
            QMetaObject metaobj = uis.value(key);
            registerObjectUI(key, metaobj);
        });
    }

    // importer interface
    GtImporterInterface* impp =
        checkInterface<GtImporterInterface>(plugin->ident(), plugin);

    if (impp)
    {
        gtImportHandler->registerClasses(impp->importer());
    }

    // exporter interface
    GtExporterInterface* expp =
        checkInterface<GtExporterInterface>(plugin->ident(), plugin);

    if (expp)
    {
        gtExportHandler->registerClasses(expp->exporter());
    }

    // property interface
    GtPropertyInterface* prop =
        checkInterface<GtPropertyInterface>(plugin->ident(), plugin);

    if (prop)
    {
        gtPropertyItemFactory->registerPropertyItems(prop->propertyItems());
    }

    // collection interface
    GtCollectionInterface* coll =
            checkInterface<GtCollectionInterface>(plugin->ident(), plugin);

    if (coll)
    {
        gtMdiLauncher->registerCollection(coll->collectionId(),
                                          coll);
    }
}

void
GtGuiModuleLoader::registerObjectUI(const char* classId,
                                    const QMetaObject& metaObj)
{
    GtObjectUI* uio = qobject_cast<GtObjectUI*>(metaObj.newInstance());

    if (!uio)
    {
        gtError().nospace()
                << QObject::tr("Object UI not invokable!")
                << " (" << classId << ')';
        return;
    }

    m_uiObjs[classId].append(uio);
}

