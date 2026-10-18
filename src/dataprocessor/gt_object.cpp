/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 24.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_objectmemento.h"
#include "gt_abstractobjectfactory.h"
#include "gt_abstractproperty.h"
#include "gt_logging.h"
#include "gt_label.h"
#include "gt_objectmementodiff.h"
#include "gt_objectio.h"
#include "gt_structproperty.h"
#include "gt_propertystructcontainer.h"
#include "gt_objectfactory.h"

#include "gt_object.h"
#include "gt_qtutilities.h"
#include "gt_objectuuidmap.h"

#include <QUuid>
#include <QSet>
#include <QThread>

#include <algorithm>
#include <vector>

struct DummyData
{
    QVector<GtObjectMemento::PropertyData> properties;
    QVector<GtObjectMemento::PropertyData> propertyContainers;
    QVector<GtObjectMemento::DynamicPropertyData> dynamicProperties;
    bool dynamicPropertiesValid{true};
    QString className;
};

struct GtObject::Impl
{
    bool isDummy() const
    {
        return m_isDummy;
    }

    void makeDummy()
    {
        m_isDummy = true;
        properties.clear();
        dynamicProperties.clear();
        allProperties.clear();
        propertyContainer.clear();
    }

    void importDummy(const GtObjectMemento& memento)
    {
        assert(isDummy());
        dummyData.properties = memento.properties;
        dummyData.propertyContainers = memento.propertyContainers;
        dummyData.dynamicProperties = memento.dynamicProperties;
        dummyData.dynamicPropertiesValid = memento.dynamicPropertiesValid;
        dummyData.className = memento.className();
    }

    void exportDummy(GtObjectMemento& memento) const
    {
        assert(isDummy());
        memento.setClassName(dummyData.className);
        memento.properties = dummyData.properties;
        memento.propertyContainers = dummyData.propertyContainers;
        memento.dynamicProperties = dummyData.dynamicProperties;
        memento.dynamicPropertiesValid = dummyData.dynamicPropertiesValid;
    }

    /// Object specific uuid
    QString uuid;

    /// Object flags
    GtObject::ObjectFlags objectFlags;

    /// factory
    GtAbstractObjectFactory* factory;

    /// object properties
    QList<GtAbstractProperty*> properties;

    /// properties created and owned at runtime
    QList<GtAbstractProperty*> dynamicProperties;

    /// declared and runtime root properties exposed through GtObject
    QList<GtAbstractProperty*> allProperties;

    /// dynamic size properties
    std::vector<std::reference_wrapper<GtPropertyStructContainer>> propertyContainer;

    /// A dummy object is not known by the factory but can store properties
    /// as mementos to avoid losing data for unknown objects
    bool m_isDummy{false};

    /// only used, if object isDummy
    /// TODO: use a variant here
    DummyData dummyData;

};

GtObject::GtObject(GtObject* parent) :
    pimpl(std::make_unique<Impl>())
{
    if (parent)
    {
        parent->appendChild(this);
    }

    // initialize uuid
    newUuid();

    // set newly created flag
    setFlag(GtObject::NewlyCreated);

    connect(this, SIGNAL(objectNameChanged(QString)), SLOT(changed()));
}

GtObject::~GtObject() = default;

GtObject::ObjectFlags
GtObject::objectFlags() const
{
    return pimpl->objectFlags;
}

bool
GtObject::isDummy() const
{
    return pimpl->isDummy();
}

bool
GtObject::hasDummyChildren() const
{
    auto const c = findChildren();
    return std::any_of(std::begin(c), std::end(c), [](const GtObject* obj) {
        return obj->isDummy();
    });
}

bool
GtObject::hasDummyParents() const
{
    if (!parentObject())
    {
        return false;
    }

    // recursively search for parent dummy objects
    return parentObject()->isDummy() || parentObject()->hasDummyParents();
}

GtObjectMemento
GtObject::toMemento(bool clone) const
{
    return GtObjectMemento(this, clone);
}

