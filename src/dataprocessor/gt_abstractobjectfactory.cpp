/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 28.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>
#include <QMutexLocker>

#include <algorithm>
#include <QSet>
#include <vector>
#include <typeindex>

#include "gt_abstractobjectfactory.h"
#include "gt_object.h"

GtAbstractObjectFactory::GtAbstractObjectFactory(bool silent) : m_silent(silent)
{
}

GtAbstractObjectFactory::~GtAbstractObjectFactory() = default;

GtObject*
GtAbstractObjectFactory::newObject(const QString& className, GtObject* parent)
{
    GtObject* retval = nullptr;
    if (m_knownClasses.contains(className))
    {
        const QMetaObject& mo = m_knownClasses[className];
        retval = newObject(mo, parent);
        if (!retval)
        {
            if (!m_silent)
            {
                qCritical() << "GtObjectFactory : Error creating " << className;
                qDebug() << knownClasses();
            }
            return nullptr;
        }
    }
    else
    {
        if (!m_silent)
        {
            // TODO: uncomment
//            qWarning() << "WARNING: classname not found! (" << className << ")";
        }
        return nullptr;
    }

    retval->setFactory(this);
    return retval;
}

QString
GtAbstractObjectFactory::superClassName(const QString& className) const
{
    if (m_knownClasses.contains(className))
    {
        const QMetaObject& mo = m_knownClasses[className];
        return mo.superClass()->className();
    }

    return QString();
}

QStringList
GtAbstractObjectFactory::classHierarchy(const QString& className)
{
    QStringList retval;

    if (m_knownClasses.contains(className))
    {
        retval << className;
        const QMetaObject& mo = m_knownClasses[className];
        if (const QMetaObject* smo = mo.superClass())
        {
            retval.append(classHierarchy(smo));
        }
    }

    return retval;
}

QStringList
GtAbstractObjectFactory::classHierarchy(const QMetaObject* metaObj)
{
    QStringList retval;

    if (!metaObj)
    {
        return retval;
    }

    retval << metaObj->className();
    retval.append(classHierarchy(metaObj->superClass()));

    return retval;
}

bool
GtAbstractObjectFactory::knownClass(const QString& className) const
{
    return m_knownClasses.contains(className);
}

QStringList
GtAbstractObjectFactory::knownClasses() const
{
    return m_knownClasses.keys();
}

bool
GtAbstractObjectFactory::registerClass(QMetaObject metaObj)
{
    return registerClass(metaObj, {});
}

bool
GtAbstractObjectFactory::registerClass(QMetaObject metaObj,
                                       const QString& moduleIdent)
{
    QString classname = metaObj.className();

    if (knownClass(classname))
    {
        if (!m_silent)
        {
            qWarning() << "WARNING" << ": " << "class already exists!" << " ("
                       << classname + ")";
        }
        return false;
    }

    m_knownClasses.insert(classname, metaObj);
    if (!moduleIdent.isEmpty())
    {
        m_classModuleIds.insert(classname, moduleIdent);
    }
    return true;
}

bool
GtAbstractObjectFactory::unregisterClass(const QMetaObject& metaObj)
{
    QString classname = metaObj.className();

    if (knownClass(classname))
    {
        m_knownClasses.remove(classname);
        m_classModuleIds.remove(classname);
        return true;
    }

    return false;
}

bool
GtAbstractObjectFactory::registerClasses(const QList<QMetaObject>& metaData)
{
    return registerClasses(metaData, {});
}

bool
GtAbstractObjectFactory::registerClasses(const QList<QMetaObject>& metaData,
                                         const QString& moduleIdent)
{
    if (containsDuplicates(metaData))
    {
        return false;
    }

    foreach (const QMetaObject& metaObj, metaData)
    {
        registerClass(metaObj, moduleIdent);
    }

    return true;
}

QString
GtAbstractObjectFactory::moduleId(const QString& className) const
{
    return m_classModuleIds.value(className);
}

GtObject*
GtAbstractObjectFactory::newObject(const QMetaObject& metaObj, GtObject* parent) const
{
    QObject* o = metaObj.newInstance();
    if (!o)
    {
        return nullptr;
    }

    GtObject* retval = qobject_cast<GtObject*>(o);
    if (!retval)
    {
        delete o;
        return nullptr;
    }

    if (parent)
    {
        parent->appendChild(retval);
    }

    return retval;
}

