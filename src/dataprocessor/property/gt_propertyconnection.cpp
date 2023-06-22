/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_propertyconnection.cpp
 *
 *  Created on: 20.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_logging.h"

#include "gt_propertyconnection.h"
#include "gt_abstractproperty.h"
#include "gt_propertyreference.h"

GtPropertyConnection::GtPropertyConnection() :
    m_sourcePropObj(nullptr),
    m_targetPropObj(nullptr)
{

}

QString
GtPropertyConnection::sourceUuid() const
{
    return m_sourceUuid;
}

void
GtPropertyConnection::setSourceUuid(const QString& sourceUuid)
{
    m_sourceUuid = sourceUuid;
}

QString
GtPropertyConnection::targetUuid() const
{
    return m_targetUuid;
}

void
GtPropertyConnection::setTargetUuid(const QString& targetUuid)
{
    m_targetUuid = targetUuid;
}

QString
GtPropertyConnection::sourceProp() const
{
    return m_sourceProp;
}

void
GtPropertyConnection::setSourceProp(const QString& sourceProp)
{
    m_sourceProp = sourceProp;
}

QString
GtPropertyConnection::targetProp() const
{
    return m_targetProp;
}

void
GtPropertyConnection::setTargetProp(const QString& targetProp)
{
    m_targetProp = targetProp;
}

void
GtPropertyConnection::makeConnection()
{
    gtDebug().verbose() << "#### make connection...";

    // check source property pointer
    if (m_sourcePropObj)
    {
        // disconnect signals
        connect(m_sourcePropObj.data(), SIGNAL(changed()),
                this, SLOT(onSourcePropertyChange()));
        connect(m_sourcePropObj.data(), SIGNAL(destroyed(QObject*)),
                this, SLOT(deleteLater()));

        m_sourcePropObj = nullptr;
    }

    // check target property pointer
    if (m_targetPropObj)
    {
        m_targetPropObj->clearConnection();

        m_targetPropObj = nullptr;
    }

    // get parent object
    GtObject* parentObj = parentObject();

    // check parent object
    if (!parentObj)
    {
        gtError() << tr("No parent found for property connection!");
        return;
    }

    // search for source object by given uuid
    GtObject* sourceObj = parentObj->getObjectByUuid(m_sourceUuid);

    // check source object
    if (!sourceObj)
    {
        gtError() << tr("Property connection could not be established!") <<
                     QStringLiteral(" ") << tr("Source object not found!");
        return;
    }

    // search for target object by given uuid
    GtObject* targetObj = parentObj->getObjectByUuid(m_targetUuid);

    // check source object
    if (!targetObj)
    {
        gtError() << tr("Property connection could not be established!") <<
                     QStringLiteral(" ") << tr("Target object not found!");
        return;
    }

    // Get Source property
    bool ok = false;
    auto sourceRef = GtPropertyReference::fromString(m_sourceProp, ok);
    if (!ok)
    {
        gtError() << tr("Cannot parse source property '%1'").arg(m_sourceProp);
        return;
    }

    // find source property
    GtAbstractProperty* sourcePropety = sourceRef.resolve(*sourceObj);

    // check property
    if (!sourcePropety)
    {
        gtError() << tr("Property connection could not be established!") <<
                     QStringLiteral(" ") << tr("Source property not found!");
        return;
    }

    // Get Target property
    auto targetRef = GtPropertyReference::fromString(m_targetProp, ok);
    if (!ok)
    {
        gtError() << tr("Cannot parse target property '%1'").arg(m_targetProp);
        return;
    }

    // find target property
    GtAbstractProperty* targetPropety = targetRef.resolve(*targetObj);

    // check property
    if (!targetPropety)
    {
        gtError() << tr("Property connection could not be established!") <<
                     QStringLiteral(" ") << tr("Target property not found!");
        return;
    }

    // TODO: check whether properties can be connected

    // save source property
    m_sourcePropObj = sourcePropety;

    // connect signals
    connect(sourcePropety, SIGNAL(changed()), 
            SLOT(onSourcePropertyChange()), Qt::DirectConnection);
    connect(sourcePropety, SIGNAL(destroyed(QObject*)), SLOT(deleteLater()));

    // establish connection
    targetPropety->propertyConnect(this);

    gtDebug().verbose() << "#### connection established!";
}

QVariant
GtPropertyConnection::valueFromSource() const
{
    // check source property
    if (!m_sourcePropObj)
    {
        // no source property set
        return QVariant();
    }

    return m_sourcePropObj->valueToVariant();
}

bool
GtPropertyConnection::isConnected()
{
    // check source property pointer
    return (m_sourcePropObj != nullptr);
}

void
GtPropertyConnection::onSourcePropertyChange()
{
    emit triggerValueTransfer();
}
