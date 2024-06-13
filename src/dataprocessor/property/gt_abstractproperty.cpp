/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 15.10.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include <QVariant>

#include "gt_logging.h"

#include "gt_abstractproperty.h"
#include "gt_propertyconnection.h"

GtAbstractProperty::~GtAbstractProperty() = default;

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
GtAbstractProperty::properties() const
{
    return m_subProperties;
}

const
QList<GtAbstractProperty*>&
GtAbstractProperty::fullProperties() const
{
    return m_subProperties;
}

void
GtAbstractProperty::registerSubProperty(GtAbstractProperty& property)
{
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
        gtWarning() << tr("Multiple property registration!")
                    << tr("Property '%1' already contains sub-property '%2'.")
                       .arg(objectName()).arg(property.objectName());
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

    // if any of the sub props has changed, emit a signal
    connect(&property, &GtAbstractProperty::changed, this,
            [this, p = &property]() {
        emit subPropChanged(p);
    });
}

bool
GtAbstractProperty::isReadOnly()
{
    // connected properties are read only by default
    if (m_connection)
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
GtAbstractProperty::propertyCount() const
{
    return std::count_if(std::begin(m_subProperties), std::end(m_subProperties),
                         [](const GtAbstractProperty* prop) {
        return !prop->isHidden();
    });
}

GtAbstractProperty const *
GtAbstractProperty::findProperty(const QString& id) const
{
    foreach (GtAbstractProperty* sub, fullProperties())
    {
        if (sub->ident() == id)
        {
            return sub;
        }

        GtAbstractProperty const * tmp = sub->findProperty(id);
        if (tmp)
        {
            return tmp;
        }
    }

    return nullptr;
}

GtAbstractProperty*
GtAbstractProperty::findProperty(const QString& id)
{
    return const_cast<GtAbstractProperty*>
        (const_cast<const GtAbstractProperty*>(this)->findProperty(id));
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
        if (tmp)
        {
            return tmp;
        }
    }

    return nullptr;
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
    if (!connection)
    {
        return;
    }

    // check whether property is already connected
    if (m_connection)
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
    if (!m_connection)
    {
        return;
    }

    disconnect(m_connection.data(), SIGNAL(triggerValueTransfer()),
               this, SLOT(onTriggerValueTransfer()));

    m_connection = nullptr;
}

bool
GtAbstractProperty::isConnected()
{
    return m_connection != nullptr;
}

bool
GtAbstractProperty::isConnected() const
{
    return const_cast<GtAbstractProperty*>(this)->isConnected();
}

bool
GtAbstractProperty::isMonitoring() const
{
    return m_monitoring;
}

void
GtAbstractProperty::setMonitoring(bool monitoring)
{
    if (monitoring)
    {
        setReadOnly(true);
    }

    m_monitoring = monitoring;
}

GtAbstractProperty::GtAbstractProperty() = default;

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

QVariant gt::getConnectedValue(const GtPropertyConnection &connection)
{
    return connection.valueFromSource();
}
