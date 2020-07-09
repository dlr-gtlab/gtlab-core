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
#include "gt_logging.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"
#include "gt_abstractproperty.h"
#include "gt_dynamicpropertycontainer.h"
#include "gt_propertyfactory.h"

#include "gt_objectio.h"

const QString GtObjectIO::S_OBJECT_TAG = QStringLiteral("object");
const QString GtObjectIO::S_OBJECTLIST_TAG = QStringLiteral("objectlist");
const QString GtObjectIO::S_UUID_TAG = QStringLiteral("uuid");
const QString GtObjectIO::S_CLASS_TAG = QStringLiteral("class");
const QString GtObjectIO::S_NAME_TAG = QStringLiteral("name");
const QString GtObjectIO::S_TYPE_TAG = QStringLiteral("type");
const QString GtObjectIO::S_ID_TAG = QStringLiteral("id");
const QString GtObjectIO::S_VALUE_TAG = QStringLiteral("value");
const QString GtObjectIO::S_OPTIONAL_TAG = QStringLiteral("optional");
const QString GtObjectIO::S_ACTIVE_TAG = QStringLiteral("active");
const QString GtObjectIO::S_PROPERTY_TAG = QStringLiteral("property");
const QString GtObjectIO::S_PROPERTYLIST_TAG = QStringLiteral("propertylist");
const QString GtObjectIO::S_DIFF_INDEX_TAG = QStringLiteral("index");
const QString GtObjectIO::S_DIFF_INDEX_CHANGED_TAG =
        QStringLiteral("diff-index-changed");
const QString GtObjectIO::S_DYNAMICPROPERTY_TAG =
    QStringLiteral("dynamic_property");
const QString GtObjectIO::S_DYNAMICPROPERTIES_TAG =
    QStringLiteral("dynamic_properties");
const QString GtObjectIO::S_ENTRY_TAG = QStringLiteral("entry");
const QString GtObjectIO::S_DIFF_OBJ_REMOVE_TAG =
    QStringLiteral("diff-object-remove");
const QString GtObjectIO::S_DIFF_OBJ_ADD_TAG =
    QStringLiteral("diff-object-add");
const QString GtObjectIO::S_DIFF_PROP_CHANGE_TAG =
    QStringLiteral("diff-property-change");
const QString GtObjectIO::S_DIFF_PROPLIST_CHANGE_TAG =
    QStringLiteral("diff-propertylist-change");
const QString GtObjectIO::S_DIFF_DYNPROP_ADD_TAG =
    QStringLiteral("diff-dynamic-property-add");
const QString GtObjectIO::S_DIFF_DYNPROP_REM_TAG =
    QStringLiteral("diff-dynamic-property-remove");
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

GtObjectMemento::MementoData
GtObjectIO::toMemento(const GtObject* o, bool clone)
{
    // global object element
    GtObjectMemento::MementoData data;

    const GtDummyObject* d_obj = qobject_cast<const GtDummyObject*>(o);

    // class name
    if (d_obj == Q_NULLPTR)
    {
        data.className = o->metaObject()->className();
    }
    else
    {
        data.className = d_obj->origClassName();
    }

    // uuid
    QString uuid(o->uuid());

    if (!clone)
    {
        uuid = QUuid::createUuid().toString();
    }

    data.uuid = uuid;

    // object name
    data.ident = o->objectName();

    // store property information
    writeProperties(data, o);

    // child objects
    QList<GtObject*> directChildren = o->findDirectChildren<GtObject*>();
    data.childObjects.reserve(directChildren.size());

    foreach (const GtObject* child, directChildren)
    {
        // recursion through GtObjectMemento constructor
        data.childObjects.push_back(GtObjectMemento(child, clone));
    }

    return data;
}

QDomElement
GtObjectIO::toDomElement(const GtObjectMemento& m, QDomDocument& doc,
                         bool skipChildren)
{
    // global object element
    QDomElement element = doc.createElement(S_OBJECT_TAG);

    // class name
    element.setAttribute(S_CLASS_TAG, m.data().className);

    // uuid
    element.setAttribute(S_UUID_TAG, m.data().uuid);

    // object name
    element.setAttribute(S_NAME_TAG, m.data().ident);

    // store property information
    writeProperties(doc, element, m.data());

    // children
    if (!skipChildren && !m.data().childObjects.isEmpty())
    {
        QDomElement childElement =
            doc.createElement(S_OBJECTLIST_TAG);

        foreach(const GtObjectMemento& child, m.data().childObjects)
        {
            childElement.appendChild(toDomElement(child, doc));
        }

        element.appendChild(childElement);
    }

    return element;
}

GtObjectMemento::MementoData
GtObjectIO::toMemento(const QDomElement& e)
{
    // global object element
    GtObjectMemento::MementoData data;

    // class name
    data.className = e.attribute(S_CLASS_TAG);

    // uuid
    data.uuid = e.attribute(S_UUID_TAG);

    // object name
    data.ident = e.attribute(S_NAME_TAG);

    // store property information
    readProperties(data, e);

    // child objects
    QDomElement children = e.firstChildElement(S_OBJECTLIST_TAG);

    if (!children.isNull())
    {
        QDomElement compElement = children.firstChildElement(S_OBJECT_TAG);

        while (!compElement.isNull())
        {
            // recursion through GtObjectMemento constructor
            data.childObjects.push_back(GtObjectMemento(compElement));

            compElement = compElement.nextSiblingElement(S_OBJECT_TAG);
        }
    }

    return data;
}

GtObject*
GtObjectIO::toObject(const GtObjectMemento& memento, GtObject* parent)
{
  return toObjectHelper(memento.documentElement(), parent);
}

