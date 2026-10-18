/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
#include <QJsonDocument>
#include <QJsonObject>
#include <QtGlobal>
#include <QDebug>
#include <QMetaType>
#include <QStringRef>

#include <cfloat>
#include <optional>
#include <typeinfo>

#include "gt_object.h"
#include "gt_externalizedobject.h"
#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"
#include "gt_abstractproperty.h"
#include "gt_dynamicpropertyfactory.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"
#include "gt_exceptions.h"
#include "gt_xmlutilities.h"
#include "gt_xmlexpr.h"
#include "gt_objectuuidmap.h"

#include "gt_objectio.h"

const QSet<QString> GtObjectIO::S_LISTTYPES = QSet<QString>()
                                            << QStringLiteral("QList<int>")
                                            << QStringLiteral("QList<bool>")
                                            << QStringLiteral("QList<QPointF>")
                                            << QStringLiteral("QVector<double>")
                                            << QStringLiteral("QList<double>")
                                            << QStringLiteral("QStringList");


namespace
{

    bool variantsEqual(const QVariant& lhs, const QVariant& rhs)
    {
        if (lhs.userType() != rhs.userType()) return false;
        if (lhs == rhs) return true;

        QByteArray lhsData;
        QByteArray rhsData;
        QDataStream lhsStream(&lhsData, QIODevice::WriteOnly);
        QDataStream rhsStream(&rhsData, QIODevice::WriteOnly);
        lhsStream << lhs;
        rhsStream << rhs;
        return lhsStream.status() == QDataStream::Ok &&
               rhsStream.status() == QDataStream::Ok && lhsData == rhsData;
    }

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
             std::make_pair(gt::xml::S_DIFF_DYNAMIC_PROPERTIES_TAG,
                [](GtObject& obj, const QDomElement& tag, DiffMode mode) {
                    return GtObjectIO::applyDynamicPropertyChanges(
                        obj, tag, mode == DiffMode::Revert);
                }),

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
    return toMementoImpl(o, clone, nullptr);
}

