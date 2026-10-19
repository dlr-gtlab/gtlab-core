/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 29.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <algorithm>

#include <QCryptographicHash>
#include <QBuffer>
#include <QIODevice>
#include <QDataStream>
#include <QMetaProperty>

#include "gt_externalizedobject.h"
#include "gt_objectmemento.h"
#include "gt_objectio.h"
#include "gt_abstractobjectfactory.h"
#include "gt_objectfactory.h"
#include "gt_abstractproperty.h"
#include "property/gt_dummyproperty.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"
#include "gt_exceptions.h"
#include "internal/varianthasher.h"
#include "internal/gt_externalizedobjectprivate.h"

using PD = GtObjectMemento::PropertyData;

namespace
{
} // namespace

GtObjectMemento::GtObjectMemento(const GtObject* obj, bool clone)
{
    if (obj)
    {
        GtObjectIO oio;
        *this = oio.toMemento(obj, clone);
    }
}

GtObjectMemento::GtObjectMemento(const QDomElement& element)
{
    if (!element.isNull())
    {
        GtObjectIO oio;
        *this = oio.toMemento(element);
    }
}

GtObjectMemento::GtObjectMemento(const QByteArray& byteArray)
{

    QDomDocument doc;
    if (!doc.setContent(byteArray))
    {
        return;
    }

    *this = GtObjectIO().toMemento(doc.documentElement());

}

bool
GtObjectMemento::isNull() const
{
    return uuid().isNull();
}

QDomElement
GtObjectMemento::documentElement() const
{
    QDomDocument doc;

    // create XML representation if necessary
    if (!isNull())
    {
        GtObjectIO oio;
        QDomElement e = oio.toDomElement(*this, doc);
        doc.appendChild(e);
    }

    return doc.documentElement();
}

QByteArray
GtObjectMemento::toByteArray() const
{
    QDomDocument doc;
    // create XML representation if necessary
    if (!isNull())
    {
        GtObjectIO oio;
        QDomElement e = oio.toDomElement(*this, doc);
        doc.appendChild(e);
    }

    return doc.toByteArray();
}

const QString&
GtObjectMemento::className() const
{
    return m_className;
}

GtObjectMemento&
GtObjectMemento::setClassName(const QString& classNameValue)
{
    m_className = classNameValue;
    return *this;
}

const QString&
GtObjectMemento::uuid() const
{
    return m_uuid;
}

GtObjectMemento&
GtObjectMemento::setUuid(const QString &uuid)
{
    m_uuid = uuid;
    return *this;
}

const QString&
GtObjectMemento::ident() const
{
    return m_ident;
}

GtObjectMemento&
GtObjectMemento::setIdent(const QString &ident)
{
    m_ident = ident;
    return *this;
}

bool
GtObjectMemento::canCastTo(const QString& classname,
                           GtAbstractObjectFactory* factory)
{
    QStringList classHierarchy = factory->classHierarchy(className());

    return classHierarchy.contains(classname);
}

void
propertyHashHelper(const PD& property, QCryptographicHash& hash,
                                    gt::detail::VariantHasher& variantHasher)
{
    QCryptographicHash propHash(QCryptographicHash::Sha256);

    // hash property
    propHash.addData(property.name.toUtf8());
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    propHash.addData((const char*)&property.isActive, sizeof(property.isActive));
#else
    propHash.addData(QByteArrayView((const char*)&property.isActive,
                                    sizeof(property.isActive)));
#endif
    propHash.addData(property.dataType().toUtf8());

    auto propertyType = property.type();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    propHash.addData((const char*)&propertyType, sizeof(propertyType));
#else
    propHash.addData(QByteArrayView((const char*)&propertyType,
                                    sizeof(propertyType)));
#endif
    variantHasher.addToHash(propHash, property.data());

    // loop recursively through all child properties
    foreach(const auto& p, property.childProperties)
    {
        propertyHashHelper(p, propHash, variantHasher);
    }

    property.hash = propHash.result();

    // hash of all properties
    hash.addData(property.hash);
}