void
GtObject::fromMemento(const GtObjectMemento& memento)
{
    // check for factory
    if (!pimpl->factory)
    {
        gtDebug().verbose() << tr("Factory is null for ") << objectName();
        return;
    }

    if (memento.isFlagEnabled(GtObjectMemento::IsUnresolved))
    {
        gtWarning() << tr("Creating dummy object '%1' due to "
                          "unresolved object file").arg(memento.ident());
        makeDummy();
        setSaveAsOwnFile(true);
    }

    // merge data
    memento.mergeTo(*this, *pimpl->factory);
}

bool
GtObject::applyDiff(GtObjectMementoDiff& diff)
{
    return GtObjectIO::applyDiff(diff, this);
}

bool
GtObject::revertDiff(GtObjectMementoDiff& diff)
{
    return GtObjectIO::revertDiff(diff, this);
}

GtObject*
GtObject::copyClone(bool cloneObject, GtObjectUUIDMap* uuidMap) const
{
    auto* fac = pimpl->factory;

    // check for factory
    if (!fac)
    {
        gtWarning().verbose()
        << QObject::tr("No factory set for %1 object '%2'! (Using default)")
                .arg(cloneObject ? QStringLiteral("copying") : QStringLiteral("cloning"),
                     this->objectName());

        assert(gtObjectFactory);

        fac = gtObjectFactory;
    }

    // generate memento
    GtObjectIO objectIO;
    GtObjectMemento memento = objectIO.toMementoImpl(this, cloneObject, uuidMap);

    if (memento.isNull())
    {
        return nullptr;
    }

    return memento.restore(fac);
}


GtObject*
GtObject::copy() const
{
    return copyClone(false, nullptr);
}

GtObject*
GtObject::copy(GtObjectUUIDMap& uuidMap) const
{
    uuidMap.clear();

    auto* result = copyClone(false, &uuidMap);

    if(!result)
    {
        uuidMap.clear();
    }

    return result;
}

GtObject*
GtObject::clone() const
{
    return copyClone(true, nullptr);
}

bool
GtObject::appendChild(GtObject* c)
{
    if (!c)
    {
        return false;
    }

    if (children().contains(c))
    {
        return false;
    }

    if (!childAccepted(c))
    {
        gtDebug() << tr("child type not accepted!");
        return false;
    }

    c->disconnectFromParent();
    c->setParent(this);

    connect(c, qOverload<GtObject*>(&GtObject::dataChanged),
            this, qOverload<GtObject*>(&GtObject::dataChanged));
    connect(c, qOverload<GtObject*, GtAbstractProperty*>(&GtObject::dataChanged),
            this, qOverload<GtObject*, GtAbstractProperty*>(&GtObject::dataChanged));
    connect(c, &GtObject::childAppended,
            this, &GtObject::childAppended);
    connect(c, qOverload<GtObject*, GtAbstractProperty*>(&GtObject::dataChanged),
            this, &GtObject::onChildDataChanged);
    connect(c, qOverload<GtObject*>(&GtObject::dataChanged),
            this, &GtObject::onChildDataChanged);
    connect(c, &QObject::destroyed,
            this, &GtObject::changed);

    changed();

    emit childAppended(c, this);

    return true;
}

bool
GtObject::appendChildren(const QList<GtObject*>& list)
{
    return std::all_of(std::begin(list), std::end(list), [this](GtObject* child) {
        return appendChild(child);
    });
}

void
GtObject::disconnectFromParent()
{
    if (GtObject* p = parentObject())
    {
        // disconnect old signals and slots
        disconnect(this, qOverload<GtObject*>(&GtObject::dataChanged),
                   p, qOverload<GtObject*>(&GtObject::dataChanged));
        disconnect(this, qOverload<GtObject*, GtAbstractProperty*>(&GtObject::dataChanged),
                   p, qOverload<GtObject*, GtAbstractProperty*>(&GtObject::dataChanged));
        disconnect(this, &GtObject::childAppended,
                   p, &GtObject::childAppended);
        disconnect(this, qOverload<GtObject*, GtAbstractProperty*>(&GtObject::dataChanged),
                   p, &GtObject::onChildDataChanged);
        disconnect(this, qOverload<GtObject*>(&GtObject::dataChanged),
                   p, &GtObject::onChildDataChanged);
        disconnect(this, &QObject::destroyed,
                   p, &GtObject::changed);
    }

    setParent(nullptr);
}

