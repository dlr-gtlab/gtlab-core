/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_propertyitemfactory.cpp
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

#include "gt_accessselectionproperty.h"
#include "gt_propertyaccessselectionitem.h"

#include "gt_doublelistproperty.h"
#include "gt_propertydoublelistitem.h"

#include "gt_propertyitemfactory.h"

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

    m_knownClasses.insert(GT_CLASSNAME(GtAccessSelectionProperty),
                          GT_METADATA(GtPropertyAccessSelectionItem));

    m_knownClasses.insert(GT_CLASSNAME(GtDoubleListProperty),
                          GT_METADATA(GtPropertyDoubleListItem));
}

GtPropertyItemFactory*
GtPropertyItemFactory::instance()
{
    static GtPropertyItemFactory* retval = nullptr;
    if (!retval)
    {
        retval = new GtPropertyItemFactory(qApp);
    }
    return retval;
}

GtAbstractPropertyItem*
GtPropertyItemFactory::newItem(GtAbstractProperty* property, GtObject* scope,
                               GtPropertyModel* model, GtObject* parent)
{
    GtAbstractPropertyItem* retval = nullptr;

    if (!knownClass(property->metaObject()->className()))
    {
//        gtDebug() << tr("property class not found!") <<
//                     QStringLiteral(" - ") <<
//                     property->metaObject()->className();
    }
    else
    {
        GtObject* obj = newObject(property->metaObject()->className(), parent);

        if (!obj)
        {
            gtDebug() << tr("could not recreate property item!") <<
                         QStringLiteral(" - ") <<
                         property->metaObject()->className();
        }
        else
        {
            retval = qobject_cast<GtAbstractPropertyItem*>(obj);

            if (!retval)
            {
                gtDebug() << tr("object not a property item!") <<
                             QStringLiteral(" - ") <<
                             property->metaObject()->className();
                delete obj;
            }
        }
    }


    if (!retval)
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

    for (auto iter = std::begin(map); iter != std::end(map); ++iter)
    {
        registerPropertyItem(iter.key(), map.value(iter.key()));
    }

    return true;
}

bool
GtPropertyItemFactory::propertyItemsExists(
        const QMap<const char*, QMetaObject>& map)
{
    for (auto iter = std::begin(map); iter != std::end(map); ++iter)
    {
        if (knownClass(iter.key()))
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