bool
GtObjectMemento::DynamicPropertyData::operator==(
    const DynamicPropertyData& other) const
{
    const auto sameData = [](const auto& self, const PD& left,
                             const PD& right) -> bool {
        if (left.name != right.name || left.isActive != right.isActive ||
            left.type() != right.type() ||
            left.dataType() != right.dataType() ||
            left.data() != right.data() ||
            left.childProperties.size() != right.childProperties.size())
            return false;
        for (qsizetype i = 0; i < left.childProperties.size(); ++i)
        {
            if (!self(self, left.childProperties.at(i),
                      right.childProperties.at(i)))
                return false;
        }
        return true;
    };
    return typeId == other.typeId && ident == other.ident &&
           name == other.name && brief == other.brief &&
           configuration == other.configuration && flags == other.flags &&
           category == other.category &&
           sameData(sameData, propertyData, other.propertyData);
}

void
GtObjectMemento::calculateHashes() const
{
    if (!m_fullHash.isNull())
    {
        return;
    }

    // initialize hash function
    QCryptographicHash hash(QCryptographicHash::Sha256);

    // hash members
    hash.addData(className().toUtf8());
    hash.addData(uuid().toUtf8());
    hash.addData(ident().toUtf8());
    // hash properties
    gt::detail::VariantHasher variantHasher;
    foreach(const auto &p, properties)
    {
        propertyHashHelper(p, hash, variantHasher);
    }

    foreach(const PropertyData &p, propertyContainers)
    {
        propertyHashHelper(p, hash, variantHasher);
    }
    foreach (const auto& property, dynamicProperties)
    {
        if (!property.propertyData.name.isEmpty())
        {
            propertyHashHelper(property.propertyData, hash, variantHasher);
        }
    }

    m_propertyHash = hash.result();
    hash.reset();

    // Definitions participate in full structural identity, but the historic
    // property hash continues to describe only serialized PropertyData.
    for (const auto& property : dynamicProperties)
    {
        QByteArray record;
        QDataStream stream(&record, QIODevice::WriteOnly);
        stream << property.typeId << property.ident << property.name
               << property.brief << property.configuration << property.flags
               << property.category;
        hash.addData(QByteArray::number(record.size()));
        hash.addData(":", 1);
        hash.addData(record);
    }

    m_dynamicDefinitionHash = hash.result();
    hash.reset();

    // hash over property hash and child elements
    hash.addData(m_propertyHash);
    hash.addData(m_dynamicDefinitionHash);
    for (int i = 0; i < childObjects.size(); i++)
    {
        childObjects[i].calculateHashes();
        hash.addData(childObjects[i].m_fullHash);
    }
    m_fullHash = hash.result();
}

bool
GtObjectMemento::isFlagEnabled(GtObjectMemento::Flag flag) const
{
    return m_flags & flag;
}

void
GtObjectMemento::setFlagEnabled(GtObjectMemento::Flag flag, bool enable)
{
    if (enable)
    {
        m_flags = m_flags | flag;
    }
    else
    {
        m_flags = m_flags & ~flag;
    }
}

bool
GtObjectMemento::isRestorable(GtAbstractObjectFactory* factory) const
{
    if (isNull() || !factory)
    {
        gtWarning() << QObject::tr("memento is NULL!");
        return false;
    }

    if (!factory->knownClass(className()))
    {
        gtWarning() << QObject::tr("class ") << className()
                    << QObject::tr(" not known!");
        return false;
    }

    // Dynamic types are resolved at restore time. An unavailable type is
    // recoverable through GtDummyProperty and must not make the entire
    // containing object unrestorable.
    if (!std::all_of(dynamicProperties.cbegin(), dynamicProperties.cend(),
                     [](const DynamicPropertyData& property) {
                         return gt::isJsonLikePropertyConfiguration(
                                    property.configuration) &&
                                !property.ident.isEmpty() &&
                                !property.typeId.isEmpty();
                     }))
        return false;


    return std::all_of(childObjects.begin(), childObjects.end(),
                       [&factory](const GtObjectMemento& child) {
        return child.isRestorable(factory);
    });
}

PD&
GtObjectMemento::PropertyData::setData(const QVariant& val)
{
    _data = val;
    _dataType = val.typeName();
    return *this;
}

PD&
GtObjectMemento::PropertyData::toStruct(const QString& structTypeName)
{
    _type = STRUCT_T;
    _dataType = structTypeName;
    _data = QVariant{}; // null
    return *this;
}


