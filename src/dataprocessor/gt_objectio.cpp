/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 29.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QSet>
#include <QStringList>
#include <QMetaProperty>
#include <QDomDocument>
#include <QUuid>
#include <QPointF>
#include <QDataStream>
#include <QtGlobal>
#include <QDebug>

#include <cfloat>
#include <typeinfo>

#include "gt_object.h"
#include "gt_dummyobject.h"
#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"
#include "gt_abstractproperty.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"

#include "gt_objectio.h"

const QString GtObjectIO::S_OBJECT_TAG = QStringLiteral("object");
const QString GtObjectIO::S_OBJECTLIST_TAG = QStringLiteral("objectlist");
const QString GtObjectIO::S_UUID_TAG = QStringLiteral("uuid");
const QString GtObjectIO::S_CLASS_TAG = QStringLiteral("class");
const QString GtObjectIO::S_NAME_TAG = QStringLiteral("name");
const QString GtObjectIO::S_ENTRY_NAME_TAG = QStringLiteral("entryName");
const QString GtObjectIO::S_TYPE_TAG = QStringLiteral("type");
const QString GtObjectIO::S_ID_TAG = QStringLiteral("id");
const QString GtObjectIO::S_VALUE_TAG = QStringLiteral("value");
const QString GtObjectIO::S_OPTIONAL_TAG = QStringLiteral("optional");
const QString GtObjectIO::S_ACTIVE_TAG = QStringLiteral("active");
const QString GtObjectIO::S_PROPERTY_TAG = QStringLiteral("property");
const QString GtObjectIO::S_PROPERTYLIST_TAG = QStringLiteral("propertylist");
const QString GtObjectIO::S_PROPERTYCONT_TAG =
    QStringLiteral("property-container");
const QString GtObjectIO::S_DIFF_INDEX_TAG = QStringLiteral("index");
const QString GtObjectIO::S_DIFF_INDEX_CHANGED_TAG =
        QStringLiteral("diff-index-changed");
const QString GtObjectIO::S_ENTRY_TAG = QStringLiteral("entry");
const QString GtObjectIO::S_DIFF_OBJ_REMOVE_TAG =
    QStringLiteral("diff-object-remove");
const QString GtObjectIO::S_DIFF_OBJ_ADD_TAG =
    QStringLiteral("diff-object-add");
const QString GtObjectIO::S_DIFF_PROP_CHANGE_TAG =
    QStringLiteral("diff-property-change");
const QString GtObjectIO::S_DIFF_PROPLIST_CHANGE_TAG =
    QStringLiteral("diff-propertylist-change");
const QString GtObjectIO::S_DIFF_PROPCONT_ENTRY_ADDED_TAG =
    QStringLiteral("diff-property-container-entry-add");
const QString GtObjectIO::S_DIFF_PROPCONT_ENTRY_REMOVE_TAG =
    QStringLiteral("diff-property-container-entry-remove");
const QString GtObjectIO::S_DIFF_PROPCONT_ENTRY_CHANGE_TAG =
    QStringLiteral("diff-property-container-entry-change");
const QString GtObjectIO::S_DIFF_ATTR_CHANGE_TAG =
    QStringLiteral("diff-attribute-change");
const QString GtObjectIO::S_DIFF_ATTR_REMOVE_TAG =
    QStringLiteral("diff-attribute-remove");
const QString GtObjectIO::S_DIFF_NEWVAL_TAG = QStringLiteral("newVal");
const QString GtObjectIO::S_DIFF_OLDVAL_TAG = QStringLiteral("oldVal");
const QSet<QString> GtObjectIO::S_LISTTYPES = QSet<QString>()
                                            << QStringLiteral("QList<int>")
                                            << QStringLiteral("QList<bool>")
                                            << QStringLiteral("QList<QPointF>")
                                            << QStringLiteral("QVector<double>")
                                            << QStringLiteral("QStringList");


namespace
{

    enum class DiffMode
    {
        Apply,
        Revert
    };

    QVariant
    propertyToVariant(const QString& value, const QString& type);
    QVariant
    propertyListToVariant(const QString& value, const QString& type);

    GtObjectMemento::PropertyData
    readProperty(const QDomElement& element, bool& error);


    bool
    handlePropContEntryChanged(GtObject& parentObject,
                               const QDomElement& propContElemC, DiffMode mode);

    bool
    handleIndexChange(GtObject* parent,
                      const QDomElement& indChanges, DiffMode mode);

    bool
    handlePropertyNodeChange(GtObject* target,
                             const QDomElement &change,
                             const bool list,
                             DiffMode mode);

    bool
    handleAttributeNodeChange(GtObject* target,
                              const QDomElement& change,
                              DiffMode mode);

    bool
    handleObjectAddRemove(GtObject* target,
                          const QDomElement& diffTag,
                          DiffMode mode);
}

/** specialization for QVector<double>
 */
template<>
inline QString GtObjectIO::listToString<QVector<double> >(
        const QVector<double>& t)
{
    // this code is optimized to avoid creating temporaries / reallocating strings,
    // please don't change unless you still make it faster...
    QString str;
    str.reserve(t.size() * (DBL_DIG + 2));

    foreach (double m, t)
    {
        str.append(QString::number(m, 'g', DBL_DIG)).append(';');
    }
    str.truncate(str.size() - 1);

    return str;
}

