/* GTlab - Gas Turbine laboratory
 * Source File: gt_objectlinkproperty.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 15.02.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_objectlinkproperty.h"
#include "gt_object.h"

GtObjectLinkProperty::GtObjectLinkProperty(const QString& ident,
        const QString& name,
        const QString& brief,
        const QString& uuid,
        GtObject* obj,
        const QStringList& allowedClasses) :
    m_obj(obj),
    m_allowedClasses(allowedClasses)
{
    setObjectName(name);

    m_id = ident;
    m_brief = brief;
    m_unitCategory = GtUnit::Category::None;
    m_value = uuid;
}

QVariant
GtObjectLinkProperty::valueToVariant(const QString& unit, bool* success) const
{
    return QVariant::fromValue(getVal(unit, success));
}

bool
GtObjectLinkProperty::setValueFromVariant(const QVariant& val,
        const QString& /*unit*/,
        bool* success)
{
    bool retval = false;

    setVal(val.toString(), success);

    if (success != nullptr)
    {
        retval = *success;
    }

    return retval;
}

GtObject*
GtObjectLinkProperty::linkedObject(GtObject* root)
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

    return rootObject->getObjectByUuid(m_value);
}

GtObject*
GtObjectLinkProperty::object()
{
    return m_obj;
}

QString
GtObjectLinkProperty::linkedObjectUUID() const
{
    return m_value;
}

const QStringList&
GtObjectLinkProperty::allowedClasses()
{
    return m_allowedClasses;
}
