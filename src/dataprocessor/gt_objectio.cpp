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
#include <QtGlobal>
#include <QDebug>
#include <QMetaType>
#include <QStringRef>

#include <cfloat>
#include <algorithm>
#include <limits>
#include <typeinfo>

#include "gt_object.h"
#include "gt_externalizedobject.h"
#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"
#include "gt_abstractproperty.h"
#include "property/gt_dummyproperty.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"
#include "property/gt_propertyconfiguration.h"
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
    /// Collect every dynamic descendant, including roots excluded from storage.
    void collectDynamicPropertyIds(const GtAbstractProperty& property,
                                   QSet<QString>& ids)
    {
        if (ids.contains(property.ident())) return;
        ids.insert(property.ident());
        for (const auto* child : property.fullProperties())
        {
            if (child) collectDynamicPropertyIds(*child, ids);
        }
    }

    /** Move a dynamic tree out of the flat property list. Nodes without their
     * own stored value pass their stored descendants up to the enclosing node.
     * Returns whether this node had its own entry in the flat list.
     */
    bool extractDynamicPropertyData(
        const GtAbstractProperty& property,
        QVector<GtObjectMemento::PropertyData>& flatProperties,
        GtObjectMemento::PropertyData& data)
    {
        auto stored = std::find_if(flatProperties.begin(), flatProperties.end(),
                                   [&property](const auto& entry) {
                                       return entry.name == property.ident();
                                   });
        const bool hasStoredData = stored != flatProperties.end();
        if (hasStoredData)
        {
            data = std::move(*stored);
            flatProperties.erase(stored);
        }
        for (const auto* child : property.fullProperties())
        {
            GtObjectMemento::PropertyData childData;
            if (extractDynamicPropertyData(*child, flatProperties, childData))
                data.childProperties.push_back(std::move(childData));
            else
                data.childProperties += childData.childProperties;
        }
        return hasStoredData;
    }

    /// Configuration XML carries explicit scalar types to preserve QVariant types.
    QDomElement dynamicConfigValueToDom(const QVariant& value,
                                        QDomDocument& doc, const QString& tag,
                                        const QString& key = {})
    {
        QDomElement element = doc.createElement(tag);
        if (!key.isNull()) element.setAttribute(QStringLiteral("key"), key);
        if (!value.isValid() || value.isNull())
        {
            element.setAttribute(QStringLiteral("type"),
                                 QStringLiteral("null"));
        }
        else if (value.userType() == QMetaType::QVariantMap)
        {
            element.setAttribute(QStringLiteral("type"),
                                 QStringLiteral("object"));
            const auto map = value.toMap();
            for (auto iter = map.cbegin(); iter != map.cend(); ++iter)
            {
                element.appendChild(dynamicConfigValueToDom(
                    iter.value(), doc, QStringLiteral("entry"), iter.key()));
            }
        }
        else if (value.userType() == QMetaType::QVariantList)
        {
            element.setAttribute(QStringLiteral("type"),
                                 QStringLiteral("array"));
            for (const auto& item : value.toList())
            {
                element.appendChild(
                    dynamicConfigValueToDom(item, doc, QStringLiteral("item")));
            }
        }
        else
        {
            QString type;
            switch (value.userType())
            {
            case QMetaType::Bool:
                type = QStringLiteral("bool");
                break;
            case QMetaType::Int:
                type = QStringLiteral("int");
                break;
            case QMetaType::UInt:
                type = QStringLiteral("uint");
                break;
            case QMetaType::LongLong:
                type = QStringLiteral("int64");
                break;
            case QMetaType::ULongLong:
                type = QStringLiteral("uint64");
                break;
            case QMetaType::Double:
                type = QStringLiteral("double");
                break;
            case QMetaType::QString:
                type = QStringLiteral("string");
                break;
            default:
                return {};
            }
            element.setAttribute(QStringLiteral("type"), type);
            const QString text =
                value.userType() == QMetaType::Double
                    ? QString::number(value.toDouble(), 'g',
                                      std::numeric_limits<double>::max_digits10)
                    : value.toString();
            element.appendChild(doc.createTextNode(text));
        }
        return element;
    }

    bool dynamicConfigValueFromDom(const QDomElement& element, QVariant& value,
                                   int depth = 0)
    {
        if (depth > gt::PropertyConfigurationMaxDepth) return false;
        const auto type = element.attribute(QStringLiteral("type"));
        if (type == QStringLiteral("null"))
        {
            value = QVariant{};
            return true;
        }
        if (type == QStringLiteral("object"))
        {
            gt::PropertyConfiguration map;
            auto child = element.firstChildElement();
            while (!child.isNull())
            {
                QVariant item;
                if (child.tagName() != QStringLiteral("entry") ||
                    !child.hasAttribute(QStringLiteral("key")) ||
                    !dynamicConfigValueFromDom(child, item, depth + 1))
                {
                    return false;
                }
                const auto key = child.attribute(QStringLiteral("key"));
                if (map.contains(key)) return false;
                map.insert(key, item);
                child = child.nextSiblingElement();
            }
            value = map;
            return true;
        }
        if (type == QStringLiteral("array"))
        {
            QVariantList list;
            auto child = element.firstChildElement();
            while (!child.isNull())
            {
                QVariant item;
                if (child.tagName() != QStringLiteral("item") ||
                    !dynamicConfigValueFromDom(child, item, depth + 1))
                {
                    return false;
                }
                list.push_back(item);
                child = child.nextSiblingElement();
            }
            value = list;
            return true;
        }

        const auto text = element.text();
        bool okay = true;
        if (type == QStringLiteral("bool"))
        {
            if (text != QStringLiteral("true") &&
                text != QStringLiteral("false"))
                return false;
            value = text == QStringLiteral("true");
        }
        else if (type == QStringLiteral("int"))
        {
            const int parsed = text.toInt(&okay);
            if (okay) value = parsed;
        }
        else if (type == QStringLiteral("uint"))
        {
            const uint parsed = text.toUInt(&okay);
            if (okay) value = parsed;
        }
        else if (type == QStringLiteral("int64"))
        {
            const qlonglong parsed = text.toLongLong(&okay);
            if (okay) value = parsed;
        }
        else if (type == QStringLiteral("uint64"))
        {
            const qulonglong parsed = text.toULongLong(&okay);
            if (okay) value = parsed;
        }
        else if (type == QStringLiteral("double"))
        {
            const double parsed = text.toDouble(&okay);
            if (okay) value = parsed;
        }
        else if (type == QStringLiteral("string"))
        {
            value = text;
        }
        else
        {
            return false;
        }
        return okay;
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

    bool handleDynamicPropertyDefinitionsChange(GtObject& target,
                                                const QDomElement& change,
                                                DiffMode mode);

    using diffFunc = std::function<bool(GtObject&, const QDomElement&, DiffMode)>;
    std::map<QString, diffFunc> const & getDiffFuncs()
    {
        static std::map<QString, diffFunc> funcMap = {
            std::make_pair(gt::xml::S_DIFF_OBJ_REMOVE_TAG,
                           handleObjectAddRemove),
            std::make_pair(gt::xml::S_DIFF_OBJ_ADD_TAG, handleObjectAddRemove),
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
            std::make_pair(QStringLiteral("dynamicPropertiesChange"),
                           handleDynamicPropertyDefinitionsChange),

            std::make_pair(
                gt::xml::S_DIFF_PROP_CHANGE_TAG,
                [](GtObject& obj, const QDomElement& tag, DiffMode mode) {
                    return  handlePropertyNodeChange(obj, tag, false,
                                                    mode);
                }),

            std::make_pair(
                gt::xml::S_DIFF_PROPLIST_CHANGE_TAG,
                [](GtObject& obj, const QDomElement& tag, DiffMode mode) {
                    return  handlePropertyNodeChange(obj, tag, true,
                                                    mode);
                }),
        };

        return funcMap;
    }
} // namespace

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
        // First collect all values, then move dynamic trees into their definitions.
        writeProperties(memento, o);
        writeDynamicProperties(memento, o);
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

