/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyconnection.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 20.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>

#include "gt_logging.h"

#include "gt_propertyconnection.h"
#include "gt_abstractproperty.h"

GtPropertyConnection::GtPropertyConnection() :
    m_sourcePropObj(Q_NULLPTR),
    m_targetPropObj(Q_NULLPTR)
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
    qDebug() << "#### make connection...";

    // check source property pointer
    if (m_sourcePropObj != Q_NULLPTR)
    {
        // disconnect signals
        connect(m_sourcePropObj.data(), SIGNAL(changed()),
                this, SLOT(onSourcePropertyChange()));
        connect(m_sourcePropObj.data(), SIGNAL(destroyed(QObject*)),
                this, SLOT(deleteLater()));

        m_sourcePropObj = Q_NULLPTR;
    }

    // check target property pointer
    if (m_targetPropObj != Q_NULLPTR)
    {
        m_targetPropObj->clearConnection();

        m_targetPropObj = Q_NULLPTR;
    }

    // get parent object
    GtObject* parentObj = parentObject();

    // check parent object
    if (parentObj == Q_NULLPTR)
    {
        gtError() << tr("No parent found for property connection!");
        return;
    }

    // search for source object by given uuid
    GtObject* sourceObj = parentObj->getObjectByUuid(m_sourceUuid);

    // check source object
    if (sourceObj == Q_NULLPTR)
    {
        gtError() << tr("Property connection could not be established!") <<
                     QStringLiteral(" ") << tr("Source object not found!");
        return;
    }

    // search for target object by given uuid
    GtObject* targetObj = parentObj->getObjectByUuid(m_targetUuid);

    // check source object
    if (targetObj == Q_NULLPTR)
    {
        gtError() << tr("Property connection could not be established!") <<
                     QStringLiteral(" ") << tr("Target object not found!");
        return;
    }

    // find source property
    GtAbstractProperty* sourcePropety = sourceObj->findProperty(m_sourceProp);

    // check property
    if (sourcePropety == Q_NULLPTR)
    {
        gtError() << tr("Property connection could not be established!") <<
                     QStringLiteral(" ") << tr("Source property not found!");
        return;
    }

    // find target property
    GtAbstractProperty* targetPropety = targetObj->findProperty(m_targetProp);

    // check property
    if (targetPropety == Q_NULLPTR)
    {
        gtError() << tr("Property connection could not be established!") <<
                     QStringLiteral(" ") << tr("Target property not found!");
        return;
    }

    // TODO: check whether properties can be connected

    // save source property
    m_sourcePropObj = sourcePropety;

    // connect signals
    connect(sourcePropety, SIGNAL(changed()), SLOT(onSourcePropertyChange()),
            Qt::DirectConnection);
    connect(sourcePropety, SIGNAL(destroyed(QObject*)), SLOT(deleteLater()));

    // establish connection
    targetPropety->propertyConnect(this);

    qDebug() << "#### connection established!";
}

QVariant
GtPropertyConnection::valueFromSource()
{
    // check source property
    if (m_sourcePropObj == Q_NULLPTR)
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
    if (m_sourcePropObj != Q_NULLPTR)
    {
        return true;
    }

    return false;
}

void
GtPropertyConnection::onSourcePropertyChange()
{
    emit triggerValueTransfer();
}