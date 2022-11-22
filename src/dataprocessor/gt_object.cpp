/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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

#include "gt_object.h"

#include <QUuid>
#include <QSignalMapper>
#include <QDebug>

#include <algorithm>

struct DummyData
{
    QVector<GtObjectMemento::PropertyData> properties;
    QVector<GtObjectMemento::PropertyData> propertyContainers;
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
        propertyContainer.clear();
    }

    void importDummy(const GtObjectMemento& memento)
    {
        assert(isDummy());
        dummyData.properties = memento.properties;
        dummyData.propertyContainers = memento.propertyContainers;
        dummyData.className = memento.className();
    }

    void exportDummy(GtObjectMemento& memento) const
    {
        assert(isDummy());
        memento.setClassName(dummyData.className);
        memento.properties = dummyData.properties;
        memento.propertyContainers = dummyData.propertyContainers;
    }

    /// Object specific uuid
    QString uuid;

    /// Object flags
    GtObject::ObjectFlags objectFlags;

    /// factory
    GtAbstractObjectFactory* factory;

    /// object properties
    QList<GtAbstractProperty*> properties;

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
    auto c = findChildren<GtObject*>();
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
        qDebug() << "factory is null";
        return;
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
GtObject::copy() const
{
    // check for factory
    if (!pimpl->factory)
    {
        gtError() << tr("No factory set!") << QStringLiteral("(")
                  << objectName() << QStringLiteral(")");
        return nullptr;
    }

    // generate memento
    GtObjectMemento memento = toMemento(false);

    if (memento.isNull())
    {
        return nullptr;
    }

    return memento.restore(pimpl->factory);
}

GtObject*
GtObject::clone() const
{
    // check for factory
    if (!pimpl->factory)
    {
        gtError() << tr("No factory set!") << QStringLiteral("(")
                  << objectName() << QStringLiteral(")");
        return nullptr;
    }

    // generate memento
    GtObjectMemento memento = toMemento(true);

    if (memento.isNull())
    {
        return nullptr;
    }

    return memento.restore(pimpl->factory);
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

    // disconnect old signals and slots
    if (c->parent())
    {
        disconnect(c, SIGNAL(dataChanged(GtObject*)),
                   c->parent(), SIGNAL(dataChanged(GtObject*)));
        disconnect(c, SIGNAL(dataChanged(GtObject*,GtAbstractProperty*)),
                   c->parent(),
                   SIGNAL(dataChanged(GtObject*,GtAbstractProperty*)));
        disconnect(c, SIGNAL(dataChanged(GtObject*,GtAbstractProperty*)),
                   c->parent(), SLOT(onChildDataChanged()));
        disconnect(c, SIGNAL(dataChanged(GtObject*)),
                   c->parent(), SLOT(onChildDataChanged()));
        disconnect(c, SIGNAL(destroyed(QObject*)),
                   c->parent(), SLOT(changed()));
        disconnect(c, SIGNAL(childAppended(GtObject*,GtObject*)),
                   c->parent(), SIGNAL(childAppended(GtObject*,GtObject*)));
    }

    c->setParent(this);

    connect(c, SIGNAL(dataChanged(GtObject*)), SIGNAL(dataChanged(GtObject*)),
            Qt::DirectConnection);
    connect(c, SIGNAL(childAppended(GtObject*,GtObject*)),
            SIGNAL(childAppended(GtObject*, GtObject*)),
            Qt::DirectConnection);
    connect(c, SIGNAL(dataChanged(GtObject*,GtAbstractProperty*)),
            SIGNAL(dataChanged(GtObject*,GtAbstractProperty*)),
            Qt::DirectConnection);
    connect(c, SIGNAL(dataChanged(GtObject*,GtAbstractProperty*)),
            SLOT(onChildDataChanged()),
            Qt::DirectConnection);
    connect(c, SIGNAL(dataChanged(GtObject*)), SLOT(onChildDataChanged()),
            Qt::DirectConnection);
    connect(c, SIGNAL(destroyed(QObject*)), SLOT(changed()),
            Qt::DirectConnection);

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
    if (parent())
    {
        disconnect(this, SIGNAL(dataChanged(GtObject*)),
                   parent(), SIGNAL(dataChanged(GtObject*)));
        disconnect(this, SIGNAL(dataChanged(GtObject*, GtAbstractProperty*)),
                   parent(),
                   SIGNAL(dataChanged(GtObject*, GtAbstractProperty*)));
        disconnect(this, SIGNAL(dataChanged(GtObject*, GtAbstractProperty*)),
                   parent(), SLOT(onChildDataChanged()));
        disconnect(this, SIGNAL(dataChanged(GtObject*)),
                   parent(), SLOT(onChildDataChanged()));
        disconnect(this, SIGNAL(destroyed(QObject*)),
                   parent(), SLOT(changed()));
        disconnect(this, SIGNAL(childAppended(GtObject*, GtObject*)),
                   parent(), SIGNAL(childAppended(GtObject*, GtObject*)));
    }

    setParent(nullptr);
}

QString
GtObject::uuid() const
{
    return pimpl->uuid;
}