PD&
GtObjectMemento::PropertyData::fromQMetaProperty(const QMetaProperty& prop,
                                                 const QVariant& val)
{
    name = prop.name();
    setData(val);
    if (prop.isEnumType())
    {
        _type = ENUM_T;
        _dataType = prop.typeName();
        QMetaEnum e = prop.enumerator();
        _data = QString(e.valueToKey(_data.toInt()));
    }

    return *this;
}

const GtObjectMemento*
GtObjectMemento::findChildByUuid(const QString &uuid) const
{
    auto iter = std::find_if(childObjects.begin(), childObjects.end(),
                             [&uuid](const GtObjectMemento& child) {
                                 return uuid == child.uuid();
                             });

    if (iter == childObjects.end())
    {
        return nullptr;
    }

    return &*iter;
}

const GtObjectMemento::PropertyData *
GtObjectMemento::findPropertyByName(const QVector<PropertyData> &list,
                                    const QString& name)
{
    auto iter = std::find_if(std::begin(list), std::end(list),
                 [&name](const PropertyData& childProp) {
        return childProp.name == name;
    });

    return iter != list.end() ? &*iter : nullptr;
}

GtObjectMemento::ExternalizationInfo
GtObjectMemento::externalizationInfo(const GtAbstractObjectFactory& factory) const
{
    auto* metaObject = factory.metaObject(m_className);

    if (!metaObject ||
        !metaObject->inherits(&GtExternalizedObject::staticMetaObject))
    {
        return {};
    }

    using namespace gt::internal;
    auto* pFetched = findPropertyByName(properties, S_EXT_OBJECT_PROP_IS_FETCHED);
    auto* pHash = findPropertyByName(properties, S_EXT_OBJECT_PROP_HASH);

    if (!pFetched || !pHash)
    {
        gtError() << QObject::tr("Failed to retrieve externalization info from "
                                 "memento for '%1'!")
                     .arg(m_className);
        return {};
    }

    return ExternalizationInfo{
        pFetched->data().toBool(),
        pHash->data().toByteArray(),
        metaObject,
    };
}

bool
readProperty(const PD& p, GtObject& obj)
{

    QString fieldType = p.dataType();
    QString fieldName = p.name;

    if (fieldType.isEmpty() || fieldName.isEmpty())
    {
        return false;
    }

    auto* prop = obj.findProperty(fieldName);

    if (prop)
    {
        const auto restoreProperty = [](const auto& self,
                                        GtAbstractProperty& target,
                                        const PD& source) -> bool {
            target.setActive(source.isActive);
            if (source.type() == PD::STRUCT_T)
            {
                // Unknown structured property state is retained on its dummy.
            }
            else
            {
                if (!target.setValueFromVariant(source.data())) return false;
            }
            for (const auto& child : source.childProperties)
            {
                if (auto* subProperty = target.findProperty(child.name))
                {
                    if (!self(self, *subProperty, child)) return false;
                }
            }
            return true;
        };
        if (!restoreProperty(restoreProperty, *prop, p)) return false;
        // TODO: should this be set by the reader ???
        //prop->setOptional(p.isOptional);
    }
    // set the data to a qproperty
    else if (obj.property(fieldName.toLatin1()) != p.data() &&
             !obj.setProperty(fieldName.toLatin1(), p.data()))
    {
        return false;
    }

    return true;
}


void
readProperties(const GtObjectMemento& memento,
               GtObject& obj)
{
    assert(obj.uuid() == memento.uuid());

    assert(!obj.isDummy());


    for (auto const & p :  memento.properties)
    {
        bool success = readProperty(p, obj);

        if (!success)
        {
            gtWarning().medium()
                << QObject::tr("Unknown property '%1' of object "
                               "'%2' (type=%3) ignored").arg(
                                   p.name,
                                   memento.ident(),
                                   obj.metaObject()->className());

        }
    }
}

