/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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

#include "gt_objectmemento.h"
#include "gt_objectio.h"
#include "gt_abstractobjectfactory.h"
#include "gt_abstractproperty.h"
#include "gt_dummyobject.h"

using PD = GtObjectMemento::MementoData::PropertyData;

GtObjectMemento::GtObjectMemento(const GtObject* obj, bool clone)
{
    if (obj)
    {
        GtObjectIO oio;
        m_data = oio.toMemento(obj, clone);
    }
}

GtObjectMemento::GtObjectMemento(const GtObjectMemento::MementoData &d)
    : m_data(d)
{
}

GtObjectMemento::GtObjectMemento(const QDomElement& element)
{
    if (!element.isNull())
    {
        GtObjectIO oio;
        m_data = oio.toMemento(element);
    }
}

GtObjectMemento::GtObjectMemento(const QByteArray& byteArray)
{

    QDomDocument doc;
    if (!doc.setContent(byteArray))
    {
        return;
    }

    m_data = GtObjectIO().toMemento(doc.documentElement());

}

bool
GtObjectMemento::isNull() const
{
    return m_data.uuid.isNull();
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

bool
GtObjectMemento::mergeTo(GtObject* obj, GtAbstractObjectFactory* factory) const
{
    if (!obj || !factory)
    {
        return false;
    }

    return mergeTo(*obj, *factory);
}

const QString&
GtObjectMemento::className() const
{
    return m_data.className;
}

const QString&
GtObjectMemento::uuid() const
{
    return m_data.uuid;
}

const QString&
GtObjectMemento::ident() const
{
    return m_data.ident;
}

bool
GtObjectMemento::canCastTo(const QString& classname,
                           GtAbstractObjectFactory* factory)
{
    QStringList classHierarchy = factory->classHierarchy(className());

    return classHierarchy.contains(classname);
}


class VariantHasher
{
public:
    VariantHasher() :
      buff(&bb), ds(&buff)
    {
        bb.reserve(64);
        buff.open(QIODevice::WriteOnly);
    }
    void addToHash(QCryptographicHash& hash, const QVariant& variant)
    {
        static QVariant::Type type_QString = QVariant::nameToType("QString");
        static QVariant::Type type_QStringList = QVariant::nameToType(
                    "QStringList");
        if ( variant.type() == type_QString )
        {
            // special case QString: we don't distinguish between empty and null strings!
            // (which the code below does...)
            hash.addData(variant.toString().toUtf8());
        }
        else if ( variant.type() == type_QStringList )
        {
            // special case QString: we don't distinguish between empty and null strings!
            // (which the code below does...)
            QStringList strList = variant.toStringList();
            foreach (const QString& s , strList)
            {
                hash.addData(s.toUtf8());
            }
        }
        else
        {
            buff.seek(0);
            ds << variant;
            hash.addData(bb);
        }
    }
private:
    QByteArray bb;
    QBuffer buff;
    QDataStream ds;
};


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
    hash.addData(m_data.className.toUtf8());
    hash.addData(m_data.uuid.toUtf8());
    hash.addData(m_data.ident.toUtf8());
    // hash properties
    VariantHasher variantHasher;
    foreach(const MementoData::PropertyData &p, m_data.properties)
    {
        propertyHashHelper(p, hash, variantHasher);
    }

    // store property hash
    m_propertyHash = hash.result();
    hash.reset();

    // hash over property hash and child elements
    hash.addData(m_propertyHash);
    for (int i = 0; i < m_data.childObjects.size(); i++)
    {
        m_data.childObjects[i].calculateHashes();
        hash.addData(m_data.childObjects[i].m_fullHash);
    }
    m_fullHash = hash.result();
}

void
GtObjectMemento::propertyHashHelper(const PD& property, QCryptographicHash& hash, VariantHasher& variantHasher) const
{
    QCryptographicHash propHash(QCryptographicHash::Sha256);

    // hash property
    propHash.addData(property.name.toUtf8());
    propHash.addData((const char*)&property.isOptional, sizeof(property.isOptional));
    propHash.addData((const char*)&property.isActive, sizeof(property.isActive));
    propHash.addData(property.dataType().toUtf8());
    propHash.addData(property.dynamicObjectName.toUtf8());

    auto propertyType = property.type();
    propHash.addData((const char*)&propertyType, sizeof(propertyType));
    variantHasher.addToHash(propHash, property.data());

    // loop recursively through all child properties
    foreach(const MementoData::PropertyData& p, property.childProperties)
    {
        propertyHashHelper(p, propHash, variantHasher);
    }

    property.hash = propHash.result();

    // hash of all properties
    hash.addData(property.hash);
}

bool
GtObjectMemento::isRestorable(GtAbstractObjectFactory* factory) const
{
    if (isNull())
    {
        gtWarning() << QObject::tr("memento is NULL!");
        return false;
    }

    const QString classname = m_data.className;

    if (!factory->knownClass(classname))
    {
        gtWarning() << QObject::tr("class ") << classname
                    << QObject::tr(" not known!");
        return false;
    }

    /* child informations */

    for  (const auto& child : m_data.childObjects)
    {
        if (!child.isRestorable(factory))
        {
            return false;
        }
    }

    return true;
}

