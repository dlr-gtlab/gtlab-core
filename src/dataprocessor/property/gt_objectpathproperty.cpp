/* GTlab - Gas Turbine laboratory
 * Source File: gt_objectpathproperty.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 14.12.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_objectpath.h"

#include "gt_objectpathproperty.h"

GtObjectPathProperty::GtObjectPathProperty(const QString& ident,
        const QString& name,
        const QString& brief,
        const QString& path,
        GtObject* obj,
        const QStringList& allowedClasses) :
    m_obj(obj),
    m_allowedClasses(allowedClasses)
{
    setObjectName(name);
    m_id = ident;
    m_brief = brief;
    m_unitCategory = GtUnit::Category::None;
    m_value = path;
}

QVariant
GtObjectPathProperty::valueToVariant(const QString& unit, bool* success) const
{
    return QVariant::fromValue(getVal(unit, success));
}

bool
GtObjectPathProperty::setValueFromVariant(const QVariant& val,
        const QString& /*unit*/,
        bool* success)
{
    bool retval = false;

    setVal(val.toString(), success);

    if (success != 0)
    {
        retval = *success;
    }

    return retval;
}

GtObject*
GtObjectPathProperty::linkedObject(GtObject* root)
{
    if (m_value.isEmpty())
    {
        return Q_NULLPTR;
    }

    GtObject* rootObject = root;

    if (rootObject == Q_NULLPTR)
    {
        rootObject = object()->findRoot<GtObject*>(object());
    }

    if (rootObject == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    return rootObject->getObjectByPath(m_value);
}

GtObject*
GtObjectPathProperty::object()
{
    return m_obj;
}

const QStringList&
GtObjectPathProperty::allowedClasses()
{
    return m_allowedClasses;
}

GtObjectPath
GtObjectPathProperty::path() const
{
    return GtObjectPath(getVal());
}