void gt::importStructEntryFromMemento(const PD& propStruct,
                                  // cppcheck-suppress constParameter
                                  GtPropertyStructInstance& structEntry)
{
    assert(propStruct.type() == PD::STRUCT_T);

    auto const & membersInMemento = propStruct.childProperties;

    // copy all member values into the new entry
    for (auto& member : structEntry.properties())
    {
        if (!member) continue;

        auto prop = GtObjectMemento::findPropertyByName(membersInMemento,
                                                        member->ident());

        if (prop)
        {
            member->setValueFromVariant(prop->data());
            member->setActive(prop->isActive);
        }
        else
        {
            // property does not exist
            gtError() << "Property '" << member->ident()
                      << "' does not exist in memento";
        }
    }
}

void createNewStructEntryFromMemento(const PD& propStruct, GtPropertyStructContainer& c)
{
    // all entries must be struct. Since the user cannot
    // modify a memento directly, an assertion is best here
    assert(propStruct.type() == PD::STRUCT_T);

    try
    {
        auto& structEntry = c.newEntry(propStruct.dataType(),
                                       propStruct.name);

        gt::importStructEntryFromMemento(propStruct, structEntry);

    }
    catch (GTlabException& e)
    {
        // might fail, if the datatype cannot be created in newEntry
        gtError() << e.what();
    }

}

void mergePropertyContainer(const PD& prop, GtPropertyStructContainer& c)
{
    assert(prop.name == c.ident());

    // search, all entries not in memento. If so, schedule for deletion
    std::vector<QString> idsToDelete;

    for (const auto& entryInC : c)
    {
        auto entryInMemento = std::find_if(std::begin(prop.childProperties),
                                           std::end(prop.childProperties),
                                           [&entryInC](const PD& entryInMemento) {
            return entryInMemento.name == entryInC.ident();
        });

        if (entryInMemento == prop.childProperties.end() ||
            entryInMemento->dataType() != entryInC.typeName())
        {
            // entry not in memento or it is incompatible
            idsToDelete.push_back(entryInC.ident());
        }
    }

    // delete all entries in c not found in memento
    for (const auto& idToDelete : idsToDelete)
    {
        auto iter = c.findEntry(idToDelete);
        // cppcheck-suppress assertWithSideEffect
        assert(iter != c.end());
        c.removeEntry(iter);
    }

    idsToDelete.clear();

    // at this time, c only contains elements in the memento, but the memento could also contain
    // elements not in c
    for (const auto& entryInMemento : prop.childProperties)
    {
        auto entryInC = c.findEntry(entryInMemento.name);

        if (entryInC != c.end())
        {
            // by construction of upper code
            assert(entryInC->typeName() == entryInMemento.dataType());

            // yes, we found the object, merge it
            gt::importStructEntryFromMemento(entryInMemento, *entryInC);
        }
        else {
            // create a new entry
            createNewStructEntryFromMemento(entryInMemento, c);
        }
    }
}

void
mergeAllPropertyContainers(const GtObjectMemento& memento, GtObject& obj)
{
    const auto& memPropConts = memento.propertyContainers;


    for (GtPropertyStructContainer & c : obj.propertyContainers())
    {
        auto iter = std::find_if(std::begin(memPropConts), std::end(memPropConts),
                    [&c](const GtObjectMemento::PropertyData& mementoProp) {
            return mementoProp.name == c.ident();
        });

        if (iter != std::end(memPropConts))
        {
            mergePropertyContainer(*iter, c);
        }
        else
        {
            gtWarning().noquote().nospace()
                    << "No memento found for property container '"
                    << c.ident() << "' of object '" << memento.ident() << "'.";
        }
    }
}

std::unique_ptr<GtObject>
GtObjectMemento::toObject(GtAbstractObjectFactory& factory) const
{
    return std::unique_ptr<GtObject>(toObject(factory, nullptr));
}

GtObject*
GtObjectMemento::toObject(GtAbstractObjectFactory& factory, GtObject* parent) const
{
    auto makeDummy = [parent](const QString& msg) -> std::unique_ptr<GtObject>
    {
        gtWarning() << msg;
        auto newObj = std::make_unique<GtObject>(parent);
        newObj->makeDummy();
        return newObj;
    };

    std::unique_ptr<GtObject> obj;


    obj.reset(factory.newObject(className(), parent));

    if (!obj)
    {
        // no class found in factory. we need a dummy object here
        obj = makeDummy(QObject::tr("Creating dummy object "
                                    "for unknown class %1.'").arg(className()));
    }

    if (isFlagEnabled(IsUnresolved))
    {
        obj->makeDummy();
        obj->setFlag(GtObject::SaveAsOwnFile, true);
    }

    if (!mergeTo(*obj, factory))
    {
        return nullptr;
    }

    return obj.release();
}

