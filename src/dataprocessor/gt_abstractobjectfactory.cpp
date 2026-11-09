/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 28.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */


#include "gt_abstractobjectfactory.h"

#include "gt_object.h"
#include "gt_logging.h"

#include <QMutex>
#include <QMutexLocker>
#include <QSet>

#include <algorithm>
#include <typeindex>
#include <vector>

namespace
{

// Helper struct to nicely log list of meta objects
struct LogMetaData
{
    const QList<QMetaObject>& list;
};

gt::log::Stream&
operator<<(gt::log::Stream& s, const LogMetaData& data)
{
    {
        gt::log::StreamStateSaver saver{s};
        s.nospace() << '\n';
        for (QMetaObject metaObj : data.list)
        {
            s << "- " << metaObj.className() << '\n';
        }
    }
    return s;
}

} // namespace

struct GtAbstractObjectFactory::Impl
{
    struct PropertyFactoryInfo
    {
        const QMetaObject* propertyType{nullptr};
        std::type_index cppType{typeid(void)};
        gt::PropertyFactoryFunction factory;
        QString moduleId;
    };

    mutable QMutex dynamicPropertyMutex;
    QHash<QString, PropertyFactoryInfo> propertyFactories;
};

GtAbstractObjectFactory::GtAbstractObjectFactory(bool silent) :
    m_silent(silent),
    m_impl(std::make_unique<Impl>())
{
}

GtAbstractObjectFactory::~GtAbstractObjectFactory() = default;

QString
GtAbstractObjectFactory::coreModuleId()
{
    return QStringLiteral("GTlab");
}

GtObject*
GtAbstractObjectFactory::newObject(const QString& className, GtObject* parent)
{
    const QMetaObject* mo = metaObject(className);
    if (!mo)
    {
        if (!isSilent())
        {
            gtError() << QObject::tr("GtAbstractObjectFactory: "
                                     "Unknown class '%1'!")
                             .arg(className);
        }
        return nullptr;
    }

    GtObject* retval = newObject(*mo, parent);
    if (!retval)
    {
        if (!isSilent())
        {
            gtError() << QObject::tr("GtAbstractObjectFactory: "
                                     "Failed to instance '%1'!")
                             .arg(className);
        }
        return nullptr;
    }

    retval->setFactory(this);
    return retval;
}

QString
GtAbstractObjectFactory::superClassName(const QString& className) const
{
    if (const QMetaObject* mo = metaObject(className))
    {
        return mo->superClass()->className();
    }

    return QString{};
}

