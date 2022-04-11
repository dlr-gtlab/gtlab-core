/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyfactory.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 08.08.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYFACTORY_H
#define GTPROPERTYFACTORY_H

#include <QObject>

#include "gt_abstractobjectfactory.h"

class GtAbstractProperty;

/**
 * @brief The GtPropertyFactory class
 */
class GtPropertyFactory : public QObject,
        public GtAbstractObjectFactory
{
public:
    /**
     * @brief instance
     * @return
     */
    static GtPropertyFactory* instance();

    /**
     * @brief Function disabled. Returns null.
     * @param className
     * @param parent object for the new object
     * @return new object
     */
    GtObject* newObject(const QString& className,
                        GtObject* parent = nullptr) override;

    /**
     * @brief Creates property object based on given classname. Returns
     * null if classname not found.
     * @param className
     * @return new property
     */
    GtAbstractProperty* newProperty(const QString& className,
                                    const QString& id,
                                    const QString& name);

private:
    /**
     * @brief GtPropertyFactory
     * @param parent
     */
    explicit GtPropertyFactory(QObject* parent = nullptr);

};

#endif // GTPROPERTYFACTORY_H
