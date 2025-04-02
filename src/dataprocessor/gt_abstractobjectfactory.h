/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 28.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTOBJECTFACTORY_H
#define GTABSTRACTOBJECTFACTORY_H

#include <QHash>
#include <QStringList>
#include <QMetaObject>

#include "gt_datamodel_exports.h"

class GtObject;

/**
 * @brief The GtAbstractObjectFactory class
 */
class GT_DATAMODEL_EXPORT GtAbstractObjectFactory
{
public:

    /// destructor
    virtual ~GtAbstractObjectFactory();

    /**
     * @brief newObject - generates gtlab objects based on given classname
     * @param className
     * @param parent object for the new object
     * @return new object
     */
    virtual GtObject* newObject(const QString& className,
                                GtObject* parent = nullptr);

    /**
     * @brief superClassName - returns superclass name of given class
     * @param className
     * @return name of superclass
     */
    virtual QString superClassName(const QString& className) const;

    /**
     * @brief classHierarchy
     * @param className
     * @return
     */
    virtual QStringList classHierarchy(const QString& className);

    /**
     * @brief knownClass - returns true if classname is registered in the
     * factory
     * @param className
     * @return whether classname is known or not
     */
    virtual bool knownClass(const QString& className) const;

    /**
     * @brief knownClasses - returns a list of all stored meta class names
     * @return class name list
     */
    virtual QStringList knownClasses() const;

    /**
     * @brief registerClass
     * @param metaObject
     * @return
     */
    virtual bool registerClass(QMetaObject metaObj);

    /**
     * @brief Removes a class from the factory
     * @param metaObj The meta object of the class to remove
     * @return
     */
    bool unregisterClass(const QMetaObject& metaObj);

    /**
     * @brief registerClasses
     * @param metaObject
     * @return
     */
    virtual bool registerClasses(const QList<QMetaObject>& metaData);

    /**
     * @brief containsDuplicates
     * @param plugin
     * @return
     */
    virtual bool containsDuplicates(const QList<QMetaObject>& metaData) const;

    /**
     * @brief invokable
     * @param metaObj
     * @return
     */
    virtual bool invokable(const QMetaObject& metaObj) const;

    /**
     * @brief allInvokable
     * @param plugin
     * @return
     */
    bool allInvokable(const QList<QMetaObject>& metaData);

    /**
     * @brief Returns the meta object for a given type
     *
     * Returns nullptr, if the type is unknown
     *
     * @param clzName Class / type name
     * @return pointer to meta object
     */
    const QMetaObject* metaObject(const QString& clzName) const;

protected:

    /// registered meta objects
    QHash<QString, QMetaObject> m_knownClasses;

    /// debug output indicator
    bool m_silent;

    /**
     * @brief GtAbstractObjectFactory
     */
    explicit GtAbstractObjectFactory(bool silent = false);

    /**
     * @brief newObject
     * @param metaObj
     * @param parent
     * @return
     */
    GtObject* newObject(const QMetaObject& metaObj,
                        GtObject* parent = nullptr) const;

private:
    QStringList classHierarchy(const QMetaObject* metaObj);

};

#endif // GTABSTRACTOBJECTFACTORY_H
