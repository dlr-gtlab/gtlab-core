/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 29.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCryptographicHash>
#include <QBuffer>
#include <QIODevice>
#include <QDataStream>
#include <QMetaProperty>

#include "gt_externalizedobject.h"
#include "gt_objectmemento.h"
#include "gt_objectio.h"
#include "gt_abstractobjectfactory.h"
#include "gt_abstractproperty.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"
#include "gt_exceptions.h"
#include "internal/varianthasher.h"
#include "internal/gt_externalizedobjectprivate.h"

using PD = GtObjectMemento::PropertyData;

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
GtObjectMemento::setClassName(const QString &className)
{
    m_className = className;
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

    // store property hash
    m_propertyHash = hash.result();
    hash.reset();

    // hash over property hash and child elements
    hash.addData(m_propertyHash);
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
    if (isNull())
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


    return std::all_of(childObjects.begin(), childObjects.end(),
                       [&factory](const GtObjectMemento& child) {
        return child.isRestorable(factory);
    });
}

PD &
PD::setData(const QVariant &val)
{
    _data = val;
    _dataType = val.typeName();
    return *this;
}

PD &
PD::toStruct(const QString &structTypeName)
{
    _type = STRUCT_T;
    _dataType = structTypeName;
    _data = QVariant{}; // null
    return *this;
}


PD &
PD::fromQMetaProperty(const QMetaProperty &prop, const QVariant& val)
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
        // it is an abstract property
        prop->setActive(p.isActive);
        prop->setValueFromVariant(p.data());
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

    std::unique_ptr<GtObject> obj(factory.newObject(className(), parent));

    if (!obj)
    {
        // no class found in factory. we need a dummy object here
        gtWarning().nospace().noquote()
            << "Creating dummy object for unknown class '"
            << className() << "'.";

        obj.reset(new GtObject(parent));
        obj->makeDummy();
    }

    mergeTo(*obj, factory);

    return obj.release();
}

bool
GtObjectMemento::mergeTo(GtObject& obj, GtAbstractObjectFactory& factory) const
{

    if (!obj.isDummy() && obj.metaObject()->className() != className())
    {
        return false;
    }

    obj.setUuid(uuid());
    obj.setObjectName(ident());

    if (!obj.isDummy())
    {
        ::readProperties(*this, obj);
        ::mergeAllPropertyContainers(*this, obj);
    }
    else
    {
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
            mementoChild.mergeTo(*child, factory);

            // since memento has been merged to child, child needs to be kept
            oldChildObjs.removeOne(child);
        }
        else
        {
            // we need to create a new object
            auto newobj = mementoChild.toObject(factory, &obj);
            assert(newobj);
            Q_UNUSED(newobj);
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