void
GtObjectIO::writeDynamicProperties(GtObjectMemento& memento, const GtObject* o)
{
    QSet<QString> dynamicPropertyIds;
    for (auto* property : o->properties())
    {
        if (!property || property->propertyFactoryTypeId().isEmpty()) continue;

        collectDynamicPropertyIds(*property, dynamicPropertyIds);

        if (!property->storeToMemento()) continue;

        GtObjectMemento::DynamicPropertyData definition;
        const auto* dummy = qobject_cast<const GtDummyProperty*>(property);
        if (dummy)
            definition = dummy->originalDefinition();
        else
        {
            definition.typeId = property->propertyFactoryTypeId();
            definition.ident = property->ident();
            definition.name = property->objectName();
            definition.brief = property->brief();
            definition.configuration = property->configuration();
            if (property->isReadOnlyConfigured())
                definition.flags |=
                    GtObjectMemento::DynamicPropertyData::ReadOnly;
            if (property->isOptional())
                definition.flags |=
                    GtObjectMemento::DynamicPropertyData::Optional;
            if (property->isHidden())
                definition.flags |=
                    GtObjectMemento::DynamicPropertyData::Hidden;
            if (property->isMonitoring())
                definition.flags |=
                    GtObjectMemento::DynamicPropertyData::Monitoring;
            if (property->category() == GtAbstractProperty::Custom)
                definition.category = property->categoryString();
        }
        if (!dummy)
        {
            definition.propertyData.name = property->ident();
            definition.propertyData.toStruct(
                QString::fromLatin1(property->metaObject()->className()));
            extractDynamicPropertyData(*property, memento.properties,
                                       definition.propertyData);
        }
        memento.dynamicProperties.push_back(std::move(definition));
    }
    // Remove remaining dynamic descendants even when their root opted out
    // of storage. They must never leak into the static property list.
    memento.properties.erase(
        std::remove_if(
            memento.properties.begin(), memento.properties.end(),
            [&dynamicPropertyIds](const GtObjectMemento::PropertyData& data) {
                return dynamicPropertyIds.contains(data.name);
            }),
        memento.properties.end());
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

    writeDynamicProperties(doc, element, memento);

    // children
    if (!skipChildren && !memento.childObjects.isEmpty())
    {
        QDomElement childElement = doc.createElement(gt::xml::S_OBJECTLIST_TAG);

        foreach (const GtObjectMemento& child, memento.childObjects)
        {
            childElement.appendChild(toDomElement(child, doc));
        }

        element.appendChild(childElement);
    }

    return element;
}

