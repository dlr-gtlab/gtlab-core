/* GTlab - Gas Turbine laboratory
 * Source File: gt_freestringproperty.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 20.11.2018
 *  Author: Björn Schneider (AT-TWK)
 *  Tel.: +49 2203 601 2642
 */

#include "gt_freestringproperty.h"

#include <QVariant>

GtFreeStringProperty::GtFreeStringProperty(const QString& ident,
                                           const QString& name)
{

    setObjectName(name);

    m_id = ident;
    m_brief = QString();
    m_unitCategory = GtUnit::Category::None;
    m_value = QString();
    m_initValue = QString();
}

GtFreeStringProperty::GtFreeStringProperty(const QString& ident,
                                           const QString& name,
                                           const QString& brief,
                                           const QString& value)
{
    setObjectName(name);

    m_id = ident;
    m_brief = brief;
    m_unitCategory = GtUnit::Category::None;
    m_value = value;
    m_initValue = value;
}

QVariant
GtFreeStringProperty::valueToVariant(const QString& unit, bool* success) const
{
    Q_UNUSED(unit);

    if (success)
    {
        *success = true;
    }

    return QVariant::fromValue(m_value);
}

bool
GtFreeStringProperty::setValueFromVariant(const QVariant& val,
                                          const QString& unit,
                                          bool* success)
{
    Q_UNUSED(unit);
    Q_UNUSED(success);

    bool retval = true;

    m_value = val.toString();
    //    setVal(val.toString(), success);

    //    if (success != 0)
    //    {
    //        retval = *success;
    //    }

    return retval;
}

GtFreeStringProperty::~GtFreeStringProperty()
{

}

QString
GtFreeStringProperty::getVal() const
{
    return m_value;
}

void
GtFreeStringProperty::setVal(const QString& val)
{
    m_value = val;
}
