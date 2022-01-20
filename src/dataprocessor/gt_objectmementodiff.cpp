/* GTlab - Gas Turbine laboratory
 * Source File: gt_objectmementodiff.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 15.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCryptographicHash>
#include <QHash>

#include "gt_objectmementodiff.h"
#include "gt_objectmemento.h"
#include "gt_objectio.h"

GtObjectMementoDiff::GtObjectMementoDiff(const GtObjectMemento& left,
        const GtObjectMemento& right)
{
    if (!left.isNull() && !right.isNull())
    {
        QDomElement nullElem;
        if (!makeDiff(left, right, nullElem))
        {
            gtError() << QObject::tr("DIFF FAILED!!!");
        }
    }
}

GtObjectMementoDiff::GtObjectMementoDiff(const QByteArray& byteArray)
{
    setContent(byteArray);
}

GtObjectMementoDiff::GtObjectMementoDiff()
{
}

bool
GtObjectMementoDiff::hasObjectTreeChanges() const
{
    if (isNull())
    {
        return false;
    }

    QDomElement parent = firstChildElement(GtObjectIO::S_OBJECT_TAG);

    while (!parent.isNull())
    {
        QDomElement diff = parent.firstChildElement();

        while (!diff.isNull())
        {
            if (diff.nodeName() == GtObjectIO::S_DIFF_OBJ_REMOVE_TAG ||
                    diff.nodeName() == GtObjectIO::S_DIFF_OBJ_ADD_TAG)
            {
                return true;
            }

            diff = diff.nextSiblingElement();
        }

        parent = parent.nextSiblingElement(GtObjectIO::S_OBJECT_TAG);
    }

    return false;
}

GtObjectMementoDiff
GtObjectMementoDiff::objectAddBranch(GtObject* child, GtObject* parent)
{
    GtObjectMementoDiff retval;

    QDomElement root = retval.createElement(GtObjectIO::S_OBJECT_TAG);

    copyAttributes(parent->toMemento().documentElement(), root);

    QDomElement  obj =
        retval.createElement(GtObjectIO::S_DIFF_OBJ_ADD_TAG);

    obj.appendChild(child->toMemento().documentElement());

    root.appendChild(obj);

    retval.appendChild(root);

    return retval;
}

GtObjectMementoDiff
GtObjectMementoDiff::objectRemoveBranch(GtObject* child, GtObject* parent)
{
    GtObjectMementoDiff retval;

    QDomElement root = retval.createElement(GtObjectIO::S_OBJECT_TAG);

    copyAttributes(parent->toMemento().documentElement(), root);

    QDomElement  obj =
        retval.createElement(GtObjectIO::S_DIFF_OBJ_REMOVE_TAG);

    obj.appendChild(child->toMemento().documentElement());

    root.appendChild(obj);

    retval.appendChild(root);

    return retval;
}

bool
GtObjectMementoDiff::appendDiff(const GtObjectMementoDiff& diff)
{
    if (diff.isNull())
    {
        return false;
    }

    QDomElement parent = diff.firstChildElement(GtObjectIO::S_OBJECT_TAG);

    while (!parent.isNull())
    {
        if (isNull())
        {
            // empty initialization if no nodes exists
            createElement(QString());
        }

        appendChild(parent.cloneNode());

        parent = parent.nextSiblingElement(GtObjectIO::S_OBJECT_TAG);
    }

    return true;
}

int
GtObjectMementoDiff::numberOfDiffSteps()
{
    if (isNull())
    {
        return 0;
    }

    int retval = 0;

    QDomElement parent = firstChildElement(GtObjectIO::S_OBJECT_TAG);

    while (!parent.isNull())
    {
        retval++;

        parent = parent.nextSiblingElement(GtObjectIO::S_OBJECT_TAG);
    }

    return retval;
}

bool
GtObjectMementoDiff::makeDiff(const GtObjectMemento& left,
                              const GtObjectMemento& right,
                              QDomElement& diffRoot)
{
    // update hashes if needed
    left.calculateHashes();
    right.calculateHashes();
    if (left.fullHash() == right.fullHash() && diffRoot.isNull())
    {
        gtDebug() << QObject::tr("compared object mementos are identical!");
        return true;
    }

    if (left.data().uuid != right.data().uuid)
    {
        gtWarning() << QObject::tr("root objects not equal!");
        QDomElement rootError =
            this->createElement(QStringLiteral("ROOT_ERROR"));
        if (diffRoot.isNull())
        {
            this->appendChild(rootError);
        }
        else
        {
            diffRoot.appendChild(rootError);
        }
        return false;
    }

    // create diff element to store changes
    QDomElement diffObj = this->createElement(GtObjectIO::S_OBJECT_TAG);
    diffObj.setAttribute(GtObjectIO::S_NAME_TAG, left.data().ident);
    diffObj.setAttribute(GtObjectIO::S_UUID_TAG, left.data().uuid);
    diffObj.setAttribute(GtObjectIO::S_CLASS_TAG, left.data().className);
    // modified child elements are stored in their own element
    // this way we may not want to actually add the diffObj element (if empty)!
    bool diffObjectEmpty = true;

    // check properties & attributes
    if (left.propertyHash() != right.propertyHash())
    {
        diffObjectEmpty = false;

        if (left.data().className != right.data().className)
        {
            handleAttributeChange(GtObjectIO::S_CLASS_TAG,
                                  left.data().className,
                                  right.data().className,
                                  diffObj);
        }

        if (left.data().ident != right.data().ident)
        {
            handleAttributeChange(GtObjectIO::S_NAME_TAG,
                                  left.data().ident,
                                  right.data().ident,
                                  diffObj);
        }


        detectPropertyChanges(left.data().properties, right.data().properties, diffObj, false);
    }

    // create map of child object uuids
    QHash<QString,int> leftChildIndexMap;
    QHash<QString, int> completeMap;
    QHash<QString, int> indexChangeMap;
    for (int lchildIndex = 0; lchildIndex < left.data().childObjects.size(); lchildIndex++)
    {
        const GtObjectMemento& lchild(left.data().childObjects[lchildIndex]);
        leftChildIndexMap.insert(lchild.data().uuid, lchildIndex);
        completeMap.insert(lchild.data().uuid, lchildIndex);
    }

    // look for modified/added children
    for (int rchildIndex = 0; rchildIndex < right.data().childObjects.size(); rchildIndex++)
    {
        const GtObjectMemento& rchild (right.data().childObjects[rchildIndex]);
        auto lchildIter = leftChildIndexMap.find(rchild.data().uuid);
        if (lchildIter != leftChildIndexMap.end())
        {
            // check for modifications
            const GtObjectMemento& lchild(left.data().childObjects[*lchildIter]);
            if (lchild.fullHash() != rchild.fullHash())
            {
                if (!makeDiff(lchild, rchild, diffRoot))
                {
                    return false;
                }
            }
            // take item from list to track removed objects
            leftChildIndexMap.erase(lchildIter);
        }
        else
        {
            diffObjectEmpty = false;
            // added child
            GtObjectIO oio;
            QDomElement rchildElem = oio.toDomElement(rchild, *this, false); 
            handleObjectAdded(rchildElem, rchildIndex, diffObj);

            completeMap.insert(rchild.data().uuid, rchildIndex);
            foreach(QString uuid, completeMap.keys())
            {
                if (uuid != rchild.data().uuid)
                {
                    if (completeMap.value(uuid) >= rchildIndex)
                    {
                        completeMap.insert(uuid, completeMap.value(uuid) + 1);

                        if (!indexChangeMap.keys().contains(uuid))
                        {
                            indexChangeMap.insert(uuid, 1);
                        }
                        else
                        {
                            indexChangeMap.insert(uuid, indexChangeMap.value(uuid) + 1);
                        }
                    }
                }
            }
        }
    }

    QList<int> leftChildIndexSort = leftChildIndexMap.values();
    qSort(leftChildIndexSort);

    // look for removed children
    foreach (int lchildIndex, leftChildIndexSort)
    {
        diffObjectEmpty = false;
        const GtObjectMemento& lchild(left.data().childObjects[lchildIndex]);
        GtObjectIO oio;
        QDomElement lchildElem = oio.toDomElement(lchild, *this, false);
        handleObjectRemoved(lchildElem, lchildIndex, diffObj);

        int completeMapIndex = completeMap.value(lchild.data().uuid);
        foreach(QString uuid, completeMap.keys())
        {
            if (uuid != lchild.data().uuid)
            {
                int index = completeMap.value(uuid);
                if (index > completeMapIndex)
                {
                    if (indexChangeMap.keys().contains(uuid))
                    {
                        indexChangeMap.insert(uuid, indexChangeMap.value(uuid) - 1);
                    }
                    else
                    {
                        indexChangeMap.insert(uuid, -1);
                    }
                }
            }
        }

        if (indexChangeMap.keys().contains(lchild.data().uuid))
        {
            indexChangeMap.remove(lchild.data().uuid);
        }
    }

    foreach(QString uuid, indexChangeMap.keys())
    {
        if (indexChangeMap.value(uuid) == 0)
        {
            indexChangeMap.remove(uuid);
        }
    }

//  look for index changes
    QHash<QString, int> lchildMap;
    for (int lchildIndex = 0; lchildIndex < left.data().childObjects.size(); lchildIndex++)
    {
        const GtObjectMemento& lchild(left.data().childObjects[lchildIndex]);
        lchildMap.insert(lchild.data().uuid, lchildIndex);
    }

    QHash<QString,int> rchildMap;
    for (int rchildIndex = 0; rchildIndex < right.data().childObjects.size(); rchildIndex++)
    {
        const GtObjectMemento& rchild(right.data().childObjects[rchildIndex]);
        rchildMap.insert(rchild.data().uuid, rchildIndex);
    }

    foreach(QString uuid, rchildMap.keys())
    {
        if (!lchildMap.keys().contains(uuid))
        {
            continue;
        }

        int indexOld = lchildMap.value(uuid);
        int indexNew = rchildMap.value(uuid);
        int indexChange = 0;

        if (indexChangeMap.contains(uuid))
        {
            indexChange = indexChangeMap.value(uuid);
        }
        int diff = indexNew - indexOld - indexChange;

        if (diff != 0)
        {
            diffObjectEmpty = false;
            const GtObjectMemento& rchild (right.data().childObjects[indexNew]);
            GtObjectIO oio;
            QDomElement childElem = oio.toDomElement(rchild, *this, false);
            handleIndexChanged(childElem, indexOld, indexNew, diffObj);
        }
    }

    if (!diffObjectEmpty)
    {
        if (diffRoot.isNull())
        {
            this->appendChild(diffObj);
        }
        else
        {
            diffRoot.appendChild(diffObj);
        }
    }
/*    qDebug() << "Result of diff:";
    qDebug() << toByteArray()*/;
    return true;
}

