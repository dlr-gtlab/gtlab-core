/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 29.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_groupproperty.h"

GtGroupProperty::GtGroupProperty(const QString& name,
                                 const QString& brief)
{
    setObjectName(name);

    m_brief = brief;
    m_unitCategory = GtUnit::Category::None;
    m_value = QString();
    m_readOnly = true;
    m_storeMemento = false;

    connect(this, SIGNAL(changed()), SLOT(onSubPropertyChange()));
}


QVariant
GtGroupProperty::valueToVariant(const QString& unit, bool* success) const
{
    return getVal(unit, success);
}

bool
GtGroupProperty::setValueFromVariant(const QVariant& /*val*/,
                                     const QString& /*unit*/)
{
//    bool retval = true;

//    setVal(val.toString(), unit, success);

//    if (success != 0)
//    {
//        retval = *success;
//    }

//    return retval;

    return false;
}

void
GtGroupProperty::registerSubProperty(GtAbstractProperty& property)
{
    GtAbstractProperty::registerSubProperty(property);

    connect(&property, SIGNAL(changed()), SLOT(onSubPropertyChange()));

    generateSummary();
}

void
GtGroupProperty::generateSummary()
{
    if (m_subProperties.isEmpty())
    {
        return;
    }

    m_value = QStringLiteral("[");
    for (int i = 0; i < m_subProperties.size() - 1; i++)
    {
        m_value = m_value + m_subProperties[i]->valueToVariant().toString() +
                  QStringLiteral(", ");
    }
    m_value = m_value + m_subProperties.last()->valueToVariant().toString() +
              QStringLiteral("]");

}

void
GtGroupProperty::onSubPropertyChange()
{
    generateSummary();
}
