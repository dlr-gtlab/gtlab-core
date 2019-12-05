/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyitemfactory.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.06.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>

#include "gt_abstractproperty.h"
#include "gt_abstractpropertyitem.h"
#include "gt_propertyitem.h"
#include "gt_logging.h"

#include "gt_labelproperty.h"
#include "gt_propertylabelitem.h"

#include "gt_modeproperty.h"
#include "gt_propertymodeitem.h"

#include "gt_objectlinkproperty.h"
#include "gt_propertyobjectlinkitem.h"

#include "gt_openfilenameproperty.h"
#include "gt_savefilenameproperty.h"
#include "gt_existingdirectoryproperty.h"
#include "gt_propertyfilechooseritem.h"

#include "gt_controlscheduleproperty.h"
#include "gt_propertycontrolscheduleitem.h"

#include "gt_nozzlemapproperty.h"
#include "gt_propertynozzlemapitem.h"

#include "gt_accessselectionproperty.h"
#include "gt_propertyaccessselectionitem.h"

#include "gt_doublelistproperty.h"
#include "gt_propertydoublelistitem.h"

#include "gt_propertyitemfactory.h"

#include "gt_freestringproperty.h"
#include "gt_propertyfreestringitem.h"

GtPropertyItemFactory::GtPropertyItemFactory(QObject* parent) : QObject(parent)
{
    m_knownClasses.insert(GT_CLASSNAME(GtLabelProperty),
                          GT_METADATA(GtPropertyLabelItem));

    m_knownClasses.insert(GT_CLASSNAME(GtModeProperty),
                          GT_METADATA(GtPropertyModeItem));

    m_knownClasses.insert(GT_CLASSNAME(GtObjectLinkProperty),
                          GT_METADATA(GtPropertyObjectLinkItem));

    m_knownClasses.insert(GT_CLASSNAME(GtOpenFileNameProperty),
                          GT_METADATA(GtPropertyFileChooserItem));

    m_knownClasses.insert(GT_CLASSNAME(GtSaveFileNameProperty),
                          GT_METADATA(GtPropertyFileChooserItem));

    m_knownClasses.insert(GT_CLASSNAME(GtExistingDirectoryProperty),
                          GT_METADATA(GtPropertyFileChooserItem));

    m_knownClasses.insert(GT_CLASSNAME(GtControlScheduleProperty),
                          GT_METADATA(GtPropertyControlScheduleItem));

    m_knownClasses.insert(GT_CLASSNAME(GtNozzleMapProperty),
                          GT_METADATA(GtPropertyNozzleMapItem));

    m_knownClasses.insert(GT_CLASSNAME(GtAccessSelectionProperty),
                          GT_METADATA(GtPropertyAccessSelectionItem));

    m_knownClasses.insert(GT_CLASSNAME(GtDoubleListProperty),
                          GT_METADATA(GtPropertyDoubleListItem));

    m_knownClasses.insert(GT_CLASSNAME(GtFreeStringProperty),
                          GT_METADATA(GtPropertyFreeStringItem));
}

GtPropertyItemFactory*
GtPropertyItemFactory::instance()
{
    static GtPropertyItemFactory* retval = 0;
    if (retval == 0)
    {
        retval = new GtPropertyItemFactory(qApp);
    }
    return retval;
}

GtAbstractPropertyItem*
GtPropertyItemFactory::newItem(GtAbstractProperty* property, GtObject* scope,
                               GtPropertyModel* model, GtObject* parent)
{
    GtAbstractPropertyItem* retval = Q_NULLPTR;

    if (!knownClass(property->metaObject()->className()))
    {
//        gtDebug() << tr("property class not found!") <<
//                     QStringLiteral(" - ") <<
//                     property->metaObject()->className();
    }
    else
    {
        GtObject* obj = newObject(property->metaObject()->className(), parent);

        if (obj == Q_NULLPTR)
        {
            gtDebug() << tr("could not recreate property item!") <<
                         QStringLiteral(" - ") <<
                         property->metaObject()->className();
        }
        else
        {
            retval = qobject_cast<GtAbstractPropertyItem*>(obj);

            if (retval == Q_NULLPTR)
            {
                gtDebug() << tr("object not a property item!") <<
                             QStringLiteral(" - ") <<
                             property->metaObject()->className();
                delete obj;
            }
        }
    }


    if (retval == Q_NULLPTR)
    {
        // create default property item
//        gtDebug() << tr("creating default property item...");
        retval = new GtPropertyItem;
        retval->setParent(parent);
    }

    // set property
    retval->setPropertyData(property);

    // set scope
    retval->setScope(scope);

    // set model
    retval->setModel(model);

    // sub properties
    retval->updateSubProperties();

    return retval;
}

bool
GtPropertyItemFactory::registerPropertyItem(const char* propertyClass,
                                            QMetaObject propertyItem)
{
    if (knownClass(propertyClass))
    {
        return false;
    }

    m_knownClasses.insert(propertyClass, propertyItem);

    return true;
}

bool
GtPropertyItemFactory::registerPropertyItems(
        const QMap<const char*, QMetaObject>& map)
{
    if (propertyItemsExists(map))
    {
        return false;
    }

    for (auto key : map.keys())
    {
        registerPropertyItem(key, map.value(key));
    }

    return true;
}

bool
GtPropertyItemFactory::propertyItemsExists(
        const QMap<const char*, QMetaObject>& map)
{
    for (auto key : map.keys())
    {
        if (knownClass(key))
        {
            return true;
        }
    }

    return false;
}

bool
GtPropertyItemFactory::propertyItemsInvokable(
        const QMap<const char*, QMetaObject>& map)
{
    return allInvokable(map.values());
}
