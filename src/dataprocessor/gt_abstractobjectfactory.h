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

#include <gt_qtutilities.h>
#include <gt_platform.h>
#include <gt_datamodel_exports.h>

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
    GT_NO_DISCARD
    virtual GtObject* newObject(const QString& className,
                                GtObject* parent = nullptr);

    /**
     * @brief Creates a new object of the given class name.Lifetime is tied to
     * the smart pointer
     * @param className Name of the class to instance
     * @return Unique pointer to new instance, managed through the smart
     * pointer. May be null if the class was not found or the object could not
     * be instantiated.
     */
    GT_NO_DISCARD
    std::unique_ptr<GtObject> makeObject(const QString& className);

    /**
     * @brief Overload, casting object to `T`.
     * @param className Class name to instantiate (must be derived of T)
     * @tparam T Type to cast to
     * @return
     */
    template <typename T,
              typename = std::enable_if_t<std::is_base_of<GtObject, T>::value>>
    GT_NO_DISCARD std::unique_ptr<T> makeObject(const QString& className)
    {
        return gt::unique_qobject_cast<T>(makeObject(className));
    }

    /**
     * @brief Creates a new object of the given class name. Sets `parent` to own
     * the new object. Lifetime is managed by the parent child relation
     * @param className Name of the class to instance
     * @param parent Parent object, owning the new instance
     * @return Raw pointer to new instance, managed through the parent child
     * relation. May be null if the class was not found or the object could not
     * be instantiated.
     */
    GT_NO_DISCARD
    GtObject* makeObject(const QString& className, GtObject& parent);

    /**
     * @brief superClassName - returns superclass name of given class
     * @param className
     * @return name of superclass
     */
    GT_NO_DISCARD
    virtual QString superClassName(const QString& className) const;

    /**
     * @brief classHierarchy
     * @param className
     * @return
     */
    GT_NO_DISCARD
    virtual QStringList classHierarchy(const QString& className);

    /**
     * @brief knownClass - returns true if classname is registered in the
     * factory
     * @param className
     * @return whether classname is known or not
     */
    GT_NO_DISCARD
    virtual bool knownClass(const QString& className) const;

    /**
     * @brief knownClasses - returns a list of all stored meta class names
     * @return class name list
     */
    GT_NO_DISCARD
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
    virtual bool registerClass(QMetaObject metaObj, const QString& moduleId);

    /**
     * @brief Removes a class from the factory
     * @param metaObj The meta object of the class to remove
     * @return
     */
    virtual bool unregisterClass(const QMetaObject& metaObj);

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
                                 const QString& moduleId);

    /**
     * @brief Returns the module providing a registered class.
     * @param className Registered class name.
     * @return Module identification or an empty string if none is known.
     */
    GT_NO_DISCARD
    virtual QString moduleId(const QString& className) const;

    /**
     * @brief containsDuplicates
     * @param plugin
     * @return
     */
    GT_NO_DISCARD
    virtual bool containsDuplicates(const QList<QMetaObject>& metaData) const;

    /**
     * @brief invokable
     * @param metaObj
     * @return
     */
    GT_NO_DISCARD
    virtual bool invokable(const QMetaObject& metaObj) const;

    /**
     * @brief allInvokable
     * @param plugin
     * @return
     */
    GT_NO_DISCARD
    bool allInvokable(const QList<QMetaObject>& metaData) const;

    /**
     * @brief Returns the meta object for a given type
     *
     * Returns nullptr, if the type is unknown
     *
     * @param clzName Class / type name
     * @return pointer to meta object
     */
    GT_NO_DISCARD
    virtual const QMetaObject* metaObject(const QString& clzName) const;

protected:

    /// registered meta objects
    QHash<QString, QMetaObject> m_knownClasses;

    /// module ids associated with registered classes
    QHash<QString, QString> m_classModuleIds;

    /// debug output indicator
    bool m_silent;

    bool isSilent() const noexcept;

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
