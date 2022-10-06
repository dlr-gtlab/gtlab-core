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

using PD = GtObjectMemento::MementoData::PropertyData;

GtObjectMemento::GtObjectMemento(const GtObject* obj, bool clone)
{
    if (obj)
    {
        GtObjectIO oio;
        m_data = oio.toMemento(obj, clone);
    }
}

GtObjectMemento::GtObjectMemento(const QDomElement& element)
{
    if (!element.isNull())
    {
        GtObjectIO oio;
        m_data = oio.toMemento(element);

        // cache XML document
        QDomNode node = m_domDocument.importNode(element, true);
        m_domDocument.appendChild(node);
    }
}

GtObjectMemento::GtObjectMemento(const QByteArray& byteArray)
{
    m_domDocument.setContent(byteArray);
    if (!m_domDocument.isNull())
    {
        GtObjectIO oio;
        m_data = oio.toMemento(m_domDocument.documentElement());
    }
}

bool
GtObjectMemento::isNull() const
{
    return m_data.uuid.isNull();
}

QDomElement
GtObjectMemento::documentElement() const
{
    // create XML representation if necessary
    if (m_domDocument.isNull() && !isNull())
    {
        GtObjectIO oio;
        QDomElement e = oio.toDomElement(*this, m_domDocument);
        m_domDocument.appendChild(e);
    }

    // delegate to internal QDomDocument
    return m_domDocument.documentElement();
}

QByteArray
GtObjectMemento::toByteArray() const
{
    // create XML representation if necessary
    if (m_domDocument.isNull() && !isNull())
    {
        GtObjectIO oio;
        QDomElement e = oio.toDomElement(*this, m_domDocument);
        m_domDocument.appendChild(e);
    }

    // delegate to internal QDomDocument
    return m_domDocument.toByteArray();
}

bool
GtObjectMemento::isRestorable(GtAbstractObjectFactory* factory)
{
    if (!factory)
    {
        gtWarning() << QObject::tr("no factory set!");
        return false;
    }

    return isRestorable(factory, documentElement());
}

bool
GtObjectMemento::mergeTo(GtObject* obj, GtAbstractObjectFactory* factory) const
{
    if (!obj || !factory)
    {
        return false;
    }

    GtObjectIO oio(factory);
    return oio.mergeObject(data(), *obj);
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

QString
GtObjectMemento::attribute(const QString& id) const
{
    QDomElement root = documentElement();

    if (root.isNull())
    {
        return QString();
    }

    return root.attribute(id);
}

GtObject*
GtObjectMemento::createObject(GtAbstractObjectFactory* factory)
{
    GtObjectIO oio(factory);
    return oio.toObject(*this);
}

bool
GtObjectMemento::isRestorable(GtAbstractObjectFactory* factory,
                              QDomElement const& element)
{
    if (element.isNull())
    {
        gtWarning() << QObject::tr("memento element is NULL!");
        return false;
    }

    const QString classname = element.attribute(GtObjectIO::S_CLASS_TAG);

    if (!factory->knownClass(classname))
    {
        gtWarning() << QObject::tr("class ") << classname
                    << QObject::tr(" not known!");
        return false;
    }

    /* child informations */
    QDomElement children =
        element.firstChildElement(GtObjectIO::S_OBJECTLIST_TAG);

    if (children.isNull())
    {
        // no children... nothing to do :)
        return true;
    }

    QDomElement child = children.firstChildElement(GtObjectIO::S_OBJECT_TAG);

    while (!child.isNull())
    {
        if (!isRestorable(factory, child))
        {
            return false;
        }

        child = child.nextSiblingElement(GtObjectIO::S_OBJECT_TAG);
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

const GtObjectMemento::MementoData* GtObjectMemento::MementoData::findChild(const QString &ident) const
{
    auto iter = std::find_if(childObjects.begin(), childObjects.end(),
                 [&ident](const GtObjectMemento& child) {
        return ident == child.ident();
    });

    if (iter == childObjects.end())
    {
        return nullptr;
    }

    return &iter->data();
}