GtObject*
GtObjectIO::toObjectHelper(const QDomElement& element, GtObject* parent)
{
    GtObject* retval = Q_NULLPTR;

    QString fieldClass = element.attribute(S_CLASS_TAG);
    QString fieldUuid = element.attribute(S_UUID_TAG);
    QString fieldName = element.attribute(S_NAME_TAG);

    if (fieldClass.isEmpty() || fieldUuid.isEmpty())
    {
        gtWarning() << "class or uuid field is empty!";
        return NULL;
    }

    if (parent && !fieldName.isEmpty())
    {
        retval = parent->findDirectChild<GtObject*>(fieldName);

        if (retval)
        {
            /* Check class and uuid */
            if (retval->metaObject()->className() != fieldClass)
            {
                retval = Q_NULLPTR;
            }

            if (retval && (retval->uuid() != fieldUuid) && !retval->isDefault())
            {
                retval = Q_NULLPTR;
            }
        }
    }

    if (retval == Q_NULLPTR)
    {
        if (m_factory == Q_NULLPTR)
        {
            qCritical() << "ERROR: no factory set!";
            return Q_NULLPTR;
        }

        retval = m_factory->newObject(fieldClass, parent);

        if (retval == Q_NULLPTR)
        {
            // no class found in factory. we need a dummy object here
            gtWarning() << "could not recreate object of unknown type " <<
                           fieldClass << "...";

            GtDummyObject* d_obj = new GtDummyObject(parent);
            d_obj->setOrigClassName(fieldClass);
            retval = d_obj;
        }
    }

    mergeObject(element, retval);

    return retval;
}

void
GtObjectIO::mergeObject(const QDomElement& element, GtObject* obj)
{
    if (obj == NULL)
    {
        return;
    }

    QString fieldUuid = element.attribute(S_UUID_TAG);
    QString fieldName = element.attribute(S_NAME_TAG);

    obj->setObjectName(fieldName);
    obj->setUuid(fieldUuid);

    QList<GtObject*> childObjs = obj->findDirectChildren<GtObject*>();

    /* child informations */
    QDomElement children =
        element.firstChildElement(S_OBJECTLIST_TAG);

    if (!children.isNull())
    {
        QDomElement compElement = children.firstChildElement(S_OBJECT_TAG);

        while (!compElement.isNull())
        {
            GtObject* newObj = toObjectHelper(compElement, obj);

            if (newObj)
            {
                childObjs.removeOne(newObj);
            }

            compElement = compElement.nextSiblingElement(S_OBJECT_TAG);
        }
    }

    foreach (GtObject* o, childObjs)
    {
        if (o->isDefault())
        {
            childObjs.removeOne(o);
        }
    }

    qDeleteAll(childObjs);

    mergeObjectProperties(element, obj);

    obj->onObjectDataMerged();
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

        if (parentObject == Q_NULLPTR)
        {
            return false;
        }

        // handle removes
        QDomElement rem = parent.firstChildElement(S_DIFF_OBJ_REMOVE_TAG);
        while (!rem.isNull())
        {
            if (!handleObjectRemove(parentObject,
                                    rem.firstChildElement(S_OBJECT_TAG),
                                    rem.attribute(S_DIFF_INDEX_TAG)))
            {
                return false;
            }

            rem = rem.nextSiblingElement(S_DIFF_OBJ_REMOVE_TAG);
        }

        // handle adds
        QDomElement adds = parent.firstChildElement(S_DIFF_OBJ_ADD_TAG);
        while (!adds.isNull())
        {
            if (!handleObjectAdd(parentObject,
                                 adds.firstChildElement(S_OBJECT_TAG),
                                 adds.attribute(S_DIFF_INDEX_TAG)))
            {
                return false;
            }

            adds = adds.nextSiblingElement(S_DIFF_OBJ_ADD_TAG);
        }

        // handle index changes
        QDomElement indChanges = parent.firstChildElement(
                    S_DIFF_INDEX_CHANGED_TAG);
        while (!indChanges.isNull())
        {
            bool ok = true;

            int newIndex = indChanges.attribute(S_DIFF_NEWVAL_TAG).toInt(&ok);
            if (!ok)
            {
                return false;
            }

            if (!handleIndexChange(parentObject,
                                   indChanges.firstChildElement(S_OBJECT_TAG),
                                   newIndex))
            {
                return false;
            }

            indChanges = indChanges.nextSiblingElement(
                        S_DIFF_INDEX_CHANGED_TAG);
        }

        // handle prop changes
        QDomElement propC = parent.firstChildElement(S_DIFF_PROP_CHANGE_TAG);
        while (!propC.isNull())
        {
            handlePropertyNodeChange(parentObject, propC);

            propC = propC.nextSiblingElement(S_DIFF_PROP_CHANGE_TAG);
        }

        // handle prop list changes
        QDomElement propLC =
                parent.firstChildElement(S_DIFF_PROPLIST_CHANGE_TAG);
        while (!propLC.isNull())
        {
            handlePropertyNodeChange(parentObject, propLC, true);

            propLC = propLC.nextSiblingElement(S_DIFF_PROPLIST_CHANGE_TAG);
        }

        // handle object attribute changes
        QDomElement propOA = parent.firstChildElement(S_DIFF_ATTR_CHANGE_TAG);
        while (!propOA.isNull())
        {
            handleAttributeNodeChange(parentObject, propOA);

            propOA = propOA.nextSiblingElement(S_DIFF_ATTR_CHANGE_TAG);
        }

        parentObject->onObjectDataMerged();

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

        if (parentObject == Q_NULLPTR)
        {
            return false;
        }

        QDomElement diff = parent.firstChildElement();

        while (!diff.isNull())
        {
            if (diff.nodeName() == S_DIFF_OBJ_REMOVE_TAG)
            {
                if (!handleObjectAdd(parentObject,
                                     diff.firstChildElement(S_OBJECT_TAG),
                                     diff.attribute(S_DIFF_INDEX_TAG)))
                {
                    return false;
                }
            }
            else if (diff.nodeName() == S_DIFF_OBJ_ADD_TAG)
            {
                if (!handleObjectRemove(parentObject,
                                        diff.firstChildElement(S_OBJECT_TAG),
                                        diff.attribute(S_DIFF_INDEX_TAG)))
                {
                    return false;
                }
            }
            else if (diff.nodeName() == S_DIFF_INDEX_CHANGED_TAG)
            {
                bool ok = true;

                int newIndex = diff.attribute(S_DIFF_OLDVAL_TAG).toInt(&ok);
                if (!ok)
                {
                    return false;
                }

                if (!handleIndexChange(parentObject,
                                       diff.firstChildElement(S_OBJECT_TAG),
                                       newIndex))
                {
                    return false;
                }
            }
            else if (diff.nodeName() == S_DIFF_PROP_CHANGE_TAG)
            {
                handlePropertyNodeChange(parentObject, diff, false, true);
            }
            else if (diff.nodeName() == S_DIFF_PROPLIST_CHANGE_TAG)
            {
                handlePropertyNodeChange(parentObject, diff, true, true);
            }
            else if (diff.nodeName() == S_DIFF_ATTR_CHANGE_TAG)
            {
                handleAttributeNodeChange(parentObject, diff, true);
            }

            diff = diff.nextSiblingElement();
        }

        parentObject->onObjectDataMerged();

        parent = parent.nextSiblingElement(S_OBJECT_TAG);
    }

    return true;
}

