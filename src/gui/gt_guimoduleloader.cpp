/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 12.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QString>
#include <QDebug>

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
//#include "gt_result.h"
//#include "gt_resultui.h"
#include "gt_label.h"
#include "gt_tasklink.h"
#include "gt_processlinkui.h"
//#include "gt_datazone0d.h"
//#include "gt_datazone.h"
#include "gt_label.h"
#include "gt_labelui.h"
//#include "gt_datazoneui.h"
#include "gt_loop.h"
#include "gt_residualloop.h"
#include "gt_parameterloop.h"
#include "gt_loopui.h"
#include "gt_algorithms.h"

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
    //registerObjectUI(GT_CLASSNAME(GtResult),
    //                 GT_METADATA(GtResultUI));
    //registerObjectUI(GT_CLASSNAME(GtDataZone0D),
    //                 GT_METADATA(GtDataZoneUI));
    //registerObjectUI(GT_CLASSNAME(GtDataZone),
    //                 GT_METADATA(GtDataZoneUI));
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
    bool retval = GtCoreModuleLoader::check(plugin);

    if (gtApp->batchMode())
    {
        return retval;
    }

    if (retval)
    {
        GtMdiInterface* mdip = dynamic_cast<GtMdiInterface*>(plugin);

        // contains dynamic linked mdi classes
        if (mdip)
        {
            if (gtMdiLauncher->containsDuplicates(mdip->mdiItems()))
            {
                return false;
            }

            if (!gtMdiLauncher->allInvokable(mdip->mdiItems()))
            {
                return false;
            }

            if (gtMdiLauncher->dockWidgetsExist(mdip->dockWidgets()))
            {
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
                return false;
            }

            if (!gtPostTemplateFactory->allInvokable(mdip->postItems()))
            {
                return false;
            }

        }

        // importer interface
        GtImporterInterface* impp = dynamic_cast<GtImporterInterface*>(plugin);

        if (impp)
        {
            if (gtImportHandler->containsDuplicates(impp->importer()))
            {
                return false;
            }

            if (!gtImportHandler->allInvokable(impp->importer()))
            {
                return false;
            }
        }

        // exporter interface
        GtExporterInterface* expp = dynamic_cast<GtExporterInterface*>(plugin);

        if (expp)
        {
            if (gtExportHandler->containsDuplicates(expp->exporter()))
            {
                return false;
            }

            if (!gtExportHandler->allInvokable(expp->exporter()))
            {
                return false;
            }
        }

        // property interface
        GtPropertyInterface* prop = dynamic_cast<GtPropertyInterface*>(plugin);

        if (prop)
        {
            if (gtPropertyItemFactory->propertyItemsExists(
                        prop->propertyItems()))
            {
                return false;
            }

            if (!gtPropertyItemFactory->propertyItemsInvokable(
                        prop->propertyItems()))
            {
                return false;
            }
        }

        // collection interface
        GtCollectionInterface* coll =
                dynamic_cast<GtCollectionInterface*>(plugin);

        if (coll)
        {
            QMetaObject meta = coll->collectionSettings();

            QObject* obj = meta.newInstance();

            if (!obj)
            {
                gtWarning() << plugin->ident() << QStringLiteral(": ") <<
                           QObject::tr("Could not invoke collection settings!");
                return false;
            }

            if (qobject_cast<GtCollectionSettings*>(obj))
            {
                delete obj;
                return true;
            }

            delete obj;
            gtWarning() << plugin->ident() << QStringLiteral(": ") <<
                         QObject::tr("Could not recreate collection settings!");
            return false;
        }
    }

    return retval;
}

void
GtGuiModuleLoader::insert(GtModuleInterface* plugin)
{
    GtCoreModuleLoader::insert(plugin);

    GtMdiInterface* mdip = dynamic_cast<GtMdiInterface*>(plugin);

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
    GtImporterInterface* impp = dynamic_cast<GtImporterInterface*>(plugin);

    if (impp)
    {
        gtImportHandler->registerClasses(impp->importer());
    }

    // exporter interface
    GtExporterInterface* expp = dynamic_cast<GtExporterInterface*>(plugin);

    if (expp)
    {
        gtExportHandler->registerClasses(expp->exporter());
    }

    // property interface
    GtPropertyInterface* prop = dynamic_cast<GtPropertyInterface*>(plugin);

    if (prop)
    {
        gtPropertyItemFactory->registerPropertyItems(prop->propertyItems());
    }

    // collection interface
    GtCollectionInterface* coll =
            dynamic_cast<GtCollectionInterface*>(plugin);

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
        gtError() << "ui object not invokable! (" << classId << ")";
        return;
    }

    m_uiObjs[classId].append(uio);
}

