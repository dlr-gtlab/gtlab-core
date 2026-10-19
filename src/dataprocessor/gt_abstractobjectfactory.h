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
#include <QMutex>

#include <memory>
#include <typeindex>

#include "gt_datamodel_exports.h"
#include "property/gt_abstractproperty.h"
#include "property/gt_propertyfactory.h"

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
     * @brief Registers a class and its providing module.
     * @param metaObj Meta object to register.
     * @param moduleId Identification of the providing module.
     * @return Whether the class was registered successfully.
     */
    virtual bool registerClass(QMetaObject metaObj, const QString& moduleIdent);

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
     * @brief Registers classes and their providing module.
     * @param metaData Meta objects to register.
     * @param moduleId Identification of the providing module.
     * @return Whether all classes were registered successfully.
     */
    virtual bool registerClasses(const QList<QMetaObject>& metaData,
                                 const QString& moduleIdent);

    /**
     * @brief Returns the module providing a registered class.
     * @param className Registered class name.
     * @return Module identification or an empty string if none is known.
     */
    virtual QString moduleId(const QString& className) const;

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

    /**
     * @brief Registers one persistent dynamic-property type.
     * @param typeId Stable identifier used to persist this property type.
     * @param propertyType Qt meta object of the concrete property class.
     * @param factory Creates properties of the registered type.
     * @param moduleIdent Identifier of the registering module, if any.
     * @param cppType Concrete C++ type used to validate factory instances.
     * @return Whether the registration succeeded.
     */
    bool registerPropertyFactory(const QString& typeId,
                                 const QMetaObject& propertyType,
                                 gt::PropertyFactoryFunction factory,
                                 const QString& moduleIdent = {},
                                 std::type_index cppType = typeid(void));

    /** @brief Checks whether one property type can be registered. */
    bool canRegisterPropertyFactory(
        const QString& typeId, const QMetaObject& propertyType,
        gt::PropertyFactoryFunction factory = {},
        std::type_index cppType = typeid(void)) const;

    /** @brief Checks whether all property types in a batch can be registered. */
    bool canRegisterPropertyFactories(
        const QList<PropertyFactoryData>& factories) const;

    /**
     * @brief Registers a batch of persistent dynamic-property types.
     * @return Whether the complete batch was registered.
     */
    bool registerPropertyFactories(const QList<PropertyFactoryData>& factories,
                                   const QString& moduleIdent = {});

    /** @brief Removes a registered type, optionally restricted to its module. */
    bool unregisterPropertyFactory(const QString& typeId,
                                   const QString& moduleIdent = {});

    /** @brief Checks whether a persistent property type is registered. */
    bool hasPropertyFactory(const QString& typeId) const;

    /** @brief Checks whether a property instance matches its registered type. */
    bool propertyTypeMatches(const QString& typeId,
                             const GtAbstractProperty& property) const;

    /** @brief Returns the registered type identifier for a Qt meta object. */
    QString propertyTypeId(const QMetaObject& propertyType) const;

    /** @brief Returns the registered type identifier for a property instance. */
    QString propertyTypeId(const GtAbstractProperty& property) const;

    /** @brief Returns the registered Qt meta object for a type identifier. */
    const QMetaObject* propertyMetaObject(const QString& typeId) const;

    /** @brief Creates a property instance from its registered type identifier. */
    std::unique_ptr<GtAbstractProperty> createProperty(
        const QString& typeId, const QString& ident) const;

protected:

    /// registered meta objects
    QHash<QString, QMetaObject> m_knownClasses;

    /// module ids associated with registered classes
    QHash<QString, QString> m_classModuleIds;

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
    struct PropertyFactoryInfo
    {
        const QMetaObject* propertyType{nullptr};
        std::type_index cppType{typeid(void)};
        gt::PropertyFactoryFunction factory;
        QString moduleId;
    };

    mutable QMutex m_dynamicPropertyMutex;
    QHash<QString, PropertyFactoryInfo> m_propertyFactories;

    QStringList classHierarchy(const QMetaObject* metaObj);

};

#endif // GTABSTRACTOBJECTFACTORY_H