void
GtObjectIO::mergeObjectProperties(const QDomElement& element,
                                  GtObject* obj)
{
    if (obj == NULL)
    {
        return;
    }

    if (obj->isDummy())
    {
        mergeDummyProperies(element, obj);
        return;
    }

    QString fieldClass = element.attribute(S_CLASS_TAG);
    QString fieldUuid = element.attribute(S_UUID_TAG);

    if (fieldClass.isEmpty() || fieldUuid.isEmpty())
    {
        qWarning() << "WARNING: class or uuid field is empty!";
        return;
    }

    if ((obj->metaObject()->className() != fieldClass) ||
            (obj->uuid() != fieldUuid))
    {
        qWarning() << "WARNING: class or uuid not equal to object data!";
        return;
    }

    /* static properties */
    QDomElement propElement =
        element.firstChildElement(S_PROPERTY_TAG);

    while (!propElement.isNull())
    {
        QString fieldType = propElement.attribute(S_TYPE_TAG);
        QString fieldName = propElement.attribute(S_NAME_TAG);

        if (!fieldType.isEmpty() && !fieldName.isEmpty())
        {

            GtAbstractProperty* prop = obj->findProperty(fieldName);

            if (prop != Q_NULLPTR)
            {
                bool s = prop->setValueFromVariant(propertyToVariant(
                                                       propElement.text(),
                                                       fieldType));

                QString fieldActive =
                    propElement.attribute(S_ACTIVE_TAG);

                if (!fieldActive.isEmpty())
                {
                    prop->setActive(QVariant(fieldActive).toBool());
                }

                if (!s)
                {
//                    qWarning() << QObject::tr("could not set property!") <<
//                               QStringLiteral(" (") << fieldClass <<
//                               QStringLiteral("::") << fieldName <<
//                               QStringLiteral(")");
                }
            }
            else
            {
                if (obj->property(fieldName.toLatin1()).toString() !=
                        propElement.text())
                {

                    bool s = obj->setProperty(fieldName.toLatin1(),
                                              propertyToVariant(
                                                  propElement.text(),
                                                  fieldType));

                    if (!s)
                    {
                        qWarning() << QObject::tr("could not find property") <<
                                   QStringLiteral(" (") << fieldClass <<
                                   QStringLiteral("::") << fieldName <<
                                   QStringLiteral(")");
                        qWarning() << "     |-> "
                                   << obj->metaObject()->className();
                        qWarning() << "     |-> " << obj->objectName();
                    }
                }
            }
        }
        else
        {
            qWarning() << "WARNING: property corrupted!";
        }

        propElement = propElement.nextSiblingElement(S_PROPERTY_TAG);
    }

    /* property lists */
    QDomElement listElement = element.firstChildElement(S_PROPERTYLIST_TAG);

    while (!listElement.isNull())
    {
        QString fieldType = listElement.attribute(S_TYPE_TAG);
        QString fieldName = listElement.attribute(S_NAME_TAG);

        if (!fieldType.isEmpty() && !fieldName.isEmpty())
        {
            GtAbstractProperty* prop = obj->findProperty(fieldName);

            if (prop != Q_NULLPTR)
            {
                bool s = prop->setValueFromVariant(propertyListToVariant(
                                                       listElement.text(),
                                                       fieldType));

                QString fieldActive =
                        listElement.attribute(S_ACTIVE_TAG);

                if (!fieldActive.isEmpty())
                {
                    prop->setActive(QVariant(fieldActive).toBool());
                }

                if (!s)
                {
//                    qWarning() << QObject::tr("could not set property!") <<
//                               QStringLiteral(" (") << fieldClass <<
//                               QStringLiteral("::") << fieldName <<
//                               QStringLiteral(")");
                }
            }
            else
            {
                QString listStr;
                QString listType;
                QVariant prop = obj->property(fieldName.toLatin1());

                propertyListStringType(prop, listStr, listType);
                if (listStr != listElement.text())
                {
                    obj->setProperty(fieldName.toLatin1(),
                                     propertyListToVariant(listElement.text(),
                                                           fieldType));
                }
            }
        }
        else
        {
            qWarning() << "WARNING: property corrupted!";
        }

        listElement = listElement.nextSiblingElement(S_PROPERTYLIST_TAG);
    }

    /* dynamic properties */
    QDomElement dynElement = element.firstChildElement(S_DYNAMICPROPERTIES_TAG);

    while (!dynElement.isNull())
    {
        qDebug() << "dynamic property container found! (restore)";

        mergeDynamicProperties(dynElement, obj);

        dynElement =
            dynElement.nextSiblingElement(S_DYNAMICPROPERTIES_TAG);
    }

}