void
GtObjectMementoDiff::handleAttributeChange(const QString& name,
                                           const QString& leftVal,
                                           const QString& rightVal,
                                           QDomElement& diffRoot)
{
    QDomElement attrDiff = this->createElement(GtObjectIO::S_DIFF_ATTR_CHANGE_TAG);

    attrDiff.setAttribute(GtObjectIO::S_ID_TAG, name);

    QDomElement oldAttr = this->createElement(GtObjectIO::S_DIFF_OLDVAL_TAG);
    QDomElement newAttr = this->createElement(GtObjectIO::S_DIFF_NEWVAL_TAG);

    oldAttr.appendChild(this->createTextNode(leftVal));
    newAttr.appendChild(this->createTextNode(rightVal));

    attrDiff.appendChild(oldAttr);
    attrDiff.appendChild(newAttr);

    diffRoot.appendChild(attrDiff);
}

void
GtObjectMementoDiff::detectPropertyChanges(const QVector<GtObjectMemento::MementoData::PropertyData>& leftProperties,
                                           const QVector<GtObjectMemento::MementoData::PropertyData>& rightProperties,
                                           QDomElement& diffRoot, bool isDynamic)
{
    QHash<QString,const GtObjectMemento::MementoData::PropertyData*> leftPropMap;
    foreach (const GtObjectMemento::MementoData::PropertyData& lprop, leftProperties)
    {
        leftPropMap.insert(lprop.name, &lprop);
    }
    // look for modified / added properties
    foreach (const GtObjectMemento::MementoData::PropertyData& rprop, rightProperties)
    {
        auto lpropIter = leftPropMap.find(rprop.name);
        if (lpropIter != leftPropMap.end())
        {
            // check for modifications
            const GtObjectMemento::MementoData::PropertyData& lprop (**lpropIter);
            if (lprop.hash != rprop.hash)
            {
                handlePropertyChange(lprop, rprop, diffRoot);
            }
            // take item from list to track removed properties
            leftPropMap.erase(lpropIter);
        }
        else
        {
            if (!isDynamic)
            {
                // added property, this is not intended to happen!
                gtDebug() << QObject::tr("Property added in diff, name: ") << rprop.name;
                // don't do anything for now
            }
            else
            {
                QDomElement dynPropAdd = this->createElement(GtObjectIO::S_DIFF_DYNPROP_ADD_TAG);
                int index = 0; // TODO
                dynPropAdd.setAttribute(GtObjectIO::S_DIFF_INDEX_TAG, QString::number(index));
                GtObjectIO oio;
                oio.writeDynamicPropertyHelper(*this, dynPropAdd, rprop);
                diffRoot.appendChild(dynPropAdd);
            }
        }
    }
    // look for removed properties
    foreach (const GtObjectMemento::MementoData::PropertyData* lpropPointer, leftPropMap)
    {
        const GtObjectMemento::MementoData::PropertyData& lprop (*lpropPointer);
        if (!isDynamic)
        {
            // removed property, this is not intended to happen!
            gtDebug() << QObject::tr("Property removed in diff, name: ") << lprop.name;
            // don't do anything for now
        }
        else
        {
            QDomElement dynPropRem = this->createElement(GtObjectIO::S_DIFF_DYNPROP_REM_TAG);
            int index = 0; // TODO
            dynPropRem.setAttribute(GtObjectIO::S_DIFF_INDEX_TAG, QString::number(index));
            GtObjectIO oio;
            oio.writeDynamicPropertyHelper(*this, dynPropRem, lprop);
            diffRoot.appendChild(dynPropRem);
        }
    }
}