/** specialization for QList<bool>
 */
template<>
inline QString GtObjectIO::listToString<QList<int> >(const QList<int>& t)
{
    QString str;
    str.reserve(t.size() * 5);

    foreach (int m, t)
    {
        str += QString::number(m) + ";";
    }
    str.truncate(str.size() - 1);

    return str;
}

/** specialization for QList<bool>
 */
template<>
inline QString GtObjectIO::listToString<QList<bool> >(const QList<bool>& t)
{
    QString str;
    str.reserve(t.size()*6);

    foreach (bool m, t)
    {
        str += m ? QStringLiteral("true;") : QStringLiteral("false;");
    }
    str.truncate(str.size() - 1);

    return str;
}

/** specialization for QList<QPointF>
 */
template<>
inline QString GtObjectIO::listToString<QList<QPointF> >(
        const QList<QPointF>& t)
{
    QString str;
    str.reserve(2 * t.size() * (DBL_DIG + 2));

    foreach (QPointF m, t)
    {
        str.append(QString::number(m.x(), 'g', DBL_DIG)).append("_").append(
                    QString::number(m.y(), 'g', DBL_DIG)).append(';');
    }
    str.truncate(str.size()-1);

    return str;
}

/** specialization for QList<QPointF>
 */
template<>
inline QString GtObjectIO::listToString<QStringList>(const QStringList& t)
{
    return t.join(';');
}

GtObjectIO::GtObjectIO(GtAbstractObjectFactory* factory) : m_factory(factory)
{
}

void
GtObjectIO::setFactory(GtAbstractObjectFactory* factory)
{
    m_factory = factory;
}

GtObjectMemento
GtObjectIO::toMemento(const GtObject* o, bool clone)
{
    // global object element
    GtObjectMemento memento;

    const GtDummyObject* d_obj = qobject_cast<const GtDummyObject*>(o);

    // class name
    if (!d_obj)
    {
        memento.setClassName(o->metaObject()->className());
    }
    else
    {
        memento.setClassName(d_obj->origClassName());
    }

    // uuid
    QString uuid(o->uuid());

    if (!clone)
    {
        uuid = QUuid::createUuid().toString();
    }

    memento.setUuid(uuid);

    // object name
    memento.setIdent(o->objectName());

    // store property information
    writeProperties(memento, o);

    // child objects
    QList<GtObject*> directChildren = o->findDirectChildren<GtObject*>();
    memento.childObjects.reserve(directChildren.size());

    foreach (const GtObject* child, directChildren)
    {
        // recursion through GtObjectMemento constructor
        memento.childObjects.push_back(GtObjectMemento(child, clone));
    }

    return memento;
}

QDomElement
GtObjectIO::toDomElement(const GtObjectMemento& memento, QDomDocument& doc,
                         bool skipChildren)
{
    // global object element
    QDomElement element = doc.createElement(S_OBJECT_TAG);

    // class name
    element.setAttribute(S_CLASS_TAG, memento.className());

    // uuid
    element.setAttribute(S_UUID_TAG, memento.uuid());

    // object name
    element.setAttribute(S_NAME_TAG, memento.ident());

    // store property information
    writeProperties(doc, element, memento);

    // children
    if (!skipChildren && ! memento.childObjects.isEmpty())
    {
        QDomElement childElement =
            doc.createElement(S_OBJECTLIST_TAG);

        foreach(const GtObjectMemento& child, memento.childObjects)
        {
            childElement.appendChild(toDomElement(child, doc));
        }

        element.appendChild(childElement);
    }

    return element;
}

GtObjectMemento
GtObjectIO::toMemento(const QDomElement& e)
{
    // global object element

    auto memento = GtObjectMemento{}
        .setClassName(e.attribute(S_CLASS_TAG))
        .setUuid(e.attribute(S_UUID_TAG))
        .setIdent(e.attribute(S_NAME_TAG));

    // store property information
    memento.properties = readProperties(e);
    memento.dynamicSizeProperties = readPropertyContainers(e);

    // child objects
    QDomElement children = e.firstChildElement(S_OBJECTLIST_TAG);

    if (!children.isNull())
    {
        QDomElement compElement = children.firstChildElement(S_OBJECT_TAG);

        while (!compElement.isNull())
        {
            // recursion through GtObjectMemento constructor
            memento.childObjects.push_back(GtObjectMemento(compElement));

            compElement = compElement.nextSiblingElement(S_OBJECT_TAG);
        }
    }

    return memento;
}