QString
GtObject::uuid() const
{
    return pimpl->uuid;
}

void
GtObject::setUuid(const QString& newValue)
{
    if (newValue.isEmpty()) return;

    if (newValue != pimpl->uuid)
    {
        QString oldUuid = pimpl->uuid;
        pimpl->uuid = newValue;
        emit uuidChanged(oldUuid);
    }
}

void
GtObject::newUuid(bool renewChildUUIDs)
{
    setUuid(QUuid::createUuid().toString());

    if (renewChildUUIDs)
    {
        newChildUUIDs(this);
    }
}

QString
GtObject::calcHash() const
{
    GtObjectMemento mem = toMemento();
    mem.calculateHashes();

    return mem.fullHash().toHex();
}

bool
GtObject::isDefault() const
{
    return (objectFlags() & GtObject::DefaultComponent);
}

void
GtObject::setDefault(bool val)
{
    setFlag(GtObject::DefaultComponent, val);
}

bool
GtObject::isUserHidden() const
{
    return (objectFlags() & GtObject::UserHidden);
}

void
GtObject::setUserHidden(bool val)
{
    setFlag(GtObject::UserHidden, val);
}

bool
GtObject::isRenamable() const
{
    return (objectFlags() & GtObject::UserRenamable);
}

bool
GtObject::isDeletable() const
{
    if (isDefault())
    {
        return false;
    }

    return (objectFlags() & GtObject::UserDeletable);
}

bool
GtObject::saveAsOwnFile() const
{
    return (objectFlags() & GtObject::SaveAsOwnFile);
}

void
GtObject::setSaveAsOwnFile(bool val)
{
    setFlag(GtObject::SaveAsOwnFile, val);

}

void
GtObject::setFactory(GtAbstractObjectFactory* factory)
{
    if (!factory || !factory->knownClass(metaObject()->className()))
    {
        return;
    }

    pimpl->factory = factory;
}

int
GtObject::childNumber() const
{
    GtObject const* p = parentObject();

    if (p)
    {
        return p->findDirectChildren().indexOf(this);
    }

    return -1;
}

QObject*
GtObject::parent()
{
    return QObject::parent();
}

QObject const*
GtObject::parent() const
{
    return QObject::parent();
}

GtObject*
GtObject::parentObject()
{
    return qobject_cast<GtObject*>(parent());
}

GtObject const*
GtObject::parentObject() const
{
    return const_cast<GtObject*>(this)->parentObject();
}

QStringList
GtObject::labelIds() const
{
    QStringList retval;

    for (GtLabel const* label : findChildren<GtLabel*>())
    {
        if (!retval.contains(label->objectName()))
        {
            retval << label->objectName();
        }
    }

    return retval;
}

bool
GtObject::hasChanges() const
{
    return (objectFlags() & GtObject::HasOwnChanges);
}

bool
GtObject::hasChildChanged() const
{
    return (objectFlags() & GtObject::HasChildChanges);
}

bool
GtObject::newlyCreated() const
{
    return (objectFlags() & GtObject::NewlyCreated);
}

void
GtObject::acceptChanges()
{
    pimpl->objectFlags = pimpl->objectFlags
                    & ~GtObject::HasOwnChanges
                    & ~GtObject::HasChildChanges
                    & ~GtObject::NewlyCreated;
}

void
GtObject::acceptChangesRecursively()
{
    acceptChanges();

    foreach (GtObject* child, findDirectChildren())
    {
        child->acceptChangesRecursively();
    }
}

