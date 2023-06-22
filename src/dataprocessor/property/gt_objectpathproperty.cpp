/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_objectpathproperty.cpp
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
        const QString& /*unit*/)
{
    bool ok = false;
    setVal(val.toString(), &ok);
    return ok;
}

GtObject*
GtObjectPathProperty::linkedObject(GtObject* root)
{
    if (m_value.isEmpty())
    {
        return nullptr;
    }

    GtObject* rootObject = root;

    if (!rootObject)
    {
        rootObject = object()->findRoot<GtObject*>(object());
    }

    if (!rootObject)
    {
        return nullptr;
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