bool
applyDiffOnObject(QDomElement& parent, GtObject* parentObject, DiffMode mode)
{
    if (parent.isNull())
    {
        return true;
    }

    if (!parentObject)
    {
        return false;
    }

    QDomElement diffTag = parent.firstChildElement();
    while (!diffTag.isNull())
    {
        const auto& tag = diffTag.tagName();

        if (tag == GtObjectIO::S_DIFF_OBJ_REMOVE_TAG ||
            tag == GtObjectIO::S_DIFF_OBJ_ADD_TAG)
        {
            // handle removes
            if (!handleObjectAddRemove(parentObject, diffTag, mode))
            {
                return false;
            }
        }

        else if (tag == GtObjectIO::S_DIFF_INDEX_CHANGED_TAG)
        {
            if (!handleIndexChange(parentObject, diffTag, mode))
            {
                return false;
            }
        }
        else if (tag == GtObjectIO::S_DIFF_PROP_CHANGE_TAG)
        {
            handlePropertyNodeChange(parentObject, diffTag, false,
                                     mode);
        }
        else if (tag  == GtObjectIO::S_DIFF_ATTR_CHANGE_TAG)
        {
            // handle object attribute changes
            handleAttributeNodeChange(parentObject, diffTag,
                                      mode);
        }
        else if (tag == GtObjectIO::S_DIFF_PROPLIST_CHANGE_TAG)
        {
            // handle prop list changes
            handlePropertyNodeChange(parentObject, diffTag, true,
                                     mode);
        }
        else if (tag == GtObjectIO::S_DIFF_PROPCONT_ENTRY_CHANGE_TAG)
        {
            // handle property container element changed
            handlePropContEntryChanged(*parentObject, diffTag,
                                      mode);
        }
        else if (tag == GtObjectIO::S_DIFF_PROPCONT_ENTRY_REMOVE_TAG)
        {
            //handlePropContEntryChanged(*parentObject, propContElemRem, DiffMode::Apply);
        }

        diffTag = diffTag.nextSiblingElement();
    }


    return true;
}

bool
GtObjectIO::applyDiff(GtObjectMementoDiff& diff, GtObject* obj)
{
    if (diff.isNull())
    {
        return true;
    }

    QDomElement parent = diff.documentElement();
    if (!parent.isNull() && parent.tagName() != S_OBJECT_TAG)
    {
        parent = parent.nextSiblingElement(S_OBJECT_TAG);
    }

    while (!parent.isNull())
    {
        const QString parentUUID = parent.attribute(S_UUID_TAG);

        if (parentUUID.isEmpty())
        {
            return false;
        }

        GtObject* parentObject = obj->getObjectByUuid(parentUUID);


        if (!applyDiffOnObject(parent, parentObject, DiffMode::Apply))
        {
            return false;
        }

        parentObject->onObjectDataMerged();
        parentObject->onObjectDiffMerged();

        parent = parent.nextSiblingElement(S_OBJECT_TAG);
    }

    return true;
}

bool
GtObjectIO::revertDiff(GtObjectMementoDiff& diff, GtObject* obj)
{
    if (diff.isNull())
    {
        return true;
    }

    QDomElement parent = diff.documentElement();
    if (!parent.isNull() && parent.tagName() != S_OBJECT_TAG)
    {
        parent = parent.nextSiblingElement(S_OBJECT_TAG);
    }

    while (!parent.isNull())
    {
        const QString parentUUID = parent.attribute(S_UUID_TAG);

        if (parentUUID.isEmpty())
        {
            return false;
        }

        GtObject* parentObject = obj->getObjectByUuid(parentUUID);


        if (!applyDiffOnObject(parent, parentObject, DiffMode::Revert))
        {
            return false;
        }

        parentObject->onObjectDataMerged();
        parentObject->onObjectDiffMerged();

        parent = parent.nextSiblingElement(S_OBJECT_TAG);
    }

    return true;
}

void
GtObjectIO::writeProperties(GtObjectMemento& memento,
                            const GtObject* obj)
{
    const QMetaObject* meta = obj->metaObject();

    const GtDummyObject* d_obj = qobject_cast<const GtDummyObject*>(obj);

    if (d_obj)
    {
        foreach (GtDummyObject::DummyProperty d_p,
                 d_obj->dummyProperties())
        {
            // static property
            GtObjectMemento::PropertyData mprop;
            mprop.name = d_p.m_id;
            mprop.setData(d_p.m_val);
            mprop.isOptional = d_p.m_optional;
            mprop.isActive = d_p.m_active;

            memento.properties.push_back(mprop);
        }

//        foreach (GtDummyObject::DummyProperty d_p,
//                 d_obj->dummyPropertyLists())
//        {
//            // static property
//            GtObjectMemento::MementoData::PropertyData mprop;
//            mprop.name = d_p.m_id;
//            mprop.data = d_p.m_val;
//            mprop.isOptional = d_p.m_optional;
//            mprop.isActive = d_p.m_active;

//            data.properties.push_back(mprop);
//        }

        return;
    }

    // static properties
    QList<GtAbstractProperty*> props = obj->properties();
    QSet<QString> storedProps;

    // GTlab properties
    foreach (GtAbstractProperty* property, props)
    {
        writePropertyHelper(memento.properties, storedProps, property);
    }

    // dynamic properties
    for (const GtPropertyStructContainer& c: obj->dynamicProperties())
    {
        auto dynamicPropData = toDynamicPropertyData(c);
        memento.dynamicSizeProperties.append(std::move(dynamicPropData));
    }


    // meta properties
    for (int i = 0; i < meta->propertyCount(); ++i)
    {
        const char* name = meta->property(i).name();

        if (storedProps.contains(name))
        {
            qDebug() << "property already stored! (" << name << ") skipping...";
            continue;
        }

        int id = meta->indexOfProperty(name);
        if (id < 0)
        {
            continue;
        }
        QMetaProperty prop = obj->metaObject()->property(id);
        if (QString(prop.name()) == QLatin1String("objectName"))
        {
            continue;
        }

        memento.properties.push_back(
            GtObjectMemento::PropertyData()
                .fromQMetaProperty(prop, prop.read(obj)));
    }
}

