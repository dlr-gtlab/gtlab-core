/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTFACTORYGROUP_H
#define GTFACTORYGROUP_H

#include <gt_version.h>
#include <gt_datamodel_exports.h>

#include <gt_abstractobjectfactory.h>

/**
 * @brief The GtFactoryGroup class
 */
class GT_DATAMODEL_EXPORT GtFactoryGroup : public GtAbstractObjectFactory
{
public:
    /**
     * @brief GtFactoryGroup
     */
    GtFactoryGroup() = default;

    /**
     * @brief newObject - generates gtlab objects based on given classname
     * @param className
     * @param parent object for the new object
     * @return new object
     */
    GT_NO_DISCARD
    GtObject* newObject(const QString& className,
                        GtObject* parent = nullptr) override;

    /**
     * @brief knownClass - returns true if classname is registered in the
     * factory
     * @param className
     * @return whether classname is known or not
     */
    GT_NO_DISCARD
    bool knownClass(const QString& className) const override;

    /**
     * @brief superClassName - returns superclass name of given class
     * @param className
     * @return name of superclass
     */
    GT_NO_DISCARD
    QString superClassName(const QString& className) const override;

    /**
     * @brief knownClasses - returns a list of all stored meta class names
     * @return class name list
     */
    GT_NO_DISCARD
    QStringList knownClasses() const override;

    /**
     * @brief Returns the id of the module that registers this class
     * @param className
     * @return Module id (may be empty)
     */
    GT_NO_DISCARD
    QString moduleId(const QString& className) const override;

    /**
     * @brief containsDuplicates
     * @param plugin
     * @return
     */
    GT_NO_DISCARD
    bool containsDuplicates(const QList<QMetaObject>& metaData) const override;

    /**
     * @brief invokable
     * @param metaObj
     * @return
     */
    GT_NO_DISCARD
    bool invokable(const QMetaObject& metaObj) const override;

    /**
     * @brief classHierarchy
     * @param className
     * @return
     */
    GT_NO_DISCARD
    QStringList classHierarchy(const QString& className) override;

    /**
     * @brief metaObject
     * @param clzName
     * @return
     */
    GT_NO_DISCARD
    const QMetaObject* metaObject(const QString& className) const override;

    /**
     * @brief addFactory
     * @param factory
     * @return
     */
    bool addFactory(GtAbstractObjectFactory* factory);

protected:
    /**
     * @brief registerClass
     * @param metaObject
     * @return
     */
    bool registerClass(QMetaObject metaObj) override;

    /**
     * @brief registerClass
     * @param metaObj
     * @param moduleId
     * @return
     */
    bool registerClass(QMetaObject metaObj, const QString& moduleId) override;

    /**
     * @brief registerClasses
     * @param metaObject
     * @return
     */
    bool registerClasses(const QList<QMetaObject>& metaData) override;

    /**
     * @brief registerClasses
     * @param metaData
     * @param moduleId
     * @return
     */
    bool registerClasses(const QList<QMetaObject>& metaData,
                         const QString& moduleId) override;

    /**
     * @brief unregisterClass
     * @param metaObj
     * @return
     */
    bool unregisterClass(const QMetaObject& metaObj) override;

private:
    QList<GtAbstractObjectFactory*> m_factories;
};

#endif // GTFACTORYGROUP_H