void
GtObjectIO::mergeDummyProperies(const QDomElement& element, GtObject* obj)
{
    GtDummyObject* d_obj = qobject_cast<GtDummyObject*>(obj);

    if (d_obj == Q_NULLPTR)
    {
        return;
    }

    /* static properties */
    QDomElement propElement = element.firstChildElement();

    while (!propElement.isNull())
    {   
        if ((propElement.tagName() == S_PROPERTY_TAG) ||
                (propElement.tagName() == S_PROPERTYLIST_TAG))
        {
            QString fieldType = propElement.attribute(S_TYPE_TAG);
            QString fieldName = propElement.attribute(S_NAME_TAG);

            if (!fieldType.isEmpty() && !fieldName.isEmpty())
            {
                QVariant val;

                if (propElement.tagName() == S_PROPERTY_TAG)
                {
                    val = propertyToVariant(propElement.text(), fieldType);
                }
                else if (propElement.tagName() == S_PROPERTYLIST_TAG)
                {
                    val = propertyListToVariant(propElement.text(), fieldType);
                }

                QString fieldActive = propElement.attribute(S_ACTIVE_TAG);
                bool opt = false;
                bool act = true;

                if (!fieldActive.isEmpty())
                {
                    opt = true;
                    act = QVariant(fieldActive).toBool();
                }



                if (propElement.tagName() == S_PROPERTY_TAG)
                {
                    d_obj->addDummyProperty(fieldName, fieldType, opt, act,
                                            val);
                }
                else if (propElement.tagName() == S_PROPERTYLIST_TAG)
                {
                    d_obj->addDummyPropertyList(fieldName, fieldType, opt, act,
                                                val);
                }
            }
            else
            {
                qWarning() << "WARNING: property corrupted!";
            }
        }

        propElement = propElement.nextSiblingElement();
    }

//    /* property lists */
//    QDomElement listElement = element.firstChildElement(S_PROPERTYLIST_TAG);

//    while (!listElement.isNull())
//    {
//        QString fieldType = listElement.attribute(S_TYPE_TAG);
//        QString fieldName = listElement.attribute(S_NAME_TAG);

//        if (!fieldType.isEmpty() && !fieldName.isEmpty())
//        {
//            QVariant val = propertyListToVariant(listElement.text(), fieldType);
//            QString fieldActive = listElement.attribute(S_ACTIVE_TAG);
//            bool opt = false;
//            bool act = true;

//            if (!fieldActive.isEmpty())
//            {
//                opt = true;
//                act = QVariant(fieldActive).toBool();
//            }

//            d_obj->addDummyPropertyList(fieldName, fieldType, opt, act, val);
//        }
//        else
//        {
//            qWarning() << "WARNING: property corrupted!";
//        }

//        listElement = listElement.nextSiblingElement(S_PROPERTYLIST_TAG);
//    }
}

void
GtObjectIO::mergeDynamicProperties(const QDomElement& element, GtObject* obj)
{
    const QString dynConId = element.attribute(S_NAME_TAG);

    if (dynConId.isEmpty())
    {
        qWarning() << QObject::tr("Dynamic property container corrupted!");
        return;
    }

    GtAbstractProperty* dynCon = obj->findProperty(dynConId);

    if (dynCon == Q_NULLPTR)
    {
        qWarning() << QObject::tr("Dynamic property container not found!") <<
                   QStringLiteral(" (") << dynConId <<
                   QStringLiteral(")");
        return;
    }

    GtPropertyFactory* factory = GtPropertyFactory::instance();

    // read entries
    QDomElement entryElement =
        element.firstChildElement(S_ENTRY_TAG);

    while (!entryElement.isNull())
    {
        QDomElement propElement =
            entryElement.firstChildElement(S_DYNAMICPROPERTY_TAG);

        if (!propElement.isNull())
        {
            const QString className =
                propElement.attribute(S_CLASS_TAG);

            const QString id =
                propElement.attribute(S_ID_TAG);

            const QString name =
                propElement.attribute(S_NAME_TAG);

            const QString opt =
                propElement.attribute(S_OPTIONAL_TAG);

            const QString active =
                propElement.attribute(S_ACTIVE_TAG);

            qDebug() << "   |-> dynamic property found! (" << className << ")";

            if (className.isEmpty() || id.isEmpty() || name.isEmpty())
            {
                qWarning() << QObject::tr("Property corrupted!") <<
                           QStringLiteral(" (") << className <<
                           QStringLiteral("::") << id <<
                           QStringLiteral(")");
                break;
            }

            if (!factory->knownClass(className))
            {
                qWarning() << QObject::tr("Unknown property type!") <<
                           QStringLiteral(" (") << className <<
                           QStringLiteral("::") << id <<
                           QStringLiteral(")");

                break;
            }

            GtAbstractProperty* prop = dynCon->findProperty(name);

            bool needsRegistration = false;

            if (prop == Q_NULLPTR)
            {
                prop = factory->newProperty(className, name, id);
                needsRegistration = true;

                if (prop == Q_NULLPTR)
                {
                    qWarning() << QObject::tr("Could not recreate property!") <<
                               QStringLiteral(" (") << className <<
                               QStringLiteral("::") << id <<
                               QStringLiteral(")");

                    break;
                }
            }

            if (!opt.isEmpty())
            {
                prop->setOptional(QVariant(opt).toBool());
                prop->setActive(QVariant(active).toBool());
            }

            prop->setValueFromVariant(QVariant(propElement.text()));

            if (needsRegistration)
            {
                dynCon->registerSubProperty(*prop);
            }

        }

        entryElement =
            entryElement.nextSiblingElement(S_ENTRY_TAG);
    }
}