void
GtObjectIO::writeProperties(QDomDocument& doc,
                            QDomElement& root,
                            const GtObjectMemento& memento)
{
    // GTlab properties
    foreach (const GtObjectMemento::PropertyData& property,
             memento.properties)
    {
        root.appendChild(toDomElement(property, doc));
    }

    foreach(const GtObjectMemento::PropertyData& property,
             memento.dynamicSizeProperties)
    {
        root.appendChild(dynamicSizePropToDomElement(property, doc));
    }
}

QVector<GtObjectMemento::PropertyData>
GtObjectIO::readProperties(const QDomElement& element) const
{
    QVector<GtObjectMemento::PropertyData> properties;

    QDomElement propElement = element.firstChildElement();

    while (!propElement.isNull())
    {
        if (propElement.tagName() == S_PROPERTY_TAG)
        {
            bool error = false;
            GtObjectMemento::PropertyData propData = readProperty(propElement,
                                                                  error);

            if (!error) properties.push_back(propData);
        }
        else if (propElement.tagName() == S_PROPERTYLIST_TAG)
        {
            QString fieldType = propElement.attribute(S_TYPE_TAG);
            QString fieldName = propElement.attribute(S_NAME_TAG);

            if (!fieldType.isEmpty() && !fieldName.isEmpty())
            {
                GtObjectMemento::PropertyData propData;
                propData.name = fieldName;
                propData.setData(propertyListToVariant(propElement.text(),
                                                       fieldType));

                properties.push_back(propData);
            }
        }

        propElement = propElement.nextSiblingElement();
    }

    return properties;

}

/**
 * @brief Helper function to read multiple property entries
 *
 * @param readPropDataFromXml Function (QDomElement, bool& error)
 *          to read a child element returning a PropertyData entry
 * @param parentElement The parent xml element containing the properties
 * @param propertyTag Xml tag name for the property
 * @return Vector of property data
 */
template <typename ReadPDFromXmlFun>
QVector<GtObjectMemento::PropertyData>
readPropertyElements(ReadPDFromXmlFun readPropDataFromXml,
                     const QDomElement& parentElement,
                     const QString& propertyTag)
{
    QVector<GtObjectMemento::PropertyData> childs;

    auto elem = parentElement.firstChildElement(propertyTag);

    while (!elem.isNull())
    {
        bool error = false;
        auto entry = readPropDataFromXml(elem.toElement(), error);

        if (!error) childs.push_back(std::move(entry));

        elem = elem.nextSiblingElement(propertyTag);
    }

    return childs;
}


GtObjectMemento::PropertyData
readStructPropertyEntry(const QDomElement& entryElem, bool& error)
{
    assert(entryElem.tagName() == GtObjectIO::S_PROPERTY_TAG);

    error = true;
    GtObjectMemento::PropertyData pd;

    const auto typeName = entryElem.attribute(GtObjectIO::S_TYPE_TAG);
    const auto name = entryElem.attribute(GtObjectIO::S_NAME_TAG);

    if (typeName.isEmpty())
    {
        gtError().noquote().nospace()
                << "Empty type in property container entry on line "
                  << entryElem.lineNumber();
        return pd;
    }

    if (name.isEmpty())
    {
        gtError().noquote().nospace()
                << "Empty name in property container entry on line "
                  << entryElem.lineNumber();
        return pd;
    }

    pd.toStruct(typeName);
    pd.name = name;

    pd.childProperties = readPropertyElements(readProperty,
                                              entryElem,
                                              GtObjectIO::S_PROPERTY_TAG);

    error = false;
    return pd;
}

GtObjectMemento::PropertyData
readPropertyContainer(const QDomElement& containerElem, bool& error)
{
    error = true;
    assert(containerElem.tagName() == GtObjectIO::S_PROPERTYCONT_TAG);

    GtObjectMemento::PropertyData pd;
    pd.name = containerElem.attribute(GtObjectIO::S_NAME_TAG);
    pd.childProperties = readPropertyElements(readStructPropertyEntry,
                                              containerElem,
                                              GtObjectIO::S_PROPERTY_TAG);


    error = false;
    return pd;
}

QVector<GtObjectMemento::PropertyData>
GtObjectIO::readPropertyContainers(const QDomElement& element) const
{
    assert(element.tagName() == S_OBJECT_TAG);

    return readPropertyElements(readPropertyContainer,
                                element,
                                GtObjectIO::S_PROPERTYCONT_TAG);
}

