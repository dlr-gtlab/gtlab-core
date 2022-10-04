/* GTlab - Gas Turbine laboratory
 * Source File: gt_objectmementodiff.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 15.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_objectmementodiff.h"
#include "gt_objectmemento.h"
#include "gt_objectio.h"
#include "gt_algorithms.h"

#include <QCryptographicHash>
#include <QHash>

#include <algorithm>

using PD = GtObjectMemento::PropertyData;

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

    if (left.uuid() != right.uuid())
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
    diffObj.setAttribute(GtObjectIO::S_NAME_TAG, left.ident());
    diffObj.setAttribute(GtObjectIO::S_UUID_TAG, left.uuid());
    diffObj.setAttribute(GtObjectIO::S_CLASS_TAG, left.className());
    // modified child elements are stored in their own element
    // this way we may not want to actually add the diffObj element (if empty)!
    bool diffObjectEmpty = true;

    // check properties & attributes
    if (left.propertyHash() != right.propertyHash())
    {
        diffObjectEmpty = false;

        if (left.className() != right.className())
        {
            handleAttributeChange(GtObjectIO::S_CLASS_TAG,
                                  left.className(),
                                  right.className(),
                                  diffObj);
        }

        if (left.ident() != right.ident())
        {
            handleAttributeChange(GtObjectIO::S_NAME_TAG,
                                  left.ident(),
                                  right.ident(),
                                  diffObj);
        }


        detectPropertyChanges(left.properties, right.properties, diffObj);
        detectDynamicPropertyChanges(left.dynamicSizeProperties,
                                     right.dynamicSizeProperties,
                                     diffObj);
    }

    // create map of child object uuids
    QHash<QString,int> leftChildIndexMap;
    QHash<QString, int> completeMap;
    QHash<QString, int> indexChangeMap;
    for (int lchildIndex = 0; lchildIndex < left.childObjects.size(); lchildIndex++)
    {
        const GtObjectMemento& lchild(left.childObjects[lchildIndex]);
        leftChildIndexMap.insert(lchild.uuid(), lchildIndex);
        completeMap.insert(lchild.uuid(), lchildIndex);
    }

    // look for modified/added children
    for (int rchildIndex = 0; rchildIndex < right.childObjects.size(); rchildIndex++)
    {
        const GtObjectMemento& rchild (right.childObjects[rchildIndex]);
        auto lchildIter = leftChildIndexMap.find(rchild.uuid());
        if (lchildIter != leftChildIndexMap.end())
        {
            // check for modifications
            const GtObjectMemento& lchild(left.childObjects[*lchildIter]);
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

            completeMap.insert(rchild.uuid(), rchildIndex);
            for_each_key(completeMap, [&](const QString& uuid)
            {
                if (uuid != rchild.uuid())
                {
                    if (completeMap.value(uuid) >= rchildIndex)
                    {
                        completeMap.insert(uuid, completeMap.value(uuid) + 1);

                        if (!indexChangeMap.contains(uuid))
                        {
                            indexChangeMap.insert(uuid, 1);
                        }
                        else
                        {
                            indexChangeMap.insert(uuid, indexChangeMap.value(uuid) + 1);
                        }
                    }
                }
            });
        }
    }

    QList<int> leftChildIndexSort = leftChildIndexMap.values();
    std::sort(std::begin(leftChildIndexSort), std::end(leftChildIndexSort));

    // look for removed children
    foreach (int lchildIndex, leftChildIndexSort)
    {
        diffObjectEmpty = false;
        const GtObjectMemento& lchild(left.childObjects[lchildIndex]);
        GtObjectIO oio;
        QDomElement lchildElem = oio.toDomElement(lchild, *this, false);
        handleObjectRemoved(lchildElem, lchildIndex, diffObj);

        int completeMapIndex = completeMap.value(lchild.uuid());
        for_each_key(completeMap, [&](const QString& uuid)
        {
            if (uuid != lchild.uuid())
            {
                int index = completeMap.value(uuid);
                if (index > completeMapIndex)
                {
                    if (indexChangeMap.contains(uuid))
                    {
                        indexChangeMap.insert(uuid, indexChangeMap.value(uuid) - 1);
                    }
                    else
                    {
                        indexChangeMap.insert(uuid, -1);
                    }
                }
            }
        });

        if (indexChangeMap.contains(lchild.uuid()))
        {
            indexChangeMap.remove(lchild.uuid());
        }
    }

    for_each_key(indexChangeMap, [&](const QString& uuid)
    {
        if (indexChangeMap.value(uuid) == 0)
        {
            indexChangeMap.remove(uuid);
        }
    });

//  look for index changes
    QHash<QString, int> lchildMap;
    for (int lchildIndex = 0; lchildIndex < left.childObjects.size(); lchildIndex++)
    {
        const GtObjectMemento& lchild(left.childObjects[lchildIndex]);
        lchildMap.insert(lchild.uuid(), lchildIndex);
    }

    QHash<QString,int> rchildMap;
    for (int rchildIndex = 0; rchildIndex < right.childObjects.size(); rchildIndex++)
    {
        const GtObjectMemento& rchild(right.childObjects[rchildIndex]);
        rchildMap.insert(rchild.uuid(), rchildIndex);
    }

    for_each_key(rchildMap, [&](const QString& uuid)
    {
        if (!lchildMap.contains(uuid))
        {
            return;
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
            const GtObjectMemento& rchild (right.childObjects[indexNew]);
            GtObjectIO oio;
            QDomElement childElem = oio.toDomElement(rchild, *this, false);
            handleIndexChanged(childElem, indexOld, indexNew, diffObj);
        }
    });

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
GtObjectMementoDiff::detectPropertyChanges(const QVector<GtObjectMemento::PropertyData>& leftProperties,
                                           const QVector<GtObjectMemento::PropertyData>& rightProperties,
                                           QDomElement& diffRoot)

{
    QHash<QString,const GtObjectMemento::PropertyData*> leftPropMap;
    foreach (const GtObjectMemento::PropertyData& lprop, leftProperties)
    {
        leftPropMap.insert(lprop.name, &lprop);
    }
    // look for modified / added properties
    foreach (const GtObjectMemento::PropertyData& rprop, rightProperties)
    {
        auto lpropIter = leftPropMap.find(rprop.name);
        if (lpropIter != leftPropMap.end())
        {
            // check for modifications
            const GtObjectMemento::PropertyData& lprop (**lpropIter);
            if (lprop.hash != rprop.hash)
            {
                handlePropertyChange(lprop, rprop, diffRoot);
            }
            // take item from list to track removed properties
            leftPropMap.erase(lpropIter);
        }
        else
        {

            // added property, this is not intended to happen!
            gtDebug() << QObject::tr("Property added in diff, name: ") << rprop.name;
            // don't do anything for now

        }
    }
    // look for removed properties
    foreach (const GtObjectMemento::PropertyData* lpropPointer, leftPropMap)
    {
        const GtObjectMemento::PropertyData& lprop (*lpropPointer);
        // removed property, this is not intended to happen!
        gtDebug() << QObject::tr("Property removed in diff, name: ") << lprop.name;
        // don't do anything for now
    }
}

void
GtObjectMementoDiff::detectDynamicPropertyChanges(
    const QVector<GtObjectMemento::PropertyData>& leftContainers,
    const QVector<GtObjectMemento::PropertyData>& rightContainers,
    QDomElement& diffRoot)
{
    // we enforce, that the number of dyanmic properties may not change
    assert (leftContainers.size() == rightContainers.size());

    for (int i = 0; i < leftContainers.size(); ++i)
    {
        const auto& left = leftContainers[i];
        const auto& right = rightContainers[i];

        if (left.hash != right.hash)
        {
            detectDynamicPropertyChanges(left, right, diffRoot);
        }
    }
}


void
GtObjectMementoDiff::detectDynamicPropertyChanges(
    const GtObjectMemento::PropertyData& leftContainer,
    const GtObjectMemento::PropertyData& rightContainer,
    QDomElement& diffRoot)

{
    assert(leftContainer.name == rightContainer.name);

    const auto& leftEntries = leftContainer.childProperties;
    const auto& rightEntries = rightContainer.childProperties;

    using index_t = int;

    std::vector<std::pair<index_t, PD>> deleted, added;
    std::vector<std::tuple<QString, PD, PD>> changed;

    // search, which entries are  deletedf and changed
    for (int leftIdx = 0; leftIdx < leftEntries.size(); ++leftIdx) {

        const auto& leftEntry = leftEntries[leftIdx];

        auto iter = std::find_if(rightEntries.constBegin(),
                                 rightEntries.constEnd(),
                                 [&leftEntry](const PD& rightData) {
            return rightData.name == leftEntry.name;
        });

        if (iter == rightEntries.cend())
        {
            // this entry was removed
            deleted.emplace_back(leftIdx, leftEntry);
        }
        else
        {
            // check, whether this entry has been changed
            const auto& rightEntry = *iter;
            assert(leftEntry.name == rightEntry.name);

            if (leftEntry.hash != rightEntry.hash)
            {
                changed.emplace_back(leftEntry.name, leftEntry, rightEntry);
            }
        }
    }

    // search for added entries
    for (int rightIdx = 0; rightIdx < rightEntries.size(); ++rightIdx) {

        const auto& rightEntry = rightEntries[rightIdx];

        auto iter = std::find_if(leftEntries.constBegin(),
                                 leftEntries.constEnd(),
                                 [&rightEntry](const PD& leftData) {
            return leftData.name == rightEntry.name;
        });

        if (iter == leftEntries.cend())
        {
            // this entry was removed
            added.emplace_back(rightIdx, rightEntry);
        }
    }

    assert(!changed.empty() || !deleted.empty() || !added.empty());

    for (const auto& addedElement : added)
    {
        const auto idx = addedElement.first;
        const auto& elem = addedElement.second;
        handleContainerElementAdded(leftContainer.name, idx, elem, diffRoot);
    }

    for (const auto& removedElement : deleted)
    {
        const auto idx = removedElement.first;
        const auto& elem = removedElement.second;
        handleContainerElementRemoved(leftContainer.name, idx, elem, diffRoot);
    }

    for (const auto& changedElem : changed)
    {
        const auto& nameUUID = std::get<0>(changedElem);
        const auto& before = std::get<1>(changedElem);
        const auto& after = std::get<2>(changedElem);

        handleContainerElementChanged(leftContainer.name, nameUUID,
                                      before, after, diffRoot);
    }
}

void
GtObjectMementoDiff::handleContainerElementAdded(
    QString containerName,
    int idx,
    const PD& data,
    QDomElement& diffRoot)
{
    auto newEntryElem = createElement(
        GtObjectIO::S_DIFF_PROPCONT_ENTRY_ADDED_TAG);

    newEntryElem.setAttribute(GtObjectIO::S_NAME_TAG, containerName);
    newEntryElem.setAttribute(GtObjectIO::S_DIFF_INDEX_TAG, idx);

    QDomElement propertyElem = GtObjectIO().toDomElement(data, *this);
    newEntryElem.appendChild(propertyElem);

    diffRoot.appendChild(newEntryElem);
}

void
GtObjectMementoDiff::handleContainerElementRemoved(
    QString containerName,
    int idx,
    const PD& data,
    QDomElement& diffRoot)
{
    auto removedElem = createElement(
        GtObjectIO::S_DIFF_PROPCONT_ENTRY_REMOVE_TAG);

    removedElem.setAttribute(GtObjectIO::S_NAME_TAG, containerName);
    removedElem.setAttribute(GtObjectIO::S_DIFF_INDEX_TAG, idx);

    QDomElement propertyElem = GtObjectIO().toDomElement(data, *this);
    removedElem.appendChild(propertyElem);

    diffRoot.appendChild(removedElem);
}

void
GtObjectMementoDiff::handleContainerElementChanged(const QString& containerName,
                                                   const QString& elementName,
                                                   const PD& before,
                                                   const PD& after,
                                                   QDomElement& diffRoot)
{
    auto changedElem = createElement(
        GtObjectIO::S_DIFF_PROPCONT_ENTRY_CHANGE_TAG);

    changedElem.setAttribute(GtObjectIO::S_NAME_TAG, containerName);
    changedElem.setAttribute("entryName", elementName);

    detectPropertyChanges(before.childProperties, after.childProperties, changedElem);

    diffRoot.appendChild(changedElem);
}

void
GtObjectMementoDiff::handlePropertyChange(const PD& leftProp,
                                          const PD& rightProp,
                                          QDomElement& diffRoot)
{
    // distinguish "types"
    QDomElement diffObj;
    bool diffObjEmpty = true;

    if (GtObjectIO::usePropertyList(leftProp.data()))
    {
        diffObj = this->createElement(GtObjectIO::S_DIFF_PROPLIST_CHANGE_TAG);
        diffObj.setAttribute(GtObjectIO::S_NAME_TAG, leftProp.name);

        diffObjEmpty = false;
        QDomElement oldVal = this->createElement(GtObjectIO::S_DIFF_OLDVAL_TAG);
        QDomElement newVal = this->createElement(GtObjectIO::S_DIFF_NEWVAL_TAG);

        QString leftVal, leftType;
        QString rightVal, rightType;
        GtObjectIO::propertyListStringType(leftProp.data(), leftVal, leftType);
        GtObjectIO::propertyListStringType(rightProp.data(), rightVal, rightType);

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
        diffObj.setAttribute(GtObjectIO::S_TYPE_TAG, leftProp.dataType());

        // handle value changes
        if (leftProp.data() != rightProp.data())
        {
            diffObjEmpty = false;
            QDomElement oldVal = this->createElement(GtObjectIO::S_DIFF_OLDVAL_TAG);
            QDomElement newVal = this->createElement(GtObjectIO::S_DIFF_NEWVAL_TAG);

            oldVal.appendChild(this->createTextNode(GtObjectIO::variantToString(leftProp.data())));
            newVal.appendChild(this->createTextNode(GtObjectIO::variantToString(rightProp.data())));

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

    // TODO: handle child properties change

    if (!diffObjEmpty)
    {
        diffRoot.appendChild(diffObj);
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