bool
GtAbstractObjectFactory::containsDuplicates(const QList<QMetaObject>& metaData) const
{
    foreach (const QMetaObject& mobj, metaData)
    {
        QString classname = mobj.className();
        if (m_knownClasses.contains(classname))
        {
            if (!m_silent)
            {
                qWarning() << "class name '"
                              + classname
                              + "' already exists!";

            }
            return true;
        }
    }
    return false;
}

bool
GtAbstractObjectFactory::allInvokable(const QList<QMetaObject>& metaData)
{
    return std::all_of(std::begin(metaData), std::end(metaData), [this](const QMetaObject& mobj) {
        return invokable(mobj);
    });
}

const QMetaObject*
GtAbstractObjectFactory::metaObject(const QString& clzName) const
{
    auto found = m_knownClasses.find(clzName);

    if (found != m_knownClasses.end()) return &found.value();

    return nullptr;
}

bool
GtAbstractObjectFactory::canRegisterPropertyFactory(
    const QString& typeId, const QMetaObject& propertyType,
    gt::PropertyFactoryFunction factory, std::type_index cppType) const
{
    if (!factory)
    {
        if (typeId.isEmpty() || !typeId.contains(QLatin1Char('.')) ||
            !propertyType.inherits(&GtAbstractProperty::staticMetaObject))
            return false;
        QMutexLocker lock(&m_dynamicPropertyMutex);
        return !m_propertyFactories.contains(typeId);
    }
    return canRegisterPropertyFactories({PropertyFactoryData{
        typeId, &propertyType, std::move(factory), cppType}});
}

bool
GtAbstractObjectFactory::canRegisterPropertyFactories(
    const QList<PropertyFactoryData>& factories) const
{
    QSet<QString> typeIds;
    std::vector<std::type_index> cppTypes;
    QList<std::pair<PropertyFactoryData, std::type_index>> probes;
    for (const auto& entry : factories)
    {
        if (entry.typeId.isEmpty() ||
            !entry.typeId.contains(QLatin1Char('.')) || !entry.propertyType ||
            !entry.factory ||
            !entry.propertyType->inherits(
                &GtAbstractProperty::staticMetaObject) ||
            typeIds.contains(entry.typeId))
            return false;
        auto probe = std::unique_ptr<GtAbstractProperty>(
            entry.factory(QStringLiteral("__gtlab_property_type_probe__")));
        if (!probe || probe->metaObject() != entry.propertyType) return false;
        const auto cppType = std::type_index(typeid(*probe));
        if ((entry.cppType != std::type_index(typeid(void)) &&
             entry.cppType != cppType) ||
            std::find(cppTypes.cbegin(), cppTypes.cend(), cppType) !=
                cppTypes.cend())
            return false;
        typeIds.insert(entry.typeId);
        cppTypes.push_back(cppType);
        probes.push_back({entry, cppType});
    }
    QMutexLocker lock(&m_dynamicPropertyMutex);
    return std::none_of(
        probes.cbegin(), probes.cend(), [this](const auto& probe) {
            return m_propertyFactories.contains(probe.first.typeId) ||
                   std::any_of(m_propertyFactories.cbegin(),
                               m_propertyFactories.cend(),
                               [&probe](const auto& registered) {
                                   return registered.cppType == probe.second;
                               });
        });
}

bool
GtAbstractObjectFactory::registerPropertyFactory(
    const QString& typeId, const QMetaObject& propertyType,
    gt::PropertyFactoryFunction factory, const QString& moduleIdent,
    std::type_index cppType)
{
    return registerPropertyFactories(
        {PropertyFactoryData{typeId, &propertyType, std::move(factory),
                             cppType}},
        moduleIdent);
}

bool
GtAbstractObjectFactory::registerPropertyFactories(
    const QList<PropertyFactoryData>& factories, const QString& moduleIdent)
{
    // Validate each factory and collect its concrete C++ type before locking.
    QSet<QString> typeIds;
    std::vector<std::type_index> cppTypes;
    QList<std::pair<PropertyFactoryData, std::type_index>> registrations;

    for (const auto& entry : factories)
    {
        if (entry.typeId.isEmpty() ||
            !entry.typeId.contains(QLatin1Char('.')) || !entry.propertyType ||
            !entry.factory ||
            !entry.propertyType->inherits(
                &GtAbstractProperty::staticMetaObject) ||
            typeIds.contains(entry.typeId))
            return false;
        auto probe = std::unique_ptr<GtAbstractProperty>(
            entry.factory(QStringLiteral("__gtlab_property_type_probe__")));
        if (!probe || probe->metaObject() != entry.propertyType) return false;
        const auto cppType = std::type_index(typeid(*probe));
        if ((entry.cppType != std::type_index(typeid(void)) &&
             entry.cppType != cppType) ||
            std::find(cppTypes.cbegin(), cppTypes.cend(), cppType) !=
                cppTypes.cend())
            return false;
        typeIds.insert(entry.typeId);
        cppTypes.push_back(cppType);
        registrations.push_back({entry, cppType});
    }

    // Check for conflicts and publish the complete batch under one lock.
    QMutexLocker lock(&m_dynamicPropertyMutex);
    for (const auto& registration : registrations)
    {
        if (m_propertyFactories.contains(registration.first.typeId))
            return false;

        for (const auto& registered : m_propertyFactories)
        {
            if (registered.cppType == registration.second) return false;
        }
    }

    // No registration can fail after validation, so the batch is all-or-nothing.
    for (const auto& registration : registrations)
    {
        const auto& entry = registration.first;
        m_propertyFactories.insert(
            entry.typeId,
            PropertyFactoryInfo{entry.propertyType, registration.second,
                                entry.factory, moduleIdent});
    }
    return true;
}

