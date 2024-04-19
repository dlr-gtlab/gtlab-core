/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processcomponent.cpp
 *
 *  Created on: 29.01.2016
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include <QDir>

#include "gt_utilities.h"
#include "gt_task.h"
#include "gt_taskgroup.h"
#include "gt_coreapplication.h"
#include "gt_objectlinkproperty.h"
#include "gt_objectpathproperty.h"
#include "gt_environment.h"
#include "gt_abstractrunnable.h"
#include "gt_monitoringproperty.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"

#include "gt_processcomponent.h"

struct GtProcessComponent::Impl
{
    Impl() :
        state(GtProcessComponent::NONE),
        progress(0),
        skipped(QStringLiteral("skip"), tr("Skip"),
                tr("Skip Process Element"), false),
        warning(false)
    {}

// protected members
    /// Runnable pointer
    QPointer<GtAbstractRunnable> runnable;

    /// Path to process/project specific temporary path.
    QString tempPath;

    /// List of linked datamodel objects.
    QList<QPointer<GtObject>> linkedObjects;
// private members
    /// Current process component state
    GtProcessComponent::STATE state;

    /// Current process component state
    int progress;

    /// Monitoring properties
    QList<GtAbstractProperty*> monitorProperties;

    /// Container monitoring property references
    QList<GtPropertyReference> containerMonitorPropertyRefs;

    /// Skip indicator
    GtBoolProperty skipped;

    /// Warning flag
    bool warning;
};

GtProcessComponent::GtProcessComponent() :
    pimpl(std::make_unique<Impl>())
{
    qRegisterMetaType<GtProcessComponent::STATE>("GtProcessComponent::STATE");

    registerProperty(pimpl->skipped, tr("Execution"));

    connect(&pimpl->skipped, &GtAbstractProperty::changed,
            this, &GtProcessComponent::skipPropertyChanged);
}

void
GtProcessComponent::registerMonitoringProperty(GtAbstractProperty& property)
{
    // check whether property has children
    if (!property.fullProperties().isEmpty())
    {
        gtError() << tr("could not add property with tree structure!");
        return;
    }

    // check whether property inherits from monitoring class
    if (!dynamic_cast<GtMonitoringProperty*>(&property))
    {
        gtError() << tr("could not add a non monitoring property!");
        return;
    }

    // register property
    if (!registerProperty(property, QStringLiteral("Monitoring")))
    {
        gtError() << tr("register monitoring property failed!");
        return;
    }

    // append property to monitoring container
    pimpl->monitorProperties << &property;
}

bool
GtProcessComponent::registerMonitoringPropertyStructContainer(
        GtPropertyStructContainer& c)
{
    if (!registerPropertyStructContainer(c))
    {
        return false;
    }

    // connect container with entryAdded signal
    connect(&c, &GtPropertyStructContainer::entryAdded, this,
            [this, &c](int idx) {
        onEntryAdded(c, idx);
    });

    return true;
}

void
GtProcessComponent::setWarningFlag(bool val)
{
    pimpl->warning = val;
}

void
GtProcessComponent::handleStateChanged(GtProcessComponent::STATE state)
{
    setState(state);
}

void
GtProcessComponent::setState(GtProcessComponent::STATE state)
{
    switch (state)
    {
    case GtProcessComponent::NONE:
    case GtProcessComponent::QUEUED:
    case GtProcessComponent::CONNECTING:
    case GtProcessComponent::SKIPPED:
        setProgress(0);
    default:
        break;
    }

    // check whether new state is already the current state
    if (state == pimpl->state) return;

    // set new state
    pimpl->state = state;

    // emit global object changed signal
    changed();

    // emit state changed signal
    emit stateChanged(pimpl->state);
}

void
GtProcessComponent::setStateRecursively(GtProcessComponent::STATE state)
{
    setState(state);

    // loop over children and set state recursively
    foreach (GtProcessComponent* child,
             findDirectChildren<GtProcessComponent*>())
    {
        child->setStateRecursively(state);
    }
}

void
GtProcessComponent::setProgress(int progress)
{
    int p = gt::clamp(progress, 0, 100);

    // only update progress if it has changed
    if (p == pimpl->progress)
    {
        return;
    }

    if (gtApp->batchMode())
    {
        gtInfo() << tr("Progress of ") << objectName() << ": "
                 << QString(progress) << " %";
    }

    // set new state
    pimpl->progress = p;

    // emit global object changed signal is needed to update the
    // visualization in the process dock widget
    changed();

    // emit state changed signal
    emit progressStateChanged(pimpl->progress);
}

const QList<GtAbstractProperty*>&
GtProcessComponent::monitoringProperties()
{
    return pimpl->monitorProperties;
}

const QList<GtPropertyReference>&
GtProcessComponent::containerMonitoringPropertyRefs() const
{
    return pimpl->containerMonitorPropertyRefs;
}

QList<GtAbstractProperty*>
GtProcessComponent::readWriteProperties()
{
    // get all properties
    QList<GtAbstractProperty*> retval = fullPropertyList();

    // iterate over properties and remove all monitoring properties
    foreach (GtAbstractProperty* prop, retval)
    {
        if (pimpl->monitorProperties.contains(prop))
        {
            retval.removeOne(prop);
        }
    }

    // return list
    return retval;
}

QList<GtAbstractProperty*>
GtProcessComponent::nonReadOnlyProperties()
{
    // get read write properties
    QList<GtAbstractProperty*> retval = readWriteProperties();

    // iterate over properties and remove all read only properties
    foreach (GtAbstractProperty* prop, retval)
    {
        if (prop->isReadOnly() && !prop->isConnected())
        {
            retval.removeOne(prop);
        }
    }

    // return list
    return retval;
}