bool
GtObjectMemento::mergeTo(GtObject& obj, GtAbstractObjectFactory& factory) const
{
    return mergeTo(obj, &factory);
}

bool
GtObjectMemento::mergeTo(GtObject& obj) const
{
    return mergeTo(obj, static_cast<GtAbstractObjectFactory*>(nullptr));
}

bool
GtObjectMemento::mergeTo(GtObject& obj, GtAbstractObjectFactory* factory) const
{

    // Validate dynamic definitions and their embedded values for this object
    // and every child before changing any live object in the tree.
    const auto validateDynamicTree =
        [factory](const auto& self, const GtObjectMemento& memento) -> bool {
        GtObject staged;
        if (factory) staged.setFactory(factory);
        if (!memento.mergeDynamicPropertiesTo(staged, factory)) return false;
        return std::all_of(memento.childObjects.cbegin(),
                           memento.childObjects.cend(),
                           [&self](const GtObjectMemento& child) {
                               return self(self, child);
                           });
    };
    if (!validateDynamicTree(validateDynamicTree, *this)) return false;

    if (!obj.isDummy() && obj.metaObject()->className() != className())
    {
        return false;
    }

    if (isFlagEnabled(IsUnresolved))
    {
        obj.setUuid(uuid());
        obj.setObjectName(ident());
        // we need to make it a dummy, to avoid changing the original object
        // and to highlight a problem
        gtWarning() << QObject::tr("Creating dummy object for unresolved object reference '%1'").arg(ident());
        obj.makeDummy();
        obj.importMementoIntoDummy(*this);
        obj.setFlag(GtObject::SaveAsOwnFile, true);

        // hide childs to not confuse the user
        auto childs = obj.findDirectChildren<GtObject*>();
        for (GtObject* c : childs) c->setUserHidden(true);
    }
    else if (!obj.isDummy())
    {
        if (!mergeDynamicPropertiesTo(obj, factory)) return false;
        obj.setUuid(uuid());
        obj.setObjectName(ident());
        ::readProperties(*this, obj);
        ::mergeAllPropertyContainers(*this, obj);
    }
    else
    {
        obj.setUuid(uuid());
        obj.setObjectName(ident());
        obj.importMementoIntoDummy(*this);
    }


    // We keep track of old objects, which need to be deleted later.
    // There are two exceptions:
    //   - a child is a default object. Then, it must not be deleted
    //   - a memento was merged into a child. Then, the child must be kept as well
    QList<GtObject*> oldChildObjs = obj.findDirectChildren<GtObject*>();

    // loop over all childs in memento, that are not yet in the object
    for (auto const & mementoChild : childObjects)
    {
        // find object with same ident
        const auto child = obj.findDirectChild<GtObject*>(mementoChild.ident());

        // check, whether this object can be merged
        if (child &&
            child->metaObject()->className() == mementoChild.m_className &&
            (child->uuid() == mementoChild.uuid() || child->isDefault()))
        {
            if (factory)
            {
                if (!mementoChild.mergeTo(*child, *factory)) return false;
            }
            else if (!mementoChild.mergeTo(*child))
            {
                return false;
            }

            // since memento has been merged to child, child needs to be kept
            oldChildObjs.removeOne(child);
        }
        else
        {
            if (!factory) return false;
            // we need to create a new object
            const GtObject* newobj = mementoChild.toObject(*factory, &obj);
            if (!newobj) return false;
        }
    }

    // Clean up all old childs, which have not been merged
    for (auto& child : oldChildObjs)
    {
        assert(child);
        if (!child->isDefault()) delete child;
    }

    obj.onObjectDataMerged();

    return true;
}

