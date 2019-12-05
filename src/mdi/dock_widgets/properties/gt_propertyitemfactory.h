/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyitemfactory.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.06.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYITEMFACTORY_H
#define GTPROPERTYITEMFACTORY_H

#include <QObject>

#include "gt_abstractobjectfactory.h"

#define gtPropertyItemFactory (GtPropertyItemFactory::instance())

class GtAbstractProperty;
class GtAbstractPropertyItem;
class GtPropertyModel;

/**
 * @brief The GtPropertyItemFactory class
 */
class GtPropertyItemFactory : public QObject,
        public GtAbstractObjectFactory
{

public:
    /**
     * @brief Returns instance of property item factory.
     * @return Instance of property item factory.
     */
    static GtPropertyItemFactory* instance();

    /**
     * @brief Creates new property item based on given property.
     * @param Property.
     * @param Property scope.
     * @param Property model.
     * @param Parent object.
     * @return New created property item.
     */
    GtAbstractPropertyItem* newItem(GtAbstractProperty* property,
                                    GtObject* scope,
                                    GtPropertyModel* model,
                                    GtObject* parent);

    /**
     * @brief Register property item meta data
     * @return
     */
    /**
     * @brief Registers property item meta data.
     * @param Property specifc class name.
     * @param Property item.
     * @return Whether property item was successfully registered or not.
     */
    bool registerPropertyItem(const char* propertyClass,
                              QMetaObject propertyItem);

    /**
     * @brief Registers list of property items meta data.
     * @param Map filled with property items and corresponding property class
     * names.
     * @return Whether full list could be registered or not.
     */
    bool registerPropertyItems(const QMap<const char*, QMetaObject>& map);

    /**
     * @brief Returns true if one item of given map already exists in factory.
     * @param Map filled with property items and corresponding property class
     * names.
     * @return Whether one item of given map already exists in factory.
     */
    bool propertyItemsExists(const QMap<const char*, QMetaObject>& map);

    /**
     * @brief Returns true if all items in given map are invokable.
     * @param Map filled with property items and corresponding property class
     * names.
     * @return Whether all items are invokable or not.
     */
    bool propertyItemsInvokable(const QMap<const char*, QMetaObject>& map);

private:
    /**
     * @brief Constructor.
     * @param Parent object.
     */
    GtPropertyItemFactory(QObject* parent = NULL);

};

#endif // GTPROPERTYITEMFACTORY_H