namespace
{

GtObjectMemento::PropertyData
readProperty(const QDomElement& element, bool& error)
{

    error = true;
    GtObjectMemento::PropertyData propData;

    QString fieldType = element.attribute(GtObjectIO::S_TYPE_TAG);
    QString fieldName = element.attribute(GtObjectIO::S_NAME_TAG);

    propData.name = fieldName;
    propData.setData(propertyToVariant(element.text(), fieldType));

    QString fieldActive = element.attribute(GtObjectIO::S_ACTIVE_TAG);
    if (!fieldActive.isEmpty())
    {
        propData.isOptional = true;
        propData.isActive = QVariant(fieldActive).toBool();
    }
    QString fieldOptional = element.attribute(GtObjectIO::S_OPTIONAL_TAG);
    if (!fieldOptional.isEmpty())
    {
        propData.isOptional = QVariant(fieldOptional).toBool();
    }

    error = false;
    return propData;
}

} // namespace


GtObjectMemento::PropertyData
GtObjectIO::toDynamicPropertyData(const GtPropertyStructContainer& vec) const
{
    GtObjectMemento::PropertyData val;

    val.name = vec.ident();

    for (size_t i = 0; i < vec.size(); ++i)
    {
        QSet<QString> stored;
        const GtAbstractProperty& child = vec.at(i);
        writePropertyHelper(val.childProperties, stored, &child);
    }


    return val;
}

void
GtObjectIO::writePropertyHelper(QVector<GtObjectMemento::PropertyData>& pVec,
                                QSet<QString>& stored,
                                const GtAbstractProperty* property) const
{
    assert(property != nullptr);

    if (stored.contains(property->ident()))
    {
        return;
    }

    bool isStructProperty = qobject_cast<const GtPropertyStructInstance*>(property) != nullptr;

    if (property->storeToMemento() && property->isValid())
    {
        // static property
        GtObjectMemento::PropertyData mprop;
        mprop.name = property->ident();
        mprop.isOptional = property->isOptional();
        mprop.isActive = property->isActive();

        if (!isStructProperty)
        {
            mprop.setData(property->valueToVariant());
        }
        else
        {
            mprop.toStruct(property->objectName());

            // process sub elements
            foreach (const GtAbstractProperty* pChild, property->properties())
            {
                writePropertyHelper(mprop.childProperties, stored, pChild);
            }
        }


        pVec.push_back(mprop);

        stored << property->ident();
    }

    if (!isStructProperty)
    {
        foreach (const GtAbstractProperty* pChild, property->fullProperties())
        {
            writePropertyHelper(pVec, stored, pChild);
        }
    }
}


QDomElement
GtObjectIO::toDomElement(
    const GtObjectMemento::PropertyData& property,
    QDomDocument& doc)
{
    QDomElement child;

    using PD = GtObjectMemento::PropertyData;

    switch(property.type())
    {
    case PD::ENUM_T:
        child = enumerationToDomElement(property.name, property.dataType(),
                                        property.data(), doc);
        break;
    case PD::STRUCT_T:
        child = propertyToDomElement(property.name, property.data(),
                                     property.dataType(), doc);


        for (const auto& subchild : property.childProperties)
        {
            child.appendChild(toDomElement(subchild, doc));
        }
        break;
    case PD::PropertyData::DATA_T:
        if (usePropertyList(property.data()))
        {
            child = propertyListToDomElement(property.name, property.data(),
                                             doc);
        }
        else
        {
            child = propertyToDomElement(property.name, property.data(),
                                         property.dataType(), doc);
        }
        break;
    }

    if (property.isOptional)
    {
        QVariant actVar = QVariant::fromValue(property.isActive);
        child.setAttribute(S_ACTIVE_TAG, actVar.toString());
    }

    return child;
}

QDomElement
GtObjectIO::dynamicSizePropToDomElement(
    const GtObjectMemento::PropertyData& property,
    QDomDocument& doc)
{
    QDomElement containerElement = doc.createElement("property-container");
    containerElement.setAttribute(S_NAME_TAG, property.name);

    // GTlab properties0
    foreach (const GtObjectMemento::PropertyData& childProperty,
             property.childProperties)
    {
        containerElement.appendChild(toDomElement(childProperty, doc));
    }

    return containerElement;
}

void
GtObjectIO::propertyListStringType(const QVariant& var, QString& valStr,
                                   QString& typeStr)
{
    static QVariant::Type type_QDoubleVector = QVariant::nameToType(
                "QVector<double>");
    static QVariant::Type type_QIntList = QVariant::nameToType(
                "QList<int>");
    static QVariant::Type type_QBoolList = QVariant::nameToType(
                "QList<bool>");
    static QVariant::Type type_QPointFList = QVariant::nameToType(
                "QList<QPointF>");
    static QVariant::Type type_QStringList = QVariant::nameToType(
                "QStringList");

    assert(type_QDoubleVector != QVariant::Invalid);
    assert(type_QIntList != QVariant::Invalid);
    assert(type_QBoolList != QVariant::Invalid);
    assert(type_QPointFList != QVariant::Invalid);
    assert(type_QStringList == QVariant::StringList);

    if (var.type() == type_QDoubleVector)
    {
        valStr = listToString(var.value<QVector<double> >());
        typeStr = QStringLiteral("double");
    }
    else if (var.type() == type_QIntList)
    {
        valStr = listToString(var.value<QList<int> >());
        typeStr = QStringLiteral("int");
    }
    else if (var.type() == type_QBoolList)
    {
        valStr = listToString(var.value<QList<bool> >());
        typeStr = QStringLiteral("bool");
    }
    else if (var.type() == type_QPointFList)
    {
        valStr = listToString(var.value<QList<QPointF> >());
        typeStr = QStringLiteral("QPointF");
    }
    else if (var.type() == type_QStringList)
    {
        valStr = listToString(var.value<QStringList>());
        typeStr = QStringLiteral("QString");
    }
    else
    {
        valStr = variantToString(var);
        typeStr = var.typeName();
    }
}

