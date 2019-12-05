/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 15.10.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include <QVariant>

#include "gt_logging.h"

#include "gt_abstractproperty.h"

QVariant
GtAbstractProperty::valueToVariant() const
{
    return valueToVariant(QString());
}

bool
GtAbstractProperty::setValueFromVariant(const QVariant& val)
{
    return setValueFromVariant(val, QString());
}

QString
GtAbstractProperty::brief() const
{
    return m_brief;
}

QString
GtAbstractProperty::siUnit() const
{
    return GtUnit::siUnit(m_unitCategory);
}

QString
GtAbstractProperty::unitCategoryToString() const
{
    return GtUnit::categoryToString(m_unitCategory);
}

GtUnit::Category
GtAbstractProperty::unitCategory() const
{
    return m_unitCategory;
}

GtAbstractProperty::PropertyCategory
GtAbstractProperty::category() const
{
    return m_category;
}

void
GtAbstractProperty::setCategory(GtAbstractProperty::PropertyCategory cat)
{
    m_category = cat;
}

void
GtAbstractProperty::setCategory(const QString& cat)
{
    if (cat.isEmpty())
    {
        return;
    }
    m_category = GtAbstractProperty::Custom;
    m_customCategoryString = cat;
}

QString
GtAbstractProperty::categoryToString()
{
    switch (m_category)
    {
    case PropertyCategory::Main:
        return QObject::tr("Main");
    case PropertyCategory::Custom:
        return m_customCategoryString;
    default:
        return QObject::tr("Main");
    }
}

const QList<GtAbstractProperty*>&
GtAbstractProperty::properties()
{
    return m_subProperties;
}

const
QList<GtAbstractProperty*>&
GtAbstractProperty::fullProperties()
{
    return m_subProperties;
}

void
GtAbstractProperty::registerSubProperty(GtAbstractProperty& property)
{
//    if (m_subProperties.contains(&property))
//    {
//        gtWarning() << tr("multiple property registration!")
//                    << " (" << objectName() << ")";
//        return;
//    }

//    QList<QString> propNames;
//    foreach(GtAbstractProperty* absProp, m_subProperties)
//    {
//        propNames.append(absProp->objectName());
//    }

//    if (propNames.contains(property.objectName()))
//    {
//        gtWarning() << tr("property name already taken!")
//                    << " (" << objectName() << ")";
//        return;
//    }

//    m_subProperties.append(&property);

//    qDebug() << "Current sub properties:";
//    foreach(GtAbstractProperty* prop, m_subProperties)
//    {
//        qDebug() << prop;
//    }

    insertSubProperty(property, m_subProperties.size());
}

void
GtAbstractProperty::insertSubProperty(GtAbstractProperty& property, int index)
{
    if (index < 0 || index > m_subProperties.size())
    {
        return;
    }

    if (m_subProperties.contains(&property))
    {
        gtWarning() << tr("multiple property registration!")
                    << " (" << objectName() << ")";
        return;
    }

    QList<QString> propNames;
    foreach(GtAbstractProperty* absProp, m_subProperties)
    {
        propNames.append(absProp->objectName());
    }

    if (propNames.contains(property.objectName()))
    {
        gtWarning() << tr("property name already taken!")
                    << " (" << objectName() << ")";
        return;
    }

    if (index == m_subProperties.size())
    {
        m_subProperties.append(&property);
    }
    else
    {
        m_subProperties.insert(index, &property);
    }
}

bool
GtAbstractProperty::isReadOnly()
{
    // connected properties are read only by default
    if (m_connection != Q_NULLPTR)
    {
        return true;
    }

    return m_readOnly;
}

void
GtAbstractProperty::setReadOnly(bool val)
{
    m_readOnly = val;
}

bool
GtAbstractProperty::isActive() const
{
    return m_active;
}

void
GtAbstractProperty::setActive(bool val)
{
    if (m_optional && m_active != val)
    {
        m_active = val;
        emit changed();
    }
}

bool
GtAbstractProperty::isHidden() const
{
    return m_hidden;
}

void
GtAbstractProperty::hide(bool val)
{
    m_hidden = val;
}

bool
GtAbstractProperty::isOptional() const
{
    return m_optional;
}

int
GtAbstractProperty::propertyCount()
{
    int n = 0;
    foreach (GtAbstractProperty* prop, m_subProperties)
    {
        if (!prop->isHidden())
        {
            n++;
        }
    }
    return n;
    //return m_subProperties.count();
}

GtAbstractProperty*
GtAbstractProperty::findProperty(const QString& id)
{
    foreach (GtAbstractProperty* sub, fullProperties())
    {
        if (sub->ident() == id)
        {
            return sub;
        }

        GtAbstractProperty* tmp = sub->findProperty(id);
        if (tmp != Q_NULLPTR)
        {
            return tmp;
        }
    }

    return Q_NULLPTR;
}

GtAbstractProperty*
GtAbstractProperty::findPropertyByName(const QString &name)
{
    foreach (GtAbstractProperty* sub, fullProperties())
    {
        if (sub->objectName() == name)
        {
            return sub;
        }

        GtAbstractProperty* tmp = sub->findPropertyByName(name);
        if (tmp != Q_NULLPTR)
        {
            return tmp;
        }
    }

    return Q_NULLPTR;
}

bool
GtAbstractProperty::isValid() const
{
    return !m_id.isEmpty();
}

const QString&
GtAbstractProperty::ident() const
{
    return m_id;
}

bool
GtAbstractProperty::storeToMemento() const
{
    return m_storeMemento;
}

void
GtAbstractProperty::revert()
{
    // nothing to do here
}

void
GtAbstractProperty::propertyConnect(GtPropertyConnection* connection)
{
    // check connection
    if (connection == Q_NULLPTR)
    {
        return;
    }

    // check whether property is already connected
    if (m_connection != Q_NULLPTR)
    {
        gtError() << tr("Could not establish property connection!");
        gtError() << QStringLiteral("   |-> ") << connection->sourceUuid() <<
                     QStringLiteral(" -> ") << connection->targetUuid();
        return;
    }

    m_connection = connection;

    onTriggerValueTransfer();

    // connect signals
    connect(connection, SIGNAL(triggerValueTransfer()),
            SLOT(onTriggerValueTransfer()), Qt::DirectConnection);
}

void
GtAbstractProperty::clearConnection()
{
    if (m_connection == Q_NULLPTR)
    {
        return;
    }

    disconnect(m_connection.data(), SIGNAL(triggerValueTransfer()),
               this, SLOT(onTriggerValueTransfer()));

    m_connection = Q_NULLPTR;
}

bool
GtAbstractProperty::isConnected()
{
    if (m_connection == Q_NULLPTR)
    {
        return false;
    }

    return true;
}

void
GtAbstractProperty::setValFromConnection()
{
    // nothing to do here
}

void
GtAbstractProperty::onTriggerValueTransfer()
{
    setValFromConnection();
}

void
GtAbstractProperty::setOptional(bool val)
{
    m_optional = val;

    if (val)
    {
        m_active = false;
    }
    else
    {
        m_active = true;
    }
}