void
GtObject::debugObjectTree(int indent)
{

    QString str = QStringLiteral("|-> ");

    for (int i = 0; i < indent; i++)
    {
        str.push_front(QStringLiteral("\t"));
    }

    if (pimpl->factory)
    {
        gtDebug() << str << objectName() << QStringLiteral(" (") <<
                  metaObject()->className() <<
                  QStringLiteral(") - ") << pimpl->factory;
    }
    else
    {
        gtDebug() << str << objectName() << QStringLiteral(" (") <<
                  metaObject()->className() << QStringLiteral(")");
    }

    foreach (GtObject* o, findDirectChildren())
    {
        o->debugObjectTree(indent + 1);
    }
}

const QList<GtAbstractProperty*>&
GtObject::properties()
{
    return pimpl->allProperties;
}

QList<GtAbstractProperty const*> const&
GtObject::properties() const
{
    return gt::container_const_cast(pimpl->allProperties);
}

bool
GtObject::addDynamicProperty(std::unique_ptr<GtAbstractProperty>&& property)
{
    if (!property || property->parent() || property->thread() != thread())
    {
        return false;
    }

    QList<QString> usedIds;
    for (const GtAbstractProperty* existing : fullPropertyList())
    {
        usedIds.append(existing->ident());
    }

    std::vector<const GtAbstractProperty*> pending{property.get()};
    while (!pending.empty())
    {
        const GtAbstractProperty* current = pending.back();
        pending.pop_back();

        if (!current->isValid() || usedIds.contains(current->ident()))
        {
            return false;
        }
        usedIds.append(current->ident());

        for (const GtAbstractProperty* child : current->fullProperties())
        {
            pending.push_back(child);
        }
    }

    GtAbstractProperty* rawProperty = property.get();
    rawProperty->setParent(this);
    property.release();

    pimpl->dynamicProperties.append(rawProperty);
    pimpl->allProperties.append(rawProperty);
    connectProperty(*rawProperty);

    setFlag(GtObject::HasOwnChanges);
    emit dataChanged(this, rawProperty);
    emit dataChanged(this);
    return true;
}

const QList<GtAbstractProperty*>&
GtObject::dynamicProperties() const
{
    return pimpl->dynamicProperties;
}

std::unique_ptr<GtAbstractProperty>
GtObject::takeDynamicProperty(const QString& id)
{
    auto iter = std::find_if(std::begin(pimpl->dynamicProperties),
                             std::end(pimpl->dynamicProperties),
                             [&id](const GtAbstractProperty* property) {
                                 return property->ident() == id;
                             });

    if (iter == std::end(pimpl->dynamicProperties))
    {
        return {};
    }

    GtAbstractProperty* property = *iter;
    pimpl->dynamicProperties.erase(iter);
    pimpl->allProperties.removeOne(property);

    std::vector<GtAbstractProperty*> pending{property};
    while (!pending.empty())
    {
        GtAbstractProperty* current = pending.back();
        pending.pop_back();
        QObject::disconnect(current, nullptr, this, nullptr);
        for (GtAbstractProperty* child : current->fullProperties())
        {
            pending.push_back(child);
        }
    }
    property->setParent(nullptr);

    setFlag(GtObject::HasOwnChanges);
    emit dataChanged(this, property);
    emit dataChanged(this);
    return std::unique_ptr<GtAbstractProperty>(property);
}

