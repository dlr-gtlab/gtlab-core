/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_objectlinkproperty.cpp
 *
 *  Created on: 15.02.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_objectlinkproperty.h"
#include "gt_object.h"
#include "gt_objectfactory.h"

#include <utility>

GtObjectLinkProperty::GtObjectLinkProperty(const QString& ident,
        const QString& name,
        const QString& brief,
        const QString& uuid,
        GtObject* obj,
        QStringList  allowedClasses,
        bool linkFromSuperClassesEnabled) :
    m_obj(obj),
    m_allowedClasses(std::move(allowedClasses)),
    m_linkFromSuperClassesEnabled(linkFromSuperClassesEnabled)
{
    setObjectName(name);

    m_id = ident;
    m_brief = brief;
    m_unitCategory = GtUnit::Category::None;
    m_value = uuid;
}

GtObjectLinkProperty::GtObjectLinkProperty(const QString& ident,
        const QString& name,
        const QString& brief,
        GtObject* obj,
        const QStringList& allowedClasses,
        bool linkFromSuperClassesEnabled) :
    GtObjectLinkProperty(ident, name, brief, "", obj, allowedClasses,
                         linkFromSuperClassesEnabled)
{
}

QVariant
GtObjectLinkProperty::valueToVariant(const QString& unit, bool* success) const
{
    return QVariant::fromValue(getVal(unit, success));
}

bool
GtObjectLinkProperty::setValueFromVariant(const QVariant& val,
        const QString& /*unit*/)
{
    bool ok = false;
    setVal(val.toString(), &ok);

    return ok;
}

void
GtObjectLinkProperty::setAllowedClasses(QStringList allowedClasses)
{
    m_allowedClasses = std::move(allowedClasses);

    if (auto* obj = linkedObject())
    {
        if (!isAllowed(*obj))
        {
            m_value.clear();
        }
    }
}

GtObject*
GtObjectLinkProperty::linkedObject(GtObject* root)
{
    if (m_value.isEmpty()) return nullptr;

    GtObject* rootObject = root;

    if (!rootObject && m_obj)
    {
        rootObject = m_obj->findRoot<GtObject*>(m_obj);
    }

    if (!rootObject) return nullptr;

    return rootObject->getObjectByUuid(m_value);
}

GtObject const*
GtObjectLinkProperty::linkedObject(const GtObject* root) const
{
    return const_cast<GtObjectLinkProperty*>(this)
           ->linkedObject(const_cast<GtObject*>(root));
}

GtObject*
GtObjectLinkProperty::object() const
{
    return m_obj;
}

QString
GtObjectLinkProperty::linkedObjectUUID() const
{
    return m_value;
}

const QStringList&
GtObjectLinkProperty::allowedClasses() const
{
    return m_allowedClasses;
}

bool
GtObjectLinkProperty::linkFromSuperClass() const
{
    return m_linkFromSuperClassesEnabled;
}

bool
GtObjectLinkProperty::isAllowed(const QString& className) const
{
    if (m_allowedClasses.contains(className)) return true;

    if (!linkFromSuperClass()) return false;

    // default to GtObjectFactory
    GtAbstractObjectFactory const* factory = gtObjectFactory;

    // prefer factory of object
    if (m_obj && m_obj->factory()) factory = m_obj->factory();

    auto* meta = factory->metaObject(className);
    if (!meta)
    {
        if (factory != gtObjectFactory)
        {
            // check GtObjectFactory (see issue 552, derived factories may not work yet)
            meta = static_cast<GtAbstractObjectFactory const*>(gtObjectFactory)
                       ->metaObject(className);
        }

        if (!meta) return false;
    }

    // check allowed objects repeatively
    return std::any_of(m_allowedClasses.cbegin(), m_allowedClasses.cend(),
                       [=](const QString& cls){
        return gt::isDerivedFromClass(meta, cls);
    });
}
