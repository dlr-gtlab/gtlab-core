/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_propertyfactory.h
 *
 *  Created on: 08.08.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYFACTORY_H
#define GTPROPERTYFACTORY_H

#include "gt_datamodel_exports.h"

#include <QObject>

class GtAbstractProperty;

/**
 * @brief The GtPropertyFactory class
 */
class GtPropertyFactory : public QObject
{
public:

    using FactoryFunction
        = std::function<GtAbstractProperty*(const QString& id,
                                            const QString& name)>;

    ~GtPropertyFactory();


    /**
     * @brief instance
     * @return
     */
    GT_DATAMODEL_EXPORT
    static GtPropertyFactory* instance();

    GT_DATAMODEL_EXPORT
    QStringList knownClasses() const;

    /**
     * @brief Creates property object based on given classname. Returns
     * null if classname not found.
     * @param className
     * @return new property
     */
    GT_DATAMODEL_EXPORT
    GtAbstractProperty* newProperty(const QString& className,
                                    const QString& id,
                                    const QString& name) const;

    /**
     * @brief registers a property type at the factory via its
     *        factory function
     * @param className Type/Class name of the property
     * @param f         The function to create the property
     * @return
     */
    GT_DATAMODEL_EXPORT
    GtPropertyFactory& registerProperty(const QString& className,
                                        FactoryFunction f);
    /**
     * @brief unregisters a property type at the factory
     */
    GT_DATAMODEL_EXPORT
    GtPropertyFactory& unregisterProperty(const QString& className);

private:
    template <class T>
    void registerQObjectClass();

    /// registered factory functions
    QHash<QString, FactoryFunction> m_knownFactories;
    bool m_silent {true};

    /**
     * @brief GtPropertyFactory
     * @param parent
     */
    explicit GtPropertyFactory(QObject* parent = nullptr);

};

#endif // GTPROPERTYFACTORY_H
