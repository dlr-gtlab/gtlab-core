/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 05.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_modeproperty.h"
#include "gt_modetypeproperty.h"

#include <algorithm>

GtModeProperty::GtModeProperty(const QString& ident, const QString& name,
                               const QString& brief)
{
    setObjectName(name);

    m_id = ident;
    m_brief = brief;
    m_unitCategory = GtUnit::Category::None;
    m_value = QString();
}

QVariant
GtModeProperty::valueToVariant(const QString& unit, bool* success) const
{
    return getVal(unit, success);
}

bool
GtModeProperty::setValueFromVariant(const QVariant& val,
                                    const QString& /*unit*/,
                                    bool* /*success*/)
{
    QString tmpVal = val.toString();

    if (tmpVal.isEmpty())
    {
        return false;
    }

    setVal(tmpVal);
    return true;
}

const QList<GtAbstractProperty*>&
GtModeProperty::properties()
{
    updateProperties();

    return m_tmpProperties;
}

void
GtModeProperty::registerSubProperty(GtAbstractProperty& property)
{
    GtModeTypeProperty* tmp = qobject_cast<GtModeTypeProperty*>(&property);
    if (tmp)
    {
        registerSubProperty(*tmp);
    }
    else
    {
        gtWarning() << tr("could not register sub property!")
                    << " (" << property.objectName() << ")";
    }
}

void
GtModeProperty::registerSubProperty(GtModeTypeProperty& property)
{
    GtAbstractProperty::registerSubProperty(property);

    if (get().isEmpty())
    {
        setVal(property.objectName());
    }
}

bool
GtModeProperty::modeExists(const QString& mode)
{
    return std::any_of(std::begin(m_subProperties), std::end(m_subProperties),
                       [&mode](const GtAbstractProperty* prop) {
        return prop->objectName() == mode;
    });
}

int
GtModeProperty::propertyCount(const QString& mode)
{
    foreach (GtAbstractProperty* prop, m_subProperties)
    {
        if (prop->objectName() == mode)
        {
            return prop->propertyCount();
        }
    }

    return 0;
}

QStringList
GtModeProperty::modes()
{
    QStringList retval;

    foreach (GtAbstractProperty* prop, m_subProperties)
    {
        retval << prop->objectName();
    }

    return retval;
}

GtModeTypeProperty*
GtModeProperty::typeProperty(const QString& mode)
{
    foreach (GtAbstractProperty* prop, m_subProperties)
    {
        if (prop->objectName() == mode)
        {
            return qobject_cast<GtModeTypeProperty*>(prop);
        }
    }

    return Q_NULLPTR;
}


void
GtModeProperty::updateProperties()
{
    m_tmpProperties.clear();

    if (get().isEmpty())
    {
        return;
    }

    foreach (GtAbstractProperty* prop, m_subProperties)
    {
        if (prop->objectName() == get())
        {
            m_tmpProperties.append(prop->properties());
        }
    }
}

bool
GtModeProperty::validateValue(const QString& value)
{
    return modes().contains(value);
}

