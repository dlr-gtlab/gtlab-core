/* GTlab - Gas Turbine laboratory
 * Source File: gt_processcomponent.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 29.01.2016
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include <QDebug>
#include <QDir>

#include "gt_task.h"
#include "gt_processdata.h"
#include "gt_objectlinkproperty.h"
#include "gt_objectpathproperty.h"
#include "gt_environment.h"
#include "gt_abstractrunnable.h"
#include "gt_monitoringproperty.h"

#include "gt_processcomponent.h"

struct GtProcessComponent::Impl
{
    Impl() :
        state(GtProcessComponent::NONE),
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

    /// Monitoring properties
    QList<GtAbstractProperty*> monitorProperties;

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

    connect(&pimpl->skipped, SIGNAL(changed()), SIGNAL(skipPropertyChanged()));
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
    // check whether new state is already the corrent state
    if (state == pimpl->state)
    {
        return;
    }

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
    // set state
    setState(state);

    // loop over children and set state recursively
    foreach (GtProcessComponent* child,
             findDirectChildren<GtProcessComponent*>())
    {
        child->setStateRecursively(state);
    }
}

const QList<GtAbstractProperty*>&
GtProcessComponent::monitoringProperties()
{
    return pimpl->monitorProperties;
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

    foreach (GtProcessComponent* child,
             findDirectChildren<GtProcessComponent*>())
    {
        child->resetMonitoringProperties();
    }
}


template <typename T, typename U>
T rootTaskHelper(U object)
{
    // check whether parent is from process data type
    if (qobject_cast<const GtProcessData*>(object->parent()))
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

    switch (root->currentState())
    {
        case GtProcessComponent::RUNNING:
        case GtProcessComponent::QUEUED:
        case GtProcessComponent::CONNECTING:
        case GtProcessComponent::TERMINATION_REQUESTED:
            return false;

        default:
            break;
    }

    return true;
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
    return pimpl->state;
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

QString
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

const QList<QPointer<GtObject> >&
GtProcessComponent::linkedObjects() const
{
    return pimpl->linkedObjects;
}

QList<QPointer<GtObject> >&
GtProcessComponent::linkedObjects()
{
    return pimpl->linkedObjects;
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