GtObjectMemento
GtObjectIO::toMementoImpl(const GtObject* o, bool clone, GtObjectUUIDMap* uuidMap)
{
    // global object element
    GtObjectMemento memento;

    if (!o)
    {
        return memento;
    }

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

    if(uuidMap)
    {
        uuidMap->insert(o->uuid(), uuid);
    }

    memento.setUuid(uuid);

    // object name
    memento.setIdent(o->objectName());

    memento.setFlagEnabled(GtObjectMemento::SaveAsOwnFile, o->saveAsOwnFile());

    // child objects
    auto const directChildren = o->findDirectChildren();
    memento.childObjects.reserve(directChildren.size());

    for (const GtObject* child : directChildren)
    {
        // recursion through GtObjectMemento constructor
        memento.childObjects.push_back(toMementoImpl(child, clone, uuidMap));
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

    // indikate as link flag, if object should be stored onto own file
    if (memento.isFlagEnabled(GtObjectMemento::SaveAsOwnFile))
    {
        if (!memento.isFlagEnabled(GtObjectMemento::IsUnresolved))
        {
            element.setAttribute(gt::xml::S_ASLINK_TAG, "true");
        }
        else
        {
            // we should only write the reference, but not the linked file
            // to avoid overwriting an unresolved file
            element.setAttribute(gt::xml::S_ASLINK_TAG, "refonly");
        }
    }

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

    if (e.tagName() == gt::xml::S_OBJECTREF_TAG)
    {
        // The object ref could not be resolved by the reader, so we
        // need to make a dummy
        memento.setFlagEnabled(GtObjectMemento::IsUnresolved, true);
        return memento;
    }

    // store property information
    memento.properties = readProperties(e);
    memento.propertyContainers = readPropertyContainers(e);

    const QDomElement dynamic =
        e.firstChildElement(QStringLiteral("dynamicProperties"));
    if (!dynamic.isNull())
    {
        if (dynamic.attribute(QStringLiteral("formatVersion")) !=
            QString::number(
                GtObjectMemento::DynamicPropertyData::CurrentFormatVersion))
        {
            memento.dynamicPropertiesValid = false;
            gtError() << "Unsupported dynamic property format version"
                      << dynamic.attribute(QStringLiteral("formatVersion"));
        }
        else
        {
            for (QDomElement entry =
                     dynamic.firstChildElement(QStringLiteral("property"));
                 !entry.isNull();
                 entry = entry.nextSiblingElement(QStringLiteral("property")))
            {
                GtObjectMemento::DynamicPropertyData data;
                data.typeId = entry.attribute(QStringLiteral("typeId"));
                data.ident = entry.attribute(QStringLiteral("ident"));
                data.name = entry.attribute(QStringLiteral("name"));
                const QString active = entry.attribute(QStringLiteral("active"));
                data.isActive = active.isEmpty() || QVariant(active).toBool();
                const auto def = QJsonDocument::fromJson(
                    entry.firstChildElement(QStringLiteral("definition"))
                        .text()
                        .toUtf8());
                bool error = false;
                const QDomElement listValue = entry.firstChildElement(
                    gt::xml::S_PROPERTYLIST_TAG);
                if (!listValue.isNull())
                {
                    data.value = propertyListToVariant(
                        listValue.text(),
                        listValue.attribute(gt::xml::S_TYPE_TAG));
                    error = !data.value.isValid();
                }
                else
                {
                    const auto property = readProperty(
                        entry.firstChildElement(gt::xml::S_PROPERTY_TAG),
                        error);
                    data.value = property.data();
                }
                if (!def.isObject() || error || data.typeId.isEmpty() ||
                    data.ident.isEmpty())
                {
                    memento.dynamicPropertiesValid = false;
                    gtError()
                        << "Invalid dynamic property record" << data.ident;
                    continue;
                }
                data.definition = def.object().toVariantMap();
                const QDomElement nested = entry.firstChildElement(
                    QStringLiteral("nestedProperties"));
                if (!nested.isNull())
                    data.nestedProperties = readProperties(nested);
                memento.dynamicProperties.append(std::move(data));
            }
        }
    }

    // child objects
    QDomElement children = e.firstChildElement(gt::xml::S_OBJECTLIST_TAG);

    if (!children.isNull())
    {
        QDomElement compElement =
            children.firstChildElement();

        while (!compElement.isNull())
        {
            if (compElement.tagName() != gt::xml::S_OBJECT_TAG && compElement.tagName() != gt::xml::S_OBJECTREF_TAG)
            {
                compElement = compElement.nextSiblingElement();
                continue;
            }

            // recursion through GtObjectMemento constructor
            memento.childObjects.push_back(GtObjectMemento(compElement));

            compElement = compElement.nextSiblingElement();
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

bool
GtObjectIO::applyDynamicPropertyChanges(GtObject& object,
                                        const QDomElement& changes,
                                        bool revert)
{
    using Data = GtObjectMemento::DynamicPropertyData;
    struct Operation
    {
        QString id;
        QString kind;
        std::optional<Data> oldData;
        std::optional<Data> newData;
    };

    auto readPayload = [](const QDomElement& operation, const char* tag,
                          std::optional<Data>& result, bool required) {
        QDomElement wrapper = operation.firstChildElement(tag);
        if (wrapper.isNull()) return !required;
        if (!wrapper.nextSiblingElement(tag).isNull()) return false;
        QDomElement element = wrapper.firstChildElement(gt::xml::S_OBJECT_TAG);
        if (element.isNull() || !element.nextSiblingElement().isNull())
            return false;
        GtObjectMemento memento = GtObjectIO().toMemento(element);
        if (!memento.dynamicPropertiesValid ||
            memento.dynamicProperties.size() != 1 ||
            !memento.childObjects.isEmpty()) return false;
        result = memento.dynamicProperties.front();
        return true;
    };

    QVector<Operation> operations;
    QSet<QString> operationIds;
    for (QDomElement entry = changes.firstChildElement(); !entry.isNull();
         entry = entry.nextSiblingElement())
    {
        if (entry.tagName() != gt::xml::S_DIFF_DYNAMIC_PROPERTY_TAG)
            return false;
        Operation op;
        op.id = entry.attribute(QStringLiteral("id"));
        op.kind = entry.attribute(gt::xml::S_DIFF_OPERATION_TAG);
        if (op.id.isEmpty() || operationIds.contains(op.id)) return false;
        operationIds.insert(op.id);
        const bool wantsOld = op.kind == QLatin1String("remove") ||
                              op.kind == QLatin1String("change");
        const bool wantsNew = op.kind == QLatin1String("add") ||
                              op.kind == QLatin1String("change");
        if ((!wantsOld && op.kind != QLatin1String("add")) ||
            (!wantsNew && op.kind != QLatin1String("remove")) ||
            !readPayload(entry, gt::xml::S_DIFF_OLDVAL_TAG,
                         op.oldData, wantsOld) ||
            !readPayload(entry, gt::xml::S_DIFF_NEWVAL_TAG,
                         op.newData, wantsNew)) return false;
        if ((op.oldData && (op.oldData->ident != op.id ||
                            op.oldData->typeId.isEmpty())) ||
            (op.newData && (op.newData->ident != op.id ||
                            op.newData->typeId.isEmpty()))) return false;
        operations.append(std::move(op));
    }
    if (operations.isEmpty()) return false;

    auto equal = [](const Data& lhs, const Data& rhs) {
        auto equalProperty = [&](const auto& self,
                                 const GtObjectMemento::PropertyData& a,
                                 const GtObjectMemento::PropertyData& b)
            -> bool {
            if (a.name != b.name || a.isActive != b.isActive ||
                a.type() != b.type() || a.dataType() != b.dataType() ||
                !variantsEqual(a.data(), b.data()) ||
                a.childProperties.size() != b.childProperties.size())
                return false;
            for (int i = 0; i < a.childProperties.size(); ++i)
                if (!self(self, a.childProperties[i], b.childProperties[i]))
                    return false;
            return true;
        };
        if (lhs.nestedProperties.size() != rhs.nestedProperties.size())
            return false;
        for (int i = 0; i < lhs.nestedProperties.size(); ++i)
            if (!equalProperty(equalProperty, lhs.nestedProperties[i],
                               rhs.nestedProperties[i])) return false;
        return lhs.typeId == rhs.typeId && lhs.ident == rhs.ident &&
               lhs.name == rhs.name && lhs.definition == rhs.definition &&
               variantsEqual(lhs.value, rhs.value) &&
               lhs.isActive == rhs.isActive;
    };
    auto currentData = [](const GtAbstractProperty& property,
                          Data& result) {
        result.ident = property.ident();
        result.name = property.objectName();
        result.value = property.valueToVariant();
        result.isActive = property.isActive();
        GtObjectIO io;
        QSet<QString> stored{property.ident()};
        for (const GtAbstractProperty* child : property.fullProperties())
            io.writePropertyHelper(result.nestedProperties, stored, child);
        const QVariant typeId = property.property("gt.dynamicTypeId");
        if (typeId.isValid() && !typeId.toString().isEmpty())
        {
            result.typeId = typeId.toString();
            return GtDynamicPropertyFactory::definition(
                result.typeId, property, result.definition);
        }
        return GtDynamicPropertyFactory::definition(
            property, result.typeId, result.definition);
    };

    QHash<QString, GtAbstractProperty*> current;
    for (GtAbstractProperty* property : object.dynamicProperties())
    {
        if (property->ident().isEmpty() || current.contains(property->ident()))
            return false;
        current.insert(property->ident(), property);
    }

    std::vector<QString> removedIds;
    std::vector<std::unique_ptr<GtAbstractProperty>> additions;
    for (const Operation& op : operations)
    {
        const Data* expected = nullptr;
        const Data* desired = nullptr;
        if (!revert)
        {
            expected = op.oldData ? &*op.oldData : nullptr;
            desired = op.newData ? &*op.newData : nullptr;
        }
        else
        {
            expected = op.newData ? &*op.newData : nullptr;
            desired = op.oldData ? &*op.oldData : nullptr;
        }

        GtAbstractProperty* existing = current.value(op.id, nullptr);
        if (expected)
        {
            Data actual;
            if (!existing || !currentData(*existing, actual) ||
                !equal(actual, *expected))
            {
                return false;
            }
            removedIds.push_back(op.id);
        }
        else if (existing)
        {
            return false;
        }
    }

    for (const Operation& op : operations)
    {
        const Data* desired = !revert
            ? (op.newData ? &*op.newData : nullptr)
            : (op.oldData ? &*op.oldData : nullptr);
        if (desired)
        {
            auto property = GtDynamicPropertyFactory::create(
                desired->typeId, desired->ident, desired->name,
                desired->definition);
            if (!property || property->parent() ||
                property->thread() != object.thread() ||
                property->ident() != desired->ident ||
                property->objectName() != desired->name ||
                !property->isValid() ||
                (!qobject_cast<GtPropertyStructInstance*>(property.get()) &&
                 !property->setValueFromVariant(desired->value, QString())))
                return false;
            property->setActive(desired->isActive);
            QHash<QString, GtAbstractProperty*> nestedById;
            std::vector<GtAbstractProperty*> pending;
            for (GtAbstractProperty* child : property->fullProperties())
                pending.push_back(child);
            while (!pending.empty())
            {
                GtAbstractProperty* nested = pending.back();
                pending.pop_back();
                if (nested->ident().isEmpty() ||
                    nestedById.contains(nested->ident())) return false;
                nestedById.insert(nested->ident(), nested);
                for (GtAbstractProperty* child : nested->fullProperties())
                    pending.push_back(child);
            }
            auto applyNested = [&](const auto& self,
                                   const GtObjectMemento::PropertyData& data)
                -> bool {
                GtAbstractProperty* nested = nestedById.value(data.name,
                                                                nullptr);
                if (!nested) return false;
                nested->setActive(data.isActive);
                if (data.type() != GtObjectMemento::PropertyData::STRUCT_T &&
                    !nested->setValueFromVariant(data.data(), QString()))
                    return false;
                for (const auto& child : data.childProperties)
                    if (!self(self, child)) return false;
                return true;
            };
            for (const auto& nested : desired->nestedProperties)
                if (!applyNested(applyNested, nested)) return false;
            Data created;
            if (!currentData(*property, created) || !equal(created, *desired))
                return false;
            additions.push_back(std::move(property));
        }
    }

    // Callbacks used by the factory may re-enter the object. Confirm the
    // expected state still matches immediately before committing the batch.
    QHash<QString, GtAbstractProperty*> latestDynamic;
    for (GtAbstractProperty* property : object.dynamicProperties())
    {
        if (property->ident().isEmpty() ||
            latestDynamic.contains(property->ident())) return false;
        latestDynamic.insert(property->ident(), property);
    }
    for (const Operation& op : operations)
    {
        const Data* expected = !revert
            ? (op.oldData ? &*op.oldData : nullptr)
            : (op.newData ? &*op.newData : nullptr);
        GtAbstractProperty* existing = object.findProperty(op.id);
        GtAbstractProperty* dynamic = latestDynamic.value(op.id, nullptr);
        if (expected)
        {
            Data actual;
            if (!dynamic || object.findProperty(op.id) != dynamic ||
                !currentData(*dynamic, actual) || !equal(actual, *expected))
                return false;
        }
        else if (dynamic || existing)
        {
            return false;
        }
    }

    return object.replaceDynamicProperties(removedIds, std::move(additions));
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
        if (obj->dynamicProperties().contains(
                const_cast<GtAbstractProperty*>(property)))
        {
            continue;
        }
        writePropertyHelper(memento.properties, storedProps, property);
    }

    // dynamic properties
    for (const GtAbstractProperty* property : obj->dynamicProperties())
    {
        GtObjectMemento::DynamicPropertyData data;
        data.ident = property->ident();
        data.name = property->objectName();
        data.isActive = property->isActive();
        data.value = property->valueToVariant();
        if (!property->storeToMemento()) continue;
        if (!GtDynamicPropertyFactory::definition(*property, data.typeId,
                                                   data.definition))
        {
            memento.dynamicPropertiesValid = false;
            gtError() << "No dynamic property serializer registered for"
                      << property->metaObject()->className();
            continue;
        }
        QSet<QString> stored{property->ident()};
        for (const GtAbstractProperty* child : property->fullProperties())
            writePropertyHelper(data.nestedProperties, stored, child);
        memento.dynamicProperties.append(std::move(data));
    }

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

    if (!memento.dynamicProperties.isEmpty() || !memento.dynamicPropertiesValid)
    {
        QDomElement dynamic =
            doc.createElement(QStringLiteral("dynamicProperties"));
        dynamic.setAttribute(QStringLiteral("formatVersion"),
                             memento.dynamicPropertiesValid
                                 ? GtObjectMemento::DynamicPropertyData::CurrentFormatVersion
                                 : 0);
        for (const auto& property : memento.dynamicProperties)
        {
            QDomElement entry = doc.createElement(QStringLiteral("property"));
            entry.setAttribute(QStringLiteral("typeId"), property.typeId);
            entry.setAttribute(QStringLiteral("ident"), property.ident);
            entry.setAttribute(QStringLiteral("name"), property.name);
            entry.setAttribute(QStringLiteral("active"), property.isActive);
            QDomElement definition =
                doc.createElement(QStringLiteral("definition"));
            definition.appendChild(doc.createTextNode(QString::fromUtf8(
                QJsonDocument::fromVariant(property.definition)
                    .toJson(QJsonDocument::Compact))));
            entry.appendChild(definition);
            if (S_LISTTYPES.contains(QString::fromLatin1(
                    property.value.typeName())))
            {
                entry.appendChild(propertyListToDomElement(
                    property.ident, property.value, doc));
            }
            else
            {
                entry.appendChild(
                    propertyToDomElement(property.ident, property.value,
                                         property.value.typeName(), doc));
            }
            if (!property.nestedProperties.isEmpty())
            {
                QDomElement nested =
                    doc.createElement(QStringLiteral("nestedProperties"));
                for (const auto& nestedProperty : property.nestedProperties)
                    nested.appendChild(toDomElement(nestedProperty, doc));
                entry.appendChild(nested);
            }
            dynamic.appendChild(entry);
        }
        root.appendChild(dynamic);
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
    if (!element.firstChildElement(gt::xml::S_PROPERTY_TAG).isNull())
    {
        if (fieldType.isEmpty() || fieldName.isEmpty()) return propData;
        propData.toStruct(fieldType);
        propData.childProperties = readPropertyElements(
            readProperty, element, gt::xml::S_PROPERTY_TAG);
    }
    else
    {
        propData.setData(propertyToVariant(element.text(), fieldType));
    }

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
    auto varType = gt::metaTypeId(var);

    static auto type_QDoubleVector = gt::metaTypeIdFromName(
                "QVector<double>");
    static auto type_QIntList = gt::metaTypeIdFromName(
                "QList<int>");
    static auto type_QBoolList = gt::metaTypeIdFromName(
                "QList<bool>");
    static auto type_QPointFList = gt::metaTypeIdFromName(
                "QList<QPointF>");
    static auto type_QStringList = gt::metaTypeIdFromName(
                "QStringList");

    assert(QMetaType::isRegistered(type_QDoubleVector));
    assert(QMetaType::isRegistered(type_QIntList));
    assert(QMetaType::isRegistered(type_QBoolList));
    assert(QMetaType::isRegistered(type_QPointFList));
    assert(QMetaType::isRegistered(type_QStringList));

    if (varType == type_QDoubleVector)
    {
        valStr = listToString(var.value<QVector<double> >());
        typeStr = QStringLiteral("double");
    }
    else if (varType == type_QIntList)
    {
        valStr = listToString(var.value<QList<int> >());
        typeStr = QStringLiteral("int");
    }
    else if (varType == type_QBoolList)
    {
        valStr = listToString(var.value<QList<bool> >());
        typeStr = QStringLiteral("bool");
    }
    else if (varType == type_QPointFList)
    {
        valStr = listToString(var.value<QList<QPointF> >());
        typeStr = QStringLiteral("QPointF");
    }
    else if (varType == type_QStringList)
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

    auto v_type = gt::metaTypeIdFromName(p);
    QVariant retval(value);

    if (QMetaType::isRegistered(v_type))
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
