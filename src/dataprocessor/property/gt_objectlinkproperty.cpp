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
}

GtObject*
GtObjectLinkProperty::linkedObject(GtObject* root)
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

    if (!m_obj || !linkFromSuperClass()) return false;

    auto const* factory = m_obj->factory();
    if (!factory)
    {
        // default to GtObjectFactory
        factory = static_cast<GtAbstractObjectFactory const*>(gtObjectFactory);
    }

    auto* meta = factory->metaObject(className);
    if (!meta)
    {
        // check GtObjectFactory (see issue 552, derived factories may not work)
        meta = static_cast<GtAbstractObjectFactory const*>(gtObjectFactory)
                   ->metaObject(className);

        if (!meta)
        {
            gtError() << tr("Failed to check if '%1' is an allowed class for "
                            "the object link property '%2'! (Metaobject for "
                            "class name not found)").arg(className, ident());
            return false;
        }
    }

    // check allowed objects repeatively
    return std::any_of(m_allowedClasses.cbegin(), m_allowedClasses.cend(),
                       [=](const QString& cls){
        return gt::isDerivedFromClass(meta, cls);
    });
}