bool
GtObject::replaceDynamicProperties(
    const std::vector<QString>& removedIds,
    std::vector<std::unique_ptr<GtAbstractProperty>>&& additions)
{
    QSet<QString> idsToRemove;
    for (const QString& id : removedIds)
    {
        if (id.isEmpty() || idsToRemove.contains(id)) return false;

        int matches = 0;
        for (const GtAbstractProperty* property : pimpl->dynamicProperties)
        {
            if (property->ident() == id) ++matches;
        }
        if (matches != 1) return false;
        idsToRemove.insert(id);
    }

    QSet<const GtAbstractProperty*> removedTree;
    for (const GtAbstractProperty* root : pimpl->dynamicProperties)
    {
        if (!idsToRemove.contains(root->ident())) continue;
        std::vector<const GtAbstractProperty*> pending{root};
        while (!pending.empty())
        {
            const GtAbstractProperty* current = pending.back();
            pending.pop_back();
            if (removedTree.contains(current)) continue;
            removedTree.insert(current);
            for (const GtAbstractProperty* child : current->fullProperties())
            {
                pending.push_back(child);
            }
        }
    }

    QSet<QString> usedIds;
    for (const GtAbstractProperty* property : fullPropertyList())
    {
        if (!removedTree.contains(property)) usedIds.insert(property->ident());
    }

    for (const auto& property : additions)
    {
        if (!property || property->parent() || property->thread() != thread())
        {
            return false;
        }

        std::vector<const GtAbstractProperty*> pending{property.get()};
        while (!pending.empty())
        {
            const GtAbstractProperty* current = pending.back();
            pending.pop_back();
            if (!current->isValid() || usedIds.contains(current->ident()))
            {
                return false;
            }
            usedIds.insert(current->ident());
            for (const GtAbstractProperty* child : current->fullProperties())
            {
                pending.push_back(child);
            }
        }
    }

    std::vector<std::unique_ptr<GtAbstractProperty>> removed;
    QList<GtAbstractProperty*> changedProperties;
    for (int i = pimpl->dynamicProperties.size() - 1; i >= 0; --i)
    {
        GtAbstractProperty* property = pimpl->dynamicProperties[i];
        if (!idsToRemove.contains(property->ident())) continue;

        pimpl->dynamicProperties.removeAt(i);
        pimpl->allProperties.removeOne(property);
        std::vector<GtAbstractProperty*> pending{property};
        while (!pending.empty())
        {
            GtAbstractProperty* current = pending.back();
            pending.pop_back();
            QObject::disconnect(current, nullptr, this, nullptr);
            for (GtAbstractProperty* child : current->fullProperties())
            {
                pending.push_back(child);
            }
        }
        property->setParent(nullptr);
        changedProperties.append(property);
        removed.emplace_back(property);
    }

    for (auto& property : additions)
    {
        GtAbstractProperty* rawProperty = property.get();
        rawProperty->setParent(this);
        property.release();
        pimpl->dynamicProperties.append(rawProperty);
        pimpl->allProperties.append(rawProperty);
        connectProperty(*rawProperty);
        changedProperties.append(rawProperty);
    }

    if (!changedProperties.isEmpty())
    {
        setFlag(GtObject::HasOwnChanges);
        for (GtAbstractProperty* property : changedProperties)
        {
            emit dataChanged(this, property);
        }
        emit dataChanged(this);
    }
    return true;
}

QList<GtAbstractProperty*>
GtObject::fullPropertyList()
{
    QList<GtAbstractProperty*> retval;

    for (GtAbstractProperty* prop : properties())
    {
        fullPropertyListHelper(prop, retval);
    }

    return retval;
}

QList<GtAbstractProperty const*>
GtObject::fullPropertyList() const
{
    return gt::container_const_cast(
        const_cast<GtObject*>(this)->fullPropertyList()
    );
}

GtAbstractProperty*
GtObject::findProperty(const QString& id)
{
    for (GtAbstractProperty* property : qAsConst(pimpl->allProperties))
    {
        if (property->ident() == id)
        {
            return property;
        }

        if (GtAbstractProperty* tmp = property->findProperty(id))
        {
            return tmp;
        }
    }

    return nullptr;
}

GtAbstractProperty const*
GtObject::findProperty(const QString& id) const
{
    return const_cast<GtObject*>(this)->findProperty(id);
}

GtAbstractProperty*
GtObject::findPropertyByName(const QString& name)
{
    for (GtAbstractProperty* property : qAsConst(pimpl->allProperties))
    {
        if (property->objectName() == name)
        {
            return property;
        }

        if (GtAbstractProperty* tmp = property->findPropertyByName(name))
        {
            return tmp;
        }
    }

    return nullptr;
}