void
GtObjectMementoDiff::handlePropertyChange(const GtObjectMemento::MementoData::PropertyData& leftProp,
                                          const GtObjectMemento::MementoData::PropertyData& rightProp,
                                          QDomElement& diffRoot)
{
    // distinguish "types"
    QDomElement diffObj;
    bool diffObjEmpty = true;
    QVariantList leftPropList, rightPropList;
    if (GtObjectIO::usePropertyList(leftProp.data))
    {
        diffObj = this->createElement(GtObjectIO::S_DIFF_PROPLIST_CHANGE_TAG);
        diffObj.setAttribute(GtObjectIO::S_NAME_TAG, leftProp.name);

        diffObjEmpty = false;
        QDomElement oldVal = this->createElement(GtObjectIO::S_DIFF_OLDVAL_TAG);
        QDomElement newVal = this->createElement(GtObjectIO::S_DIFF_NEWVAL_TAG);

        QString leftVal, leftType;
        QString rightVal, rightType;
        GtObjectIO::propertyListStringType(leftProp.data, leftVal, leftType);
        GtObjectIO::propertyListStringType(rightProp.data, rightVal, rightType);

        diffObj.setAttribute(GtObjectIO::S_TYPE_TAG, leftType);

        oldVal.appendChild(this->createTextNode(leftVal));
        newVal.appendChild(this->createTextNode(rightVal));

        diffObj.appendChild(oldVal);
        diffObj.appendChild(newVal);
    }
    else
    {
        diffObj = this->createElement(GtObjectIO::S_DIFF_PROP_CHANGE_TAG);
        diffObj.setAttribute(GtObjectIO::S_NAME_TAG, leftProp.name);
        if (!leftProp.enumType.isNull())
        {
            diffObj.setAttribute(GtObjectIO::S_TYPE_TAG, leftProp.enumType);
        }
        else
        {
            diffObj.setAttribute(GtObjectIO::S_TYPE_TAG, leftProp.data.typeName());
        }

        // handle value changes
        if (leftProp.data != rightProp.data)
        {
            diffObjEmpty = false;
            QDomElement oldVal = this->createElement(GtObjectIO::S_DIFF_OLDVAL_TAG);
            QDomElement newVal = this->createElement(GtObjectIO::S_DIFF_NEWVAL_TAG);

            oldVal.appendChild(this->createTextNode(GtObjectIO::variantToString(leftProp.data)));
            newVal.appendChild(this->createTextNode(GtObjectIO::variantToString(rightProp.data)));

            diffObj.appendChild(oldVal);
            diffObj.appendChild(newVal);
        }
    }

    // handle further attribute changes
    if (leftProp.isOptional != rightProp.isOptional)
    {
        diffObjEmpty = false;
        handleAttributeChange(GtObjectIO::S_OPTIONAL_TAG, QVariant(leftProp.isOptional).toString(), QVariant(rightProp.isOptional).toString(), diffObj);
    }

    if (leftProp.isActive != rightProp.isActive)
    {
        diffObjEmpty = false;
        handleAttributeChange(GtObjectIO::S_ACTIVE_TAG, QVariant(leftProp.isActive).toString(), QVariant(rightProp.isActive).toString(), diffObj);
    }

    if (leftProp.dynamicClassName != rightProp.dynamicClassName)
    {
        diffObjEmpty = false;
        handleAttributeChange(GtObjectIO::S_CLASS_TAG, leftProp.dynamicClassName, rightProp.dynamicClassName, diffObj);
    }

    if (leftProp.dynamicObjectName != rightProp.dynamicObjectName)
    {
        diffObjEmpty = false;
        handleAttributeChange(GtObjectIO::S_ID_TAG, leftProp.dynamicObjectName, rightProp.dynamicObjectName, diffObj);
    }


    if (!diffObjEmpty)
    {
        diffRoot.appendChild(diffObj);
    }


    // handle dynamic properties
    if (leftProp.isDynamicContainer || rightProp.isDynamicContainer)
    {
        // handle sub-properties
        detectPropertyChanges(leftProp.childProperties, rightProp.childProperties, diffRoot, true);
    }
}