QDomElement
GtObjectIO::propertyToDomElement(const QString& name,
                                 const QVariant& var,
                                 const QString& dataType,
                                 QDomDocument& doc)
{
    QDomElement element = doc.createElement(S_PROPERTY_TAG);

    element.setAttribute(S_NAME_TAG, name);
    element.setAttribute(S_TYPE_TAG, dataType);

    if (!var.isNull())
    {
        QDomText t = doc.createTextNode(variantToString(var));
        element.appendChild(t);
    }

    return element;
}

QDomElement
GtObjectIO::enumerationToDomElement(const QString& name,
                                    const QString& typeName,
                                    const QVariant& var,
                                    QDomDocument& doc)
{
    QDomElement element = doc.createElement(S_PROPERTY_TAG);

    element.setAttribute(S_NAME_TAG, name);
    element.setAttribute(S_TYPE_TAG, typeName);

    QDomText t = doc.createTextNode(var.toString());
    element.appendChild(t);

    return element;
}

QDomElement
GtObjectIO::propertyListToDomElement(const QString& name,
                                     const QVariant& var,
                                     QDomDocument& doc)
{
    QDomElement element = doc.createElement(S_PROPERTYLIST_TAG);
    element.setAttribute(S_NAME_TAG, name);

    QString varStr, varType;

    propertyListStringType(var, varStr, varType);

    element.setAttribute(S_TYPE_TAG, varType);

    QDomText t = doc.createTextNode(varStr);
    element.appendChild(t);

    return element;
}