GtAbstractProperty const*
GtObject::findPropertyByName(const QString& name) const
{
    return const_cast<GtObject*>(this)->findPropertyByName(name);
}

GtPropertyStructContainer const *
GtObject::findPropertyContainer(const QString &id) const
{
    auto iter = std::find_if(std::begin(pimpl->propertyContainer),
                             std::end(pimpl->propertyContainer),
                             [&id](const GtPropertyStructContainer& current)
    {
        return current.ident() == id;
    });

    if (iter == pimpl->propertyContainer.end())
    {
        return nullptr;
    }

    auto const & prop = iter->get();
    return &prop;

}

GtPropertyStructContainer*
GtObject::findPropertyContainer(const QString &id)
{
    return const_cast<GtPropertyStructContainer*>
        (const_cast<const GtObject*>(this)->findPropertyContainer(id));
}

std::vector<std::reference_wrapper<GtPropertyStructContainer>>&
GtObject::propertyContainers()
{
    return pimpl->propertyContainer;
}

std::vector<std::reference_wrapper<const GtPropertyStructContainer> >
GtObject::propertyContainers() const
{
    return gt::container_const_cast(pimpl->propertyContainer);
}

void
GtObject::changed()
{
    setFlag(GtObject::HasOwnChanges);
    emit dataChanged(this);
}

QString
GtObject::objectPath(QString& str) const
{
    if (!str.isEmpty())
    {
        str.insert(0, QStringLiteral(";"));
    }

    str.insert(0, objectName());

    if (parentObject())
    {
        return parentObject()->objectPath(str);
    }

    return str;
}

GtObject*
GtObject::getObjectByPath(QStringList& objectPath)
{
    if (objectPath.isEmpty())
    {
        return nullptr;
    }

    if (objectPath.size() > 1)
    {
        if (objectPath.first() == objectName())
        {
            objectPath.takeFirst();

            GtObject* child = findDirectChild(objectPath.first());

            if (!child)
            {
                return nullptr;
            }

            return child->getObjectByPath(objectPath);
        }

        return nullptr;
    }

    if (objectPath.first() == objectName())
    {
        return this;
    }

    return nullptr;
}

const GtObject*
GtObject::getObjectByPath(QStringList& objectPath) const
{
    return const_cast<GtObject*>(this)->getObjectByPath(objectPath);
}

QString
GtObject::objectPath() const
{
    QString str;

    return objectPath(str);
}

void
GtObject::setFlag(GtObject::ObjectFlag flag, bool enable)
{
    if (enable)
    {
        pimpl->objectFlags = pimpl->objectFlags | flag;
    }
    else
    {
        pimpl->objectFlags = pimpl->objectFlags & ~flag;
    }
}

GtAbstractObjectFactory*
GtObject::factory() const
{
    return pimpl->factory;
}

void
GtObject::onChildDataChanged()
{
    setFlag(GtObject::HasChildChanges);
}

GtObject*
GtObject::getObjectByUuid(const QString& objectUUID)
{
    if (uuid() == objectUUID)
    {
        return this;
    }

    return gt::findObject(objectUUID, findChildren());
}

const GtObject*
GtObject::getObjectByUuid(const QString& objectUUID) const
{
    return const_cast<GtObject*>(this)->getObjectByUuid(objectUUID);
}

GtObject*
GtObject::getDirectChildByUuid(const QString& objectUUID)
{
    return gt::findObject(objectUUID, findDirectChildren());
}

const GtObject*
GtObject::getDirectChildByUuid(const QString& objectUUID) const
{
    return const_cast<GtObject*>(this)->getDirectChildByUuid(objectUUID);
}

GtObject*
GtObject::getObjectByPath(const QString& objectPath)
{
    QStringList list = objectPath.split(QStringLiteral(";"));
    return getObjectByPath(list);
}