void
GtObjectIO::writeProperties(GtObjectMemento::MementoData& data,
                            const GtObject* obj)
{
    const QMetaObject* meta = obj->metaObject();

    const GtDummyObject* d_obj = qobject_cast<const GtDummyObject*>(obj);

    if (d_obj != Q_NULLPTR)
    {
        foreach (GtDummyObject::DummyProperty d_p,
                 d_obj->dummyProperties())
        {
            // static property
            GtObjectMemento::MementoData::PropertyData mprop;
            mprop.name = d_p.m_id;
            mprop.data = d_p.m_val;
            mprop.isOptional = d_p.m_optional;
            mprop.isActive = d_p.m_active;

            data.properties.push_back(mprop);
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
        writePropertyHelper(data.properties, storedProps, property);
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

        GtObjectMemento::MementoData::PropertyData p;
        p.name = prop.name();
        p.data = prop.read(obj);
        if (prop.isEnumType())
        {
            p.enumType = prop.typeName();
            QMetaEnum e = prop.enumerator();
            p.data = QString(e.valueToKey(p.data.toInt()));
        }
        data.properties.push_back(p);

    }
}

void
GtObjectIO::writeProperties(QDomDocument& doc,
                            QDomElement& root,
                            const GtObjectMemento::MementoData& data)
{
    // GTlab properties
    foreach (const GtObjectMemento::MementoData::PropertyData& property,
             data.properties)
    {
        writePropertyHelper(doc, root, property);
    }
}

void
GtObjectIO::readProperties(GtObjectMemento::MementoData& data,
                           const QDomElement& element)
{

    QDomElement propElement = element.firstChildElement();

    while (!propElement.isNull())
    {
        if (propElement.tagName() == S_PROPERTY_TAG)
        {
            GtObjectMemento::MementoData::PropertyData propData;

            readPropertyHelper(propData, propElement);

            data.properties.push_back(propData);
        }
        else if (propElement.tagName() == S_PROPERTYLIST_TAG)
        {
            QString fieldType = propElement.attribute(S_TYPE_TAG);
            QString fieldName = propElement.attribute(S_NAME_TAG);

            if (!fieldType.isEmpty() && !fieldName.isEmpty())
            {
                GtObjectMemento::MementoData::PropertyData propData;
                propData.name = fieldName;
                propData.data = propertyListToVariant(propElement.text(),
                                                      fieldType);

                data.properties.push_back(propData);
            }
        }
        else if (propElement.tagName() == S_DYNAMICPROPERTIES_TAG)
        {
            GtObjectMemento::MementoData::PropertyData propData;
            propData.isDynamicContainer = true;
            propData.name = propElement.attribute(S_NAME_TAG);

            readDynamicProperties(propData, propElement);

            data.properties.push_back(propData);
        }

        propElement = propElement.nextSiblingElement();
    }

//    // static properties
//    QDomElement propElement = element.firstChildElement(S_PROPERTY_TAG);

//    while (!propElement.isNull())
//    {
//        GtObjectMemento::MementoData::PropertyData propData;

//        readPropertyHelper(propData, propElement);

//        data.properties.push_back(propData);

//        propElement = propElement.nextSiblingElement(S_PROPERTY_TAG);
//    }

//    // property lists
//    QDomElement listElement = element.firstChildElement(S_PROPERTYLIST_TAG);

//    while (!listElement.isNull())
//    {
//        QString fieldType = listElement.attribute(S_TYPE_TAG);
//        QString fieldName = listElement.attribute(S_NAME_TAG);

//        if (!fieldType.isEmpty() && !fieldName.isEmpty())
//        {
//            GtObjectMemento::MementoData::PropertyData propData;
//            propData.name = fieldName;
//            propData.data = propertyListToVariant(listElement.text(),
//                                                  fieldType);

//            data.properties.push_back(propData);
//        }

//        listElement = listElement.nextSiblingElement(S_PROPERTYLIST_TAG);
//    }

//    // dynamic properties
//    QDomElement dynElement = element.firstChildElement(S_DYNAMICPROPERTIES_TAG);

//    while (!dynElement.isNull())
//    {
//        GtObjectMemento::MementoData::PropertyData propData;
//        propData.isDynamicContainer = true;
//        propData.name = dynElement.attribute(S_NAME_TAG);

//        readDynamicProperties(propData, dynElement);

//        data.properties.push_back(propData);

//        dynElement = dynElement.nextSiblingElement(S_DYNAMICPROPERTIES_TAG);
//    }
}

void
GtObjectIO::readPropertyHelper(
        GtObjectMemento::MementoData::PropertyData& propData,
        const QDomElement& element)
{
    QString fieldType = element.attribute(S_TYPE_TAG);
    QString fieldName = element.attribute(S_NAME_TAG);

    propData.name = fieldName;
    propData.data = propertyToVariant(element.text(), fieldType);

    QString fieldActive = element.attribute(S_ACTIVE_TAG);
    if (!fieldActive.isEmpty())
    {
        propData.isOptional = true;
        propData.isActive = QVariant(fieldActive).toBool();
    }
    QString fieldOptional = element.attribute(S_OPTIONAL_TAG);
    if (!fieldOptional.isEmpty())
    {
        propData.isOptional = QVariant(fieldOptional).toBool();
    }


    /* // should not be needed!
    QDomElement propElement = element.firstChildElement(S_PROPERTY_TAG);

    while (!propElement.isNull())
    {
        GtObjectMemento::MementoData::PropertyData childPropData;

        readPropertyHelper(childPropData, propElement);

        propData.childProperties.push_back(childPropData);

        propElement = propElement.nextSiblingElement(S_PROPERTY_TAG);
    }
    */
}

void
GtObjectIO::writeDynamicProperties(QDomDocument& doc,
                                   QDomElement& root,
                                   const GtObjectMemento::MementoData::PropertyData& property)
{
    foreach (const GtObjectMemento::MementoData::PropertyData& pChild, property.childProperties)
    {
        QDomElement entryElement = doc.createElement(S_ENTRY_TAG);
        writeDynamicPropertyHelper(doc, entryElement, pChild);

        root.appendChild(entryElement);
    }
}

void
GtObjectIO::readDynamicProperties(GtObjectMemento::MementoData::PropertyData& propData,
                                  const QDomElement& element)
{
    QDomElement entryElement = element.firstChildElement(S_ENTRY_TAG);

    while (!entryElement.isNull())
    {
        GtObjectMemento::MementoData::PropertyData childPropData;
        childPropData.name = entryElement.attribute(S_NAME_TAG);
        childPropData.dynamicClassName = entryElement.attribute(S_CLASS_TAG);
        childPropData.dynamicObjectName = entryElement.attribute(S_ID_TAG);
        childPropData.isOptional = QVariant(entryElement.attribute(S_OPTIONAL_TAG)).toBool();
        childPropData.isActive = QVariant(entryElement.attribute(S_ACTIVE_TAG)).toBool();
        childPropData.data = QVariant(entryElement.text());

        propData.childProperties.push_back(childPropData);

        entryElement = entryElement.nextSiblingElement(S_ENTRY_TAG);
    }
}

void
GtObjectIO::writePropertyHelper(QVector<GtObjectMemento::MementoData::PropertyData>& pVec,
                                QSet<QString>& stored,
                                GtAbstractProperty* property)
{
    if (stored.contains(property->ident()))
    {
        return;
    }

    bool isDynamic = false;

    if (property->storeToMemento() && property->isValid())
    {
        GtDynamicPropertyContainer* dynCon =
            qobject_cast<GtDynamicPropertyContainer*>(property);

        if (dynCon != Q_NULLPTR)
        {
            GtObjectMemento::MementoData::PropertyData mprop;
            mprop.name = dynCon->ident();
            mprop.data = QVariant();
            mprop.isDynamicContainer = true;

            foreach (GtAbstractProperty* pChild, dynCon->fullProperties())
            {
                if (pChild->storeToMemento() && pChild->isValid())
                {
                    GtObjectMemento::MementoData::PropertyData mcp;
                    mcp.name = pChild->ident();
                    mcp.dynamicClassName = pChild->metaObject()->className();
                    mcp.dynamicObjectName = pChild->objectName();
                    mcp.isOptional = pChild->isOptional();
                    mcp.isActive = pChild->isActive();
                    mcp.data = pChild->valueToVariant();
                    mprop.childProperties.push_back(mcp);
                }
            }

            pVec.push_back(mprop);

            isDynamic = true;
        }
        else
        {
            // static property
            GtObjectMemento::MementoData::PropertyData mprop;
            mprop.name = property->ident();
            mprop.data = property->valueToVariant();
            mprop.isOptional = property->isOptional();
            mprop.isActive = property->isActive();

            pVec.push_back(mprop);
        }

        stored << property->ident();
    }

    if (!isDynamic)
    {
        QList<GtAbstractProperty*> childProps = property->fullProperties();
        foreach (GtAbstractProperty* pChild, childProps)
        {
            writePropertyHelper(pVec, stored, pChild);
        }
    }
}

void
GtObjectIO::writePropertyHelper(
        QDomDocument& doc, QDomElement& root,
        const GtObjectMemento::MementoData::PropertyData& property)
{
    bool isDynamic = false;

    if (property.isDynamicContainer)
    {
        QDomElement dynElement = dynamicPropertyElement(doc, root,
                                                        property.name);
        writeDynamicProperties(doc, dynElement, property);

        isDynamic = true;
    }
    else
    {
        // static property
        QDomElement child;
        QVariantList list;
        QString listType;

        if (!property.enumType.isNull())
        {
            child = enumerationToDomElement(property.name, property.enumType,
                                            property.data, doc);
        }
        else
        {
            if (usePropertyList(property.data))
            {
                child = propertyListToDomElement(property.name, property.data,
                                                 doc);
            }
            else
            {
                child = propertyToDomElement(property.name, property.data, doc);
            }
        }

        if (property.isOptional)
        {
            QVariant actVar = QVariant::fromValue(property.isActive);
            child.setAttribute(S_ACTIVE_TAG, actVar.toString());
        }

        root.appendChild(child);
    }

    /* // should not be needed
    if (!isDynamic)
    {
        foreach (const GtObjectMemento::MementoData::PropertyData& pChild, property.childProperties)
        {
            writePropertyHelper(doc, root, pChild);
        }
    }
    */
}

void
GtObjectIO::writeDynamicPropertyHelper(
        QDomDocument& doc, QDomElement& root,
        const GtObjectMemento::MementoData::PropertyData& property)
{
    QDomElement child = propertyToDomElement(property.name, property.data, doc);
    child.setTagName(S_DYNAMICPROPERTY_TAG);

    child.removeAttribute(S_TYPE_TAG);

    child.setAttribute(S_CLASS_TAG, property.dynamicClassName);
    child.setAttribute(S_ID_TAG, property.dynamicObjectName);

    if (property.isOptional)
    {
        // optional attribute
        QVariant optVar = QVariant::fromValue(property.isOptional);
        child.setAttribute(S_OPTIONAL_TAG, optVar.toString());

        // active attribute
        QVariant actVar = QVariant::fromValue(property.isActive);
        child.setAttribute(S_ACTIVE_TAG, actVar.toString());
    }

    root.appendChild(child);
}

namespace
{
    // we need to be able to serialize these data types
    template<class T>
    class RegisterStreamOperators
    {
    public:
        RegisterStreamOperators(const char* typeName)
        {
            qRegisterMetaTypeStreamOperators<T>(typeName);
        }
    };
}

QDataStream& operator>>(QDataStream& s, QVector<double>& vector)
{
    //size_t size = 0;
    quint64 size = 0;
    s >> size;

    for (size_t i = 0; i < size; ++i) {
        s >> vector;
    }
    return s;
}

QDataStream& operator<<(QDataStream& s, const QVector<double>& vector)
{
    s << vector.size();

    for(auto v : vector) {
        s << v;
    }
    return s;
}

QDataStream& operator>>(QDataStream& s, QList<QPointF>& vector)
{
    //size_t size = 0;
    quint64 size = 0;
    s >> size;

    for (size_t i = 0; i < size; ++i) {
        s >> vector;
    }
    return s;
}

QDataStream& operator<<(QDataStream& s, const QList<QPointF>& vector)
{
    s << vector.size();

    for(auto v : vector) {
        s << v;
    }
    return s;
}

QDataStream& operator>>(QDataStream& s, QList<int>& vector)
{
    //size_t size = 0;
    quint64 size = 0;
    s >> size;

    for (size_t i = 0; i < size; ++i) {
        s >> vector;
    }
    return s;
}

QDataStream& operator<<(QDataStream& s, const QList<int>& vector)
{
    s << vector.size();

    for(auto v : vector) {
        s << v;
    }
    return s;
}

QDataStream& operator>>(QDataStream& s, QList<bool>& vector)
{
    //size_t size = 0;
    quint64 size = 0;
    s >> size;

    for (size_t i = 0; i < size; ++i) {
        s >> vector;
    }
    return s;
}

QDataStream& operator<<(QDataStream& s, const QList<bool>& vector)
{
    s << vector.size();

    for(auto v : vector) {
        s << v;
    }
    return s;
}

#if QT_VERSION >= 0x050900
QDataStream& operator>>(QDataStream& s, QStringList& vector)
{
    //size_t size = 0;
    quint64 size = 0;
    s >> size;

    for (size_t i = 0; i < size; ++i) {
        s >> vector;
    }
    return s;
}

QDataStream& operator<<(QDataStream& s, const QStringList& vector)
{
    s << vector.size();

    for(auto v : vector) {
        s << v;
    }
    return s;
}
#endif

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
    static RegisterStreamOperators< QVector<double> > streamOp_QDoubleVector(
                "QVector<double>");
    static RegisterStreamOperators< QList<int> > streamOp_QIntList(
                "QList<int>");
    static RegisterStreamOperators< QList<bool> > streamOp_QBoolList(
                "QList<bool>");
    static RegisterStreamOperators< QList<QPointF> > streamOp_QPointFList(
                "QList<QPointF>");
    static RegisterStreamOperators< QStringList > streamOp_QStringList(
                "QStringList");

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
                                 QDomDocument& doc)
{
    QDomElement element = doc.createElement(S_PROPERTY_TAG);

    element.setAttribute(S_NAME_TAG, name);
    element.setAttribute(S_TYPE_TAG, var.typeName());

    QDomText t = doc.createTextNode(variantToString(var));
    element.appendChild(t);

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

QVariant
GtObjectIO::propertyToVariant(const QString& value, const QString& type)
{
//    return QVariant(value);

    std::string str = type.toStdString();
    const char* p = str.c_str();

    QVariant::Type v_type = QVariant::nameToType(p);
    QVariant retval(value);

    retval.convert(v_type);

    return retval;
}

QVariant
GtObjectIO::propertyListToVariant(const QString& value, const QString& type)
{
    QVariant var;

    if (type == QStringLiteral("double"))
    {
        QVector<QStringRef> strList =
                QStringRef(&value).split(';', QString::SkipEmptyParts);
        QVector<double> list(strList.size());

        for (int i = 0; i < strList.size(); i++)
        {
            list[i] = strList[i].toDouble();
        }

        var.setValue(list);
    }
    else if (type == QStringLiteral("bool"))
    {
        QVector<QStringRef> strList =
                QStringRef(&value).split(';', QString::SkipEmptyParts);
        QList<bool> list;
        list.reserve(strList.size());

        for (int i = 0; i < strList.size(); i++)
        {
            list[i] = QVariant(strList[i].toString()).toBool();
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

QDomElement
GtObjectIO::dynamicPropertyElement(QDomDocument& doc,
                                   QDomElement& root,
                                   const QString& id)
{

    QDomElement retval = doc.createElement(S_DYNAMICPROPERTIES_TAG);
    retval.setAttribute(S_NAME_TAG, id);
    root.appendChild(retval);

    return retval;
}

bool
GtObjectIO::handlePropertyNodeChange(GtObject* target,
                                     const QDomElement &change,
                                     const bool list,
                                     const bool revert)
{
    QString propName = change.attribute(S_NAME_TAG);

    if (propName.isEmpty())
    {
        return false;
    }

    QString propType = change.attribute(S_TYPE_TAG);

    if (propType.isEmpty())
    {
        return false;
    }

    QDomElement newValNode = change.firstChildElement(S_DIFF_NEWVAL_TAG);
    QDomElement oldValNode = change.firstChildElement(S_DIFF_OLDVAL_TAG);

    if (newValNode.isNull() || oldValNode.isNull())
    {
        return false;
    }

    QString newVal;
    QString oldVal;

    if (revert)
    {
        newVal = oldValNode.text();
        oldVal = newValNode.text();
    }
    else
    {
        newVal = newValNode.text();
        oldVal = oldValNode.text();
    }

    GtAbstractProperty* prop = target->findProperty(propName);

    if (prop == Q_NULLPTR)
    {
        if (target->property(propName.toLatin1()) != newVal)
        {
            bool success = false;

            if (!list)
            {
                success = target->setProperty(propName.toLatin1(),
                                              propertyToVariant(newVal,
                                                                propType));
            }
            else
            {
                success = target->setProperty(propName.toLatin1(),
                                              propertyListToVariant(newVal,
                                                                    propType));
            }

            if (!success)
            {
                return false;
            }
        }
    }
    else
    {
        if (!list)
        {
            prop->setValueFromVariant(propertyToVariant(newVal, propType));
        }
        else
        {
            prop->setValueFromVariant(propertyListToVariant(newVal, propType));
        }

        QDomNodeList attrChanges = change.childNodes();
        QList<QDomElement> attrChangeElements;

        for (int i = 0; i < attrChanges.count(); i++)
        {
            QDomElement element = attrChanges.at(i).toElement();

            if (element.nodeName() == S_DIFF_ATTR_CHANGE_TAG)
            {
                attrChangeElements.append(element);
            }
        }

        foreach (QDomElement attrChange, attrChangeElements)
        {
            QString newAttrVal;
            QString oldAttrVal;
            QString attrID = attrChange.attribute(S_ID_TAG);

            if (attrID.isEmpty())
            {
                return false;
            }

            QDomElement oldAttrElement =
                    attrChange.firstChildElement(S_DIFF_OLDVAL_TAG);
            QDomElement newAttrElement =
                    attrChange.firstChildElement(S_DIFF_NEWVAL_TAG);

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

            if (attrID == S_ACTIVE_TAG)
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
    }

    return true;
}

bool
GtObjectIO::handleAttributeNodeChange(GtObject* target,
                                      const QDomElement& change,
                                      const bool revert)
{
    QDomElement newValNode = change.firstChildElement(S_DIFF_NEWVAL_TAG);
    QDomElement oldValNode = change.firstChildElement(S_DIFF_OLDVAL_TAG);

    if (newValNode.isNull() || oldValNode.isNull())
    {
        return false;
    }

    QString newVal;
    QString oldVal;

    if (revert)
    {
        newVal = oldValNode.text();
        oldVal = newValNode.text();
    }
    else
    {
        newVal = newValNode.text();
        oldVal = oldValNode.text();
    }

    QString attrID = change.attribute(S_ID_TAG);

    if (attrID == S_NAME_TAG)
    {
        target->setObjectName(newVal);
    }

    return true;
}

bool
GtObjectIO::handleDynamicPropertyAdd(GtObject* /*target*/,
                                     const QDomElement& /*objectToAdd*/,
                                     const QString /*index*/)
{
//    bool ok = true;
//    int ind = index.toInt(&ok);

//    if (!ok)
//    {
//        gtDebug() << "Index conversion failed!";
//        return false;
//    }

//    if (objectToAdd.isNull())
//    {
//        gtDebug() << "Dynamic Property to add is Null";
//        return false;
//    }

//    GtPropertyFactory* factory = GtPropertyFactory::instance();

//    if (factory == Q_NULLPTR)
//    {
//        return false;
//    }

//    GtDynamicPropertyContainer* dynPropCont =
//            target->findDirectChild<GtDynamicPropertyContainer*>();

//    if (dynPropCont == Q_NULLPTR)
//    {
//        gtDebug() << "Could not find dynamic properties container -> could not add property";
//        return false;
//    }

//    QDomElement dynProp =
//            objectToAdd.firstChildElement(GtObjectIO::S_DYNAMICPROPERTY_TAG);

//    while (!dynProp.isNull())
//    {
//        GtObjectMemento myMemento(dynProp);

//        if (!myMemento.isRestorable(factory))
//        {
//            gtDebug() << "Memento of dynamic property can not be restored!";
//            return false;
//        }

//        GtAbstractProperty* newProperty =
//                myMemento.restore<GtAbstractProperty*>(factory, false);

//        if (newProperty == Q_NULLPTR)
//        {
//            gtDebug() << "Could not restore object from memento";
//            return false;
//        }

//        dynPropCont->insertSubProperty(*newProperty, ind);

//        dynProp = dynProp.nextSiblingElement();
//    }

    return true;
}

bool
GtObjectIO::handleDynamicPropertyRemove(GtObject* /*target*/,
                                        const QDomElement& /*objectToRemove*/,
                                        const QString /*index*/)
{
//    if (objectToRemove.isNull())
//    {
//        gtDebug() << "Dynamic Property to remove is Null";
//        return false;
//    }

//    GtDynamicPropertyContainer* dynPropCont =
//            target->findDirectChild<GtDynamicPropertyContainer*>();

//    if (dynPropCont == Q_NULLPTR)
//    {
//        gtDebug() << "Could not find dynamic properties container -> could not add property";
//        return false;
//    }


    return true;
}

bool
GtObjectIO::handleObjectAdd(GtObject* parent,
                            const QDomElement& objectToAdd,
                            const QString index)
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
    GtObject* newObj = oio.toObjectHelper(objectToAdd);


    if (newObj == Q_NULLPTR)
    {
        gtDebug() << "Could not restore object from memento";
        return false;
    }

    int numberOfChildren = parent->findDirectChildren<GtObject*>().size();

    if (ind >= numberOfChildren)
    {
        parent->appendChild(newObj);
    }
    else if (ind < numberOfChildren)
    {
        parent->insertChild(ind, newObj);
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
GtObjectIO::handleObjectRemove(GtObject* parent,
                               const QDomElement& objectToRemove,
                               const QString /*index*/)
{
    if (objectToRemove.isNull())
    {
        qDebug() << "Object to remove is Null";
        return false;
    }

    const QString objUUID = objectToRemove.attribute(S_UUID_TAG);

    if (objUUID.isEmpty())
    {
        qDebug() << "No uuid found!";
        return false;
    }

    GtObject* toRemove = parent->getDirectChildByUuid(objUUID);

    if (toRemove == Q_NULLPTR)
    {
        qDebug() << "object not found! (" << objUUID << ")";
        qDebug() << "   |-> " << parent->objectName() << " [" <<
                    parent->uuid() << "]";
        return false;
    }

    delete toRemove;

    return true;
}

bool
GtObjectIO::handleIndexChange(GtObject* parent,
                              const QDomElement& object,
                              const int newIndex)
{
    if (object.isNull())
    {
        return false;
    }

    if (parent == Q_NULLPTR)
    {
        return false;
    }

    GtObject* toMove = parent->getDirectChildByUuid(
                object.attribute(S_UUID_TAG));

    if (toMove == Q_NULLPTR)
    {
        return false;
    }

    toMove->setParent(Q_NULLPTR);

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

QList<GtDynamicPropertyContainer*>
GtObjectIO::structProperties(GtObject* obj)
{
    QList<GtDynamicPropertyContainer*> retval;

    if (obj == Q_NULLPTR)
    {
        return retval;
    }

    foreach (GtAbstractProperty* prop, obj->properties())
    {
        retval.append(structPropertyHelper(prop));
    }

    return retval;
}

QList<GtDynamicPropertyContainer*>
GtObjectIO::structPropertyHelper(GtAbstractProperty* prop)
{
    QList<GtDynamicPropertyContainer*> retval;

    if (prop == Q_NULLPTR)
    {
        return retval;
    }

    GtDynamicPropertyContainer* sp = qobject_cast<GtDynamicPropertyContainer*>
                                     (prop);

    if (sp != Q_NULLPTR)
    {
        retval << sp;
    }

    foreach (GtAbstractProperty* prop, prop->fullProperties())
    {
        retval.append(structPropertyHelper(prop));
    }

    return retval;
}