QStringList
GtAbstractObjectFactory::classHierarchy(const QString& className)
{
    QStringList retval;

    if (const QMetaObject* mo = metaObject(className))
    {
        retval << className;
        if (const QMetaObject* smo = mo->superClass())
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
GtAbstractObjectFactory::registerClass(QMetaObject metaObj,
                                       const QString& moduleIdent)
{
    QString classname = metaObj.className();
    assert(!classname.isEmpty());

    if (moduleIdent.isEmpty())
    {
        if (!isSilent())
        {
            gtWarning() << QObject::tr("Failed to register class '%1', "
                                       "invalid module id!")
                               .arg(classname);
        }
        return false;
    }

    if (knownClass(classname))
    {
        if (!isSilent())
        {
            gtWarning() << QObject::tr("Failed to register class '%1', "
                                       "class already exists!")
                               .arg(classname);
        }
        return false;
    }

    m_knownClasses.insert(classname, metaObj);
    m_classModuleIds.insert(classname, moduleIdent);

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
GtAbstractObjectFactory::registerClasses(const QList<QMetaObject>& metaData,
                                         const QString& moduleIdent)
{
    if (moduleIdent.isEmpty())
    {
        if (!isSilent())
        {
            gtWarning() << QObject::tr("Invalid module id, failed to register "
                                       "classes:")
                        << LogMetaData{metaData};
        }
        return false;
    }

    if (containsDuplicates(metaData)) return false;

    bool success = true;
    for (const QMetaObject& metaObj : metaData)
    {
        success &= registerClass(metaObj, moduleIdent);
    }
    return success;
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
    const auto duplicate = std::find_if(
        metaData.begin(), metaData.end(), [this](const QMetaObject& mobj) {
            const QString classname = mobj.className();
            return knownClass(classname);
        });
    const bool hasDuplicate = duplicate != metaData.end();
    if (hasDuplicate && !isSilent())
    {
        gtWarning() << QObject::tr("class '%1' already exists!")
                           .arg(duplicate->className());
    }
    return hasDuplicate;
}

bool
GtAbstractObjectFactory::allInvokable(const QList<QMetaObject>& metaData)
{
    return std::all_of(
        std::begin(metaData), std::end(metaData),
        [this](const QMetaObject& mobj) { return invokable(mobj); });
}

const QMetaObject*
GtAbstractObjectFactory::metaObject(const QString& clzName) const
{
    auto found = m_knownClasses.find(clzName);

    if (found != m_knownClasses.end()) return &found.value();

    return nullptr;
}

namespace
{
/** Validate constructors outside the registry lock and resolve C++ types.
     * The resulting batch contains no duplicate IDs or concrete C++ types.
     */
bool
preparePropertyFactories(const QList<PropertyFactoryData>& factories,
                         QList<PropertyFactoryData>& prepared, QString* failure)
{
    const auto reject = [failure](const QString& typeId,
                                  const QString& reason) {
        if (failure)
        {
            *failure = QStringLiteral("'%1': %2").arg(typeId, reason);
        }
        return false;
    };

    QSet<QString> typeIds;
    std::vector<std::type_index> cppTypes;
    for (const auto& entry : factories)
    {
        if (entry.typeId.isEmpty())
        {
            return reject(entry.typeId, QStringLiteral("type ID is empty"));
        }

        if (!entry.typeId.contains(QLatin1Char('.')))
        {
            return reject(entry.typeId,
                          QStringLiteral("type ID must be namespaced"));
        }

        if (!entry.propertyType || !entry.factory)
        {
            return reject(
                entry.typeId,
                QStringLiteral("property type or factory is missing"));
        }

        if (!entry.propertyType->inherits(
                &GtAbstractProperty::staticMetaObject))
        {
            return reject(entry.typeId,
                          QStringLiteral("class is not a property type"));
        }

        if (typeIds.contains(entry.typeId))
        {
            return reject(entry.typeId,
                          QStringLiteral("type ID is duplicated in the batch"));
        }

        std::unique_ptr<GtAbstractProperty> probe(
            entry.factory(QStringLiteral("__gtlab_property_type_probe__")));
        if (!probe)
        {
            return reject(entry.typeId,
                          QStringLiteral("factory returned no property"));
        }

        if (probe->metaObject() != entry.propertyType)
        {
            return reject(
                entry.typeId,
                QStringLiteral("factory returned a different Qt type"));
        }

        const std::type_index cppType(typeid(*probe));
        if (entry.cppType != std::type_index(typeid(void)) &&
            entry.cppType != cppType)
        {
            return reject(
                entry.typeId,
                QStringLiteral("factory returned a different C++ type"));
        }

        if (std::find(cppTypes.cbegin(), cppTypes.cend(), cppType) !=
            cppTypes.cend())
        {
            return reject(
                entry.typeId,
                QStringLiteral("C++ property type is duplicated in the batch"));
        }

        typeIds.insert(entry.typeId);
        cppTypes.push_back(cppType);
        prepared.push_back(entry);
        prepared.back().cppType = cppType;
    }
    return true;
}
} // namespace

bool
GtAbstractObjectFactory::hasPropertyFactoryConflicts(
    const QList<PropertyFactoryData>& prepared) const
{
    // The caller holds the registry mutex through conflict check/insertion.
    for (const auto& entry : prepared)
    {
        if (m_impl->propertyFactories.contains(entry.typeId))
        {
            gtWarning().verbose()
                << QObject::tr("Rejecting dynamic property factory '%1': "
                               "type ID is already registered.")
                       .arg(entry.typeId);
            return true;
        }

        for (const auto& registered : m_impl->propertyFactories)
        {
            if (registered.cppType == entry.cppType)
            { // cppcheck-suppress useStlAlgorithm
                gtWarning().verbose()
                    << QObject::tr("Rejecting dynamic property factory '%1': "
                                   "C++ property type is already registered.")
                           .arg(entry.typeId);
                return true;
            }
        }
    }
    return false;
}

bool
GtAbstractObjectFactory::canRegisterPropertyFactory(
    const QString& typeId, const QMetaObject& propertyType,
    gt::PropertyFactoryFunction factory, std::type_index cppType) const
{
    return canRegisterPropertyFactories({PropertyFactoryData{
        typeId, &propertyType, std::move(factory), cppType}});
}

bool
GtAbstractObjectFactory::canRegisterPropertyFactories(
    const QList<PropertyFactoryData>& factories) const
{
    QList<PropertyFactoryData> prepared;
    QString failure;
    if (!preparePropertyFactories(factories, prepared, &failure))
    {
        gtWarning().verbose()
            << QObject::tr("Rejecting dynamic property factory %1.")
                   .arg(failure);
        return false;
    }

    QMutexLocker lock(&m_impl->dynamicPropertyMutex);
    return !hasPropertyFactoryConflicts(prepared);
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
    QList<PropertyFactoryData> prepared;
    QString failure;
    if (!preparePropertyFactories(factories, prepared, &failure))
    {
        gtWarning().verbose()
            << QObject::tr("Rejecting dynamic property factory %1.")
                   .arg(failure);
        return false;
    }

    // Check conflicts and publish the complete batch under one lock. No
    // registration can fail after validation, so the batch is all-or-nothing.
    QMutexLocker lock(&m_impl->dynamicPropertyMutex);
    if (hasPropertyFactoryConflicts(prepared)) return false;

    for (const auto& entry : prepared)
    {
        // cppcheck-suppress useStlAlgorithm
        m_impl->propertyFactories.insert(
            entry.typeId,
            Impl::PropertyFactoryInfo{entry.propertyType, entry.cppType,
                                      entry.factory, moduleIdent});
    }
    return true;
}

bool
GtAbstractObjectFactory::unregisterPropertyFactory(const QString& typeId,
                                                   const QString& moduleIdent)
{
    QMutexLocker lock(&m_impl->dynamicPropertyMutex);
    auto iter = m_impl->propertyFactories.find(typeId);
    if (iter == m_impl->propertyFactories.end() ||
        (!moduleIdent.isEmpty() && iter->moduleId != moduleIdent))
    {
        return false;
    }
    m_impl->propertyFactories.erase(iter);
    return true;
}

bool
GtAbstractObjectFactory::hasPropertyFactory(const QString& typeId) const
{
    QMutexLocker lock(&m_impl->dynamicPropertyMutex);
    return m_impl->propertyFactories.contains(typeId);
}

QString
GtAbstractObjectFactory::propertyTypeId(const QMetaObject& propertyType) const
{
    QMutexLocker lock(&m_impl->dynamicPropertyMutex);
    QString result;
    for (auto iter = m_impl->propertyFactories.cbegin();
         iter != m_impl->propertyFactories.cend(); ++iter)
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
    QMutexLocker lock(&m_impl->dynamicPropertyMutex);
    const std::type_index cppType(typeid(property));
    const auto iter = std::find_if(
        m_impl->propertyFactories.cbegin(), m_impl->propertyFactories.cend(),
        [&cppType](const auto& entry) { return entry.cppType == cppType; });
    return iter == m_impl->propertyFactories.cend() ? QString{} : iter.key();
}

bool
GtAbstractObjectFactory::propertyTypeMatches(
    const QString& typeId, const GtAbstractProperty& property) const
{
    QMutexLocker lock(&m_impl->dynamicPropertyMutex);
    const auto iter = m_impl->propertyFactories.constFind(typeId);
    return iter != m_impl->propertyFactories.cend() &&
           iter->cppType == std::type_index(typeid(property));
}

const QMetaObject*
GtAbstractObjectFactory::propertyMetaObject(const QString& typeId) const
{
    QMutexLocker lock(&m_impl->dynamicPropertyMutex);
    auto iter = m_impl->propertyFactories.constFind(typeId);
    return iter == m_impl->propertyFactories.cend() ? nullptr
                                                    : iter->propertyType;
}

std::unique_ptr<GtAbstractProperty>
GtAbstractObjectFactory::createProperty(const QString& typeId,
                                        const QString& ident) const
{
    gt::PropertyFactoryFunction factory;
    const QMetaObject* propertyType = nullptr;
    std::type_index cppType(typeid(void));
    {
        QMutexLocker lock(&m_impl->dynamicPropertyMutex);
        auto iter = m_impl->propertyFactories.constFind(typeId);
        if (iter == m_impl->propertyFactories.cend()) return {};
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
GtAbstractObjectFactory::isSilent() const noexcept
{
    return m_silent;
}

void
GtAbstractObjectFactory::setSilent(bool silent) noexcept
{
    m_silent = silent;
}

bool
GtAbstractObjectFactory::invokable(const QMetaObject& metaObj) const
{
    GtObject* obj = newObject(metaObj);
    if (!obj)
    {
        if (!isSilent())
        {
            gtWarning() << QObject::tr("class '%1' is not default invokable!")
                               .arg(metaObj.className());
        }
        return false;
    }

    delete obj;

    return true;
}