void
GtObjectIO::writeDynamicProperties(QDomDocument& doc, QDomElement& element,
                                   const GtObjectMemento& memento)
{
    if (!memento.dynamicProperties.isEmpty())
    {
        QDomElement definitions =
            doc.createElement(QStringLiteral("dynamicProperties"));
        for (const auto& property : memento.dynamicProperties)
        {
            QDomElement entry = doc.createElement(QStringLiteral("property"));
            entry.setAttribute(QStringLiteral("typeId"), property.typeId);
            entry.setAttribute(QStringLiteral("ident"), property.ident);
            entry.setAttribute(QStringLiteral("name"), property.name);
            entry.setAttribute(QStringLiteral("brief"), property.brief);
            entry.setAttribute(QStringLiteral("flags"),
                               QString::number(property.flags));
            entry.setAttribute(QStringLiteral("category"), property.category);
            QDomElement config = dynamicConfigValueToDom(
                property.configuration, doc, QStringLiteral("config"));
            entry.appendChild(config);
            QDomElement propertyData =
                doc.createElement(QStringLiteral("propertyData"));
            if (!property.propertyData.name.isEmpty())
            {
                propertyData.appendChild(
                    toDomElement(property.propertyData, doc));
            }
            entry.appendChild(propertyData);
            definitions.appendChild(entry);
        }
        element.appendChild(definitions);
    }
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

    if (!readDynamicProperties(e, memento)) return GtObjectMemento{};

    // child objects
    QDomElement children = e.firstChildElement(gt::xml::S_OBJECTLIST_TAG);

    if (!children.isNull())
    {
        QDomElement compElement = children.firstChildElement();

        while (!compElement.isNull())
        {
            if (compElement.tagName() != gt::xml::S_OBJECT_TAG &&
                compElement.tagName() != gt::xml::S_OBJECTREF_TAG)
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
GtObjectIO::readDynamicProperties(const QDomElement& e,
                                  GtObjectMemento& memento) const
{
    const auto definitions =
        e.firstChildElement(QStringLiteral("dynamicProperties"));
    if (!definitions.isNull())
    {
        auto entry = definitions.firstChildElement(QStringLiteral("property"));
        while (!entry.isNull())
        {
            GtObjectMemento::DynamicPropertyData property;
            property.typeId = entry.attribute(QStringLiteral("typeId"));
            property.ident = entry.attribute(QStringLiteral("ident"));
            property.name = entry.attribute(QStringLiteral("name"));
            property.brief = entry.attribute(QStringLiteral("brief"));
            if (entry.hasAttribute(QStringLiteral("flags")))
            {
                bool okay = false;
                property.flags =
                    entry.attribute(QStringLiteral("flags")).toUInt(&okay);
                constexpr quint32 knownFlags =
                    GtObjectMemento::DynamicPropertyData::ReadOnly |
                    GtObjectMemento::DynamicPropertyData::Optional |
                    GtObjectMemento::DynamicPropertyData::Hidden |
                    GtObjectMemento::DynamicPropertyData::Monitoring;
                if (!okay || (property.flags & ~knownFlags) != 0) return false;
            }
            else
            {
                const auto readLegacyFlag =
                    [&entry](const QString& key, quint32 flag, quint32& flags) {
                        if (!entry.hasAttribute(key)) return true;
                        const auto value = entry.attribute(key);
                        if (value != QStringLiteral("true") &&
                            value != QStringLiteral("false"))
                            return false;
                        if (value == QStringLiteral("true")) flags |= flag;
                        return true;
                    };
                if (!readLegacyFlag(
                        QStringLiteral("readOnly"),
                        GtObjectMemento::DynamicPropertyData::ReadOnly,
                        property.flags) ||
                    !readLegacyFlag(
                        QStringLiteral("optional"),
                        GtObjectMemento::DynamicPropertyData::Optional,
                        property.flags) ||
                    !readLegacyFlag(
                        QStringLiteral("hidden"),
                        GtObjectMemento::DynamicPropertyData::Hidden,
                        property.flags) ||
                    !readLegacyFlag(
                        QStringLiteral("monitoring"),
                        GtObjectMemento::DynamicPropertyData::Monitoring,
                        property.flags))
                    return false;
            }
            const auto category = entry.attribute(QStringLiteral("category"));
            if (category == QStringLiteral("custom"))
                property.category =
                    entry.attribute(QStringLiteral("customCategory"));
            else if (category != QStringLiteral("main") && !category.isEmpty())
                property.category = category;
            QVariant serializedConfiguration;
            if (!dynamicConfigValueFromDom(
                    entry.firstChildElement(QStringLiteral("config")),
                    serializedConfiguration) ||
                serializedConfiguration.userType() != QMetaType::QVariantMap)
            {
                return false;
            }
            property.configuration = serializedConfiguration.toMap();
            if (!gt::isJsonLikePropertyConfiguration(property.configuration))
            {
                return false;
            }
            const auto propertyDataElement =
                entry.firstChildElement(QStringLiteral("propertyData"));
            if (!propertyDataElement.isNull())
            {
                const auto storedProperties =
                    readProperties(propertyDataElement);
                if (storedProperties.size() > 1) return false;
                if (!storedProperties.isEmpty())
                {
                    property.propertyData = storedProperties.front();
                    if (property.propertyData.name != property.ident)
                    {
                        return false;
                    }
                }
            }
            memento.dynamicProperties.push_back(std::move(property));
            entry = entry.nextSiblingElement(QStringLiteral("property"));
        }
    }

    return true;
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

    if (!element.firstChildElement(gt::xml::S_PROPERTY_TAG).isNull() &&
        QMetaType::type(fieldType.toLatin1().constData()) ==
            QMetaType::UnknownType)
    {
        return readStructPropertyEntry(element, error);
    }

    propData.name = fieldName;
    QString valueText;
    for (auto child = element.firstChild(); !child.isNull();
         child = child.nextSibling())
    {
        if (child.isText() || child.isCDATASection())
        {
            valueText += child.nodeValue();
        }
    }
    propData.setData(propertyToVariant(valueText, fieldType));
    propData.childProperties =
        readPropertyElements(readProperty, element, gt::xml::S_PROPERTY_TAG);

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
    case PD::DATA_T:
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
        for (const auto& subchild : property.childProperties)
        {
            child.appendChild(toDomElement(subchild, doc));
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

bool handleDynamicPropertyDefinitionsChange(GtObject& target,
                                            const QDomElement& change,
                                            DiffMode mode)
{
    const auto oldSnapshot = change.firstChildElement(QStringLiteral("old"))
                                 .firstChildElement(gt::xml::S_OBJECT_TAG);
    const auto newSnapshot = change.firstChildElement(QStringLiteral("new"))
                                 .firstChildElement(gt::xml::S_OBJECT_TAG);
    if (oldSnapshot.isNull() || newSnapshot.isNull()) return false;
    const GtObjectMemento oldMemento(oldSnapshot);
    const GtObjectMemento newMemento(newSnapshot);
    const auto definitionsById = [](const auto& definitions) {
        QHash<QString, GtObjectMemento::DynamicPropertyData> result;
        result.reserve(definitions.size());
        for (const auto& definition : definitions)
        {
            // cppcheck-suppress useStlAlgorithm
            result.insert(definition.ident, definition);
        }
        return result;
    };
    const auto oldById = definitionsById(oldMemento.dynamicProperties);
    const auto newById = definitionsById(newMemento.dynamicProperties);
    QSet<QString> changedIds;
    // cppcheck-suppress useStlAlgorithm
    for (auto iter = oldById.cbegin(); iter != oldById.cend(); ++iter)
    {
        if (!newById.contains(iter.key()) ||
            newById.value(iter.key()) != iter.value())
            changedIds.insert(iter.key());
    }
    // cppcheck-suppress useStlAlgorithm
    for (auto iter = newById.cbegin(); iter != newById.cend(); ++iter)
    {
        if (!oldById.contains(iter.key())) changedIds.insert(iter.key());
    }
    if (changedIds.isEmpty()) return true;

    GtObjectMemento result = GtObjectIO(target.factory()).toMemento(&target);
    auto resultById = definitionsById(result.dynamicProperties);
    const auto& desiredById = mode == DiffMode::Revert ? oldById : newById;
    for (const auto& id : qAsConst(changedIds))
    {
        resultById.remove(id);
        if (desiredById.contains(id))
            resultById.insert(id, desiredById.value(id));
    }
    // Keep the target's existing order and unrelated edits. Append new IDs in
    // snapshot order so applying a diff does not reshuffle the property editor.
    const auto currentDefinitions = std::move(result.dynamicProperties);
    result.dynamicProperties.clear();
    QSet<QString> written;
    for (const auto& current : currentDefinitions)
    {
        if (!resultById.contains(current.ident)) continue;
        result.dynamicProperties.push_back(resultById.value(current.ident));
        written.insert(current.ident);
    }
    const auto& desiredDefinitions = mode == DiffMode::Revert
                                         ? oldMemento.dynamicProperties
                                         : newMemento.dynamicProperties;
    for (const auto& definition : desiredDefinitions)
        if (resultById.contains(definition.ident) &&
            !written.contains(definition.ident))
            result.dynamicProperties.push_back(
                resultById.value(definition.ident));
    return result.mergeDynamicPropertiesTo(target, target.factory());
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