bool
GtAbstractObjectFactory::unregisterPropertyFactory(const QString& typeId,
                                                   const QString& moduleIdent)
{
    QMutexLocker lock(&m_dynamicPropertyMutex);
    auto iter = m_propertyFactories.find(typeId);
    if (iter == m_propertyFactories.end() ||
        (!moduleIdent.isEmpty() && iter->moduleId != moduleIdent))
    {
        return false;
    }
    m_propertyFactories.erase(iter);
    return true;
}

bool
GtAbstractObjectFactory::hasPropertyFactory(const QString& typeId) const
{
    QMutexLocker lock(&m_dynamicPropertyMutex);
    return m_propertyFactories.contains(typeId);
}

QString
GtAbstractObjectFactory::propertyTypeId(const QMetaObject& propertyType) const
{
    QMutexLocker lock(&m_dynamicPropertyMutex);
    QString result;
    for (auto iter = m_propertyFactories.cbegin();
         iter != m_propertyFactories.cend(); ++iter)
    {
        if (iter->propertyType != &propertyType) continue;
        if (!result.isEmpty()) return {};
        result = iter.key();
    }
    return result;
}

QString
GtAbstractObjectFactory::propertyTypeId(
    const GtAbstractProperty& property) const
{
    QMutexLocker lock(&m_dynamicPropertyMutex);
    const std::type_index cppType(typeid(property));
    const auto iter = std::find_if(
        m_propertyFactories.cbegin(), m_propertyFactories.cend(),
        [&cppType](const auto& entry) { return entry.cppType == cppType; });
    return iter == m_propertyFactories.cend() ? QString{} : iter.key();
}

bool
GtAbstractObjectFactory::propertyTypeMatches(
    const QString& typeId, const GtAbstractProperty& property) const
{
    QMutexLocker lock(&m_dynamicPropertyMutex);
    const auto iter = m_propertyFactories.constFind(typeId);
    return iter != m_propertyFactories.cend() &&
           iter->cppType == std::type_index(typeid(property));
}

const QMetaObject*
GtAbstractObjectFactory::propertyMetaObject(const QString& typeId) const
{
    QMutexLocker lock(&m_dynamicPropertyMutex);
    auto iter = m_propertyFactories.constFind(typeId);
    return iter == m_propertyFactories.cend() ? nullptr : iter->propertyType;
}

std::unique_ptr<GtAbstractProperty>
GtAbstractObjectFactory::createProperty(const QString& typeId,
                                        const QString& ident) const
{
    gt::PropertyFactoryFunction factory;
    const QMetaObject* propertyType = nullptr;
    std::type_index cppType(typeid(void));
    {
        QMutexLocker lock(&m_dynamicPropertyMutex);
        auto iter = m_propertyFactories.constFind(typeId);
        if (iter == m_propertyFactories.cend()) return {};
        factory = iter->factory;
        propertyType = iter->propertyType;
        cppType = iter->cppType;
    }

    std::unique_ptr<GtAbstractProperty> property(factory(ident));
    if (!property || property->ident() != ident || !propertyType ||
        property->metaObject() != propertyType ||
        std::type_index(typeid(*property)) != cppType)
    {
        return {};
    }
    return property;
}

bool
GtAbstractObjectFactory::invokable(const QMetaObject& metaObj) const
{
    GtObject* obj = newObject(metaObj);
    if (!obj)
    {
        if (!m_silent)
        {
            qWarning() << "class " << metaObj.className() << " not invokable!";
        }
        return false;
    }

    delete obj;

    return true;
}