void
GtObject::setUuid(const QString& val)
{
    if (val.isEmpty())
    {
        return;
    }

    pimpl->uuid = val;
}

void
GtObject::newUuid(bool renewChildUUIDs)
{
    pimpl->uuid = QUuid::createUuid().toString();

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
GtObject::childNumber()
{
    GtObject* p = parentObject();

    if (p)
    {
        return p->findDirectChildren<GtObject*>().indexOf(this);
    }

    return -1;
}

GtObject*
GtObject::parentObject() const
{
    return qobject_cast<GtObject*>(parent());
}

QStringList
GtObject::labelIds() const
{
    QStringList retval;

    foreach (GtLabel* label, findChildren<GtLabel*>())
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

    foreach (GtObject* child, findDirectChildren<GtObject*>())
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

    foreach (GtObject* o, findDirectChildren<GtObject*>())
    {
        o->debugObjectTree(indent + 1);
    }
}

const QList<GtAbstractProperty*>&
GtObject::properties() const
{
    return pimpl->properties;
}

QList<GtAbstractProperty*>
GtObject::fullPropertyList() const
{
    QList<GtAbstractProperty*> retval;

    foreach (GtAbstractProperty* prop, properties())
    {
        fullPropertyListHelper(prop, retval);
    }

    return retval;
}

GtAbstractProperty*
GtObject::findProperty(const QString& id) const
{
    foreach (GtAbstractProperty* property, pimpl->properties)
    {
        if (property->ident() == id)
        {
            return property;
        }

        GtAbstractProperty* tmp = property->findProperty(id);

        if (tmp)
        {
            return tmp;
        }
    }

    return nullptr;
}

GtAbstractProperty*
GtObject::findPropertyByName(const QString& name) const
{
    foreach (GtAbstractProperty* property, pimpl->properties)
    {
        if (property->objectName() == name)
        {
            return property;
        }

        GtAbstractProperty* tmp = property->findPropertyByName(name);

        if (tmp)
        {
            return tmp;
        }
    }

    return nullptr;
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

GtPropertyStructContainer *
GtObject::findPropertyContainer(const QString &id)
{
    return const_cast<GtPropertyStructContainer*>
        (const_cast<const GtObject*>(this)->findPropertyContainer(id));
}

std::vector<std::reference_wrapper<const GtPropertyStructContainer> >
GtObject::propertyContainers() const
{
    return {std::begin(pimpl->propertyContainer), std::end(pimpl->propertyContainer)};

}

std::vector<std::reference_wrapper<GtPropertyStructContainer>>&
GtObject::propertyContainers()
{
    return pimpl->propertyContainer;
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

            GtObject* child = findDirectChild<GtObject*>(objectPath.first());

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
GtObject::propertyChanged(QObject* obj)
{
    setFlag(GtObject::HasOwnChanges, true);

    if (GtAbstractProperty* prop = qobject_cast<GtAbstractProperty*>(obj))
    {
        emit dataChanged(this, prop);
    }
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


    return gt::findObject(objectUUID, findChildren<GtObject*>());
}

GtObject*
GtObject::getDirectChildByUuid(const QString& objectUUID) const
{
    return gt::findObject(objectUUID, findDirectChildren<GtObject*>());
}

GtObject*
GtObject::getObjectByPath(const QString& objectPath)
{
    QStringList list = objectPath.split(QStringLiteral(";"));

    return getObjectByPath(list);
}

//void
//GtObject::changed()
//{
//    emit dataChanged(this);
//}

void
GtObject::connectProperty(GtAbstractProperty& property)
{
    connect(&property, &GtAbstractProperty::changed, this,
        [this, p = &property]() {
            propertyChanged(p);
        });

    foreach (GtAbstractProperty* child, property.fullProperties())
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

    foreach (GtAbstractProperty* childProp, p->fullProperties())
    {
        fullPropertyListHelper(childProp, list);
    }
}

void
GtObject::newChildUUIDs(GtObject* parent) const
{
    foreach(GtObject* child, parent->findChildren<GtObject*>())
    {
        child->newUuid();
    }
}

void GtObject::makeDummy()
{
    pimpl->makeDummy();
}

void GtObject::importMementoIntoDummy(const GtObjectMemento& memento)
{
    pimpl->importDummy(memento);
}

void GtObject::exportDummyIntoMemento(GtObjectMemento& memento) const
{
    pimpl->exportDummy(memento);
}

bool
GtObject::registerProperty(GtAbstractProperty& property)
{
    if (pimpl->properties.contains(&property))
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

    return true;
}

bool
GtObject::registerProperty(GtAbstractProperty& property, const QString& cat)
{
    property.setCategory(cat);

    return registerProperty(property);
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
    if (!obj)
    {
        return false;
    }

    if (superClassName.isEmpty())
    {
        return false;
    }

    const QMetaObject* currentMetaObject = obj->metaObject();

    while (superClassName != "QObject")
    {
        const QMetaObject* currentSuperClass =
                currentMetaObject->superClass();

        if (!currentSuperClass)
        {
            return false;
        }

        if (superClassName == currentSuperClass->className())
        {
            return true;
        }

        currentMetaObject = currentSuperClass;
    }

    return false;
}
