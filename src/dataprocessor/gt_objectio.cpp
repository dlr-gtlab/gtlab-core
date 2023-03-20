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
#include <QMetaType>

#include <cfloat>
#include <typeinfo>

#include "gt_object.h"
#include "gt_externalizedobject.h"
#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"
#include "gt_abstractproperty.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"
#include "gt_exceptions.h"
#include "gt_xmlutilities.h"
#include "gt_xmlexpr.h"

#include "gt_objectio.h"

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
    handleIndexChange(GtObject& parent,
                      const QDomElement& indChanges, DiffMode mode);

    bool
    handlePropertyNodeChange(GtObject& target,
                             const QDomElement &change,
                             const bool list,
                             DiffMode mode);

    bool
    handleAttributeNodeChange(GtObject& target,
                              const QDomElement& change,
                              DiffMode mode);

    bool
    handleObjectAddRemove(GtObject& target,
                          const QDomElement& diffTag,
                          DiffMode mode);

    bool
    handlePropContEntryAddRemove(GtObject& parentObject,
                                 const QDomElement& diffTag,
                                 DiffMode mode);

    using diffFunc = std::function<bool(GtObject&, const QDomElement&, DiffMode)>;
    std::map<QString, diffFunc> const & getDiffFuncs()
    {
        static std::map<QString, diffFunc> funcMap =
            {
             std::make_pair(gt::xml::S_DIFF_OBJ_REMOVE_TAG,
                            handleObjectAddRemove),
             std::make_pair(gt::xml::S_DIFF_OBJ_ADD_TAG,
                            handleObjectAddRemove),
             std::make_pair(gt::xml::S_DIFF_INDEX_CHANGED_TAG,
                            handleIndexChange),
             std::make_pair(gt::xml::S_DIFF_ATTR_CHANGE_TAG,
                            handleAttributeNodeChange),
             std::make_pair(gt::xml::S_DIFF_PROPCONT_ENTRY_CHANGE_TAG,
                            handlePropContEntryChanged),
             std::make_pair(gt::xml::S_DIFF_PROPCONT_ENTRY_ADDED_TAG,
                            handlePropContEntryAddRemove),
             std::make_pair(gt::xml::S_DIFF_PROPCONT_ENTRY_REMOVE_TAG,
                            handlePropContEntryAddRemove),

             std::make_pair(gt::xml::S_DIFF_PROP_CHANGE_TAG,
                [](GtObject& obj, const QDomElement& tag , DiffMode mode) {
                    return  handlePropertyNodeChange(obj, tag, false,
                                                    mode);
                }),

             std::make_pair(gt::xml::S_DIFF_PROPLIST_CHANGE_TAG,
                [](GtObject& obj, const QDomElement& tag , DiffMode mode) {
                    return  handlePropertyNodeChange(obj, tag, true,
                                                    mode);
                }),
             };

        return funcMap;
    }
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

    for (QPointF const& m : t)
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


    // class name
    if (o->isDummy())
    {
        o->exportDummyIntoMemento(memento);
    }
    else
    {
        memento.setClassName(o->metaObject()->className());
        // store property information
        writeProperties(memento, o);
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

    // child objects
    auto const directChildren = o->findDirectChildren();
    memento.childObjects.reserve(directChildren.size());

    for (const GtObject* child : directChildren)
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
    QDomElement element = doc.createElement(gt::xml::S_OBJECT_TAG);

    // class name
    element.setAttribute(gt::xml::S_CLASS_TAG, memento.className());

    // uuid
    element.setAttribute(gt::xml::S_UUID_TAG, memento.uuid());

    // object name
    element.setAttribute(gt::xml::S_NAME_TAG, memento.ident());

    // store property information
    writeProperties(doc, element, memento);

    // children
    if (!skipChildren && ! memento.childObjects.isEmpty())
    {
        QDomElement childElement =
            doc.createElement(gt::xml::S_OBJECTLIST_TAG);

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
        .setClassName(e.attribute(gt::xml::S_CLASS_TAG))
        .setUuid(e.attribute(gt::xml::S_UUID_TAG))
        .setIdent(e.attribute(gt::xml::S_NAME_TAG));

    // store property information
    memento.properties = readProperties(e);
    memento.propertyContainers = readPropertyContainers(e);

    // child objects
    QDomElement children = e.firstChildElement(gt::xml::S_OBJECTLIST_TAG);

    if (!children.isNull())
    {
        QDomElement compElement =
                children.firstChildElement(gt::xml::S_OBJECT_TAG);

        while (!compElement.isNull())
        {
            // recursion through GtObjectMemento constructor
            memento.childObjects.push_back(GtObjectMemento(compElement));

            compElement = compElement.nextSiblingElement(gt::xml::S_OBJECT_TAG);
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

    bool okay = true;

    const auto& diffFuncs = getDiffFuncs();

    QDomElement diffTag = parent.firstChildElement();

    if (diffTag.isNull())
    {
        return okay;
    }

    // object must be fetched when applying/reverting diff
    if (auto* ext = qobject_cast<GtExternalizedObject*>(parentObject))
    {
        ext->internalize();
    }

    do
    {
        auto iter = diffFuncs.find(diffTag.tagName());

        if (iter != diffFuncs.end())
        {
            // call diff function
            okay = okay && iter->second(*parentObject, diffTag, mode);
        }

        diffTag = diffTag.nextSiblingElement();
    }
    while (!diffTag.isNull());

    return okay;
}

bool
GtObjectIO::applyDiff(GtObjectMementoDiff& diff, GtObject* obj)
{
    if (diff.isNull())
    {
        return true;
    }

    QDomElement parent = diff.documentElement();
    if (!parent.isNull() && parent.tagName() != gt::xml::S_OBJECT_TAG)
    {
        parent = parent.nextSiblingElement(gt::xml::S_OBJECT_TAG);
    }

    while (!parent.isNull())
    {
        const QString parentUUID = parent.attribute(gt::xml::S_UUID_TAG);

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

        parent = parent.nextSiblingElement(gt::xml::S_OBJECT_TAG);
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
    if (!parent.isNull() && parent.tagName() != gt::xml::S_OBJECT_TAG)
    {
        parent = parent.nextSiblingElement(gt::xml::S_OBJECT_TAG);
    }

    while (!parent.isNull())
    {
        const QString parentUUID = parent.attribute(gt::xml::S_UUID_TAG);

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

        parent = parent.nextSiblingElement(gt::xml::S_OBJECT_TAG);
    }

    return true;
}

void
GtObjectIO::writeProperties(GtObjectMemento& memento,
                            const GtObject* obj)
{
    const QMetaObject* meta = obj->metaObject();

    // static properties
    QList<GtAbstractProperty const*> const props = obj->properties();
    QSet<QString> storedProps;

    // GTlab properties
    for (GtAbstractProperty const* property : props)
    {
        writePropertyHelper(memento.properties, storedProps, property);
    }

    // dynamic properties
    for (const GtPropertyStructContainer& c: obj->propertyContainers())
    {
        auto propertyContainerData = toPropertyContainerData(c);
        memento.propertyContainers.append(std::move(propertyContainerData));
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
        if (QString(prop.name()) == QStringLiteral("objectName"))
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
             memento.propertyContainers)
    {
        root.appendChild(propertyContainerDataToDomElement(property, doc));
    }
}

QVector<GtObjectMemento::PropertyData>
GtObjectIO::readProperties(const QDomElement& element) const
{
    QVector<GtObjectMemento::PropertyData> properties;

    QDomElement propElement = element.firstChildElement();

    while (!propElement.isNull())
    {
        if (propElement.tagName() == gt::xml::S_PROPERTY_TAG)
        {
            bool error = false;
            GtObjectMemento::PropertyData propData = readProperty(propElement,
                                                                  error);

            if (!error) properties.push_back(propData);
        }
        else if (propElement.tagName() == gt::xml::S_PROPERTYLIST_TAG)
        {
            QString fieldType = propElement.attribute(gt::xml::S_TYPE_TAG);
            QString fieldName = propElement.attribute(gt::xml::S_NAME_TAG);

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
    assert(entryElem.tagName() == gt::xml::S_PROPERTY_TAG);

    error = true;
    GtObjectMemento::PropertyData pd;

    const auto typeName = entryElem.attribute(gt::xml::S_TYPE_TAG);
    const auto name = entryElem.attribute(gt::xml::S_NAME_TAG);

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
                                              gt::xml::S_PROPERTY_TAG);

    error = false;
    return pd;
}

GtObjectMemento::PropertyData
readPropertyContainer(const QDomElement& containerElem, bool& error)
{
    error = true;
    assert(containerElem.tagName() == gt::xml::S_PROPERTYCONT_TAG);

    GtObjectMemento::PropertyData pd;
    pd.name = containerElem.attribute(gt::xml::S_NAME_TAG);
    pd.childProperties = readPropertyElements(readStructPropertyEntry,
                                              containerElem,
                                              gt::xml::S_PROPERTY_TAG);


    error = false;
    return pd;
}

QVector<GtObjectMemento::PropertyData>
GtObjectIO::readPropertyContainers(const QDomElement& element) const
{
    assert(element.tagName() == gt::xml::S_OBJECT_TAG);

    return readPropertyElements(readPropertyContainer,
                                element,
                                gt::xml::S_PROPERTYCONT_TAG);
}

namespace
{

GtObjectMemento::PropertyData
readProperty(const QDomElement& element, bool& error)
{

    error = true;
    GtObjectMemento::PropertyData propData;

    QString fieldType = element.attribute(gt::xml::S_TYPE_TAG);
    QString fieldName = element.attribute(gt::xml::S_NAME_TAG);

    propData.name = fieldName;
    propData.setData(propertyToVariant(element.text(), fieldType));

    QString fieldActive = element.attribute(gt::xml::S_ACTIVE_TAG);
    if (!fieldActive.isEmpty())
    {
        propData.isActive = QVariant(fieldActive).toBool();
    }

    error = false;
    return propData;
}

} // namespace


GtObjectMemento::PropertyData
GtObjectIO::toPropertyContainerData(const GtPropertyStructContainer& vec) const
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

    // only write active, if active == false, otherwise assume true
    if (!property.isActive)
    {
        child.setAttribute(gt::xml::S_ACTIVE_TAG, QVariant(false).toString());
    }
    else if (child.hasAttribute(gt::xml::S_ACTIVE_TAG))
    {
        child.removeAttribute(gt::xml::S_ACTIVE_TAG);
    }


    return child;
}

QDomElement
GtObjectIO::propertyContainerDataToDomElement(
    const GtObjectMemento::PropertyData& property,
    QDomDocument& doc)
{
    QDomElement containerElement = doc.createElement("property-container");
    containerElement.setAttribute(gt::xml::S_NAME_TAG, property.name);

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
    return gt::xml::createPropertyElement(doc, name, dataType,
                                          variantToString(var));
}

QDomElement
GtObjectIO::enumerationToDomElement(const QString& name,
                                    const QString& typeName,
                                    const QVariant& var,
                                    QDomDocument& doc)
{
    QDomElement element = doc.createElement(gt::xml::S_PROPERTY_TAG);

    element.setAttribute(gt::xml::S_NAME_TAG, name);
    element.setAttribute(gt::xml::S_TYPE_TAG, typeName);

    QDomText t = doc.createTextNode(var.toString());
    element.appendChild(t);

    return element;
}

QDomElement
GtObjectIO::propertyListToDomElement(const QString& name,
                                     const QVariant& var,
                                     QDomDocument& doc)
{
    QDomElement element = doc.createElement(gt::xml::S_PROPERTYLIST_TAG);
    element.setAttribute(gt::xml::S_NAME_TAG, name);

    QString varStr, varType;

    propertyListStringType(var, varStr, varType);

    element.setAttribute(gt::xml::S_TYPE_TAG, varType);

    QDomText t = doc.createTextNode(varStr);
    element.appendChild(t);

    return element;
}

namespace
{

QVariant
propertyToVariant(const QString& value, const QString& type)
{

    if (type.isEmpty() && value.trimmed().isEmpty())
    {
        return QVariant{};
    }

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
        if (value.isEmpty())
        {
            return QVariant(QStringList{});
        }

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

bool applyDiffQProperty(QObject& target, const QDomElement& change, bool isPropList, DiffMode mode)
{
    QString propType = change.attribute(gt::xml::S_TYPE_TAG);
    QString propName = change.attribute(gt::xml::S_NAME_TAG);

    if (propType.isEmpty() || propName.isEmpty())
    {
        return false;
    }

    QDomElement newValNode = change.firstChildElement(gt::xml::S_DIFF_NEWVAL_TAG);
    QDomElement oldValNode = change.firstChildElement(gt::xml::S_DIFF_OLDVAL_TAG);

    if (newValNode.isNull() || oldValNode.isNull())
    {
        return false;
    }

    QString newVal = mode==DiffMode::Revert ? oldValNode.text() : newValNode.text();

    if (target.property(propName.toLatin1()) != newVal)
    {
        if (!isPropList)
        {
            return target.setProperty(propName.toLatin1(),
                                          propertyToVariant(newVal,
                                                            propType));
        }
        else
        {
            return target.setProperty(propName.toLatin1(),
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
    QString propType = propDiffElem.attribute(gt::xml::S_TYPE_TAG);

    if (propType.isEmpty())
    {
        return false;
    }


    QDomElement newValNode =
            propDiffElem.firstChildElement(gt::xml::S_DIFF_NEWVAL_TAG);
    QDomElement oldValNode =
            propDiffElem.firstChildElement(gt::xml::S_DIFF_OLDVAL_TAG);

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

        if (element.nodeName() == gt::xml::S_DIFF_ATTR_CHANGE_TAG)
        {
            attrChangeElements.append(element);
        }
    }

    foreach (QDomElement attrChange, attrChangeElements)
    {
        QString newAttrVal;
        QString oldAttrVal;
        QString attrID = attrChange.attribute(gt::xml::S_ID_TAG);

        if (attrID.isEmpty())
        {
            return false;
        }

        QDomElement oldAttrElement =
            attrChange.firstChildElement(gt::xml::S_DIFF_OLDVAL_TAG);
        QDomElement newAttrElement =
            attrChange.firstChildElement(gt::xml::S_DIFF_NEWVAL_TAG);

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

        if (attrID == gt::xml::S_ACTIVE_TAG)
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
handlePropContEntryChanged(GtObject& parentObject,
                           const QDomElement& propContElemC, DiffMode mode)
{

    QString containerName = propContElemC.attribute(gt::xml::S_NAME_TAG);

    if (containerName.isEmpty())
    {
        gtError() << "Empty container name in memento diff. Cannot apply";
        return false;
    }

    auto container = parentObject.findPropertyContainer(containerName);
    if (!container)
    {
        gtError() << "Invalid container name in memento diff. Cannot apply";
        return false;
    }

    QString entryId = propContElemC.attribute(gt::xml::S_ENTRY_NAME_TAG);

    auto entry = container->findEntry(entryId);
    if (entry == container->end())
    {
        gtError() << "Invalid entryName value in memento diff";
        return false;
    }

    auto handlePropertyNodeChangeInStruct = [mode, &entry]
        (const QDomElement& elem, bool isList)
    {
        QString propName = elem.attribute(gt::xml::S_NAME_TAG);
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
    QDomElement propC =
            propContElemC.firstChildElement(gt::xml::S_DIFF_PROP_CHANGE_TAG);
    while (!propC.isNull())
    {
        handlePropertyNodeChangeInStruct(propC, /*isList*/ false);
        propC = propC.nextSiblingElement(gt::xml::S_DIFF_PROP_CHANGE_TAG);
    }

    // handle prop list changes
    QDomElement propLC =
        propContElemC.firstChildElement(gt::xml::S_DIFF_PROPLIST_CHANGE_TAG);
    while (!propLC.isNull())
    {
        handlePropertyNodeChangeInStruct(propLC, /*isList*/ true);
        propLC = propLC.nextSiblingElement(gt::xml::S_DIFF_PROPLIST_CHANGE_TAG);
    }

    return true;
}

bool handlePropContEntryAdd(const QDomElement& diffTag,
                            GtPropertyStructContainer& container)
{

    QDomElement structElement =
        diffTag.firstChildElement(gt::xml::S_PROPERTY_TAG);

    if (structElement.isNull())
    {
        return true;
    }

    auto uuid = structElement.attribute(gt::xml::S_NAME_TAG);

    // check of object element already included
    auto iter = container.findEntry(uuid);
    if (iter != container.end())
    {
        return false;
    }

    auto typeId = structElement.attribute(gt::xml::S_TYPE_TAG);
    auto idxStr = diffTag.attribute(gt::xml::S_DIFF_INDEX_TAG);

    bool okay = false;
    auto idx = idxStr.toInt(&okay);

    if (!okay || idx < 0 || idx > static_cast<long>(container.size()))
        return false; // index cannot be parsed

    auto pos = container.begin();
    std::advance(pos, idx);

    // convert xml to memento
    bool error = true;
    auto pd = readStructPropertyEntry(structElement, error);
    if (error)
        return false;

    try
    {
        // might throw due to unknown typeid
        auto& entry = container.newEntry(typeId, pos, uuid);

        // import memento to struct
        gt::importStructEntryFromMemento(pd, entry);
    }
    catch(GTlabException& e)
    {
        gtError() << e.what();
    }

    return true;
}

bool handlePropContEntryRemove(const QDomElement& diffTag,
                               GtPropertyStructContainer& container)
{
    QDomElement structElement =
        diffTag.firstChildElement(gt::xml::S_PROPERTY_TAG);

    if (structElement.isNull())
    {
        return true;
    }

    auto uuid = structElement.attribute(gt::xml::S_NAME_TAG);

    // remove
    if (uuid.isEmpty())
    {
        return false;
    }

    auto iter = container.findEntry(uuid);
    if (iter == container.end())
    {
        // uuid not in container
        return false;
    }

    container.removeEntry(iter);

    return true;
}

bool
handlePropContEntryAddRemove(GtObject& parentObject,
                             const QDomElement& diffTag, DiffMode mode)
{
    assert(diffTag.tagName() == gt::xml::S_DIFF_PROPCONT_ENTRY_ADDED_TAG ||
           diffTag.tagName() == gt::xml::S_DIFF_PROPCONT_ENTRY_REMOVE_TAG);

    const QString containerName = diffTag.attribute(gt::xml::S_NAME_TAG);

    if (containerName.isEmpty())
    {
        gtError() << "Empty container name in memento diff. Cannot apply";
        return false;
    }

    auto container = parentObject.findPropertyContainer(containerName);
    if (!container)
    {
        gtError() << "Invalid container name in memento diff. Cannot apply";
        return false;
    }

    const bool doAdd =
        (diffTag.tagName() == gt::xml::S_DIFF_PROPCONT_ENTRY_ADDED_TAG) !=
        (mode == DiffMode::Revert);

    if (doAdd)
        return handlePropContEntryAdd(diffTag, *container);
    else
        return handlePropContEntryRemove(diffTag, *container);
}

bool
handlePropertyNodeChange(GtObject& target,
                         const QDomElement &change,
                         const bool list,
                         DiffMode mode)
{
    QString propName = change.attribute(gt::xml::S_NAME_TAG);

    if (propName.isEmpty())
    {
        return false;
    }

    GtAbstractProperty* prop = target.findProperty(propName);

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
handleAttributeNodeChange(GtObject& target,
                          const QDomElement& change,
                          DiffMode mode)
{
    QDomElement newValNode =
            change.firstChildElement(gt::xml::S_DIFF_NEWVAL_TAG);
    QDomElement oldValNode =
            change.firstChildElement(gt::xml::S_DIFF_OLDVAL_TAG);

    if (newValNode.isNull() || oldValNode.isNull())
    {
        return false;
    }

    QString newVal = mode == DiffMode::Revert ? oldValNode.text()
                                              : newValNode.text();

    QString attrID = change.attribute(gt::xml::S_ID_TAG);

    if (attrID == gt::xml::S_NAME_TAG)
    {
        target.setObjectName(newVal);
    }

    return true;
}

bool
handleObjectAdd(GtObject& parent,
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

    int numberOfChildren = parent.findDirectChildren().size();

    if (ind >= numberOfChildren)
    {
        parent.appendChild(newObj.release());
    }
    else if (ind >= 0)
    {
        parent.insertChild(ind, newObj.release());
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
handleObjectRemove(GtObject& parent,
                   const QDomElement& objectToRemove,
                   const QString& /*index*/)
{
    if (objectToRemove.isNull())
    {
        qDebug() << "Object to remove is Null";
        return false;
    }

    const QString objUUID = objectToRemove.attribute(gt::xml::S_UUID_TAG);

    if (objUUID.isEmpty())
    {
        qDebug() << "No uuid found!";
        return false;
    }

    GtObject* toRemove = parent.getDirectChildByUuid(objUUID);

    if (!toRemove)
    {
        qDebug() << "object not found! (" << objUUID << ")";
        qDebug() << "   |-> " << parent.objectName() << " [" <<
                    parent.uuid() << "]";
        return false;
    }

    delete toRemove;

    return true;
}

bool
handleObjectAddRemove(GtObject& target,
                      const QDomElement& diffTag,
                      DiffMode mode)
{
    assert(diffTag.tagName() == gt::xml::S_DIFF_OBJ_ADD_TAG ||
           diffTag.tagName() == gt::xml::S_DIFF_OBJ_REMOVE_TAG);

    const auto objectTag = diffTag.firstChildElement(gt::xml::S_OBJECT_TAG);
    const auto index = diffTag.attribute(gt::xml::S_DIFF_INDEX_TAG);

    const bool doAdd =
        (diffTag.tagName() == gt::xml::S_DIFF_OBJ_ADD_TAG) !=
        (mode == DiffMode::Revert);

    if (doAdd)
        return handleObjectAdd(target, objectTag, index);
    else
        return handleObjectRemove(target, objectTag, index);
}

bool
handleIndexChange(GtObject& parent,
                  const QDomElement& indChanges, DiffMode mode)
{
    if (indChanges.isNull())
    {
        return false;
    }

    bool ok = true;

    auto tag = mode == DiffMode::Apply ? gt::xml::S_DIFF_NEWVAL_TAG
                                       : gt::xml::S_DIFF_OLDVAL_TAG;

    int newIndex = indChanges.attribute(tag).toInt(&ok);
    if (!ok)
    {
        return false;
    }

    auto object = indChanges.firstChildElement(gt::xml::S_OBJECT_TAG);

    if (object.isNull())
    {
        return false;
    }

    GtObject* toMove = parent.getDirectChildByUuid(
                object.attribute(gt::xml::S_UUID_TAG));

    if (!toMove)
    {
        return false;
    }

    toMove->setParent(nullptr);

    if (newIndex > parent.findDirectChildren().size())
    {
        parent.appendChild(toMove);
    }
    else
    {
        parent.insertChild(newIndex, toMove);
    }

    return true;
}

} // namespace