bool
GtObjectMemento::mergeDynamicPropertiesTo(
    GtObject& obj, const GtAbstractObjectFactory* factory) const
{
    if (!factory) factory = obj.factory();
    const auto applyData = [](const auto& self, GtAbstractProperty& property,
                              const PD& data) -> bool {
        property.setActive(data.isActive);
        if (data.type() != PD::STRUCT_T &&
            !property.setValueFromVariant(data.data()))
            return false;
        for (const auto& childData : data.childProperties)
        {
            auto* child = property.findProperty(childData.name);
            if (!child || !self(self, *child, childData)) return false;
        }
        return true;
    };
    const auto sameDefinition = [](const DynamicPropertyData& definition,
                                   const GtAbstractProperty& property,
                                   const GtAbstractProperty& candidate) {
        if (const auto* dummy = qobject_cast<const GtDummyProperty*>(&property))
            return qobject_cast<const GtDummyProperty*>(&candidate) &&
                   definition == dummy->originalDefinition();
        if (qobject_cast<const GtDummyProperty*>(&candidate) ||
            typeid(property) != typeid(candidate))
            return false;
        const quint32 flags =
            (property.isReadOnlyConfigured() ? DynamicPropertyData::ReadOnly
                                             : 0) |
            (property.isOptional() ? DynamicPropertyData::Optional : 0) |
            (property.isHidden() ? DynamicPropertyData::Hidden : 0) |
            (property.isMonitoring() ? DynamicPropertyData::Monitoring : 0);
        const QString category =
            property.category() == GtAbstractProperty::Custom
                ? property.categoryString()
                : QString{};
        return property.propertyFactoryTypeId() == definition.typeId &&
               property.ident() == definition.ident &&
               property.objectName() == definition.name &&
               property.brief() == definition.brief &&
               property.configuration() == definition.configuration &&
               flags == definition.flags && category == definition.category;
    };

    QList<GtAbstractProperty*> retained;
    std::vector<std::unique_ptr<GtAbstractProperty>> replacements;
    QStringList order;
    std::vector<std::pair<GtAbstractProperty*, const PD*>> retainedData;
    QSet<QString> ids;
    for (const auto& definition : dynamicProperties)
    {
        if (definition.ident.isEmpty() || ids.contains(definition.ident) ||
            !gt::isJsonLikePropertyConfiguration(definition.configuration))
            return false;
        ids.insert(definition.ident);
        auto* current = obj.findProperty(definition.ident);
        GtAbstractProperty* candidate = nullptr;
        std::unique_ptr<GtAbstractProperty> property;
        // The Core registry also holds the built-in and module property types.
        const auto* registry = factory;
        if (!registry || !registry->hasPropertyFactory(definition.typeId))
            registry = gtObjectFactory;
        if (registry->hasPropertyFactory(definition.typeId))
        {
            property = gt::createProperty(*registry, definition.typeId,
                                          definition.ident);
        }
        if (property)
        {
            property->setObjectName(definition.name);
            property->setBrief(definition.brief);
            property->setReadOnly(
                definition.hasFlag(DynamicPropertyData::ReadOnly));
            property->setOptional(
                definition.hasFlag(DynamicPropertyData::Optional));
            property->hide(definition.hasFlag(DynamicPropertyData::Hidden));
            property->setMonitoring(
                definition.hasFlag(DynamicPropertyData::Monitoring));
            if (!definition.category.isEmpty())
                property->setCategory(definition.category);
            if (!property->applyConfiguration(definition.configuration))
                property.reset();
        }
        if (!property)
        {
            property = std::make_unique<GtDummyProperty>(definition);
        }
        property->setPropertyFactoryTypeId(definition.typeId);
        candidate = property.get();
        if (!qobject_cast<GtDummyProperty*>(candidate) &&
            !definition.propertyData.name.isEmpty() &&
            !applyData(applyData, *candidate, definition.propertyData))
            return false;

        if (current && sameDefinition(definition, *current, *candidate))
        {
            retained.push_back(current);
            retainedData.emplace_back(current, &definition.propertyData);
        }
        else
        {
            replacements.push_back(std::move(property));
        }
        order.push_back(definition.ident);
    }

    // Stage construction and value data before changing ownership or views.
    // Retained instances receive only the prevalidated PropertyData below.
    if (!obj.replaceDynamicProperties(retained, std::move(replacements), order))
        return false;

    for (const auto& entry : retainedData)
    {
        if (entry.second->name.isEmpty() ||
            qobject_cast<GtDummyProperty*>(entry.first))
            continue;
        if (!applyData(applyData, *entry.first, *entry.second)) return false;
    }
    return true;
}
