/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractprocessprovider.cpp
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_processfactory.h"
#include "gt_processcomponent.h"
#include "gt_objectmemento.h"

#include "gt_abstractprocessprovider.h"

GtAbstractProcessProvider::GtAbstractProcessProvider() :
    m_component(nullptr)
{

}

GtObjectMemento
GtAbstractProcessProvider::componentData()
{
    if (!m_component)
    {
        return GtObjectMemento();
    }

    return m_component->toMemento();
}

bool
GtAbstractProcessProvider::setComponentData(const GtObjectMemento& memento)
{
    if (!m_component)
    {
        return false;
    }

    m_component->fromMemento(memento);

    return true;
}

bool
GtAbstractProcessProvider::initComponent(const QString& classname)
{
    if (m_component)
    {
        if (m_component->metaObject()->className() == classname)
        {
            return true;
        }

        delete m_component;
    }

    GtObject* obj = gtProcessFactory->newObject(classname);

    if (!obj)
    {
        gtDebug() << "could not create new object" <<
                     " (GtAbstractProcessProvider::initComponent)";
        gtDebug() << "  |-> " << classname;
        return false;
    }

    m_component = qobject_cast<GtProcessComponent*>(obj);

    if (!m_component)
    {
        gtDebug() << "could not cast new object" <<
                     " (GtAbstractProcessProvider::initComponent)";
        delete obj;
        return false;
    }

    m_component->setParent(this);

    return true;
}

bool
GtAbstractProcessProvider::initComponent(const QMetaObject* metaObj)
{
    return initComponent(metaObj->className());
}

bool
GtAbstractProcessProvider::initComponent(const QMetaObject& metaObj)
{
    return initComponent(metaObj.className());
}

bool
GtAbstractProcessProvider::componentInitialized()
{
    return (m_component != nullptr);
}

QString
GtAbstractProcessProvider::componentClassName()
{
    if (!m_component)
    {
        return QString();
    }

    return m_component->metaObject()->className();
}