const GtObject*
GtObject::getObjectByPath(const QString& objectPath) const
{
    return const_cast<GtObject*>(this)->getObjectByPath(objectPath);
}

void
GtObject::connectProperty(GtAbstractProperty& property)
{
    connect(&property, &GtAbstractProperty::changed, this,
            [this, p = &property]() {
        setFlag(GtObject::HasOwnChanges, true);
        emit dataChanged(this, p);
    });

    for (GtAbstractProperty* child : qAsConst(property.fullProperties()))
    {
        connectProperty(*child);
    }
}

void
GtObject::fullPropertyListHelper(GtAbstractProperty* p,
                                 QList<GtAbstractProperty*>& list) const
{
    // check property
    if (!p)
    {
        return;
    }

    if (!list.contains(p))
    {
        list << p;
    }

    for (GtAbstractProperty* childProp : qAsConst(p->fullProperties()))
    {
        fullPropertyListHelper(childProp, list);
    }
}

void
GtObject::newChildUUIDs(GtObject* parent) const
{
    foreach (GtObject* child, parent->findChildren())
    {
        child->newUuid();
    }
}

void
GtObject::makeDummy()
{
    pimpl->makeDummy();
}

void
GtObject::importMementoIntoDummy(const GtObjectMemento& memento)
{
    pimpl->importDummy(memento);
}

void
GtObject::exportDummyIntoMemento(GtObjectMemento& memento) const
{
    pimpl->exportDummy(memento);

    if (saveAsOwnFile())
    {
        memento.setFlagEnabled(GtObjectMemento::IsUnresolved, true);
        memento.setFlagEnabled(GtObjectMemento::SaveAsOwnFile, true);
    }
}

bool
GtObject::registerProperty(GtAbstractProperty& property)
{
    if (pimpl->allProperties.contains(&property))
    {
        gtWarning() << tr("multiple property registration!")
                    << QStringLiteral(" Object Name (") << objectName()
                    << QStringLiteral(")")
                    << QStringLiteral(" Property Name (") << property.ident()
                    << QStringLiteral(")");
        return false;
    }

    connectProperty(property);

    pimpl->properties.append(&property);
    pimpl->allProperties.append(&property);

    return true;
}

bool
GtObject::registerPropertyStructContainer(GtPropertyStructContainer & c)
{
    auto iter = std::find_if(std::begin(pimpl->propertyContainer),
                             std::end(pimpl->propertyContainer),
                             [&](const GtPropertyStructContainer& current)
    {
        return &current == &c;
    });

    if (iter != pimpl->propertyContainer.end())
    {
        gtWarning() << tr("multiple property container registration!")
                    << QStringLiteral(" Object Name (") << objectName()
                    << QStringLiteral(")")
                    << QStringLiteral(" Property Name (") << c.ident()
                    << QStringLiteral(")");
        return false;
    }

    pimpl->propertyContainer.push_back(c);

    connect(&c, &GtPropertyStructContainer::entryChanged, this,
            [this](int, GtAbstractProperty* property) {
        setFlag(GtObject::HasOwnChanges, true);
        emit dataChanged(this, property);
    });

    connect(&c, &GtPropertyStructContainer::entryAdded, this,
            [this](int) {
        setFlag(GtObject::HasOwnChanges, true);
        emit dataChanged(this);
    });

    connect(&c, &GtPropertyStructContainer::entryRemoved, this,
            [this](int) {
        setFlag(GtObject::HasOwnChanges, true);
        emit dataChanged(this);
    });

    return true;
}

bool
GtObject::registerProperty(GtAbstractProperty& property,
                           const QString& cat)
{
    property.setCategory(cat);
    return registerProperty(property);
}

bool
GtObject::registerProperty(GtAbstractProperty& property,
                           const QString& cat, int catPriority,
                           bool collapsedByDefault)
{
    QString catString = QString("%1||%2").arg(catPriority).arg(cat);
    if (collapsedByDefault) catString += '-';

    return registerProperty(property, catString);
}