PD &
PD::setData(const QVariant &val)
{
    _data = val;
    _dataType = val.typeName();
    return *this;
}

PD
PD::makeDynamicContainer(const QString &objName)
{
    PD pd;
    pd._type = DYNCONT_T;
    pd._data = {};
    pd.dynamicObjectName = objName;
    pd._dataType = "";

    return pd;
}

PD
PD::makeDynamicChild(const QVariant &value, const QString &objName,
                const QString &dynamicTypeName)
{
    PD pd;
    pd._type = DATA_T;
    pd._data = value;
    pd.dynamicObjectName = objName;
    pd._dataType = dynamicTypeName;

    return pd;
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
GtObjectMemento::MementoData::findChild(const QString &ident) const
{
    auto iter = std::find_if(childObjects.begin(), childObjects.end(),
                 [&ident](const GtObjectMemento& child) {
        return ident == child.ident();
    });

    if (iter == childObjects.end())
    {
        return nullptr;
    }

    return &*iter;
}

bool
readDummyProperty(const GtObjectMemento::MementoData::PropertyData& p,
                  GtDummyObject& obj)
{

    QString const fieldType = p.dataType();
    QString const fieldName = p.name;

    if (fieldType.isEmpty() || fieldName.isEmpty())
    {
        return false;
    }

    auto const & val = p.data();
    auto const opt = p.isOptional;
    auto const act = p.isActive;

    if (!GtObjectIO::usePropertyList(val))
    {
        obj.addDummyProperty(fieldName, fieldType, opt, act, val);
    }
    else
    {
        obj.addDummyPropertyList(fieldName, fieldType, opt, act, val);
    }

    return true;
}

bool
readProperty(const GtObjectMemento::MementoData::PropertyData& p, GtObject& obj)
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
readProperties(const GtObjectMemento::MementoData& data,
               GtObject& obj)
{
    assert(obj.uuid() == data.uuid);

    bool isDummy = obj.isDummy();
    auto * dummyObject = qobject_cast<GtDummyObject*>(&obj);

    assert(isDummy || obj.metaObject()->className() == data.className);
    assert(!isDummy || dummyObject);


    for (auto const & p :  data.properties)
    {
        bool success = !isDummy ?
                           readProperty(p, obj) :
                           readDummyProperty(p, *dummyObject);

        if (!success)
        {

            gtWarning() << QObject::tr("could not find property") <<
                QStringLiteral(" (") << data.className <<
                QStringLiteral("::") << p.name <<
                QStringLiteral(")");
            gtWarning() << "     |-> "
                        << obj.metaObject()->className();
            gtWarning() << "     |-> " << obj.objectName();
        }
    }
}

std::unique_ptr<GtObject>
GtObjectMemento::toObject(GtAbstractObjectFactory& factory) const
{

    auto clzname = m_data.className;

    std::unique_ptr<GtObject> obj(factory.newObject(clzname, nullptr));

    if (!obj)
    {
        // no class found in factory. we need a dummy object here
        gtWarning().nospace().noquote()
            << "Creating dummy object for unknown class '"
            << clzname << "'.";

        auto tmp = new GtDummyObject(nullptr);
        tmp->setOrigClassName(clzname);
        obj.reset(tmp);
    }


    mergeTo(*obj, factory);

    return obj;
}

bool
GtObjectMemento::mergeTo(GtObject& obj, GtAbstractObjectFactory& factory) const
{

    const GtObjectMemento::MementoData& data = m_data;

    if (!obj.isDummy() && obj.metaObject()->className() != data.className)
    {
        gtError() << "Object class name and object type does not match";

        return false;
    }

    obj.setUuid(data.uuid);
    obj.setObjectName(data.ident);


    ::readProperties(m_data, obj);

    // child objects

    auto childs = obj.findDirectChildren<GtObject*>();

    // loop over all existing childs in the current object
    for (auto& child : childs)
    {
        assert(child);

        // check, that memento data contain object
        auto const * mementoChild = data.findChild(child->objectName());

        bool wasMerged = false;

        // can only be merged if name and uuid match
        if (mementoChild && mementoChild->uuid() == child->uuid())
        {
            assert(mementoChild->ident() == child->objectName());

            // read in props of mementochild to child
            wasMerged = mementoChild->mergeTo(*child, factory);
        }

        if (!child->isDefault() && !wasMerged)
        {
            // the child object could not be merged, we need to read
            // it back in later

            // remove child, if it is not in memento and not a default child
            delete child;
        }
    }

    // loop over all childs in memento, that are not yet in the object
    for (auto const & mementoChild : data.childObjects)
    {
        // skip if mementoChild already in object,
        // it has been alrady merged before
        auto childObj = obj.getDirectChildByUuid(mementoChild.uuid());

        // double check that if it was properly merged
        assert(!childObj || childObj->objectName() == mementoChild.ident());

        if (childObj) continue;

        auto newObject = mementoChild.toObject(factory);
        if (newObject)
        {
            obj.appendChild(newObject.release());
        }

    }

    return true;
}