void
GtObjectMementoDiff::handleObjectAdded(const QDomElement& addedObj,
                                       int index,
                                       QDomElement& diffRoot)
{
    QDomElement objAdd =
        this->createElement(GtObjectIO::S_DIFF_OBJ_ADD_TAG);

    objAdd.setAttribute(GtObjectIO::S_DIFF_INDEX_TAG,
                        QString::number(index));

    objAdd.appendChild(addedObj);

    diffRoot.appendChild(objAdd);
}

void
GtObjectMementoDiff::handleObjectRemoved(const QDomElement& removedObject,
                                         int index,
                                         QDomElement& diffRoot)
{
    QDomElement objRem =
        this->createElement(GtObjectIO::S_DIFF_OBJ_REMOVE_TAG);

    objRem.setAttribute(GtObjectIO::S_DIFF_INDEX_TAG,
                        QString::number(index));

    objRem.appendChild(removedObject);

    diffRoot.appendChild(objRem);
}

void
GtObjectMementoDiff::handleIndexChanged(const QDomElement& changedObject,
                                        int oldIndex, int newIndex,
                                        QDomElement& diffRoot)
{
    QDomElement indChanged =
        this->createElement(GtObjectIO::S_DIFF_INDEX_CHANGED_TAG);

    indChanged.setAttribute(GtObjectIO::S_DIFF_NEWVAL_TAG, newIndex);
    indChanged.setAttribute(GtObjectIO::S_DIFF_OLDVAL_TAG, oldIndex);

    QDomElement obj = this->createElement(GtObjectIO::S_OBJECT_TAG);
    obj.setAttribute(GtObjectIO::S_NAME_TAG, changedObject.attribute("name"));
    obj.setAttribute(GtObjectIO::S_UUID_TAG, changedObject.attribute("uuid"));
    obj.setAttribute(GtObjectIO::S_CLASS_TAG, changedObject.attribute("class"));

    indChanged.appendChild(obj);

    diffRoot.appendChild(indChanged);
}

void
GtObjectMementoDiff::copyAttributes(const QDomElement& root,
                                    QDomElement& target)
{
    QDomNamedNodeMap rootAttrMap = root.attributes();
    QList<QDomAttr> rootAttributes;

    for (int i = 0; i < rootAttrMap.count(); i++)
    {
        rootAttributes.append(rootAttrMap.item(i).toAttr());
    }

    foreach (QDomAttr attr, rootAttributes)
    {
        target.setAttribute(attr.name(), attr.value());
    }
}