namespace
{

QVariant
propertyToVariant(const QString& value, const QString& type)
{

    std::string str = type.toStdString();
    const char* p = str.c_str();

    QVariant::Type v_type = QVariant::nameToType(p);
    QVariant retval(value);

    if (v_type != QVariant::Invalid)
    {
        retval.convert(v_type);
    }

    return retval;
}

QVariant
propertyListToVariant(const QString& value, const QString& type)
{
    QVariant var;

    if (type == QStringLiteral("double"))
    {
        QVector<QStringRef> strList =
                QStringRef(&value).split(';', Qt::SkipEmptyParts);
        QVector<double> list;
        list.reserve(strList.size());

        foreach (const auto& val, strList)
        {
            list.append(val.toDouble());
        }

        var.setValue(list);
    }
    else if (type == QStringLiteral("bool"))
    {
        QVector<QStringRef> strList =
                QStringRef(&value).split(';', Qt::SkipEmptyParts);
        QList<bool> list;
        list.reserve(strList.size());

        foreach (const auto& val, strList)
        {
            list.append(QVariant(val.toString()).toBool());
        }

        var.setValue(list);
    }
    else if (type == QStringLiteral("QString") ||
             type == QStringLiteral("string"))
    {
        QStringList list = value.split(QStringLiteral(";"));
        var.setValue(list);
    }
    else if (type == QStringLiteral("QPointF"))
    {
        QList<QPointF> list;
        QStringList values = value.split(QStringLiteral(";"));

        foreach (const QString& val, values)
        {
            QStringList pvars = val.split(QStringLiteral("_"));

            if (pvars.size() == 2)
            {
                list << QPointF(pvars[0].toDouble(), pvars[1].toDouble());
            }
        }

        var.setValue(list);
    }

    return var;
}

bool applyDiffQProperty(QObject* target, const QDomElement& change, bool isPropList, DiffMode mode)
{
    QString propType = change.attribute(GtObjectIO::S_TYPE_TAG);
    QString propName = change.attribute(GtObjectIO::S_NAME_TAG);

    if (propType.isEmpty() || propName.isEmpty())
    {
        return false;
    }

    QDomElement newValNode = change.firstChildElement(GtObjectIO::S_DIFF_NEWVAL_TAG);
    QDomElement oldValNode = change.firstChildElement(GtObjectIO::S_DIFF_OLDVAL_TAG);

    if (newValNode.isNull() || oldValNode.isNull())
    {
        return false;
    }

    QString newVal = mode==DiffMode::Revert ? oldValNode.text() : newValNode.text();

    if (target->property(propName.toLatin1()) != newVal)
    {
        if (!isPropList)
        {
            return target->setProperty(propName.toLatin1(),
                                          propertyToVariant(newVal,
                                                            propType));
        }
        else
        {
            return target->setProperty(propName.toLatin1(),
                                          propertyListToVariant(newVal,
                                                                propType));
        }
    }

    return true;
}

bool
applyDiffProperty(GtAbstractProperty* prop, const QDomElement& propDiffElem,
                  bool isPropertyList, DiffMode mode)
{
    QString propType = propDiffElem.attribute(GtObjectIO::S_TYPE_TAG);

    if (propType.isEmpty())
    {
        return false;
    }


    QDomElement newValNode = propDiffElem.firstChildElement(GtObjectIO::S_DIFF_NEWVAL_TAG);
    QDomElement oldValNode = propDiffElem.firstChildElement(GtObjectIO::S_DIFF_OLDVAL_TAG);

    if (newValNode.isNull() || oldValNode.isNull())
    {
        return false;
    }

    const bool revert = mode == DiffMode::Revert;
    QString newVal = revert ? oldValNode.text() : newValNode.text();

    if (!isPropertyList)
    {
        prop->setValueFromVariant(propertyToVariant(newVal, propType));
    }
    else
    {
        prop->setValueFromVariant(propertyListToVariant(newVal, propType));
    }

    QDomNodeList attrChanges = propDiffElem.childNodes();
    QList<QDomElement> attrChangeElements;

    for (int i = 0; i < attrChanges.count(); i++)
    {
        QDomElement element = attrChanges.at(i).toElement();

        if (element.nodeName() == GtObjectIO::S_DIFF_ATTR_CHANGE_TAG)
        {
            attrChangeElements.append(element);
        }
    }

    foreach (QDomElement attrChange, attrChangeElements)
    {
        QString newAttrVal;
        QString oldAttrVal;
        QString attrID = attrChange.attribute(GtObjectIO::S_ID_TAG);

        if (attrID.isEmpty())
        {
            return false;
        }

        QDomElement oldAttrElement =
            attrChange.firstChildElement(GtObjectIO::S_DIFF_OLDVAL_TAG);
        QDomElement newAttrElement =
            attrChange.firstChildElement(GtObjectIO::S_DIFF_NEWVAL_TAG);

        if (oldAttrElement.isNull() || newAttrElement.isNull())
        {
            return false;
        }

        if (revert)
        {
            newAttrVal = oldAttrElement.text();
            oldAttrVal = newAttrElement.text();
        }
        else
        {
            newAttrVal = newAttrElement.text();
            oldAttrVal = oldAttrElement.text();
        }

        if (attrID == GtObjectIO::S_ACTIVE_TAG)
        {
            if (newAttrVal == QLatin1String("true"))
            {
                prop->setActive(true);
            }
            else if (newAttrVal == QLatin1String("false"))
            {
                prop->setActive(false);
            }
            else
            {
                gtDebug() << "Property change makes no sense --> aborting!";
                return false;
            }
        }
    }
    return true;
}

bool
// This must be non-const, this is a false positive
// cppcheck-suppress constParameter
handlePropContEntryChanged(GtObject& parentObject,
                           const QDomElement& propContElemC, DiffMode mode)
{

    QString containerName = propContElemC.attribute(GtObjectIO::S_NAME_TAG);

    if (containerName.isEmpty())
    {
        gtError() << "Empty container name in memento diff. Cannot apply";
        return false;
    }

    auto container = parentObject.findDynamicSizeProperty(containerName);
    if (!container)
    {
        gtError() << "Invalid container name in memento diff. Cannot apply";
        return false;
    }

    QString entryId = propContElemC.attribute(GtObjectIO::S_ENTRY_NAME_TAG);

    auto entry = container->findEntry(entryId);
    if (!entry)
    {
        gtError() << "Invalid entryName value in memento diff";
        return false;
    }

    auto handlePropertyNodeChangeInStruct = [mode, &entry]
        (const QDomElement& elem, bool isList)
    {
        QString propName = elem.attribute(GtObjectIO::S_NAME_TAG);
        if (propName.isEmpty())
        {
            gtError() << "Empty property name in memento diff";
            return;
        }

        auto* prop = entry->findProperty(propName);
        if (prop)
            applyDiffProperty(prop, elem, isList, mode);
        else
            gtError() << "Property " << propName << "not found in property container entry";
    };

    // handle prop changes
    QDomElement propC = propContElemC.firstChildElement(GtObjectIO::S_DIFF_PROP_CHANGE_TAG);
    while (!propC.isNull())
    {
        handlePropertyNodeChangeInStruct(propC, /*isList*/ false);
        propC = propC.nextSiblingElement(GtObjectIO::S_DIFF_PROP_CHANGE_TAG);
    }

    // handle prop list changes
    QDomElement propLC =
        propContElemC.firstChildElement(GtObjectIO::S_DIFF_PROPLIST_CHANGE_TAG);
    while (!propLC.isNull())
    {
        handlePropertyNodeChangeInStruct(propLC, /*isList*/ true);
        propLC = propLC.nextSiblingElement(GtObjectIO::S_DIFF_PROPLIST_CHANGE_TAG);
    }

    return true;
}

bool
handlePropContEntryAddRemove(GtObject& parentObject,
                             const QDomElement& propContElemC, DiffMode mode)
{
    QString containerName = propContElemC.attribute(GtObjectIO::S_NAME_TAG);

    if (containerName.isEmpty())
    {
        gtError() << "Empty container name in memento diff. Cannot apply";
        return false;
    }

    auto container = parentObject.findDynamicSizeProperty(containerName);
    if (!container)
    {
        gtError() << "Invalid container name in memento diff. Cannot apply";
        return false;
    }



    return true;
}

bool
handlePropertyNodeChange(GtObject* target,
                         const QDomElement &change,
                         const bool list,
                         DiffMode mode)
{
    QString propName = change.attribute(GtObjectIO::S_NAME_TAG);

    if (propName.isEmpty())
    {
        return false;
    }

    GtAbstractProperty* prop = target->findProperty(propName);

    // ------------------------------------------------------- //

    if (prop)
    {
        return applyDiffProperty(prop, change, list, mode);
    }
    else
    {
        return applyDiffQProperty(target, change, list, mode);
    }
}

bool
handleAttributeNodeChange(GtObject* target,
                          const QDomElement& change,
                          DiffMode mode)
{
    QDomElement newValNode = change.firstChildElement(GtObjectIO::S_DIFF_NEWVAL_TAG);
    QDomElement oldValNode = change.firstChildElement(GtObjectIO::S_DIFF_OLDVAL_TAG);

    if (newValNode.isNull() || oldValNode.isNull())
    {
        return false;
    }

    QString newVal = mode == DiffMode::Revert ? oldValNode.text()
                                              : newValNode.text();

    QString attrID = change.attribute(GtObjectIO::S_ID_TAG);

    if (attrID == GtObjectIO::S_NAME_TAG)
    {
        target->setObjectName(newVal);
    }

    return true;
}

bool
handleObjectAdd(GtObject* parent,
                const QDomElement& objectToAdd,
                const QString& index)
{
    bool ok = true;
    int ind = index.toInt(&ok);

    if (!ok)
    {
        gtDebug() << "Index conversion failed!";
        return false;
    }

    if (objectToAdd.isNull())
    {
        gtDebug() << "Object to add is Null";
        return false;
    }

    GtObjectFactory* factory = GtObjectFactory::instance();
    GtObjectIO oio(factory);

    auto memento = GtObjectMemento(oio.toMemento(objectToAdd));
    auto newObj = memento.toObject(*factory);


    if (!newObj)
    {
        gtDebug() << "Could not restore object from memento";
        return false;
    }

    int numberOfChildren = parent->findDirectChildren<GtObject*>().size();

    if (ind >= numberOfChildren)
    {
        parent->appendChild(newObj.release());
    }
    else if (ind >= 0)
    {
        parent->insertChild(ind, newObj.release());
    }
    else
    {
        gtDebug() << "INDEX:" << ind;
        gtDebug() << "INDEX NOT IN RANGE -> CANNOT ADD OBJECT!";
        return false;
    }

    return true;
}

bool
handleObjectRemove(GtObject* parent,
                   const QDomElement& objectToRemove,
                   const QString& /*index*/)
{
    if (objectToRemove.isNull())
    {
        qDebug() << "Object to remove is Null";
        return false;
    }

    const QString objUUID = objectToRemove.attribute(GtObjectIO::S_UUID_TAG);

    if (objUUID.isEmpty())
    {
        qDebug() << "No uuid found!";
        return false;
    }

    GtObject* toRemove = parent->getDirectChildByUuid(objUUID);

    if (!toRemove)
    {
        qDebug() << "object not found! (" << objUUID << ")";
        qDebug() << "   |-> " << parent->objectName() << " [" <<
                    parent->uuid() << "]";
        return false;
    }

    delete toRemove;

    return true;
}

bool handleObjectAddRemove(GtObject* target,
                           const QDomElement& diffTag,
                           DiffMode mode)
{
    assert(diffTag.tagName() == GtObjectIO::S_DIFF_OBJ_ADD_TAG ||
           diffTag.tagName() == GtObjectIO::S_DIFF_OBJ_REMOVE_TAG);

    auto objectTag = diffTag.firstChildElement(GtObjectIO::S_OBJECT_TAG);
    auto index = diffTag.attribute(GtObjectIO::S_DIFF_INDEX_TAG);
    bool doAdd = diffTag.tagName() == GtObjectIO::S_DIFF_OBJ_ADD_TAG;

    if (mode == DiffMode::Revert) doAdd = !doAdd;

    if (doAdd)
    {
        return handleObjectAdd(target, objectTag, index);
    }
    else
    {
        return handleObjectRemove(target, objectTag, index);
    }
}

bool
handleIndexChange(GtObject* parent,
                  const QDomElement& indChanges, DiffMode mode)
{
    if (indChanges.isNull())
    {
        return false;
    }

    if (!parent)
    {
        return false;
    }

    bool ok = true;

    auto tag = mode == DiffMode::Apply ? GtObjectIO::S_DIFF_NEWVAL_TAG
                                       : GtObjectIO::S_DIFF_OLDVAL_TAG;

    int newIndex = indChanges.attribute(tag).toInt(&ok);
    if (!ok)
    {
        return false;
    }

    auto object = indChanges.firstChildElement(GtObjectIO::S_OBJECT_TAG);

    if (object.isNull())
    {
        return false;
    }

    GtObject* toMove = parent->getDirectChildByUuid(
                object.attribute(GtObjectIO::S_UUID_TAG));

    if (!toMove)
    {
        return false;
    }

    toMove->setParent(nullptr);

    if (newIndex > parent->findDirectChildren<GtObject*>().size())
    {
        parent->appendChild(toMove);
    }
    else
    {
        parent->insertChild(newIndex, toMove);
    }

    return true;
}

} // namespace