bool
GtObject::registerSilentProperty(GtAbstractProperty& property)
{
    if (pimpl->allProperties.contains(&property))
    {
        gtWarning() << tr("multiple property registration!")
                    << QStringLiteral(" Object Name (") << objectName()
                    << QStringLiteral(")")
                    << QStringLiteral(" Property Name (") << property.ident()
                    << QStringLiteral(")");
        return false;
    }

    pimpl->properties.append(&property);
    pimpl->allProperties.append(&property);
    return true;
}

bool
GtObject::registerSilentProperty(GtAbstractProperty& property,
                                 const QString& cat)
{
    property.setCategory(cat);
    return registerSilentProperty(property);
}

void
GtObject::onObjectDataMerged()
{
    // nothing to do here
}

void
GtObject::onObjectDiffMerged()
{
    // nothing to do here
}

bool
GtObject::childAccepted(GtObject* /*child*/)
{
    // accept anything
    return true;
}

bool
gt::isDerivedFromClass(GtObject* obj, const QString& superClassName)
{
    if (!obj) return false;

    const QMetaObject* metaObject = obj->metaObject();

    return isDerivedFromClass(metaObject, superClassName);
}

bool
gt::isDerivedFromClass(const QMetaObject* metaObject,
                       const QString& superClassName)
{
    if (superClassName.isEmpty()) return false;

    // check recursively if the current meta object matches the super class name
    while (metaObject && superClassName != QStringLiteral("QObject"))
    {
        const QMetaObject* superClass = metaObject->superClass();

        if (!superClass) return false;

        if (superClassName == superClass->className()) return true;

        metaObject = superClass;
    }

    return false;
}

void
moveToThread(GtAbstractProperty* prop, QThread* thread)
{
    if (!prop || !thread) return;

    if (prop->parent()) return;

    prop->QObject::moveToThread(thread);
}

void
moveToThread(GtPropertyStructContainer& container, QThread* thread)
{
    if (!thread) return;

    // iterate over struct entries and move to new thread
    for (auto& containerEntry : container)
    {
        // iterate over props of struct entry and move to new thread
        for (auto* entryProp : containerEntry.fullProperties())
        {
            moveToThread(entryProp, thread);
        }

        moveToThread(&containerEntry, thread);
    }

    container.QObject::moveToThread(thread);
}

void
gt::moveToThread(GtObject& object, QThread* thread)
{
    if (!thread) return;

    // collect all child objects and add root object to list
    auto objs = object.findChildren<GtObject*>();
    objs.push_front(&object);

    for (auto* obj : qAsConst(objs))
    {
        const auto allChildProps = obj->fullPropertyList();
        // Runtime roots are QObject children of their owning object, so they
        // follow the object move. Their nested properties may be independent
        // QObjects and need the same explicit handling as static properties.
        QSet<GtAbstractProperty*> dynamicPropertyTree;
        std::vector<GtAbstractProperty*> pending;
        for (GtAbstractProperty* root : obj->dynamicProperties())
        {
            pending.push_back(root);
        }
        while (!pending.empty())
        {
            GtAbstractProperty* property = pending.back();
            pending.pop_back();
            if (!property || dynamicPropertyTree.contains(property)) continue;
            dynamicPropertyTree.insert(property);
            for (GtAbstractProperty* child : property->fullProperties())
            {
                pending.push_back(child);
            }
        }

        // Preserve the existing static property and property-container path.
        for (auto* childProp : allChildProps)
        {
            if (dynamicPropertyTree.contains(childProp)) continue;
            ::moveToThread(childProp, thread);
        }

        // Visit each runtime property exactly once. Parentless nested
        // properties move here; parented roots follow the object below.
        for (GtAbstractProperty* property : qAsConst(dynamicPropertyTree))
        {
            ::moveToThread(property, thread);
        }

        // iterate over struct container and move to new thread
        for (auto& container: obj->propertyContainers())
        {
            ::moveToThread(container, thread);
        }
    }

    object.QObject::moveToThread(thread);
}