void
GtProcessComponent::resetMonitoringProperties()
{
    foreach (GtAbstractProperty* prop, monitoringProperties())
    {
        prop->revert();
    }

    for (const auto& ref : containerMonitoringPropertyRefs())
    {
        if (auto* prop = ref.resolve(*this))
        {
            prop->revert();
        }
    }

    foreach (GtProcessComponent* child,
             findDirectChildren<GtProcessComponent*>())
    {
        child->resetMonitoringProperties();
    }
}


template <typename T, typename U>
T rootTaskHelper(U object)
{
    // check whether parent is from task group data type
    if (qobject_cast<const GtTaskGroup*>(object->parent()))
    {
        return qobject_cast<T>(object);
    }

    // cast parent to process component
    auto* parentComponent = qobject_cast<U>(object->parent());

    // check casted parent
    if (!parentComponent)
    {
        return nullptr;
    }

    // search for root task recursively
    return rootTaskHelper<T>(parentComponent);
}

const GtTask*
GtProcessComponent::rootTask() const
{
    return rootTaskHelper<const GtTask*>(this);
}

GtTask*
GtProcessComponent::rootTask()
{
    return rootTaskHelper<GtTask*>(this);
}

bool
GtProcessComponent::isReady() const
{
    const GtTask* root = rootTask();

    if (!root)
    {
        return false;
    }

    return root->isComponentReady();
}

bool
GtProcessComponent::isComponentReady() const
{
    switch (currentState())
    {
        case GtProcessComponent::RUNNING:
        case GtProcessComponent::QUEUED:
        case GtProcessComponent::CONNECTING:
        case GtProcessComponent::TERMINATION_REQUESTED:
            return false;
        default:
            return true;
    }
}

bool
GtProcessComponent::hasWarnings() const
{
    return pimpl->warning;
}

GtProcessComponent::~GtProcessComponent() = default;

GtProcessComponent::STATE
GtProcessComponent::currentState() const
{
    // always forward skipped state
    if (pimpl->skipped) return GtProcessComponent::SKIPPED;

    return pimpl->state;
}

int
GtProcessComponent::currentProgress() const
{
    return pimpl->progress;
}

bool
GtProcessComponent::isSkipped() const
{
    return pimpl->skipped;
}

void
GtProcessComponent::setSkipped(bool val)
{
    pimpl->skipped = val;
}

QString
GtProcessComponent::dataHelper(GtObjectLinkProperty& prop) const
{
    return prop.linkedObjectUUID();
}

GtObjectPath
GtProcessComponent::pathHelper(GtObjectPathProperty& prop) const
{
    return prop.path();
}


QString
GtProcessComponent::environmentVariable(const QString& var) const
{
    QVariant variant = gtEnvironment->value(var);

    if (variant.isNull())
    {
        return {};
    }

    return variant.toString();
}

QString
GtProcessComponent::projectPath() const
{
    if (!pimpl->runnable)
    {
        return {};
    }

    return pimpl->runnable->projectPath();
}

const QPointer<GtAbstractRunnable>&
GtProcessComponent::runnable() const
{
    return pimpl->runnable;
}

GtProcessComponent&
GtProcessComponent::setRunnable(QPointer<GtAbstractRunnable> p)
{
    pimpl->runnable = std::move(p);
    return *this;
}

const QString&
GtProcessComponent::tempPath() const
{
    return pimpl->tempPath;
}

GtProcessComponent&
GtProcessComponent::setTempPath(QString path)
{
    pimpl->tempPath = std::move(path);
    return *this;
}

QList<GtObject const*>
GtProcessComponent::linkedObjects() const
{
    return {pimpl->linkedObjects.begin(), pimpl->linkedObjects.end()};
}

QList<QPointer<GtObject>>&
GtProcessComponent::linkedObjects()
{
    return pimpl->linkedObjects;
}

void
GtProcessComponent::onEntryAdded(const GtPropertyStructContainer& c, int idx)
{
    // get added entry
    const auto& entry = c.at(idx);

    // get container monitoring property reference list
    auto& refs = pimpl->containerMonitorPropertyRefs;
    const auto refsSize = refs.size();

    // add the monitoring properties of the new entry to the reference list
    // of container monitoring properties
    for (auto* prop : entry.fullProperties())
    {
        if (dynamic_cast<GtMonitoringProperty*>(prop))
        {
            refs << GtPropertyReference{c.ident(), entry.ident(), prop->ident()};
        }
    }

    // return if no property reference was added
    if (refs.size() <= refsSize)
    {
        return;
    }

    // Connect the newly created entry with the destroyed signal.
    // This recognizes when an element needs to be removed from the
    // reference list.
    connect(&entry, &GtPropertyStructInstance::destroyed, this,
            [this, &c, &entry](QObject*) {
        onEntryDestroyed(c.ident(), entry.ident());
    });
}

void
GtProcessComponent::onEntryDestroyed(
        const QString& cIdent, const QString& eIdent)
{
    auto& refs = pimpl->containerMonitorPropertyRefs;

    // remove elements from the reference list that refer to properties
    // of the given entry
    refs.erase(std::remove_if(refs.begin(), refs.end(),
                              [&cIdent, &eIdent](const GtPropertyReference& ref) {
        return ref.containerId() == cIdent && ref.entryId() == eIdent;
    }),
               refs.end());
}

QDir
GtProcessComponent::tempDir()
{
    if (!pimpl->runnable)
    {
        return {};
    }

    // check whether temporary dir was already set
    if (pimpl->tempPath.isEmpty())
    {
        // create new temporary directory
        pimpl->tempPath = pimpl->runnable->tempDir().absolutePath();
    }

    return QDir(pimpl->tempPath);
}

void
GtProcessComponent::appendToLinkObjects(QPointer<GtObject> p)
{
    linkedObjects().append(p);
}
